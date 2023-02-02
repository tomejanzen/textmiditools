//
// TextMIDITools Version 1.0.29
//
// textmidicgm 1.0
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(COMPOSER)
#    define  COMPOSER

#include <vector>
#include <list>
#include <fstream>

#include "RhythmRational.h"
#include "MusicalForm.h"

namespace textmidi
{
    namespace cgm
    {
        enum class TrackScrambleEnum : int
        {
            None,
            RotateRight,
            RotateLeft,
            Reverse,
            PreviousPermutation,
            NextPermutation,
            SwapPairs,
            Shuffle,
        };

        class Composer
        {
          public:
            struct TrackScramble
            {
                TrackScramble(TrackScrambleEnum scramble = TrackScrambleEnum::None,
                              TicksDuration     period = TicksDuration{120LU * TicksPerQuarter})
                  : scramble_(scramble),
                    period_(period)
                {}

                TrackScrambleEnum scramble_;
                TicksDuration     period_;
            };
            Composer(bool gnuplot, bool answer, TrackScramble track_scramble)
              : gnuplot_(gnuplot),
                answer_(answer),
                track_scramble_(track_scramble)
            {
            }

            Composer(const Composer& ) = delete;
            Composer(Composer& ) = delete;
            explicit Composer(Composer&& ) = delete;
            Composer& operator=(const Composer& ) = delete;
            Composer& operator=(Composer&& ) = delete;

            void operator()(std::ofstream& textmidi_file, const MusicalForm& xml_form);

          private:
            rational::RhythmRational duration_to_rhythm(double duration);
            rational::RhythmRational snap_to_pulse(rational::RhythmRational rhythm, double pulse_per_second);
            void build_track_scramble_sequences(std::vector<std::vector<int>>& track_scramble_sequences,
                int track_qty, TicksDuration total_duration);
            void build_composition_priority_graph(const MusicalForm& xml_form,
                std::vector<std::list<int>>& leaders_topo_sort);
            bool gnuplot_;
            bool answer_;
            TrackScramble track_scramble_;
        };

        typedef std::map<std::string, TrackScrambleEnum> TrackScrambleMap;
        const TrackScrambleMap track_scramble_map
        {
            {"none", TrackScrambleEnum::None},
            {"rotateright", TrackScrambleEnum::RotateRight},
            {"rotateleft", TrackScrambleEnum::RotateLeft},
            {"reverse", TrackScrambleEnum::Reverse},
            {"previousPermutation", TrackScrambleEnum::PreviousPermutation},
            {"nextPermutation", TrackScrambleEnum::NextPermutation},
            {"swappairs", TrackScrambleEnum::SwapPairs},
            {"shuffle", TrackScrambleEnum::Shuffle}
        };

    }
}

#endif
