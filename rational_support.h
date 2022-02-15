//
// TextMIDITools Version 1.0.19
//
// textmidi 1.0.6
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(RATIONAL_SUPPORT)
#    define  RATIONAL_SUPPORT

#include "TextmidiRational.h"

namespace textmidi
{

    namespace rational
    {

        TextmidiRational operator%(TextmidiRational dividend, const TextmidiRational& divisor);

        TextmidiRational round(TextmidiRational ratnum);

        std::int64_t snap_to_int(TextmidiRational num, TextmidiRational grid);

        TextmidiRational snap(TextmidiRational num, TextmidiRational grid);

    } // rational

} // midi

#endif // RATIONAL_SUPPORT
