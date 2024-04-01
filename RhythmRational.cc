//
// TextMIDITools Version 1.0.76
//
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cctype>
#include <cmath> // abs()
#include <cstdlib>

#include <iostream>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits> // swap()

#include <boost/lexical_cast.hpp>

#include "RhythmRational.h"

using namespace std;
using namespace textmidi;
using namespace textmidi::rational;


int64_t textmidi::rational::RhythmRational::numerator() const noexcept
{
    return numerator_;
}

int64_t textmidi::rational::RhythmRational::denominator() const noexcept
{
    return denominator_;
}

void textmidi::rational::RhythmRational::numerator(int64_t numerator) noexcept
{
    numerator_ = numerator;
}

void textmidi::rational::RhythmRational::denominator(int64_t denominator) noexcept
{
    denominator_ = denominator;
}

bool textmidi::rational::RhythmRational::operator==(const RhythmRational& comparand) const
{
    auto v{*this};
    auto w{comparand};
    v.reduce();
    w.reduce();
    return ((w.numerator() == v.numerator()) && (w.denominator() == v.denominator()));
}

bool textmidi::rational::RhythmRational::operator!=(const RhythmRational& comparand) const
{
    return !(comparand == *this);
}

bool textmidi::rational::RhythmRational::operator<(RhythmRational comparand) const
{
    auto v{*this};
    //
    // Cross-multiply the denominators prior to comparison.
    // To avoid overflowing if possible, divide the denominators by the "greatest common divisor" or
    // most_positive_equal_divisor; this produced the "least common multiple" or
    // least positive equal product.
    // Cf. William J. Leveque "Elementary Theory of Numbers". Dovoer ublications, p. 35 for
    // identities on the least-common multiple.
    // For rational arithmetic, cf. Donald Knuth, "The Art of Computer Programming", 3rd Ed.,
    // Volume 2, "Seminumerical Algorithms", Section 4.5: Rational Arithmetic.
    make_denominators_coherent(v, comparand);
    return v.numerator() < comparand.numerator();
}

bool textmidi::rational::RhythmRational::operator>(RhythmRational comparand) const
{
    auto v{*this};
    make_denominators_coherent(v, comparand);
    return v.numerator() > comparand.numerator();
}

bool textmidi::rational::RhythmRational::operator<=(RhythmRational comparand) const
{
    auto v{*this};
    make_denominators_coherent(v, comparand);
    return v.numerator() <= comparand.numerator();
}

bool textmidi::rational::RhythmRational::operator>=(RhythmRational comparand) const
{
    auto v{*this};
    make_denominators_coherent(v, comparand);
    return v.numerator() >= comparand.numerator();
}

RhythmRational& textmidi::rational::RhythmRational::operator+=(RhythmRational addend)
{
    make_denominators_coherent(*this, addend);
    numerator_ = numerator_ + addend.numerator();
    reduce();
    return *this;
}

RhythmRational& textmidi::rational::RhythmRational::operator-=(RhythmRational subtractor)
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

RhythmRational textmidi::rational::operator+(RhythmRational addend1, RhythmRational addend2)
{
    addend1 += addend2;
    return addend1;
}

RhythmRational textmidi::rational::operator-(RhythmRational value, RhythmRational subtrahend)
{
    value -= subtrahend;
    return value;
}

RhythmRational& RhythmRational::unreduced_product(const RhythmRational& multiplier)
{
    numerator_   *= multiplier.numerator();
    denominator_ *= multiplier.denominator();
    return *this;
}

RhythmRational& RhythmRational::operator*=(RhythmRational multiplier)
{
    unreduced_product(multiplier);
    reduce();
    return *this;
}

RhythmRational textmidi::rational::operator*(RhythmRational value, RhythmRational multiplier)
{
    value *= multiplier;
    return value;
}

RhythmRational& RhythmRational::operator/=(RhythmRational divisor)
{
    RhythmRational w{divisor.denominator(), divisor.numerator()};
    numerator_ *= w.numerator();
    denominator_ *= w.denominator();
    reduce();
    return *this;
}

RhythmRational textmidi::rational::operator/(RhythmRational dividend, RhythmRational divisor)
{
    dividend /= divisor;
    return dividend;
}

istream& textmidi::rational::operator>>(istream& is, RhythmRational& tr)
{
    auto cnt{is.rdbuf()->in_avail()};
    const regex rhythm_rational_re{R"(([[:space:]]*)(([-+]?[[:digit:]]{1,19})(([/])([-+]?[1-9][[:digit:]]{0,19}))?)(.*))"};

    enum MatchEnum
    {
        leading_space_match     = 1,
        rational_match          = 2,
        numerator_match         = 3,
        slash_denominator_match = 4,
        denominator_match       = 6,
        remainder_match         = 7,
    };
    string buf_string(cnt + 1, '\0');
    // not expected to be null-terminated
    is.rdbuf()->sgetn(buf_string.data(), cnt);
    if (buf_string.size())
    {
        buf_string[buf_string.size() - 1] = '\0';
    }
    smatch matches{};
    const auto mat{regex_match(buf_string, matches, rhythm_rational_re)};
    if (!mat)
    {
        cerr << "Improper rational number: " << buf_string << '\n';
    }
    else
    {
        // If there was no numerator
        int64_t numer{};
        int64_t denom{1L};
        // If there is a match for a rational
        if (!matches[rational_match].str().empty())
        {
            numer = std::atol(matches[numerator_match].str().c_str());
            // If there is a slash and denominator
            if (!matches[slash_denominator_match].str().empty())
            {
                // matches[6] is denominator
                denom = std::atol(matches[denominator_match].str().c_str());
            }
        }
        else
        {
            // error
            // set stream error
            is.setstate(ios_base::failbit);
            is.clear();
            string message("bad read of RhythmsRational: ");
            message += buf_string;
            throw runtime_error(message);
        }
        const auto offs{-matches[remainder_match].str().size()};
        if (offs != 0)
        {
            is.seekg(offs, ios_base::cur);
        }
        RhythmRational rtn{numer, denom};
        tr = rtn;
    }
    return is;
}

textmidi::rational::RhythmRational textmidi::rational::abs(RhythmRational val)
{
    val.denominator(std::abs(val.denominator()));
    val.numerator(std::abs(val.numerator()));
    val.reduce();
    return val;
}

ostream& textmidi::rational::operator<<(ostream& os, RhythmRational tr)
{
    auto flags{os.flags()};
    tr.reduce();
    if (1L == tr.denominator())
    {
        os << dec << tr.numerator();
    }
    else
    {
        os << dec << tr.numerator() << '/' << tr.denominator();
    }
    static_cast<void>(os.flags(flags));
    return os;
}

textmidi::rational::RhythmRational::operator bool() const
{
    return numerator_ != 0;
}

void textmidi::rational::RhythmRational::make_denominators_coherent(RhythmRational& a, RhythmRational& b) const
{
    const auto mped{most_positive_equal_divisor(a.denominator(), b.denominator_)};
    const auto lcm{(a.denominator() / mped)* b.denominator()};
    const auto a_factor{lcm / a.denominator()};
    const auto b_factor{lcm / b.denominator()};
    a.unreduced_product(RhythmRational{a_factor, a_factor, false});
    b.unreduced_product(RhythmRational{b_factor, b_factor, false});
}

textmidi::rational::RhythmRational
    textmidi::rational::RhythmRational::continued_fraction_list_to_rational(std::list<value_type> &denoms)
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

istream& textmidi::rational::operator>>(istream& is, RhythmRational::SimpleContinuedFraction& scf)
{
    const regex continued_fraction_re{R"(\[([[:digit:]]+)((;[1-9][[:digit:]]*)(,[1-9][[:digit:]]*)*)?\])"};
    const regex continued_fraction_re2{R"(([,;])([[:digit:]]+))"};
    string s;
    is >> s;
    smatch matches{};
    auto sts{regex_search(s, matches, continued_fraction_re)};
    if (sts)
    {
        scf.first = boost::lexical_cast<int64_t>(matches[1]);
        if (matches.length() > 2)
        {
            if (!matches[2].str().empty())
            {
                string first_fraction_str{matches[2].str()};
                auto denom_begin{sregex_iterator(s.begin(), s.end(), continued_fraction_re2)};
                auto denom_end{sregex_iterator()};
                for (auto mi{denom_begin}; mi != denom_end; ++mi)
                {
                    istringstream mi_iss{mi->str()};
                    int64_t denom;
                    char waste{};
                    mi_iss >> waste >> denom;
                    scf.second.push_back(denom);
                }
            }
        }
    }
    return is;
}

ostream& textmidi::rational::operator<<(ostream& os, textmidi::rational::RhythmRational::SimpleContinuedFraction scf)
{
    os << "[" << scf.first;
    if (!scf.second.empty())
    {
        auto ni{scf.second.begin()};
        os << ';' << *ni++;
        while (ni != scf.second.end())
        {
            os << ',' << *ni++;
        }
    }
    os << "]";
    return os;
}

textmidi::rational::RhythmRational::operator SimpleContinuedFraction() const
{
    SimpleContinuedFraction scf{};
    scf.first = numerator_ / denominator_;
    auto a{numerator_ % denominator_};
    auto b{denominator_};
    while (a)
    {
        std::swap(a, b);
        scf.second.push_back(a / b);
        a %= b;
    }
    if (!scf.second.empty() && (*scf.second.rbegin() > 1L))
    {
        --*(scf.second.rbegin());
        scf.second.push_back(1L);
    }
    return scf;
}

textmidi::rational::RhythmRational textmidi::rational::RhythmRational::reciprocal()
{
    return RhythmRational(denominator_, numerator_);
}

textmidi::rational::RhythmRational::operator double() const
{
    return static_cast<double>(numerator_ / denominator_)
        + static_cast<double>(numerator_ % denominator_)
        / static_cast<double>(denominator_);
}

std::unique_ptr<PrintRhythmBase> textmidi::rational::print_rhythm{new PrintRhythmRational};

std::ostream& textmidi::rational::PrintRhythmRational::operator()(std::ostream& os, const RhythmRational& tr)
{
    auto flags{os.flags()};
    auto tr_temp{tr};
    tr_temp.reduce();
    if (tr_temp > RhythmRational{})
    {
        // textmidi has a convention for quickness of typing that
        // a 1/4 note is written 4, or 1/4, or 2/8 etc.
        // So a 4-wholenote long rhythm must be written 4/1.
        if ((tr_temp.denominator() == 1L) && (tr_temp.numerator() != 1L))
        {
            os << dec << tr_temp.numerator() << '/' << tr_temp.denominator();
        }
        else
        {
            switch (tr_temp.numerator())
            {
              case 1L:
                os << tr_temp.denominator() << ' ';
                break;
              case 3L: // single dot possible
                if ((tr_temp.denominator() % 2L) == 0L)
                {
                    os << (tr_temp.denominator() / 2L) << ".";
                }
                else
                {
                    os << tr_temp;
                }
                break;
              case 7L: // double dot possible
                if ((tr_temp.denominator() % 4L) == 0L)
                {
                    os << (tr_temp.denominator() / 4L) << "..";
                }
                else
                {
                    os << tr_temp;
                }
                break;
              default:
                os << tr_temp;
              break;
            }
        }
    }
    static_cast<void>(os.flags(flags));
    return os;
}

std::ostream& textmidi::rational::PrintRhythmSimpleContinuedFraction::operator()(std::ostream& os, const RhythmRational& ratio64)
{
    auto flags{os.flags()};
    if (ratio64 > RhythmRational{})
    {
        auto scf{static_cast<RhythmRational::SimpleContinuedFraction>(ratio64)};
        os << scf;
    }
    static_cast<void>(os.flags(flags));
    return os;
}

