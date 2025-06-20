//
// TextMIDITools Version 1.0.98
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Sun 25 Dec 2022 01:47 EST add space between TicksPerQuarter and tracks type

#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cmath>
#include <cstdint>
#include <cctype>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <limits>
#include <list>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>

#if defined(TEXTMIDICGM_PRINT)
#include <iostream>
#include <iomanip>
#endif

#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "Composer.h"
#include "GnuPlot.h"
#include "Midi.h"
#include "NoteEvent.h"
#include "RandomDouble.h"
#include "RandomInt.h"
#include "RhythmRational.h"
#include "Track.h"

using std::numeric_limits, std::list, std::vector, std::string, std::cerr,
      std::ofstream, std::int32_t, std::max, std::min, std::cout, std::toupper;
using std::ranges::copy, std::ranges::find, std::ranges::for_each, std::cmp_greater_equal;
using boost::lexical_cast;
using midi::SecondsPerMinuteI;
using textmidi::rational::RhythmRational;
using arrangements::PermutationEnum;

namespace
{
    // might become settable at a later time.
    constexpr int32_t RestPitchIndex{numeric_limits<int32_t>().max()};

    using KeyScaleSeq = std::vector<int32_t>;
}

//
// Convert a double duration in seconds to a musical ratio.
RhythmRational textmidi::cgm::Composer::duration_to_rhythm(double duration, const MusicTime& music_time)
    const noexcept
{
    //  beat    quarter   whole   minute
    // ------ * ------- * ----- * -------
    // minute    whole    beat    seconds

    //  beat    whole   minute
    // ------ * ----- * -------
    // minute   beat    seconds
    RhythmRational wholes_per_second{RhythmRational{music_time.beat_tempo_}
      * music_time.beat_ / RhythmRational{midi::SecondsPerMinuteI} };
    // wholes_per_second.reduce();
    //  Ticks    Quarters    whole
    // ------- * -------- * -------
    // Quarter     Whole    second
    const RhythmRational ticks_per_second{QuartersPerWholeRat * wholes_per_second
        * RhythmRational{music_time.ticks_per_quarter_}};
    // Turn the rhythm (duration) into a ratio and multiply
    // both the numerator and the denominator by ticks_per_second.
    // This gives us the actual musical rhythm value.
    const double ticks_per_second_dbl{
          static_cast<double>(ticks_per_second.numerator())
        / static_cast<double>(ticks_per_second.denominator())};
    const int64_t ticks_per_second_int
        {static_cast<int64_t>(round(ticks_per_second_dbl))};
    RhythmRational duration_rat{};
    duration_rat.from_double(duration, ticks_per_second_int);
    RhythmRational beat_tempo_rat{};
    RhythmRational ticks_per_beat{QuartersPerWholeRat * music_time.beat_
        * RhythmRational{music_time.ticks_per_quarter_}};
    beat_tempo_rat.from_double(music_time.beat_tempo_, ticks_per_beat.numerator()
        * ticks_per_beat.denominator());
    auto rhythm{duration_rat * wholes_per_second};
    return rhythm;
}

//
// Coerce a duration to be in multiples of the pulse/second value.
RhythmRational textmidi::cgm::Composer::snap_to_pulse(RhythmRational rhythm,
        double pulse_per_second, const MusicTime& music_time) const noexcept
{
    RhythmRational wholes_per_second{
      RhythmRational{music_time.beat_tempo_}
      * WholesPerBeat / RhythmRational{SecondsPerMinuteI} };
    // wholes_per_second.reduce();
    const RhythmRational TicksPerSecond{QuartersPerWholeRat * wholes_per_second
        * RhythmRational{music_time.ticks_per_quarter_}};
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
        const auto rem{pulse_per_rhythm.numerator() % pulse_per_rhythm.denominator()};
        // snap the rhythm (duration) to pulses.
        pulse_per_rhythm = RhythmRational{pulse_per_rhythm.numerator() - rem,
            pulse_per_rhythm.denominator()};
        // pulse_per_rhythm.reduce();
        if (!pulse_per_rhythm)
        {
            pulse_per_rhythm = RhythmRational{1L};  // i.e., one pulse minimum.
        }
    }
    rhythm = pulse_per_rhythm * whole_per_pulse;
    return rhythm;
}

void textmidi::cgm::Composer
    ::build_composition_priority_graph(const MusicalForm& xml_form,
    vector<list<int32_t>>& leaders_topo_sort) noexcept
{
    using std::ranges::none_of;
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
    // 1 is follower of 2
    // 2 is follower of 0
    // 3 is follower of 0
    // |  F  F  F  F  |
    // |  F  F  T  F  |
    // |  T  F  F  F  |
    // |  T  F  F  F  |
    //
    // 0 is follower of 1
    // 1 is follower of 2
    // 2 is follower of 3
    // |  F  T  F  F  |
    // |  F  F  T  F  |
    // |  F  F  F  T  |
    // |  F  F  F  F  |
    // If there is no true in a row, then the voice is not a follower.
    // Use the graph to follow the leaders to the leader
    //     with no trues in its row.
    // Then that is the real leader of a group.
    // Then generate the voices backwards from the leader
    //     to all followers in its column,
    // and follow the leaders to the next followers.

    // Make a square matrix of false. followers_graph[followers][leaders]
    vector<vector<bool>> followers_graph(xml_form.voices().size(),
            vector<bool>(xml_form.voices().size(), false));

    for (int32_t follower{};
         std::cmp_less(follower, xml_form.voices().size());
         ++follower)
    {
        if (xml_form.voices()[follower].follower().follow()
            &&  std::cmp_less(xml_form.voices()[follower].follower().leader(),
                xml_form.voices().size()))
        {
            followers_graph[follower][xml_form.voices()[follower].follower().leader()] = true;
            if (xml_form.voices()[follower].follower().leader() == follower)
            {
                const string errstr{((((string{__FILE__ } += ':')
                    += BOOST_PP_STRINGIZE(__LINE__)) += " voice ")
                    += lexical_cast<string>(follower)) += " is a self-follower!\n"};
                cerr << errstr;
            }
        }
    }
#if defined(TEXTMIDICGM_PRINT)
    for (auto& row : followers_graph)
    {
        copy(row, ostream_iterator<bool>(cout, " "));
        cout << '\n';
    }
#endif
    // This is not wrong to resize. 2023-02-20
    leaders_topo_sort.resize(followers_graph.size());

    // Check each voice for being only a leader and not a follower.
    for (int32_t follower_index{};
         std::cmp_less(follower_index, followers_graph[0].size());
         ++follower_index)
    {
        const auto& vb{followers_graph[follower_index]};
        if (none_of(vb, [&](bool b) { return b; } ))
        {
            // If not a follower then save it.
            leaders_topo_sort[0].push_back(follower_index);
        }
    }
#if defined(TEXTMIDICGM_PRINT)
    cout << "List of non-followers: ";
    copy(leaders_topo_sort[0], ostream_iterator<int32_t>(cout, " "));
    cout << '\n';
#endif
    for (int32_t g{1}; std::cmp_less(g, followers_graph.size()); ++g)
    {
        for (int32_t follower_index{};
             std::cmp_less(follower_index, followers_graph.size());
             ++follower_index)
        {
            for (int32_t leader_index{};
                 std::cmp_less(leader_index, followers_graph[0].size());
                 ++leader_index)
            {
#if defined(TEXTMIDICGM_PRINT)
                cout << "** " << leader_index << ' ' << follower_index << ' '
                     << boolalpha
                     << followers_graph[leader_index][follower_index] << '\n';
#endif
                // If this is a follower...
                if (followers_graph[follower_index][leader_index]
                    && (find(leaders_topo_sort[g - 1], leader_index)
                        != leaders_topo_sort[g - 1].end()))
                    {
                        leaders_topo_sort[g].push_back(follower_index);
                    }
            }
#if defined(TEXTMIDICGM_PRINT)
            cout << '\n';
#endif
        }
    }
#if defined(TEXTMIDICGM_PRINT)
    cout << "order of composing:\n";
    for_each(leaders_topo_sort, [&](auto lts) {
        copy(lts, ostream_iterator<int32_t>(cout, " ")); cout << '\n'; } );
#endif
}

void textmidi::cgm::Composer::
    build_track_scramble_sequences(
    vector<vector<int32_t>>& track_scramble_sequences,
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
    for_each(track_scramble_sequences, [&](auto tss)
        { copy(tss, ostream_iterator<int32_t>(cout, " ")); cout << '\n'; } );
#endif
#undef TEXTMIDICGM_PRINT
}

//
// The compose() executive function for textmidicgm.  // It implements
// the composing engine from AlgoRhythms 3.0 for the Commodore Amiga
// with minor enhancements.
//
void textmidi::cgm::Composer::operator()(ofstream& textmidi_file,
    const MusicalForm& xml_form, bool write_header)
{
    using std::pair, std::ostringstream, std::put_time, std::ranges::reverse;
    using std::chrono::time_point;
    using midi::MaxDynamic, midi::MinDynamic, midi::MIDI_Format;

    const auto ticks_per_second_{static_cast<std::int64_t>((
        static_cast<double>(xml_form.music_time().ticks_per_quarter_
        * midi::QuartersPerWhole * xml_form.music_time().beat_.numerator())
        * xml_form.music_time().beat_tempo_)
        / static_cast<double>(SecondsPerMinute * xml_form.music_time().beat_.denominator()))};
    const auto seconds_per_whole_{static_cast<std::int64_t>((static_cast<double>(
        midi::QuartersPerWhole * xml_form.music_time().beat_.numerator())
        * xml_form.music_time().beat_tempo_)
        / static_cast<double>(SecondsPerMinute * xml_form.music_time().beat_.denominator()))};

    // If the command line did not set arrangements,
    // then set them from the XML file.
    track_scramble_.period_
        = ((PermutationEnum::Undefined == track_scramble_.scramble_)
        ? (static_cast<TicksDuration>(
        static_cast<int32_t>(round(xml_form.arrangement_definition().period()))))
        : track_scramble_.period_);

    track_scramble_.scramble_ = (track_scramble_.scramble_
        == arrangements::PermutationEnum::Undefined)
        ? xml_form.arrangement_definition().algorithm()
        : track_scramble_.scramble_;
    RandomDouble random_double{};
    vector<Track> tracks(xml_form.voices().size());
    vector<pair<int32_t, int32_t>> tessitura;

    track_scramble_.arrangements_
        = ArrangementsFactory(track_scramble_.scramble_, xml_form.voices().size());

    for_each(xml_form.voices(), [&](auto v) {
        tessitura.emplace_back( textmidi::
        pitchname_to_keynumber(v.low_pitch()).first,
        textmidi::pitchname_to_keynumber(v.high_pitch()).first); } );
    KeyScaleSeq key_scale;
    xml_form.string_scale_to_int_scale(key_scale);
    const TicksDuration time_step{xml_form.pulse() ? 1 / xml_form.pulse() : 1};

    const TicksDuration total_duration(duration_to_rhythm(xml_form.len(), music_time_));

    vector<list<int32_t>> leaders_topo_sort;
    build_composition_priority_graph(xml_form, leaders_topo_sort);

    vector<vector<int32_t>> track_scramble_sequences;
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
                while ((track.the_next_time() < total_duration)
                    && (track_note_events.size() < max_events_per_track_))
                {
                    size_t scramble_index{(track.the_next_time() / track_scramble_.period_).numerator()};
                    scramble_index
                        = (std::cmp_less(scramble_index, track_scramble_sequences.size())
                        ? scramble_index
                        : track_scramble_sequences.size() - 1);
#if defined(TEXTMIDICGM_PRINT)
                    cout << "scramble_index: " << scramble_index << '\n';
#endif
                    MusicalCharacter musical_character{};
                    xml_form.character_now(track.the_next_time(), musical_character);
                    int32_t number_of_voices
                        = static_cast<int32_t>(musical_character.texture_range) + 1;
                    number_of_voices = std::cmp_greater(number_of_voices, xml_form.voices().size())
                        ? xml_form.voices().size()
                        : number_of_voices;
                    number_of_voices = (number_of_voices < 1) ? 1 : number_of_voices;
#if defined(TEXTMIDICGM_PRINT)
                    cout << "texture_range: "
                         << musical_character.texture_range
                         << " number_of_voices: " << number_of_voices << '\n';
#endif
                    double dynamicd{(musical_character.dynamic_range * random_double())
                      - (musical_character.dynamic_range / 2.0)
                      +  musical_character.dynamic_mean};
                    int32_t dynamic{static_cast<int32_t>(round(dynamicd))};

                    dynamic = std::min(dynamic, static_cast<int32_t>(std::round(MaxDynamic)));
                    dynamic = std::max(dynamic, static_cast<int32_t>(std::round(MinDynamic)));

                    double rhythmd {musical_character.duration(random_double())};
                    auto rhythm{duration_to_rhythm(rhythmd, xml_form.music_time())};

                    // Now apply the pulse.
                    // pulse is in pulses per second.
                    if (xml_form.pulse() != 0)
                    {
                        rhythm = snap_to_pulse(rhythm, xml_form.pulse(), xml_form.music_time());
                    }
                    const RhythmRational wholes_per_second{
                        RhythmRational{xml_form.music_time().beat_tempo_} * WholesPerBeat
                            / RhythmRational{SecondsPerMinuteI}};
                    track.the_last_time(track.the_next_time());
                    track.the_next_time(track.the_next_time() + rhythm);

                    int32_t pitch_index{};

                    MelodyProbabilities::MelodyDirection
                        direction{xml_form.melody_probabilities()(random_double())};
                    bool crisp_walking{};
                    if (xml_form.voices()[tr].walking() > 0.0)
                    {
                        crisp_walking = (xml_form.voices()[tr].walking() >= random_double()) ? true : false;
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
                            pitch_index = track.last_pitch_index();
                            switch (direction)
                            {
                                case MelodyProbabilities::MelodyDirection::Up:
                                    if (!xml_form.scale().empty())
                                    {
                                        if (std::cmp_less(pitch_index, xml_form.scale().size() - 1))
                                        {
                                            ++pitch_index;
                                        }
                                        else
                                        {
                                            pitch_index = xml_form.scale().size() - 1;
                                        }
                                    }
                                    else
                                    {
                                        pitch_index = 0;
                                        cerr << "You have a zero-long scale!  Fill in a scale!\n";
                                    }
                                    break;
                                case MelodyProbabilities::MelodyDirection::Same:
                                    break;
                                case MelodyProbabilities::MelodyDirection::Down:
                                    if (pitch_index > 0)
                                    {
                                        --pitch_index;
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
                        // Play random ("jumpy") keys.
                        if (direction != MelodyProbabilities::MelodyDirection::Rest)
                        {
                            pitch_index = musical_character.pitch_index(random_double());

                            // coerce to be on scale
                            if (!xml_form.scale().empty())
                            {
                                if (!std::cmp_less(pitch_index, xml_form.scale().size()))
                                {
                                    // This is probably why it keeps banging
                                    // on the top of the scale.
                                    // Alternatively it could be changed to a rest.
                                    pitch_index = xml_form.scale().size() - 1;
                                }
                                pitch_index = ((pitch_index < 0) ? 0 : pitch_index);
                                // Same pitch index as the last pitch index
                            }
                            else
                            {
                                cerr << "You have a zero-long scale!  Fill in a scale!\n";
                            }
                        }
                        else
                        {
                            pitch_index = RestPitchIndex;
                        }
                    }
                    track.last_pitch_index((pitch_index != RestPitchIndex)
                        ? pitch_index
                        : track.last_pitch_index());

                    // Use number_of_voices to subset
                    // track_scramble_sequences[scramble_index].
                    // If you can find tr in the sub sequence, then play.

                    auto trit{find(
                        track_scramble_sequences[scramble_index].begin(),
                        track_scramble_sequences[scramble_index].begin() + number_of_voices, tr)};
                    if ((pitch_index != RestPitchIndex)
                        && (trit
                        != (track_scramble_sequences[scramble_index].begin() + number_of_voices)))
                    {
                        auto key_number(key_scale[pitch_index]);
                        key_number = max(key_number, tessitura[tr].first);
                        key_number = min(key_number, tessitura[tr].second);
                        int32_t key_index{0U};
                        {
                            KeyScaleSeq::const_iterator key_iter{
                                find(key_scale, key_number)};
                            if (key_iter != key_scale.cend())
                            {
                                key_index = std::distance(key_scale.cbegin(), key_iter);
                            }
                        }
                        if (pitch_index != RestPitchIndex)
                        {
                            track.last_pitch_index(key_index);
                        }
                        NoteEvent temp_note_event{key_number, dynamic, rhythm};
                        track_note_events[tr].push_back(temp_note_event);
                    } else {
                        NoteEvent temp_note_event{RestPitch, 0, rhythm};
                        track_note_events[tr].push_back(temp_note_event);
                    }
                }
            }
            else [[unlikely]] // it is a follower
            {
                using std::out_of_range;
                try
                {
                    track_note_events[tr] = track_note_events.at(
                        xml_form.voices()[tr].follower().leader());
                }
                catch (out_of_range& exc)
                {
                    cerr << __FILE__ << ':' << BOOST_PP_STRINGIZE(__LINE__)
                         << ':'
                         << " follower leader must be in (0..# of tracks-1) "
                         << exc.what() << '\n';
                    exit(0);
                }
                int32_t pivot_key{RestPitch};
                int64_t pivot_key_index{static_cast<int32_t>(key_scale.size())};
                for (auto& ne : track_note_events[tr])
                {
                    switch (xml_form.voices()[tr].follower().interval_type())
                    {
                      [[likely]] case VoiceXml::Follower::IntervalType::Scalar:
                        {
                            KeyScaleSeq::const_iterator key_iter{find(key_scale, ne.pitch())};
                            if (key_iter != key_scale.cend())
                            {
                                int64_t key_index{std::distance(key_scale.cbegin(), key_iter)};
                                if (xml_form.voices()[tr].follower().interval() < 0)
                                {
                                    // Check there's room down the scale
                                    if (abs(xml_form.voices()[tr].follower().interval()) < key_index)
                                    {
                                        key_index += xml_form.voices()[tr].follower().interval();
                                    }
                                    if (std::cmp_equal(key_scale.size(), pivot_key_index))
                                    {
                                        pivot_key_index = key_index;
                                    }
                                }
                                else // follower interval is 0 or positive
                                {
                                    // Check there's room up the scale
                                    if (std::cmp_less(xml_form.voices()[tr].follower().interval() + key_index,
                                        key_scale.size()))
                                    {
                                        key_index += xml_form.voices()[tr].follower().interval();
                                        if (std::cmp_equal(key_scale.size(), pivot_key_index))
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
                                    const int64_t shift{2L * (key_index - pivot_key_index)};
                                    const int64_t temp_key_index = key_index - shift;
                                    key_index = (temp_key_index < 0L) ? 0L : temp_key_index;
                                    if (!key_scale.empty())
                                    {
                                        key_index = cmp_greater_equal(temp_key_index, key_scale.size())
                                            ? key_scale.size() - 1
                                            : temp_key_index;
                                    }
                                }
                                key_index = std::max(key_index, 0L);
                                const auto pitch{key_scale[key_index]};
                                ne.pitch(pitch);
                            }
                        }
                        break;
                      case VoiceXml::Follower::IntervalType::Chromatic:
                        {
                          int32_t key{ne.pitch()};

                          if (xml_form.voices()[tr].follower().interval() < 0)
                          {
                              if (abs(xml_form.voices()[tr].follower().interval()) < key)
                              {
                                  ne.pitch(key + xml_form.voices()[tr].follower().interval());
                                  if (RestPitch == pivot_key)
                                  {
                                      pivot_key = ne.pitch();
                                  }
                              }
                          }
                          else
                          {
                              if (key + xml_form.voices()[tr].follower().interval() < midi::MidiPitchQty)
                              {
                                  ne.pitch(key + xml_form.voices()[tr].follower().interval());
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
                reverse(track_note_events[tr]);
            }
            if (xml_form.voices()[tr].follower().duration_factor()
                    != RhythmRational{1L})
            {
                for_each(track_note_events[tr],
                    [&](cgm::NoteEvent& ne) { ne.rhythm(ne.rhythm()
                    * xml_form.voices()[tr].follower().duration_factor() ); });
            }

            if (xml_form.voices()[tr].follower().delay())
            {
                // Insert a rest for delay for canon effects.
                const auto start_delay = cgm::NoteEvent{RestPitch, 0,
                    xml_form.voices()[tr].follower().delay()};
                track_note_events[tr].insert(track_note_events[tr].cbegin(), start_delay);
            }
        }
    }

    //
    // Write the form sinusoids to a gnuplot_ load file.
    //
    if (gnuplot_) [[unlikely]]
    {
        using std::cin;
        using std::error_code;
        // gnuplot : plot 'concerto.form.plot' index [0123]
        // using 1:2:3:4 with yerror
        const string gnuplot_filename{xml_form.name() + ".plot"};
        error_code ec;
        if (answer_ && std::filesystem::exists(gnuplot_filename, ec))
        {
            cout << "Overwrite " << gnuplot_filename << "?\n";
            string answerstr{};
            cin >> answerstr;
            if (!(toupper(answerstr[0]) == 'Y'))
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
        ((((((textmidi_str += "FILEHEADER ")
            += lexical_cast<string>(xml_form.voices().size() + 1))
            += ' ') += lexical_cast<string>(music_time_.ticks_per_quarter_)) += ' ')
            += lexical_cast<string>(MIDI_Format::MultiTrack)) += "\n\n";
        textmidi_file << textmidi_str;
    }
    time_point<std::chrono::system_clock> now{std::chrono::system_clock::now()};
    time_t t{std::chrono::system_clock::to_time_t(now)};
    ostringstream timeoss;
    struct ::tm tm_temp{};
    timeoss << put_time(localtime_r(&t, &tm_temp), "%FT%T%Z");
    textmidi_str.clear();
    const auto quarter_tempo{music_time_.beat_tempo_ * static_cast<double>(music_time_.beat_ * QuartersPerWholeRat)};
    (((((((((((textmidi_str += "STARTTRACK\nTEXT Generated ") += timeoss.str())
        += "\nTIME_SIGNATURE ") += lexical_cast<string>(music_time_.meter_.numerator()))
        += ' ') += lexical_cast<string>(music_time_.meter_.denominator()) += ' ')
        += lexical_cast<string>(music_time_.ticks_per_quarter_))
        += "\nTEMPO ") += lexical_cast<string>(quarter_tempo))
        += "\nKEY_SIGNATURE C\nTEXT Computer-generated music "
           "from the textmidicgm software\nTRACK ")
        += xml_form.name()) += "\nLAZY\n";
    textmidi_file << textmidi_str;
    for (TicksDuration aTime{0L}; aTime < total_duration;
         aTime += TicksDuration{4L * music_time_.ticks_per_quarter_})
    {
        textmidi_file << "R ";
        textmidi::rational::RhythmRational rhythm{music_time_.meter_};
        (*textmidi::rational::print_rhythm)(textmidi_file, rhythm);
        textmidi_file << '\n';
    }
    textmidi_file << "END_LAZY\nticks 'End of Track'\nEND_OF_TRACK\n\n";

    for (int32_t track_index{}; auto voice : xml_form.voices())
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
        (((((((((((((((textmidi_str += "STARTTRACK\nTRACK ")
            += lexical_cast<string>(track_index))
            += "\nPROGRAM ") += lexical_cast<string>(voice.channel())) += ' ')
            += lexical_cast<string>(voice.program()))
            += "\nPAN ") += lexical_cast<string>(voice.channel())) += ' ')
            += lexical_cast<string>(voice.pan()))
            += "\nCONTROL ") += lexical_cast<string>(voice.channel()))
            += " CHANNEL_VOLUME 100\n")
            += "\nLAZY\nchan ") += lexical_cast<string>(voice.channel()))
            += '\n';
        textmidi_file << textmidi_str;
        int32_t lastVel{128};
        for_each(track_note_events[track_index], [&](const auto& eventRef)
        {
            // round the duration in seconds to be in pulses.
            // This program assumes that one quarter note
            // is one second and the tempo
            // is M.M. quarter = 60.
            //
            if ((RestPitch != eventRef.pitch()) && (eventRef.vel() != lastVel))
            {
                textmidi_file << "vel " << eventRef.vel() << " ";
                lastVel = eventRef.vel();
            }
            textmidi_file << eventRef << '\n';
        });
        textmidi_file << "END_LAZY\nticks 'End of Track'\nEND_OF_TRACK\n\n";
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

