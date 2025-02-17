//
// TextMIDITools Version 1.0.88
//
// RhythmRational 1.0
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Patterned as a concrete class after the Complex example in "The C++ Programming Language"
// by Bjarne Stroustrup, 4th ed., page 542, section 18.3.6, for example use of
// pass-by-value and not reference to const as in the past.
// Lastly it is broadly similar to generic Ada-83 packages Tom Janzen wrote in about 1987 or 1988
// for complex numbers and basic vector and matrix operations.
//
#if !defined(TEXTMIDI_RATIONAL_H)
#    define  TEXTMIDI_RATIONAL_H

#include <cstdint>
#include <iostream>
#include <list>
#include <stdexcept>
#include <memory>
#include <limits>
#include <bitset>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>

namespace textmidi
{
    namespace rational
    {
        enum class RhythmExpression : int
        {
            Rational = 1,
            SimpleContinuedFraction = 2,
        };


#pragma pack(8)
        class RhythmRational
        {
            public:
                using value_type = std::int64_t;
                using SimpleContinuedFraction = std::pair<value_type, std::list<value_type>>;

                explicit constexpr RhythmRational(std::int64_t numerator = 0L,
                    std::int64_t denominator = 1L, bool reduce_it = true)
                  : numerator_{numerator},
                    denominator_{denominator}
                {
                    if (0L == denominator)
                    {
                        throw std::out_of_range("RhythmRational constructor zero denominator");
                    }
                    if (reduce_it)
                    {
                        reduce();
                    }
                }

                constexpr operator double() const
                {
                    return static_cast<double>(numerator_ / denominator_)
                        + static_cast<double>(numerator_ % denominator_)
                        / static_cast<double>(denominator_);
                }

                RhythmRational continued_fraction_list_to_rational(std::list<value_type> &denoms);

                explicit RhythmRational(SimpleContinuedFraction scf)
                  : numerator_{scf.first},
                    denominator_{1L}
                {
                    *this += continued_fraction_list_to_rational(scf.second);
                }

                constexpr std::int64_t numerator() const noexcept
                {
                    return numerator_;
                }

                constexpr std::int64_t denominator() const noexcept
                {
                    return denominator_;
                }

                void numerator(std::int64_t numerator) noexcept;
                void denominator(std::int64_t denominator) noexcept;
                constexpr operator bool() const
                {
                    return numerator_ != 0;
                }

                void invert();
                RhythmRational(const RhythmRational& ) = default;
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
                operator SimpleContinuedFraction() const;
                RhythmRational reciprocal();
                void reduce();
                template<class Archive>
                    void serialize(Archive& arc, const unsigned int )
                {
                    arc & BOOST_SERIALIZATION_NVP(numerator_);
                    arc & BOOST_SERIALIZATION_NVP(denominator_);
                }
            private:
                std::int64_t numerator_{0L};
                std::int64_t denominator_{1L};
                std::int64_t most_positive_equal_divisor() const;
                std::int64_t most_positive_equal_divisor(std::int64_t a, std::int64_t b) const;
                RhythmRational& unreduced_product(const RhythmRational& multiplier);
                void make_denominators_coherent(RhythmRational& a, RhythmRational& b) const;
                void reduce_sign();
        };
        RhythmRational operator+(RhythmRational addend1, RhythmRational addend2);
        RhythmRational operator-(RhythmRational value, RhythmRational subtrahend);
        RhythmRational operator/(RhythmRational divisor, RhythmRational dividend);
        RhythmRational operator*(RhythmRational value, RhythmRational multiplier);
        RhythmRational abs(RhythmRational val);
        std::istream& operator>>(std::istream& is, RhythmRational& tr);
        std::ostream& operator<<(std::ostream& os, RhythmRational tr);
        std::istream& operator>>(std::istream& is, RhythmRational::SimpleContinuedFraction& scf);
        std::ostream& operator<<(std::ostream& os, RhythmRational::SimpleContinuedFraction scf);

        struct ReadMusicRational
        {
            std::istream& operator()(std::istream& , RhythmRational& );
        };

        struct PrintRhythmBase
        {
            virtual std::ostream& operator()(std::ostream& , const RhythmRational& ) = 0;
            virtual ~PrintRhythmBase() {}
        };

        struct PrintRhythmRational : public PrintRhythmBase
        {
            PrintRhythmRational() = default;
            PrintRhythmRational(bool dotted_rhythm)
                : dotted_rhythm_{dotted_rhythm}
            {
            }
            std::ostream& operator()(std::ostream& , const RhythmRational& tr) override;
          private:
            long int convert_to_dotted_rhythm(RhythmRational& q);
            bool dotted_rhythm_{true};
        };

        struct PrintRhythmSimpleContinuedFraction : public PrintRhythmBase
        {
            std::ostream& operator()(std::ostream& , const RhythmRational& tr) override;
        };
        extern std::unique_ptr<PrintRhythmBase> print_rhythm;

    }
#pragma pack()

}

#endif
