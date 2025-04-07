//
// TextMIDITools Version 1.0.96
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cctype>
#include <cmath> // abs()
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "RhythmRational.h"

using std::unique_ptr, std::string, std::make_unique;

using textmidi::rational::RhythmRational, textmidi::rational::print_rhythm,
      textmidi::rational::PrintRhythmSimpleContinuedFraction,
      textmidi::rational::SimpleContinuedFraction,
      textmidi::rational::PrintRhythmBase;

void textmidi::rational::RhythmRational::numerator(int64_t numerator) noexcept
{
    numerator_ = numerator;
}

void textmidi::rational::RhythmRational::denominator(int64_t denominator)
{
    if (0 == denominator)
    {
        throw std::overflow_error{string((string(__FUNCTION__) += ' ')
            += "denominator is zero")};
    }
    denominator_ = denominator;
}

bool textmidi::rational::RhythmRational::
    operator==(const RhythmRational& comparand) const
{
    auto v{*this};
    auto w{comparand};
    v.reduce();
    w.reduce();
    return ((w.numerator() == v.numerator())
        && (w.denominator() == v.denominator()));
}

bool textmidi::rational::RhythmRational::
    operator!=(const RhythmRational& comparand) const
{
    return !(comparand == *this);
}

bool textmidi::rational::RhythmRational::
    operator<(RhythmRational comparand) const
{
    auto v{*this};
    //
    // Cross-multiply the denominators prior to comparison.
    // To avoid overflowing if possible,
    // divide the denominators by the "greatest common divisor" or
    // most_positive_equal_divisor;
    // this produced the "least common multiple" or
    // least positive equal product.
    // Cf. William J. Leveque "Elementary Theory of Numbers".
    // Dover Publications, p. 35 for
    // identities on the least-common multiple.
    // For rational arithmetic, cf. Donald Knuth,
    // "The Art of Computer Programming", 3rd Ed.,
    // Volume 2, "Seminumerical Algorithms", Section 4.5: Rational Arithmetic.
    make_denominators_coherent(v, comparand);
    return v.numerator() < comparand.numerator();
}

bool textmidi::rational::RhythmRational::operator>(RhythmRational comparand)
    const
{
    auto v{*this};
    make_denominators_coherent(v, comparand);
    return v.numerator() > comparand.numerator();
}

bool textmidi::rational::RhythmRational::operator<=(RhythmRational comparand)
    const
{
    auto v{*this};
    make_denominators_coherent(v, comparand);
    return v.numerator() <= comparand.numerator();
}

bool textmidi::rational::RhythmRational::operator>=(RhythmRational comparand)
    const
{
    auto v{*this};
    make_denominators_coherent(v, comparand);
    return v.numerator() >= comparand.numerator();
}

RhythmRational& textmidi::rational::RhythmRational::
    operator+=(RhythmRational addend)
{
    make_denominators_coherent(*this, addend);
    numerator_ = numerator_ + addend.numerator();
    reduce();
    return *this;
}

RhythmRational& textmidi::rational::RhythmRational::
    operator-=(RhythmRational subtractor)
{
    make_denominators_coherent(*this, subtractor);
    numerator_ = numerator_ - subtractor.numerator();
    reduce();
    return *this;
}

void textmidi::rational::RhythmRational::reduce_sign()
{
    // If both the denominator and numerator are negative,
    // coerce them positive.
    // If only the denominator is negative, swap the negative
    // sign with the numerator.
    if (denominator_ < 0)
    {
        if (numerator_ < 0)
        {
            numerator_   = std::abs(numerator_);
            denominator_ = std::abs(denominator_);
        }
        else
        {
            numerator_   = -numerator_;
            denominator_ = -denominator_;
        }
    }
}

void textmidi::rational::RhythmRational::reduce()
{
    reduce_sign();

    // Find the greatest common divisor to reduce.
    const auto mped{most_positive_equal_divisor()};
    if (mped > 1)
    {
        numerator_   /= mped;
        denominator_ /= mped;
    }
}

int64_t RhythmRational::most_positive_equal_divisor() const
{
    return most_positive_equal_divisor(numerator_, denominator_);
}

int64_t RhythmRational::most_positive_equal_divisor(int64_t a, int64_t b) const
{
    int64_t twos{1L};
    a = std::abs(a);
    b = std::abs(b);
    if (b != 0)
    {
        while (((a & 1L) == 0L) && ((b & 1L) == 0L))
        {
            a >>= 1L;
            b >>= 1L;
            twos *= 2L;
        }
    }
    if (a < b)
    {
        std::swap(a, b);
    }
    while ((b != 0L) && (a != 1L) && (a != 0))
    {
        a %= b;
        if (a < b)
        {
            std::swap(a, b);
        }
    }
    return a * twos;
}

RhythmRational& RhythmRational::operator*=(RhythmRational multiplier)
{
    unreduced_product(multiplier);
    reduce();
    return *this;
}

RhythmRational& RhythmRational::operator/=(RhythmRational divisor)
{
    RhythmRational w{divisor.denominator(), divisor.numerator()};
    numerator_ *= w.numerator();
    denominator_ *= w.denominator();
    reduce();
    return *this;
}

void textmidi::rational::RhythmRational::invert()
{
    std::swap(numerator_, denominator_);
}

void textmidi::rational::RhythmRational::
    make_denominators_coherent(RhythmRational& a, RhythmRational& b) const
{
    const auto mped{most_positive_equal_divisor(
        a.denominator(), b.denominator_)};
    const auto lcm{(a.denominator() / mped)* b.denominator()};
    const auto a_factor{lcm / a.denominator()};
    const auto b_factor{lcm / b.denominator()};
    a.unreduced_product(RhythmRational{a_factor, a_factor, false});
    b.unreduced_product(RhythmRational{b_factor, b_factor, false});
}

textmidi::rational::RhythmRational
    textmidi::rational::SimpleContinuedFraction::
    continued_fraction_list_to_rational(std::list<num_type> &denoms)
{
    RhythmRational rr{0L, 1L};
    for (auto di{denoms.rbegin()}; di != denoms.rend(); ++di)
    {
        RhythmRational divisor{*di, 1L};
        divisor += rr;
        rr = divisor.reciprocal();
    }
    return rr;
}

textmidi::rational::RhythmRational
    textmidi::rational::RhythmRational::reciprocal()
{
    return RhythmRational(denominator_, numerator_);
}

std::istream& textmidi::rational::ReadMusicRational::
    operator()(std::istream& is, RhythmRational& tr)
{
    using std::smatch, std::regex;
    using std::istringstream;
    // Check for a single integer, because in textmidi lazy mode
    // this is a short hand for a numerator of 1.
    auto flags{is.flags()};
    string str;
    is >> str;
    // +1234... 1234  +1234  but not 1234/
    const regex int_re{
        R"(([[:space:]]*)([-+]?[[:digit:]]{1,19})(([/])|([.]+))?.*)"};
    enum MatchEnum
    {
        leading_space_match     = 1,
        denominator_match       = 2,
        slash_or_dots_match     = 3,
        slash_match             = 4,
        dots_match              = 5,
    };
    smatch matches{};
    auto strcopy{str};
    const auto mat{regex_match(strcopy, matches, int_re)};

    if (mat) // if it's just an int
    {
        // If there is no slash but there is a denominator:
        if (matches[slash_match].str().empty()
            && !matches[denominator_match].str().empty())
        {
            auto denom = std::atol(matches[denominator_match].str().c_str());
            tr = RhythmRational{1L, denom};
            const auto rtn_save{tr};
            for (int64_t dot{1}; static_cast<size_t>(dot)
                 <= matches[dots_match].str().size(); ++dot)
            {
                tr += rtn_save / RhythmRational{1L << dot};
            }
        }
        else
        {
            istringstream iss(str);
            iss >> tr;
        }
    }
    else
    {
        istringstream iss(str);
        iss >> tr;
    }
    static_cast<void>(is.flags(flags));
    return is;
}

std::int64_t textmidi::rational::PrintRhythmRational::
    convert_to_dotted_rhythm(RhythmRational& q)
{
    using std::bitset;
    auto numerator{q.numerator()};
    RhythmRational denominator_q{q.denominator()};

    std::uint64_t numerator_inc{numerator + 1UL};
    bitset<64> bs{numerator_inc};
    int32_t dots{};
    if ((bs.count() == 1) && dotted_rhythm_) // then the numerator is 2^n - 1
    {
        while (numerator > 1)
        {
            numerator     /= 2;
            denominator_q /= RhythmRational{2};
            ++dots;
        }
        denominator_q.invert();
        q = RhythmRational{numerator} * denominator_q;
    }
    return dots;
}

std::ostream& textmidi::rational::PrintRhythmRational::
    operator()(std::ostream& os, const RhythmRational& tr)
{
    using std::dec;
    auto flags{os.flags()};
    RhythmRational tr_temp{tr};
    tr_temp.reduce();
    // if (tr_temp > RhythmRational{})
    {
        // textmidi has a convention for quickness of typing that
        // a 1/4 note is written 4, or 1/4, or 2/8 etc.
        // So a 4-wholenote long rhythm must be written 4/1.
        auto dotcount{convert_to_dotted_rhythm(tr_temp)};
        if ((tr_temp.denominator() == 1L) && (tr_temp.numerator() != 1L))
        {
            os << dec << tr_temp.numerator() << '/' << tr_temp.denominator();
        }
        else
        {
            if (tr_temp.numerator() == 1L)
            {
                os << tr_temp.denominator();
            }
            else
            {
                os << tr_temp;
            }
        }
        for ( ; dotcount; --dotcount)
        {
            os << '.';
        }
    }
    static_cast<void>(os.flags(flags));
    return os;
}

std::ostream& textmidi::rational::PrintRhythmSimpleContinuedFraction::
    operator()(std::ostream& os, const RhythmRational& ratio64)
{
    auto flags{os.flags()};
    if (ratio64 > RhythmRational{})
    {
        auto temp{ratio64};
        auto scf{static_cast<SimpleContinuedFraction>(temp)};
        os << scf;
    }
    static_cast<void>(os.flags(flags));
    return os;
}

std::unique_ptr<PrintRhythmBase> textmidi::rational::print_rhythm
    = make_unique<PrintRhythmRational>();
