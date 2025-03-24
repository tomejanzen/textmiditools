//
// TextMIDITools Version 1.0.94
//
// RhythmRational 1.0
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Patterned as a concrete class after the Complex example
// in "The C++ Programming Language"
// by Bjarne Stroustrup, 4th ed., page 542, section 18.3.6, for example use of
// pass-by-value and not reference to const as in the past.
// Lastly it is broadly similar to generic Ada-83 packages
// Tom Janzen wrote in about 1987 or 1988
// for complex numbers and basic vector and matrix operations.
//
#if !defined(TEXTMIDI_RATIONAL_H)
#    define  TEXTMIDI_RATIONAL_H

#include <cstdint>

#include <bitset>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <regex>
#include <stdexcept>
#include <utility>
#include <string>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
#include <boost/lexical_cast.hpp>

namespace textmidi
{
    namespace rational
    {
        enum class RhythmExpression : std::int32_t
        {
            Rational = 1,
            SimpleContinuedFraction = 2,
        };

#pragma pack(8)
        class RhythmRational
        {
            public:
                using value_type = std::int64_t;

                explicit constexpr RhythmRational(std::int64_t numerator = 0L,
                    std::int64_t denominator = 1L, bool reduce_it = true)
                  : numerator_{numerator},
                    denominator_{denominator}
                {
                    if (0L == denominator)
                    {
                        throw std::overflow_error(
                            "RhythmRational constructor zero denominator");
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


                constexpr std::int64_t numerator() const noexcept
                {
                    return numerator_;
                }

                constexpr std::int64_t denominator() const noexcept
                {
                    return denominator_;
                }

                void numerator(std::int64_t numerator) noexcept;
                void denominator(std::int64_t denominator);
                constexpr operator bool() const
                {
                    return numerator_ != 0;
                }

                void invert();
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
                std::int64_t most_positive_equal_divisor(
                    std::int64_t a, std::int64_t b) const;
                RhythmRational& unreduced_product(
                    const RhythmRational& multiplier)
                {
                    numerator_   *= multiplier.numerator();
                    denominator_ *= multiplier.denominator();
                    return *this;
                }
                void make_denominators_coherent(
                    RhythmRational& a, RhythmRational& b) const;
                void reduce_sign();

                friend std::ostream&
                    operator<<(std::ostream& os, RhythmRational tr)
                {
                    auto flags{os.flags()};
                    tr.reduce();
                    if (1L == tr.denominator())
                    {
                        os << std::dec << tr.numerator();
                    }
                    else
                    {
                        os << std::dec << tr.numerator() << '/'
                            << tr.denominator();
                    }
                    static_cast<void>(os.flags(flags));
                    return os;
                }

                friend RhythmRational operator+(RhythmRational addend1,
                        const RhythmRational& addend2)
                {
                    addend1 += addend2;
                    return addend1;
                }
                friend RhythmRational
                    operator-(RhythmRational value,
                              const RhythmRational& subtrahend)
                {
                    value -= subtrahend;
                    return value;
                }

                friend RhythmRational
                    operator/(RhythmRational dividend, RhythmRational divisor)
                {
                    dividend /= divisor;
                    return dividend;
                }

                friend RhythmRational
                    operator*(RhythmRational value,
                    const RhythmRational& multiplier)
                {
                    value *= multiplier;
                    return value;
                }

                friend RhythmRational abs(RhythmRational val)
                {
                    val.denominator(std::abs(val.denominator()));
                    val.numerator(std::abs(val.numerator()));
                    val.reduce();
                    return val;
                }

                friend std::istream& operator>>(std::istream& is,
                    RhythmRational& tr)
                {
                    auto cnt{is.rdbuf()->in_avail()};
                    const std::regex rhythm_rational_re{
                        R"(([[:space:]]*)(([-+]?[[:digit:]]{1,19})(([/])([-+]?[1-9][[:digit:]]{0,19}))?)([.]*)(.*))"};

                    enum MatchEnum
                    {
                        leading_space_match     = 1,
                        rational_match          = 2,
                        numerator_match         = 3,
                        slash_denominator_match = 4,
                        denominator_match       = 6,
                        dots_match              = 7,
                        remainder_match         = 8,
                    };
                    std::string buf_string(cnt + 1, '\0');
                    // not expected to be null-terminated
                    is.rdbuf()->sgetn(buf_string.data(), cnt);
                    if (buf_string.size())
                    {
                        buf_string[buf_string.size() - 1] = '\0';
                    }
                    std::smatch matches{};
                    const auto mat{regex_match(buf_string, matches,
                        rhythm_rational_re)};
                    if (!mat)
                    {
                        std::cerr << "Improper rational number: " << buf_string
                            << '\n';
                    }
                    else
                    {
                        // If there was no numerator
                        int64_t numer{};
                        int64_t denom{1L};
                        // If there is a match for a rational
                        if (!matches[rational_match].str().empty())
                        {
                            numer = std::atol(
                                matches[numerator_match].str().c_str());
                            // If there is a slash and denominator
                            if (!matches[slash_denominator_match]
                                .str().empty())
                            {
                                // matches[6] is denominator
                                denom = std::atol(matches[denominator_match]
                                    .str().c_str());
                            }
                        }
                        else
                        {
                            // error
                            // set stream error
                            is.setstate(std::ios_base::failbit);
                            is.clear();
                            std::string message(
                                "bad read of RhythmsRational: ");
                            message += buf_string;
                            throw std::runtime_error(message);
                        }
                        const auto offs{-matches[remainder_match]
                            .str().size()};
                        if (offs != 0)
                        {
                            is.seekg(offs, std::ios_base::cur);
                        }
                        RhythmRational rtn{numer, denom};
                        const RhythmRational rtn_save{numer, denom};
                        for (int64_t dot{1}; static_cast<size_t>(dot)
                            <= matches[dots_match].str().size(); ++dot)
                        {
                            rtn += rtn_save / RhythmRational{1L << dot};
                        }
                        tr = rtn;
                    }
                    return is;
                }
        };

        class SimpleContinuedFraction
        {
          public:
            using num_type = std::int64_t;
            using value_type = std::pair<num_type, std::list<num_type>>;
            SimpleContinuedFraction() = default;
            explicit SimpleContinuedFraction(const value_type& scf)
                : val_{scf}
            {}
            explicit SimpleContinuedFraction(const RhythmRational& rr)
            {
                val_.first = rr.numerator() / rr.denominator();
                auto a{rr.numerator() % rr.denominator()};
                auto b{rr.denominator()};
                while (a)
                {
                    std::swap(a, b);
                    val_.second.push_back(a / b);
                    a %= b;
                }
                if (!val_.second.empty() && (*val_.second.rbegin() > 1L))
                {
                    --*(val_.second.rbegin());
                    val_.second.push_back(1L);
                }
            }
            operator RhythmRational()
            {
                auto rtn {continued_fraction_list_to_rational(val_.second)};
                rtn += RhythmRational{val_.first};
                return rtn;
            }
          private:
            value_type val_{};

            RhythmRational continued_fraction_list_to_rational(
                std::list<num_type> &denoms);

            friend std::ostream& operator<<(std::ostream& os,
                const SimpleContinuedFraction& scf)
            {
                os << "[" << scf.val_.first;
                if (!scf.val_.second.empty())
                {
                    auto ni{scf.val_.second.begin()};
                    os << ';' << *ni++;
                    while (ni != scf.val_.second.end())
                    {
                        os << ',' << *ni++;
                    }
                }
                os << "]";
                return os;
            }

            friend std::istream& operator>>(
                std::istream& is, SimpleContinuedFraction& scf)
            {
                const std::regex continued_fraction_re{
                    R"(\[([[:digit:]]+)((;[1-9][[:digit:]]*)(,[1-9][[:digit:]]*)*)?\])"};
                const std::regex
                    continued_fraction_re2{R"(([,;])([[:digit:]]+))"};
                std::string s;
                is >> s;
                std::smatch matches{};
                auto sts{std::regex_search(s, matches, continued_fraction_re)};
                if (sts)
                {
                    scf.val_.first = boost::lexical_cast<int64_t>(matches[1]);
                    if (matches.length() > 2)
                    {
                        if (!matches[2].str().empty())
                        {
                            auto denom_begin{std::sregex_iterator(s.begin(),
                                s.end(), continued_fraction_re2)};
                            auto denom_end{std::sregex_iterator()};
                            for (auto mi{denom_begin}; mi != denom_end; ++mi)
                            {
                                std::istringstream mi_iss{mi->str()};
                                int64_t denom;
                                char waste{};
                                mi_iss >> waste >> denom;
                                scf.val_.second.push_back(denom);
                            }
                        }
                    }
                }
                return is;
            }
        };

        struct ReadMusicRational
        {
            std::istream& operator()(std::istream& , RhythmRational& );
        };

        struct PrintRhythmBase
        {
            virtual std::ostream& operator()(
                std::ostream& , const RhythmRational& ) = 0;
            virtual ~PrintRhythmBase() {}
        };

        struct PrintRhythmRational : public PrintRhythmBase
        {
            PrintRhythmRational() = default;
            explicit PrintRhythmRational(bool dotted_rhythm)
                : dotted_rhythm_{dotted_rhythm}
            {
            }
            std::ostream& operator()(
                std::ostream& , const RhythmRational& tr) final;
          private:
            std::int64_t convert_to_dotted_rhythm(RhythmRational& q);
            bool dotted_rhythm_{true};
        };

        struct PrintRhythmSimpleContinuedFraction : public PrintRhythmBase
        {
            std::ostream& operator()(
                std::ostream& , const RhythmRational& tr) final;
        };
        extern std::unique_ptr<PrintRhythmBase> print_rhythm;

    } // namespace rational
#pragma pack()

} // namespace textmidi

namespace std
{
    template<>
    struct numeric_limits<textmidi::rational::RhythmRational>
    {
        static constexpr textmidi::rational::RhythmRational max() noexcept
            { return textmidi::rational::RhythmRational
            {std::numeric_limits<textmidi::rational::RhythmRational
                ::value_type>::max(), 1L}; }
        static constexpr textmidi::rational::RhythmRational min() noexcept
            { return textmidi::rational::RhythmRational
            {-std::numeric_limits<textmidi::rational::RhythmRational
                ::value_type>::max(), 1L}; }
        static constexpr textmidi::rational::RhythmRational lowest() noexcept
            { return textmidi::rational::RhythmRational
            {std::numeric_limits<textmidi::rational::RhythmRational
                ::value_type>::lowest(), 1L}; }
        static constexpr textmidi::rational::RhythmRational epsilon() noexcept
            { return textmidi::rational::RhythmRational
            {1L, std::numeric_limits<textmidi::rational::RhythmRational
                ::value_type>::max()}; }
        static constexpr textmidi::rational::RhythmRational round_error()
            noexcept
            { return textmidi::rational::RhythmRational{0L}; }
        static constexpr textmidi::rational::RhythmRational infinity() noexcept
            { return textmidi::rational::RhythmRational{0L}; }
        static constexpr textmidi::rational::RhythmRational quiet_NaN()
            noexcept
            { return textmidi::rational::RhythmRational{0L}; }
        static constexpr textmidi::rational::RhythmRational signaling_NaN()
            noexcept
            { return textmidi::rational::RhythmRational{0L}; }
        static constexpr textmidi::rational::RhythmRational denorm_min()
            noexcept
            { return textmidi::rational::RhythmRational{0L}; }
        static const bool is_specialized = true;
        static const bool is_signed = true;
        static const bool is_integer = false;
        static const bool is_exact = true;
        static const bool has_infinity = false;
        static const bool has_quiet_NaN = false;
        static const bool has_signaling_NaN = false;
        static const bool has_denorm = false;
        static const bool has_denorm_loss = false;
        static const float_round_style round_style = std::round_to_nearest;
        static const bool is_iec559 = false;
        static const bool is_bounded = true;
        static const bool is_modulo = false;
        static const std::int32_t digits = 63;
        static const std::int32_t digits10 = 19;
        static const std::int32_t max_digits10 = 19;
        static const std::int32_t radix = 2;
        static const std::int32_t min_exponent = 0;
        static const std::int32_t max_exponent = 0;
        static const bool traps = true;
        static const bool tinyness_before = false;
    };
} // namespace std

#endif
