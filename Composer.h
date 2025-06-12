//
// TextMIDITools Version 1.0.98
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(COMPOSER_H)
#    define  COMPOSER_H

#include <fstream>
#include <list>
#include <memory>
#include <random>
#include <vector>

#include "Arrangements.h"
#include "Midi.h"
#include "MidiMaps.h"
#include "MusicalForm.h"
#include "RhythmRational.h"

namespace textmidi
{
    namespace cgm
    {
        class Composer
        {
          public:
            struct TrackScramble
            {
                TrackScramble() = default;
                explicit constexpr TrackScramble(
                    arrangements::PermutationEnum scramble,
                    TicksDuration period) noexcept
                  : scramble_(scramble),
                    period_(period),
                    arrangements_{}
                {}

                arrangements::PermutationEnum scramble_
                    {arrangements::PermutationEnum::Identity};
                TicksDuration     period_{120UL};
                std::shared_ptr<arrangements::Arrangements> arrangements_{};
            };

            Composer(bool gnuplot, bool answer,
                arrangements::PermutationEnum track_scramble,
                TicksDuration track_scramble_period, const MusicTime& music_time,
                size_t max_events_per_track = 100000) noexcept
              : gnuplot_(gnuplot),
                answer_(answer),
                track_scramble_(track_scramble, track_scramble_period),
                max_events_per_track_{max_events_per_track},
                random_dev_(),
                generator_{random_dev_()},
                music_time_{music_time}
            {
            }

            // Rule of 5: If you delete or declare any of assign, copy, move,
            // move copy, d-tor, then delete or declare all of them.
            Composer(const Composer& ) = delete;
            Composer(Composer&& ) = delete;
            Composer& operator=(const Composer& ) = delete;
            Composer& operator=(Composer&& ) = delete;

            void operator()(std::ofstream& textmidi_file,
                const MusicalForm& xml_form, bool write_header = true);

          private:
            rational::RhythmRational
                duration_to_rhythm(double duration, const MusicTime& music_time) const noexcept;
            rational::RhythmRational snap_to_pulse(
                rational::RhythmRational rhythm, double pulse_per_second, const MusicTime& music_time)
                const noexcept;
            void build_track_scramble_sequences(
                std::vector<std::vector<std::int32_t>>&
                track_scramble_sequences,
                TicksDuration total_duration) noexcept;
            void build_composition_priority_graph(const MusicalForm& xml_form,
                std::vector<std::list<std::int32_t>>&
                leaders_topo_sort) noexcept;
            bool gnuplot_;
            bool answer_;
            TrackScramble track_scramble_;
            size_t max_events_per_track_;
            std::random_device random_dev_;
            std::mt19937 generator_;
            MusicTime music_time_;
        };

        extern const midi::NumStringMap<arrangements::PermutationEnum>
            arrangement_map;
    } // namespace cgm
} // namespace textmidi

#endif

