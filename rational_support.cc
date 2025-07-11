//
// TextMIDITools Version 1.1.0
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "rational_support.h"

using textmidi::rational::RhythmRational;

RhythmRational textmidi::rational::operator%(RhythmRational dividend,
    const RhythmRational& divisor) noexcept
{
    while ((dividend > divisor) && (divisor != RhythmRational{0L}))
    {
        dividend -= divisor;
    }
    return dividend;
}

RhythmRational textmidi::rational::round(RhythmRational ratnum) noexcept
{
    const RhythmRational remainder(
        ratnum.numerator() % ratnum.denominator(),
        ratnum.denominator());
    if (remainder >= RhythmRational{1L, 2L})
    {
        ratnum += RhythmRational{1L} - remainder;
    }
    else
    {
       ratnum -= remainder;
    }
    return ratnum;
}

RhythmRational textmidi::rational::
    snap(RhythmRational num, RhythmRational grid) noexcept
{
    const auto modnum{(num % grid)};
    if (grid != RhythmRational{0})
    {
        return (modnum >= (grid / RhythmRational{2})) ?
          num + (grid - modnum) : num - modnum;
    }
    else
    {
        return num;
    }
}
