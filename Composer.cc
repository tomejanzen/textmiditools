//
// TextMIDITools Version 1.0.87
//
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Sun 25 Dec 2022 01:47:08 AM EST add space between TicksPerQuarter and tracks type

#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cmath>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <list>
#include <ranges>
#include <chrono>

#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "Composer.h"
#include "NoteEvent.h"
#include "RandomDouble.h"
#include "RandomInt.h"
#include "Track.h"
#include "GnuPlot.h"
#include "Midi.h"
#include "RhythmRational.h"

using namespace std;
using namespace std::chrono;
using namespace boost;
using namespace textmidi;
using namespace midi;
using namespace textmidi::cgm;
using namespace textmidi::rational;
using namespace arrangements;

namespace
{
    // might become settable at a later time.
    constexpr int TempoBeatsPerMinute{60};
    constexpr int RestPitchIndex{numeric_limits<int>().max()};

    using KeyScaleSeq = std::vector<int>;
}

//
// Convert a double duration in seconds to a musical ratio.
RhythmRational textmidi::cgm::Composer::duration_to_rhythm(double duration) const noexcept
{
    //  beat    quarter   whole   minute
    // ------ * ------- * ----- * -------
    // minute    whole    beat    seconds

    //  beat    whole   minute
    // ------ * ----- * -------
    // minute   beat    seconds
    RhythmRational wholes_per_second{
      RhythmRational{TempoBeatsPerMinute} * WholesPerBeat / RhythmRational{midi::SecondsPerMinuteI} };
    //wholes_per_second.reduce();
    //  Ticks    Quarters    whole
    // ------- * -------- * -------
    // Quarter     Whole    second
    const RhythmRational TicksPerSecond{
         RhythmRational{TicksPerQuarter} * QuartersPerWholeRat * wholes_per_second};
    // Turn the rhythm (duration) into a ratio and multiply
    // both the numerator and the denominator by TicksPerSecond.
    // This gives us the actual musical rhythm value.
    const double TicksPerSecondDouble{
          static_cast<double>(TicksPerSecond.numerator())
        / static_cast<double>(TicksPerSecond.denominator())};
    const int64_t TicksPerSecondInt64
        {static_cast<int64_t>(round(TicksPerSecondDouble))};
    RhythmRational rhythm
        {static_cast<int64_t>(duration * TicksPerSecondDouble),
        TicksPerSecondInt64};
    rhythm *= wholes_per_second;
    return abs(rhythm);
}

//
// Coerce a duration to be in multiples of the pulse/second value.
RhythmRational textmidi::cgm::Composer::snap_to_pulse(RhythmRational rhythm, double pulse_per_second) const noexcept
{
    RhythmRational wholes_per_second{
      RhythmRational{TempoBeatsPerMinute} * WholesPerBeat / RhythmRational{SecondsPerMinuteI} };
    //wholes_per_second.reduce();
    const RhythmRational TicksPerSecond{
         RhythmRational{TicksPerQuarter} * QuartersPerWholeRat * wholes_per_second};
    const double TicksPerSecondDouble{
          static_cast<double>(TicksPerSecond.numerator())
        / static_cast<double>(TicksPerSecond.denominator())};
    const int64_t TicksPerSecondInt64
        {static_cast<int64_t>(round(TicksPerSecondDouble))};
    RhythmRational whole_per_pulse(
        wholes_per_second *
        RhythmRational(TicksPerSecondInt64,
        static_cast<int64_t>
            (round(pulse_per_second * TicksPerSecondDouble))));
    // Get rid of the remainder.
    // but first round off by adding a half-pulse.
    RhythmRational
        pulse_per_rhythm{(rhythm + whole_per_pulse / RhythmRational{2L}) / whole_per_pulse};
    if (pulse_per_rhythm.denominator() != 1)
    {
        const auto rem{pulse_per_rhythm.numerator()
                     % pulse_per_rhythm.denominator()};
        // snap the rhythm (duration) to pulses.
        pulse_per_rhythm = RhythmRational{pulse_per_rhythm.numerator() - rem,
            pulse_per_rhythm.denominator()};
        //pulse_per_rhythm.reduce();
        if (!pulse_per_rhythm)
        {
            pulse_per_rhythm = RhythmRational{1L}; // i.e., one pulse minimum.
        }
    }
    rhythm = pulse_per_rhythm * whole_per_pulse;
    return rhythm;
}

void textmidi::cgm::Composer::build_composition_priority_graph(const MusicalForm& xml_form,
    vector<list<int>>& leaders_topo_sort) noexcept
{
    leaders_topo_sort.clear();
    // Build a composition priority graph.
    // The leaders have to be composed first.
    // The resulting leaders_topo_sort is a list of lists.
    //     leader1 follower5
    //     leader3 follower2 follower6 follower7
    //     leader4 follower8 follower9
    //
    // It is acceptable to have citing circles with no leaders;
    // they just won't get any note events.  That's why this is not
    // a tree graph; it can have loops.
    // It is also acceptable for a follower to be a lower-numbered
    // voice then its leader.
    // The following graph is not affected by track scrambling.
    // Track scrambling is just track priority scrambling under
    // the texture curve.
    //
    // For all follower tracks
    //
    // Square matrix to represent the follower graph.
    // 1 follows 2
    // 2 follows 0
    // 3 follows 0
    // |  F  F  F  F  |
    // |  F  F  T  F  |
    // |  T  F  F  F  |
    // |  T  F  F  F  |
    //
    // 0 follows 1
    // 1 follows 2
    // 2 follows 3
    // |  F  T  F  F  |
    // |  F  F  T  F  |
    // |  F  F  F  T  |
    // |  F  F  F  F  |
    // If there is no true in a row, then the voice is not a follower.
    // Use the graph to follow the leaders to the leader with no trues in its row.
    // Then that is the real leader of a group.
    // Then generate the voices backwards from the leader to all followers in its column,
    // and follow the leaders to the next followers.

    // Make a square matrix of false.
    vector<vector<bool>> followers_graph(xml_form.voices().size(), vector<bool>(xml_form.voices().size(), false));

    for (int tr{}; tr < static_cast<int>(xml_form.voices().size()); ++tr)
    {
        if (xml_form.voices()[tr].follower().follow()) [[unlikely]]
        {
            if (xml_form.voices()[tr].follower().leader() < static_cast<int>(followers_graph.size()))
            {
                followers_graph[xml_form.voices()[tr].follower().leader()][tr] = true;
                if (xml_form.voices()[tr].follower().leader() == tr)
                {
                    const string errstr{((((string{__FILE__ } += ':')
                        += BOOST_PP_STRINGIZE(__LINE__)) += " voice ")
                        += lexical_cast<string>(tr)) += " is a self-follower!\n"};
                    cerr << errstr;
                }
            }
        }
    }
#undef TEXTMIDICGM_PRINT
#if defined(TEXTMIDICGM_PRINT)
    for (auto& row : followers_graph)
    {
        ranges::copy(row, ostream_iterator<bool>(cout, " "));
        cout << '\n';
    }
#endif
    // This is not wrong to resize. 2023-02-20
    leaders_topo_sort.resize(followers_graph.size());

    for (int follower_index{}; follower_index < static_cast<int>(followers_graph.size()); ++follower_index)
    {
        bool leader_only{true};
        for (int leader_index{}; leader_index < static_cast<int>(followers_graph[0].size()); ++leader_index)
        {
            if (followers_graph[leader_index][follower_index]) // if this is a follower
            {
                leader_only = false;
            }
        }
        if (leader_only)
        {
            leaders_topo_sort[0].push_back(follower_index); // if not a follower save it.
        }
    }
#if defined(TEXTMIDICGM_PRINT)
    cout << "List of non-followers: ";
    ranges::copy(leaders_topo_sort[0], ostream_iterator<int>(cout, " "));
    cout << '\n';
#endif
    for (int g{1}; g < static_cast<int>(followers_graph.size()); ++g)
    {
        for (int follower_index{}; follower_index < static_cast<int>(followers_graph[0].size()); ++follower_index)
        {
            for (int leader_index{}; leader_index < static_cast<int>(followers_graph.size()); ++leader_index)
             {
#if defined(TEXTMIDICGM_PRINT)
                cout << "** " << leader_index << ' ' << follower_index << ' ' << followers_graph[leader_index][follower_index] << '\n';
#endif
                if (followers_graph[leader_index][follower_index]) // if this is a follower
                {
                    const auto it{ranges::find(leaders_topo_sort[g - 1], leader_index)};
                    if (it != leaders_topo_sort[g - 1].end())
                    {
                        leaders_topo_sort[g].push_back(follower_index);
                    }
                }
            }
#if defined(TEXTMIDICGM_PRINT)
            cout << '\n';
#endif
        }
    }
#if defined(TEXTMIDICGM_PRINT)
    cout << "order of composing:\n";
    for (auto& lts : leaders_topo_sort)
    {
        ranges::copy(lts, ostream_iterator<int>(cout, " "));
        cout << '\n';
    }
#endif
}

void textmidi::cgm::Composer::build_track_scramble_sequences(vector<vector<int>>& track_scramble_sequences,
    TicksDuration total_duration) noexcept
{
    for (auto scramble_time{TicksDuration(0)}; scramble_time < total_duration;
        scramble_time = scramble_time + track_scramble_.period_)
    {
        track_scramble_sequences.push_back(track_scramble_.arrangements_->arrangement());
        track_scramble_.arrangements_->next();
    }
#undef TEXTMIDICGM_PRINT
#if defined(TEXTMIDICGM_PRINT)
    cout << "track_scramble_sequences\n";
    for (auto tss : track_scramble_sequences)
    {
        ranges::copy(tss, ostream_iterator<int>(cout, " "));
        cout << '\n';
    }
#endif
#undef TEXTMIDICGM_PRINT
}

//
// The compose() executive function for textmidicgm.
// It implements the composing engine from AlgoRhythms 3.0 for the Commodore Amiga
// with minor enhancements.
//
void textmidi::cgm::Composer::operator()(ofstream& textmidi_file, const MusicalForm& xml_form, bool write_header)
{
    // If the command line did not set arrangements, then set them from the XML file.
    track_scramble_.period_ = ((PermutationEnum::Undefined == track_scramble_.scramble_)
        ? (static_cast<TicksDuration>(static_cast<int>(round(xml_form.arrangement_definition().period()))) * TicksPerQuarter)
        : track_scramble_.period_);

    track_scramble_.scramble_
        = (track_scramble_.scramble_ == arrangements::PermutationEnum::Undefined)
        ? xml_form.arrangement_definition().algorithm() : track_scramble_.scramble_;
    RandomDouble random_double{};
    vector<Track> tracks(xml_form.voices().size()/*, Track{ xml_form.scale().size() / 2 }*/);
    vector<pair<int, int>> tessitura;

    track_scramble_.arrangements_ = ArrangementsFactory(track_scramble_.scramble_,
        xml_form.voices().size());

    for (auto v : xml_form.voices())
    {
        tessitura.emplace_back(
            textmidi::pitchname_to_keynumber(v.low_pitch()).first,
            textmidi::pitchname_to_keynumber(v.high_pitch()).first);
    }
    KeyScaleSeq key_scale;
    xml_form.string_scale_to_int_scale(key_scale);
    const TicksDuration time_step{xml_form.pulse()
        ? (static_cast<int64_t>(static_cast<double>
          (TicksPerQuarter) / xml_form.pulse())) : 1};
    const TicksDuration total_duration(static_cast<int64_t>(floor(xml_form.len()
          * double(TicksPerQuarter))));

    vector<list<int>> leaders_topo_sort;
    build_composition_priority_graph(xml_form, leaders_topo_sort);

    vector<vector<int>> track_scramble_sequences;
    build_track_scramble_sequences(track_scramble_sequences, total_duration);

    vector<vector<cgm::NoteEvent>> track_note_events(xml_form.voices().size());

    for (auto& lts : leaders_topo_sort)
    {
        // loop over the list of voice indices
        for (auto tr : lts)
        {
#if defined(TEXTMIDICGM_PRINT)
            cout << "tr: " << tr << '\n';
#endif
            auto& track{tracks[tr]};
            if (!xml_form.voices()[tr].follower().follow()) [[likely]]
            {
                while ((track.the_next_time() < total_duration) && (track_note_events.size() < max_events_per_track_))
                {
                    auto scramble_index{track.the_next_time() / track_scramble_.period_};
                    scramble_index = ((scramble_index < static_cast<long int>(track_scramble_sequences.size()))
                        ?  scramble_index : track_scramble_sequences.size() - 1);
#if defined(TEXTMIDICGM_PRINT)
                    cout << "scramble_index: " << scramble_index << '\n';
#endif
                    MusicalCharacter musical_character{};
                    xml_form.character_now(track.the_next_time(), musical_character);
                    int number_of_voices = static_cast<int>(musical_character.texture_range) + 1;
                    number_of_voices = (static_cast<size_t>(number_of_voices) > xml_form.voices().size()) ? xml_form.voices().size() : number_of_voices;
                    number_of_voices = (number_of_voices < 1) ? 1 : number_of_voices;
#if defined(TEXTMIDICGM_PRINT)
                    cout << "texture_range: " << musical_character.texture_range << " number_of_voices: " << number_of_voices << '\n';
#endif

                    double dynamicd{(musical_character.dynamic_range
                                   * random_double())
                      - (musical_character.dynamic_range / 2.0)
                      +  musical_character.dynamic_mean};
                    int dynamic{static_cast<int>(round(dynamicd))};

                    dynamic = std::min(dynamic, static_cast<int>(std::round(MaxDynamic)));
                    dynamic = std::max(dynamic, static_cast<int>(std::round(MinDynamic)));

                    double rhythmd
                        {musical_character.duration(random_double())};
                    auto rhythm{duration_to_rhythm(rhythmd)};

                    // Now apply the pulse.
                    // pulse is in pulses per second.
                    if (xml_form.pulse() != 0)
                    {
                        rhythm = snap_to_pulse(rhythm, xml_form.pulse());
                    }
                    const RhythmRational wholes_per_second{
                        RhythmRational{TempoBeatsPerMinute} * WholesPerBeat
                            / RhythmRational{SecondsPerMinuteI}};
                    const RhythmRational TicksPerSecond(
                        RhythmRational{TicksPerQuarter} * QuartersPerWholeRat
                                        * wholes_per_second);
                    // wholes   Ticks    second
                    // ------ * ------ * ------
                    // note     Second   wholes
                    auto rhythmtdrational{rhythm * TicksPerSecond
                        / wholes_per_second};
                    TicksDuration rhythmtd{
                        ( rhythmtdrational.numerator()
                        + rhythmtdrational.denominator() / 2)
                        / rhythmtdrational.denominator()};
                    track.the_last_time(track.the_next_time());
                    track.the_next_time(track.the_next_time() + rhythmtd);

                    int pitch_index{};

                    MelodyProbabilities::MelodyDirection
                        direction{xml_form.melody_probabilities()
                        (random_double())};
                    bool crisp_walking{};
                    if (1.0 == xml_form.voices()[tr].walking())
                    {
                        crisp_walking = true;
                    }
                    else
                    {
                        if (xml_form.voices()[tr].walking() > 0.0)
                        {
                            crisp_walking = (xml_form.voices()[tr].walking() > random_double()) ? true : false;
                        }
                    }
                    if (crisp_walking)
                    {
                        // first walking note should be in mid-scale
                        if (track_note_events[tr].empty()) [[unlikely]]
                        {
                            pitch_index = xml_form.scale().size() / 2;
                        }
                        else
                        {
                            switch (direction)
                            {
                                case MelodyProbabilities::MelodyDirection::Up:
                                    if (track.last_pitch_index()
                                        < static_cast<int>(xml_form.scale().size() - 1))
                                    {
                                        pitch_index = track.last_pitch_index() + 1;
                                    }
                                    else
                                    {
                                        pitch_index = track.last_pitch_index();
                                    }
                                    break;
                                case MelodyProbabilities::MelodyDirection::Same:
                                    pitch_index = track.last_pitch_index();
                                    break;
                                case MelodyProbabilities::MelodyDirection::Down:
                                    if (track.last_pitch_index() > 0)
                                    {
                                        pitch_index = track.last_pitch_index()
                                                    - 1;
                                    }
                                    else
                                    {
                                        pitch_index = track.last_pitch_index();
                                    }
                                    break;
                                case MelodyProbabilities::MelodyDirection::Rest:
                                    pitch_index = RestPitchIndex;
                                    break;
                            }
                        }
                    }
                    else
                    {
                        if (direction
                            != MelodyProbabilities::MelodyDirection::Rest)
                        {
                            pitch_index = musical_character.pitch_index
                                (random_double());
                        }
                        else
                        {
                            pitch_index = RestPitchIndex;
                        }
                    }

                    // coerce to be on scale
                    if ((pitch_index != RestPitchIndex)
                        && (pitch_index >= static_cast<int>(xml_form.scale().size())))
                    {
                        // This is probably why it keeps banging
                        // on the top of the scale.
                        // Alternatively it could be changed to a rest.
                        pitch_index = xml_form.scale().size() - 1;
                    }
                    if (pitch_index < 0)
                    {
                        pitch_index = 0;
                    }
                    if ((pitch_index >= xml_form.scale().size()) && (pitch_index < RestPitchIndex))
                    {
                        pitch_index = (xml_form.scale().size() ? (xml_form.scale().size() - 1) : 0);
                    }

                    // Same pitch index as the last pitch index
                    if (pitch_index != RestPitchIndex)
                    {
                        track.last_pitch_index(pitch_index);
                    }

                    // use number_of_voices to subset track_scramble_sequences[scramble_index].
                    // If you can find tr in the sub sequence, then play.

                    auto trit{ranges::find(track_scramble_sequences[scramble_index].begin(), track_scramble_sequences[scramble_index].begin() + number_of_voices, tr)};
                    if ((pitch_index != RestPitchIndex)
                        && (trit != (track_scramble_sequences[scramble_index].begin() + number_of_voices)))
                    {
                        auto key_number(key_scale[pitch_index]);
                        key_number = max(key_number, tessitura[tr].first);
                        key_number = min(key_number, tessitura[tr].second);
                        int key_index{0U};
                        {
                            KeyScaleSeq::const_iterator key_iter{ranges::find(key_scale, key_number)};
                            if (key_iter != key_scale.cend())
                            {
                                key_index = std::distance(key_scale.cbegin(), key_iter);
                            }
                        }
                        if (pitch_index != RestPitchIndex)
                        {
                            track.last_pitch_index(key_index);
                        }
                        NoteEvent temp_note_event{key_number,
                            dynamic, rhythm};
                        track_note_events[tr].push_back(temp_note_event);
                    } else {
                        NoteEvent temp_note_event{RestPitch, 0, rhythm};
                        track_note_events[tr].push_back(temp_note_event);
                    }
                }
            }
            else [[unlikely]] // it is a follower
            {
                try
                {
                    track_note_events[tr] = track_note_events.at(xml_form.voices()[tr].follower().leader());
                }
                catch (out_of_range& exc)
                {
                    cerr << __FILE__ << ':' << BOOST_PP_STRINGIZE(__LINE__) << ':'
                         << " follower leader must be in (0..# of tracks-1) " << exc.what() << '\n';
                    exit(0);
                }
                int pivot_key{RestPitch};
                int pivot_key_index{static_cast<int>(key_scale.size())};
                for (auto& ne : track_note_events[tr])
                {
                    switch (xml_form.voices()[tr].follower().interval_type())
                    {
                      [[likely]] case VoiceXml::Follower::IntervalType::Scalar:
                        {
                            int key_index{0U};

                            KeyScaleSeq::const_iterator key_iter{ranges::find(key_scale, ne.pitch())};
                            if (key_iter != key_scale.cend())
                            {
                                key_index = std::distance(key_scale.cbegin(), key_iter);
                                if (xml_form.voices()[tr].follower().interval() < 0)
                                {
                                    // Check there's room down the scale
                                    if (abs(xml_form.voices()[tr]
                                        .follower().interval()) < key_index)
                                    {
                                        key_index += xml_form.voices()[tr].follower().interval();
                                    }
                                    if (key_scale.size() == static_cast<long unsigned>(pivot_key_index))
                                    {
                                        pivot_key_index = key_index;
                                    }
                                }
                                else // follower interval is 0 or positive
                                {
                                    // Check there's room up the scale
                                    if ((xml_form.voices()[tr]
                                        .follower().interval() + key_index)
                                           < static_cast<int>(key_scale.size()))
                                    {
                                        key_index += xml_form.voices()[tr]
                                            .follower().interval();
                                        if (key_scale.size() == static_cast<long unsigned>(pivot_key_index))
                                        {
                                            pivot_key_index = key_index;
                                        }
                                        if (RestPitch == pivot_key)
                                        {
                                            pivot_key = key_scale[key_index];
                                        }
                                    }
                                }
                                if (xml_form.voices()[tr].follower().inversion())
                                {
                                    const int shift{2 * (key_index - pivot_key_index)};
                                    if ((  (shift > 0) && (key_index >= shift))
                                       || ((shift < 0) && (static_cast<size_t>(key_index) < (key_scale.size() - shift))))
                                    {
                                        key_index -= shift;
                                    }
                                }
                                const auto pitch{key_scale[key_index]};
                                ne.pitch(pitch);
                            }
                        }
                        break;
                      case VoiceXml::Follower::IntervalType::Chromatic:
                        {
                          int key{ne.pitch()};

                          if (xml_form.voices()[tr].follower().interval() < 0)
                          {
                              if (abs(xml_form.voices()[tr]
                                 .follower().interval()) < key)
                              {
                                  ne.pitch(key + xml_form.voices()[tr]
                                      .follower().interval());
                                  if (RestPitch == pivot_key)
                                  {
                                      pivot_key = ne.pitch();
                                  }
                              }
                          }
                          else
                          {
                              if (key + xml_form.voices()[tr]
                                  .follower().interval() < midi::MidiPitchQty)
                              {
                                  ne.pitch(key + xml_form.voices()[tr]
                                      .follower().interval());
                                  if (RestPitch == pivot_key)
                                  {
                                      pivot_key = ne.pitch();
                                  }
                              }
                          }
                          if (xml_form.voices()[tr].follower().inversion())
                          {
                              ne.pitch(ne.pitch() - 2 * (ne.pitch() - pivot_key));
                          }
                        }
                        break;
                      default:
                          break;
                    }
                }
            }
            if (xml_form.voices()[tr].follower().retrograde())
            {
                ranges::reverse(track_note_events[tr]);
            }
            if (xml_form.voices()[tr].follower().duration_factor() != RhythmRational{1L})
            {
                ranges::for_each(track_note_events[tr],
                    [&](cgm::NoteEvent& ne) { ne.musical_rhythm(
                    ne.musical_rhythm() * xml_form.voices()[tr].follower().duration_factor() ); } );
            }

            if (xml_form.voices()[tr].follower().delay())
            {
                // Insert a rest for delay for canon effects.
                const auto start_delay = cgm::NoteEvent{RestPitch, 0, xml_form.voices()[tr].follower().delay()};
                track_note_events[tr].insert(track_note_events[tr].cbegin(), start_delay);
            }
        }
    }

    //
    // Write the form sinusoids to a gnuplot_ load file.
    //
    if (gnuplot_) [[unlikely]]
    {
        // gnuplot : plot 'concerto.form.plot' index [0123]
        // using 1:2:3:4 with yerror
        const string gnuplot_filename{xml_form.name() + ".plot"};
        error_code ec;
        if (answer_ && filesystem::exists(gnuplot_filename, ec))
        {
            cout << "Overwrite " << gnuplot_filename << "?\n";
            string answerstr{};
            cin >> answerstr;
            if (!((answerstr[0] == 'y') || (answerstr[0] == 'Y')))
            {
                exit(0);
            }
        }
        write_form_as_gnuplot_data(xml_form, gnuplot_filename);
    }

    // Need a TEMPO or rhythm track, so add 1 to the number of playing tracks
    // The last integer on the line, is the ticks per quarter;
    // feel free to make it higher.
    string textmidi_str{};
    textmidi_str.reserve(256);
    if (write_header)
    {
        ((((((textmidi_str += "FILEHEADER ") += lexical_cast<string>(xml_form.voices().size() + 1))
            += ' ') += lexical_cast<string>(TicksPerQuarter)) += ' ')
            += lexical_cast<string>(MIDI_Format::MultiTrack)) += "\n\n";
        textmidi_file << textmidi_str;
    }
    time_point<chrono::system_clock> now{chrono::system_clock::now()};
    time_t t{chrono::system_clock::to_time_t(now)};
    ostringstream timeoss;
    timeoss << put_time(localtime(&t), "%FT%T%Z");
    TicksDuration maxTime(total_duration);
    textmidi_str.clear();
    ((((((((textmidi_str += "STARTTRACK\nTEXT Generated ") += timeoss.str()) += "\nTIME_SIGNATURE 4 4 ") += lexical_cast<string>(TicksPerQuarter))
        += "\nTEMPO ") += lexical_cast<string>(TempoBeatsPerMinute))
        += "\nKEY_SIGNATURE C\nTEXT Computer-generated music from the textmidicgm software\nTRACK ")
        += xml_form.name()) += "\nLAZY\n";
    textmidi_file << textmidi_str;
    for (TicksDuration aTime(0); aTime < maxTime; aTime += TicksDuration(4 * TicksPerQuarter))
    {
        textmidi_file << "R ";
        textmidi::rational::RhythmRational musical_rhythm{1L};
        (*textmidi::rational::print_rhythm)(textmidi_file, musical_rhythm);
        textmidi_file << '\n';
    }
    textmidi_file << "END_LAZY\nticks \"End of Track\"\nEND_OF_TRACK\n";

    for (int track_index{}; auto voice : xml_form.voices())
    {
#if defined(TEXTMIDICGM_PRINT)
        cout << "STARTTRACK\nTRACK " << track_index << "\n"
            << "PROGRAM " << voice.channel()
            << ' ' << voice.program() << "\n"
            << "PAN " << voice.channel() << ' '
                      << voice.pan() << "\n"
            << "LAZY\nchan " << voice.channel() << '\n';
#endif
        textmidi_str.clear();
        (((((((((((((((textmidi_str += "STARTTRACK\nTRACK ") += lexical_cast<string>(track_index))
            += "\nPROGRAM ") += lexical_cast<string>(voice.channel())) += ' ') += lexical_cast<string>(voice.program()))
            += "\nPAN ") += lexical_cast<string>(voice.channel())) += ' ') += lexical_cast<string>(voice.pan()))
            += "\nCONTROL ") += lexical_cast<string>(voice.channel())) += " CHANNEL_VOLUME 100\n")
            += "\nLAZY\nchan ") += lexical_cast<string>(voice.channel())) += '\n';
        textmidi_file << textmidi_str;
        int lastVel{128};
        for (const auto&  eventRef : track_note_events[track_index])
        {
            // round the duration in seconds to be in pulses.
            // This program assumes that one quarter note
            // is one second and the tempo
            // is M.M. quarter = 60.
            //
            if ((RestPitch != eventRef.pitch())
               && (eventRef.vel() != lastVel))
            {
                textmidi_file << "vel " << eventRef.vel() << " ";
                lastVel = eventRef.vel();
            }
            textmidi_file << eventRef << '\n';
        }
        textmidi_file << "ticks \"End of Track\"\nEND_LAZY\nEND_OF_TRACK\n";
        ++track_index;
    }
}

const midi::NumStringMap<PermutationEnum> textmidi::cgm::arrangement_map
{
    {"undefined",           PermutationEnum::Undefined},
    {"none",                PermutationEnum::Identity},
    {"rotateright",         PermutationEnum::RotateRight},
    {"rotateleft",          PermutationEnum::RotateLeft},
    {"reverse",             PermutationEnum::Reverse},
    {"previouspermutation", PermutationEnum::LexicographicBackward},
    {"nextpermutation",     PermutationEnum::LexicographicForward},
    {"swappairs",           PermutationEnum::SwapPairs},
    {"shuffle",             PermutationEnum::Shuffle},
    {"skip",                PermutationEnum::Skip},
    {"heaps",               PermutationEnum::Heaps}
};

