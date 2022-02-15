//
// TextMIDITools Version 1.0.19
//
// TextmidiRational 1.0
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
        class TextmidiRational
        {
            public:
                TextmidiRational(std::int64_t numerator = 0L, std::int64_t denominator = 1L)
                  : numerator_{numerator},
                    denominator_{denominator}
                {
                    if (0L == denominator)
                    {
                        throw std::out_of_range("TextmidiRational constructor zero denominator");
                    }
                }
                TextmidiRational(const TextmidiRational& ) = default;
                TextmidiRational(TextmidiRational& ) = default;
                explicit TextmidiRational(TextmidiRational&& ) = default;
                TextmidiRational& operator=(const TextmidiRational& ) = default;
                TextmidiRational& operator=(TextmidiRational&& ) = default;
                std::int64_t numerator() const;
                std::int64_t denominator() const;
                void numerator(std::int64_t numerator);
                void denominator(std::int64_t denominator);
                operator bool() const;
                bool operator==(const TextmidiRational& comparand) const;
                bool operator!=(const TextmidiRational& comparand) const;
                bool operator<(TextmidiRational comparand) const;
                bool operator>(TextmidiRational comparand) const;
                bool operator<=(TextmidiRational comparand) const;
                bool operator>=(TextmidiRational comparand) const;
                TextmidiRational& operator+=(TextmidiRational addend);
                TextmidiRational& operator-=(TextmidiRational subtractor);
                TextmidiRational& operator/=(TextmidiRational divisor);
                TextmidiRational& operator*=(TextmidiRational multiplier);
                void reduce();
            private:
                std::int64_t numerator_;
                std::int64_t denominator_;
                std::int64_t most_positive_equal_divisor() const;
                std::int64_t most_positive_equal_divisor(std::int64_t a, std::int64_t b) const;
                TextmidiRational& unreduced_product(const TextmidiRational& multiplier);
                void make_denominators_coherent(TextmidiRational& a, TextmidiRational& b) const;
        };
        TextmidiRational operator+(TextmidiRational addend1, TextmidiRational addend2);
        TextmidiRational operator-(TextmidiRational value, TextmidiRational subtrahend);
        TextmidiRational operator/(TextmidiRational divisor, TextmidiRational dividend);
        TextmidiRational operator*(TextmidiRational value, TextmidiRational multiplier);
        TextmidiRational abs(TextmidiRational val);
        std::istream& operator>>(std::istream& is, TextmidiRational& tr);
        std::ostream& operator<<(std::ostream& os, TextmidiRational tr);
    }
#pragma pack()

}

#endif
