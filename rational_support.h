//
// TextMIDITools Version 1.0.10
//
// textmidi 1.0.6
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(RATIONAL_SUPPORT)
#    define  RATIONAL_SUPPORT

#include <boost/rational.hpp>

namespace textmidi
{

    namespace rational
    {

        template<typename IntType>
        boost::rational<IntType> operator%(boost::rational<IntType> dividend,
            const boost::rational<IntType>& divisor)
        {
            while ((dividend > divisor) && (divisor != boost::rational<IntType>{0}))
            {
                dividend -= divisor;
            }
            return dividend;
        }

        template<typename IntType>
        boost::rational<IntType> round(boost::rational<IntType> ratnum)
        {
           const boost::rational<IntType> remainder(
               ratnum.numerator() % ratnum.denominator(),
               ratnum.denominator());
           if (remainder >= boost::rational<IntType>(1, 2))
           {
               ratnum += 1 - remainder;
           }
           else
           {
               ratnum -= remainder;
           }
           return ratnum;
        }

        template <typename IntType>
        typename boost::rational<IntType>::int_type
        snap_to_int(boost::rational<IntType> num,
            boost::rational<IntType> grid)
        {
            const auto modnum{(num % grid)};
            boost::rational<IntType> snapped{ (modnum >= (grid / 2)) ?
                  num + (grid - modnum) : num - modnum};
            return round(snapped).numerator();
        }

        template <typename IntType>
        typename boost::rational<IntType> snap(boost::rational<IntType> num,
            boost::rational<IntType> grid)
        {
            const auto modnum{(num % grid)};
            if (grid != 0)
            {
                return (modnum >= (grid / 2)) ?
                  num + (grid - modnum) : num - modnum;
            }
            else
            {
                return num;
            }
        }

    } // rational

} // midi

#endif // RATIONAL_SUPPORT
