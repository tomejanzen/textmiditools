//
// TextMIDITools Version 1.0.22
//
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cmath> // abs()
#include <type_traits> // swap()
#include <string>
#include <iostream>
#include <sstream>

#include "RhythmRational.h"

using namespace std;
using namespace textmidi;
using namespace textmidi::rational;


std::int64_t textmidi::rational::RhythmRational::numerator() const
{
    return numerator_;
}

std::int64_t textmidi::rational::RhythmRational::denominator() const
{
    return denominator_;
}

void textmidi::rational::RhythmRational::numerator(std::int64_t numerator)
{
    numerator_ = numerator;
}

void textmidi::rational::RhythmRational::denominator(std::int64_t denominator)
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

void textmidi::rational::RhythmRational::reduce()
{
    const auto mped{most_positive_equal_divisor()};
    if (mped > 1)
    {
        numerator_   /= mped;
        denominator_ /= mped;
    }
}

std::int64_t RhythmRational::most_positive_equal_divisor() const
{
    return most_positive_equal_divisor(numerator_, denominator_);
}

std::int64_t RhythmRational::most_positive_equal_divisor(int64_t a, int64_t b) const
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

std::istream& textmidi::rational::operator>>(std::istream& is, RhythmRational& tr)
{
    RhythmRational rtn{};
    std::string instr{};
    is >> instr;
    int64_t n{}, d{1L};

    auto slashpos{instr.find('/')};
    if (instr.npos == slashpos)
    {
        stringstream numerator_ss(instr);
        numerator_ss >> n;
        rtn = RhythmRational{};
    }
    else
    {
        instr[slashpos] = ' ';
        if (slashpos < instr.length() - 1)
        {
            stringstream ss(instr);
            ss >> n >> d;
        }
    }
    tr.numerator(n);
    tr.denominator(d);
    return is;
}

textmidi::rational::RhythmRational textmidi::rational::abs(RhythmRational val)
{
    val.denominator(std::abs(val.denominator()));
    val.numerator(std::abs(val.numerator()));
    val.reduce();
    return val;
}

std::ostream& textmidi::rational::operator<<(std::ostream& os, RhythmRational tr)
{
    tr.reduce();
    // This looks backwards but textmidi has a convention that 1/4 can be written as 4,
    // but 4/1 has to be written as 4/1.  This is because it's faster to type in scores
    // in which 1/4 notes and 1/16 notes are far more common than 4/1 notes.
    if (1L == tr.denominator())
    {
        os << tr.numerator();
    }
    else
    {
        os << tr.numerator() << '/' << tr.denominator();
    }
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
    a.unreduced_product(RhythmRational{a_factor, a_factor});
    b.unreduced_product(RhythmRational{b_factor, b_factor});
}

//
// Print a lazy delay value using dotted rhythms.
//
ostream& textmidi::rational::print_rhythm(ostream& os, const RhythmRational& tr)
{
    // textmidi has a convention for quickness of typing that
    // a 1/4 note is written 4, or 1/4, or 2/8 etc.
    if ((tr.denominator() == 1L) && (tr.numerator() != 1L))
    {
        os << tr.numerator() << '/' << tr.denominator();
    }
    else
    {
        switch (tr.numerator())
        {
          case 1L:
            os << tr.denominator() << ' ';
            break;
          case 3L: // single dot possible
            if ((tr.denominator() % 2L) == 0L)
            {
                os << (tr.denominator() / 2L) << ".";
            }
            else
            {
                os << tr;
            }
            break;
          case 7L: // double dot possible
            if ((tr.denominator() % 4L) == 0L)
            {
                os << (tr.denominator() / 4L) << "..";
            }
            else
            {
                os << tr;
            }
            break;
          default:
            os << tr;
          break;
        }
    }
    return os;
}

