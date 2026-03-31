//
// TextMIDITools Version 1.1.1
//
//
// Copyright © 2025 Thomas E. Janzen
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

        RhythmRational operator%(RhythmRational dividend,
            const RhythmRational& divisor);

        RhythmRational round(RhythmRational ratnum);

        RhythmRational snap(RhythmRational num, RhythmRational grid);

    } // namespace rational

} // namespace textmidi

#endif // RATIONAL_SUPPORT
