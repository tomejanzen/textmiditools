//
// TextMIDITools Version 1.0.14
//
// textmidicgm 1.0
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(COMPOSER)
#    define  COMPOSER

#include <vector>
#include <fstream>

#include "MusicalForm.h"

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
        RandomShuffle,
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
        {"randomshuffle", TrackScrambleEnum::RandomShuffle}
    };
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

    void compose(const MusicalForm& xml_form, std::ofstream& textmidi_file,
        bool gnuplot, bool answer, TrackScramble track_scramble);

}

#endif
