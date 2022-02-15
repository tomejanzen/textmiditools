//
// TextMIDITools Version 1.0.19
//
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "rational_support.h"

using namespace textmidi;
using namespace textmidi::rational;

TextmidiRational textmidi::rational::operator%(TextmidiRational dividend,
    const TextmidiRational& divisor)
{
    while ((dividend > divisor) && (divisor != TextmidiRational{0L}))
    {
        dividend -= divisor;
    }
    return dividend;
}

TextmidiRational textmidi::rational::round(TextmidiRational ratnum)
{
   const TextmidiRational remainder(
       ratnum.numerator() % ratnum.denominator(),
       ratnum.denominator());
   if (remainder >= TextmidiRational{1L, 2L})
   {
       ratnum += TextmidiRational{1L} - remainder;
   }
   else
   {
       ratnum -= remainder;
   }
   return ratnum;
}

std::int64_t textmidi::rational::snap_to_int(TextmidiRational num, TextmidiRational grid)
{
    const auto modnum{(num % grid)};
    TextmidiRational snapped{ (modnum >= (grid / TextmidiRational{2})) ?
          num + (grid - modnum) : num - modnum};
    return round(snapped).numerator();
}

TextmidiRational textmidi::rational::snap(TextmidiRational num, TextmidiRational grid)
{
    const auto modnum{(num % grid)};
    if (grid != TextmidiRational{0})
    {
        return (modnum >= (grid / TextmidiRational{2})) ?
          num + (grid - modnum) : num - modnum;
    }
    else
    {
        return num;
    }
}


