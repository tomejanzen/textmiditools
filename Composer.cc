//
// TextMIDITools Version 1.0.10
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cstdlib>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <list>

#include <boost/preprocessor/stringize.hpp>

#include "Midi.h"
#include "Composer.h"
#include "NoteEvent.h"
#include "RandomDouble.h"
#include "Track.h"
#include "GnuPlot.h"

using namespace std;
using namespace boost;
using namespace cgm;

namespace
{
    // might become settable at a later time.
    constexpr int SecondsPerMinute{60};
    constexpr int TempoBeatsPerMinute{60};
    constexpr int RestPitchIndex{numeric_limits<int>().max()};

    //
    // Convert a double duration in seconds to a musical ratio.
    MusicalRhythm duration_to_rhythm(double duration)
    {
        //  beat    quarter   whole   minute
        // ------ * ------- * ----- * -------
        // minute    whole    beat    seconds

        //  beat    whole   minute
        // ------ * ----- * -------
        // minute   beat    seconds
        const MusicalRhythm wholes_per_second{
          TempoBeatsPerMinute * WholesPerBeat / SecondsPerMinute };
        //  Ticks    Quarters    whole
        // ------- * -------- * -------
        // Quarter     Whole    second
        const MusicalRhythm TicksPerSecond{
             TicksPerQuarter * QuartersPerWhole * wholes_per_second};
        // Turn the rhythm (duration) into a ratio and multiply
        // both the numerator and the denominator by TicksPerSecond.
        // This gives us the actual musical rhythm value.
        const double TicksPerSecondDouble{
              static_cast<double>(TicksPerSecond.numerator())
            / static_cast<double>(TicksPerSecond.denominator())};
        const int64_t TicksPerSecondInt64
            {static_cast<int64_t>(round(TicksPerSecondDouble))};
        MusicalRhythm rhythm
            {static_cast<int64_t>(duration * TicksPerSecondDouble),
            TicksPerSecondInt64};
        rhythm *= wholes_per_second;
        return abs(rhythm);
    }

    //
    // Coerce a duration to be in multiples of the pulse/second value.
    MusicalRhythm snap_to_pulse(MusicalRhythm rhythm, double pulse_per_second)
    {
        const MusicalRhythm wholes_per_second{
          TempoBeatsPerMinute * WholesPerBeat / SecondsPerMinute };
        const MusicalRhythm TicksPerSecond{
             TicksPerQuarter * QuartersPerWhole * wholes_per_second};
        const double TicksPerSecondDouble{
              static_cast<double>(TicksPerSecond.numerator())
            / static_cast<double>(TicksPerSecond.denominator())};
        const int64_t TicksPerSecondInt64
            {static_cast<int64_t>(round(TicksPerSecondDouble))};
        MusicalRhythm whole_per_pulse(
            wholes_per_second *
            MusicalRhythm(TicksPerSecondInt64,
            static_cast<int64_t>
                (round(pulse_per_second * TicksPerSecondDouble))));
        // Get rid of the remainder.
        // but first round off by adding a half-pulse.
        MusicalRhythm
            pulse_per_rhythm{(rhythm + whole_per_pulse / 2) / whole_per_pulse};
        if (pulse_per_rhythm.denominator() != 1)
        {
            const auto rem{pulse_per_rhythm.numerator()
                         % pulse_per_rhythm.denominator()};
            // snap the rhythm (duration) to pulses.
            pulse_per_rhythm.assign(pulse_per_rhythm.numerator() - rem,
                pulse_per_rhythm.denominator());
            if (!pulse_per_rhythm)
            {
                pulse_per_rhythm = 1; // i.e., one pulse minimum.
            }
        }
        rhythm = pulse_per_rhythm * whole_per_pulse;
        return rhythm;
    }

}

//
// The compose() executive function for textmidicgm.
// It implements the composing engine from AlgoRhythms 3.0 for the Commodore Amiga
// with minor enhancements.
//
void cgm::compose(const MusicalForm& xml_form, ofstream& textmidi_file, bool gnuplot, bool answer)
{
    std::vector<std::vector<cgm::NoteEvent>> track_note_events;
    RandomDouble random_double{};
    vector<Track> tracks(xml_form.voices().size()/*, Track{ xml_form.scale().size() / 2 }*/);
    vector<pair<int, int>> tessitura;
    for (auto v : xml_form.voices())
    {
        tessitura.push_back(make_pair
            (textmidi::pitchname_to_keynumber(v.low_pitch()).first,
            textmidi::pitchname_to_keynumber(v.high_pitch()).first));
    }
    vector<int> key_scale;
    xml_form.string_scale_to_int_scale(key_scale);
    const TicksDuration time_step{xml_form.pulse()
        ? (static_cast<int64_t>(static_cast<double>
          (TicksPerQuarter) / xml_form.pulse())) : 1};
    const TicksDuration totalDuration(static_cast<int64_t>(floor(xml_form.len()
          * double(TicksPerQuarter))));

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
    vector<vector<bool>> followers_graph(tracks.size(), vector<bool>(tracks.size(), false));

    for (int tr{}; tr < tracks.size(); ++tr)
    {
        if (xml_form.voices()[tr].follower().follow_) [[unlikely]]
        {
            if (xml_form.voices()[tr].follower().leader_ < followers_graph.size())
            {
                followers_graph[xml_form.voices()[tr].follower().leader_][tr] = true;
                if (xml_form.voices()[tr].follower().leader_ == tr)
                {
                    cerr << __FILE__ << ':' << BOOST_PP_STRINGIZE(__LINE__) << " voice "
                         << tr << " is a self-follower!\n";
                }
            }
        }
    }
#undef PRINT
#if defined(PRINT)
    for (auto& row : followers_graph)
    {
        copy(row.begin(), row.end(), ostream_iterator<bool>(cout, " "));
        cout << '\n';
    }
#endif
    vector<list<int>> leaders_topo_sort(followers_graph.size());

    for (int follower_index{}; follower_index < followers_graph.size(); ++follower_index)
    {
        bool leader_only{true};
        for (int leader_index{}; leader_index < followers_graph[0].size(); ++leader_index)
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
#if defined(PRINT)
    cout << "List of non-followers: ";
    copy(leaders_topo_sort[0].begin(), leaders_topo_sort[0].end(), ostream_iterator<int>(cout, " "));
    cout << '\n';
#endif
    for (auto g{1}; g < followers_graph.size(); ++g)
    {
        for (int follower_index{}; follower_index < followers_graph[0].size(); ++follower_index)
        {
            for (int leader_index{}; leader_index < followers_graph.size(); ++leader_index)
            {
#if defined(PRINT)
                cout << "** " << leader_index << ' ' << follower_index << ' ' << followers_graph[leader_index][follower_index] << '\n';
#endif
                if (followers_graph[leader_index][follower_index]) // if this is a follower
                {
                    auto it{find(leaders_topo_sort[g - 1].begin(),
                                 leaders_topo_sort[g - 1].end(), leader_index)};
                    if (it != leaders_topo_sort[g - 1].end())
                    {
                        leaders_topo_sort[g].push_back(follower_index);
                    }
                }
            }
#if defined(PRINT)
            cout << '\n';
#endif
        }
    }
#if defined(PRINT)
    cout << "order of composing:\n";
    for (auto& lts : leaders_topo_sort)
    {
        copy(lts.begin(), lts.end(), ostream_iterator<int>(cout, " "));
        cout << '\n';
    }
#endif
    track_note_events.resize(tracks.size());
    // loop over the vector of lists of voice indices:
    for (auto& lts : leaders_topo_sort)
    {
        // loop over the list of voice indices
        for (int i{}; auto tr : lts)
        {
            auto& track{tracks[tr]};
            if (!xml_form.voices()[tr].follower().follow_) [[likely]]
            {
                while (track.the_next_time() < totalDuration)
                {
                    MusicalCharacter musical_character{};
                    xml_form.character_now(track.the_next_time(), musical_character);


                    double dynamicd{(musical_character.dynamic_range
                                   * random_double())
                      - (musical_character.dynamic_range / 2.0)
                      +  musical_character.dynamic_mean};
                    int dynamic{static_cast<int>(round(dynamicd))};

                    dynamic = min(dynamic, 127);
                    dynamic = max(dynamic, 0);

                    double rhythmd
                        {musical_character.duration(random_double())};
                    auto rhythm{duration_to_rhythm(rhythmd)};

                    // Now apply the pulse.
                    // pulse is in pulses per second.
                    if (xml_form.pulse() != 0)
                    {
                        rhythm = snap_to_pulse(rhythm, xml_form.pulse());
                    }
                    const double rhythm_double{
                          static_cast<double>(rhythm.numerator())
                        / static_cast<double>(rhythm.denominator())};
                    const MusicalRhythm wholes_per_second{
                        TempoBeatsPerMinute * WholesPerBeat
                            / SecondsPerMinute};
                    const MusicalRhythm TicksPerSecond{
                        TicksPerQuarter * QuartersPerWhole
                                        * wholes_per_second};
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

                    int pitch_index{0};

                    MelodyProbabilities::MelodyDirection
                        direction{xml_form.melody_probabilities()
                        (random_double())};
                    if (xml_form.voices()[tr].walking())
                    {
                        switch (direction)
                        {
                            case MelodyProbabilities::MelodyDirection::Up:
                                if (track.last_pitch_index()
                                    < (xml_form.scale().size() - 1))
                                {
                                    pitch_index
                                        = track.last_pitch_index() + 1;
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
                    else
                    {
                        if (direction
                            != MelodyProbabilities::MelodyDirection::Rest)
                        {
                            pitch_index
                                = musical_character.pitch_index
                                (random_double());
                        }
                        else
                        {
                            pitch_index = RestPitchIndex;
                        }
                    }

                    // coerce to be on scale
                    if ((pitch_index != RestPitchIndex)
                        && (pitch_index
                            >= static_cast<int>(xml_form.scale().size())))
                    {
                        // This is probably why it keeps banging
                        // on the top of the scale.
                        // Alternatively it could be changed to a rest.
                        pitch_index = xml_form.scale().size() - 1;
                    }
                    // Same pitch index as the last pitch index
                    if (pitch_index != RestPitchIndex)
                    {
                        track.last_pitch_index(pitch_index);
                    }

                    if ((pitch_index != RestPitchIndex)
                        && (tr <= musical_character.texture_range))
                    {
                        auto key_number(key_scale[pitch_index]);
                        key_number = max(key_number, tessitura[tr].first);
                        key_number = min(key_number, tessitura[tr].second);
                        int key_index{0U};
                        {
                            auto key_iter{find(key_scale.begin(),
                                key_scale.end(), key_number)};
                            if (key_iter != key_scale.end())
                            {
                                key_index = std::distance(key_scale.begin(),
                                            key_iter);
                            }
                        }
                        if (pitch_index != RestPitchIndex)
                        {
                            track.last_pitch_index(key_index);
                        }
                        NoteEvent temp_note_event{key_number,
                            dynamic, rhythm_double, rhythm};
                        track_note_events[i].push_back(temp_note_event);
                    } else {
                        NoteEvent temp_note_event{RestPitch, 0,
                            rhythm_double, rhythm};
                        track_note_events[i].push_back(temp_note_event);
                    }
                }
            }
            else [[unlikely]] // it is a follower
            {
                track_note_events[tr] = track_note_events[xml_form.voices()[tr].follower().leader_];
                for (auto& ne : track_note_events[tr])
                {
                    switch (xml_form.voices()[tr]
                            .follower().interval_type_)
                    {
                      [[likely]] case VoiceXml::Follower::IntervalType::Scalar:
                        {
                          int key_index{0U};
                          {
                              auto key_iter{find(key_scale.begin(),
                                  key_scale.end(), ne.pitch())};
                              if (key_iter != key_scale.end())
                              {
                                  key_index
                                      = std::distance(key_scale.begin(),
                                        key_iter);
                                  if (xml_form.voices()[tr]
                                      .follower().interval_ < 0)
                                  {
                                      if (abs(xml_form.voices()[tr]
                                          .follower().interval_)
                                              < key_index)
                                      {
                                         ne.pitch(key_scale[
                                             xml_form.voices()[tr]
                                             .follower().interval_
                                             + key_index]);
                                      }
                                  }
                                  else
                                  {
                                      if ((xml_form.voices()[tr]
                                          .follower().interval_
                                          + key_index)
                                              < key_scale.size())
                                      {
                                         auto pitch{
                                             key_scale[key_index
                                             + xml_form.voices()[tr]
                                             .follower().interval_]};
                                         ne.pitch(pitch);
                                      }

                                  }
                              }
                          }
                        }
                        break;
                      case VoiceXml::Follower::IntervalType::Chromatic:
                        {
                          if (xml_form.voices()[tr]
                              .follower().interval_ < 0)
                          {
                              if (abs(xml_form.voices()[tr]
                                 .follower().interval_) < ne.pitch())
                              {
                                  ne.pitch(ne.pitch()
                                      + xml_form.voices()[tr]
                                      .follower().interval_);
                              }
                          }
                          else
                          {
                              if (ne.pitch() + xml_form.voices()[tr]
                                  .follower().interval_ < textmidi::MidiPitchQty)
                              {
                                  ne.pitch(ne.pitch()
                                      + xml_form.voices()[tr]
                                      .follower().interval_);
                              }
                          }
                        }
                        break;
                      default:
                          break;
                    }
                }
            }
            ++i;
        }
    }

    //
    // Write the form sinusoids to a gnuplot load file.
    //
    if (gnuplot) [[unlikely]]
    {
        // gnuplot : plot 'concerto.form.plot' index [0123]
        // using 1:2:3:4 with yerror
        const string gnuplot_filename{xml_form.name() + ".plot"};
        error_code ec;
        if (answer && filesystem::exists(gnuplot_filename, ec))
        {
            cout << "Overwrite " << gnuplot_filename << "?" << '\n';
            string answer{};
            cin >> answer;
            if (!((answer[0] == 'y') || (answer[0] == 'Y')))
            {
                exit(0);
            }
        }
        write_form_as_gnuplot_data(xml_form, gnuplot_filename);
    }

    // Need a TEMPO or rhythm track, so add 1 to the number of playing tracks
    // The last integer on the line, is the ticks per quarter;
    // feel free to make it higher.
    textmidi_file << "FILEHEADER " << (xml_form.voices().size() + 1)
                  << " " << TicksPerQuarter << "\n\n";

    TicksDuration maxTime(totalDuration);
    textmidi_file << "STARTTRACK\n"
                  << "TIME_SIGNATURE 4 4 " << TicksPerQuarter
                  << '\n' // could be 960
                  << "TEMPO " << TempoBeatsPerMinute << "\n"
                  << "KEY_SIGNATURE C\n"
                  << "TEXT CREATED BY TEXTMIDICGM BY THOMAS JANZEN\n"
                  << "TRACK " << xml_form.name() << '\n'
                  << "LAZY\n";
    for (TicksDuration aTime(0); aTime < maxTime; aTime += TicksDuration(4 * TicksPerQuarter))
    {
        textmidi_file << "R 1\n";
    }
    textmidi_file << "END_LAZY\n";
    textmidi_file << "ticks \"End of Track\"\n";
    textmidi_file << "END_OF_TRACK\n";

    for (int track_index{}; track_index < tracks.size(); ++track_index)
    {
        textmidi_file << "STARTTRACK\n"
                      << "TRACK " << track_index << "\n"
            << "PROGRAM " << xml_form.voices()[track_index].channel()
            << ' ' << xml_form.voices()[track_index].program() << "\n"
            << "PAN " << xml_form.voices()[track_index].channel() << ' '
                      << xml_form.voices()[track_index].pan() << "\n"
            << "LAZY\n"
            << "chan " << xml_form.voices()[track_index].channel() << '\n';
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
        textmidi_file << "ticks \"End of Track\"\n";
        textmidi_file << "END_LAZY\nEND_OF_TRACK\n";
    }
}

