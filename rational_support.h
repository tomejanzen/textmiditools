//
// TextMIDITools Version 1.0.88
//
// textmidi 1.0.6
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(RATIONAL_SUPPORT_H)
#    define  RATIONAL_SUPPORT_H

#include "RhythmRational.h"

namespace textmidi
{

    namespace rational
    {

        RhythmRational operator%(RhythmRational dividend, const RhythmRational& divisor) noexcept;

        RhythmRational round(RhythmRational ratnum) noexcept;

        std::int64_t snap_to_int(RhythmRational num, RhythmRational grid) noexcept;

        RhythmRational snap(RhythmRational num, RhythmRational grid) noexcept;

    } // rational

} // midi

#endif // RATIONAL_SUPPORT
