//
// TextMIDITools Version 1.0.87
//
// textmidicgm 1.0
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(COMPOSER_H)
#    define  COMPOSER_H

#include <vector>
#include <list>
#include <fstream>
#include <random>

#include "Midi.h"
#include "MidiMaps.h"
#include "RhythmRational.h"
#include "MusicalForm.h"
#include "Arrangements.h"

namespace textmidi
{
    namespace cgm
    {
        class Composer
        {
          public:
            struct TrackScramble
            {
                explicit constexpr TrackScramble(arrangements::PermutationEnum scramble = arrangements::PermutationEnum::Identity,
                    TicksDuration period = TicksDuration{120LU * TicksPerQuarter}) noexcept
                  : scramble_(scramble),
                    period_(period),
                    arrangements_{}
                {}

                arrangements::PermutationEnum scramble_;
                TicksDuration     period_;
                std::shared_ptr<arrangements::Arrangements> arrangements_;
            };

            Composer(bool gnuplot, bool answer, arrangements::PermutationEnum track_scramble,
                TicksDuration track_scramble_period, size_t max_events_per_track = 100000) noexcept
              : gnuplot_(gnuplot),
                answer_(answer),
                track_scramble_(track_scramble, track_scramble_period),
                max_events_per_track_{max_events_per_track},
                random_dev_(),
                generator_{random_dev_()}
            {
            }

            // Rule of 5: If you delete or declare any of assign, copy, move, move copy, d-tor,
            // then delete or declare all of them.
            Composer(const Composer& ) = delete;
            Composer(Composer&& ) = delete;
            Composer& operator=(const Composer& ) = delete;
            Composer& operator=(Composer&& ) = delete;

            void operator()(std::ofstream& textmidi_file, const MusicalForm& xml_form, bool write_header = true);

          private:
            rational::RhythmRational duration_to_rhythm(double duration) const noexcept;
            rational::RhythmRational snap_to_pulse(rational::RhythmRational rhythm, double pulse_per_second) const noexcept;
            void build_track_scramble_sequences(std::vector<std::vector<int>>& track_scramble_sequences,
                TicksDuration total_duration) noexcept;
            void build_composition_priority_graph(const MusicalForm& xml_form,
                std::vector<std::list<int>>& leaders_topo_sort) noexcept;
            bool gnuplot_;
            bool answer_;
            TrackScramble track_scramble_;
            size_t max_events_per_track_;
            std::random_device random_dev_;
            std::mt19937 generator_;
        };

        extern const midi::NumStringMap<arrangements::PermutationEnum> arrangement_map;
    }
}

#endif

