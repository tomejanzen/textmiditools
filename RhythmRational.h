//
// TextMIDITools Version 1.0.26
//
// RhythmRational 1.0
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Patterned as a concrete class after the Complex example in "The C++ Programming Language"
// by Bjarne Stroustrup, 4th ed., page 542, section 18.3.6, for example use of
// pass-by-value and not reference to const as in the past.
//
#if !defined(TEXTMIDI_RATIONAL)
#    define  TEXTMIDI_RATIONAL

#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace textmidi
{
    namespace rational
    {
#pragma pack(8)
        class RhythmRational
        {
            public:
                typedef std::int64_t value_type;
                RhythmRational(std::int64_t numerator = 0L, std::int64_t denominator = 1L)
                  : numerator_{numerator},
                    denominator_{denominator}
                {
                    if (0L == denominator)
                    {
                        throw std::out_of_range("RhythmRational constructor zero denominator");
                    }
                }
                RhythmRational(const RhythmRational& ) = default;
                RhythmRational(RhythmRational& ) = default;
                explicit RhythmRational(RhythmRational&& ) = default;
                RhythmRational& operator=(const RhythmRational& ) = default;
                RhythmRational& operator=(RhythmRational&& ) = default;
                std::int64_t numerator() const;
                std::int64_t denominator() const;
                void numerator(std::int64_t numerator);
                void denominator(std::int64_t denominator);
                operator bool() const;
                bool operator==(const RhythmRational& comparand) const;
                bool operator!=(const RhythmRational& comparand) const;
                bool operator<(RhythmRational comparand) const;
                bool operator>(RhythmRational comparand) const;
                bool operator<=(RhythmRational comparand) const;
                bool operator>=(RhythmRational comparand) const;
                RhythmRational& operator+=(RhythmRational addend);
                RhythmRational& operator-=(RhythmRational subtractor);
                RhythmRational& operator/=(RhythmRational divisor);
                RhythmRational& operator*=(RhythmRational multiplier);
                void reduce();
            private:
                std::int64_t numerator_;
                std::int64_t denominator_;
                std::int64_t most_positive_equal_divisor() const;
                std::int64_t most_positive_equal_divisor(std::int64_t a, std::int64_t b) const;
                RhythmRational& unreduced_product(const RhythmRational& multiplier);
                void make_denominators_coherent(RhythmRational& a, RhythmRational& b) const;
        };
        RhythmRational operator+(RhythmRational addend1, RhythmRational addend2);
        RhythmRational operator-(RhythmRational value, RhythmRational subtrahend);
        RhythmRational operator/(RhythmRational divisor, RhythmRational dividend);
        RhythmRational operator*(RhythmRational value, RhythmRational multiplier);
        RhythmRational abs(RhythmRational val);
        std::istream& operator>>(std::istream& is, RhythmRational& tr);
        std::ostream& operator<<(std::ostream& os, RhythmRational tr);
        std::ostream& print_rhythm(std::ostream& os, const RhythmRational& ratio64);
    }
#pragma pack()

}

#endif
