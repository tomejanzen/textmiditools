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

#include <cmath> // abs()
#include <type_traits> // swap()
#include <string>
#include <iostream>
#include <sstream>

#include "TextmidiRational.h"

using namespace std;
using namespace textmidi;
using namespace textmidi::rational;


std::int64_t textmidi::rational::TextmidiRational::numerator() const
{
    return numerator_;
}

std::int64_t textmidi::rational::TextmidiRational::denominator() const
{
    return denominator_;
}

void textmidi::rational::TextmidiRational::numerator(std::int64_t numerator)
{
    numerator_ = numerator;
}

void textmidi::rational::TextmidiRational::denominator(std::int64_t denominator)
{
    denominator_ = denominator;
}

bool textmidi::rational::TextmidiRational::operator==(const TextmidiRational& comparand) const
{
    auto v{*this};
    auto w{comparand};
    v.reduce();
    w.reduce();
    return ((w.numerator() == v.numerator()) && (w.denominator() == v.denominator()));
}

bool textmidi::rational::TextmidiRational::operator!=(const TextmidiRational& comparand) const
{
    return !(comparand == *this);
}

bool textmidi::rational::TextmidiRational::operator<(TextmidiRational comparand) const
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

bool textmidi::rational::TextmidiRational::operator>(TextmidiRational comparand) const
{
    auto v{*this};
    make_denominators_coherent(v, comparand);
    return v.numerator() > comparand.numerator();
}

bool textmidi::rational::TextmidiRational::operator<=(TextmidiRational comparand) const
{
    auto v{*this};
    make_denominators_coherent(v, comparand);
    return v.numerator() <= comparand.numerator();
}

bool textmidi::rational::TextmidiRational::operator>=(TextmidiRational comparand) const
{
    auto v{*this};
    make_denominators_coherent(v, comparand);
    return v.numerator() >= comparand.numerator();
}

TextmidiRational& textmidi::rational::TextmidiRational::operator+=(TextmidiRational addend)
{
    make_denominators_coherent(*this, addend);
    numerator_ = numerator_ + addend.numerator();
    reduce();
    return *this;
}

TextmidiRational& textmidi::rational::TextmidiRational::operator-=(TextmidiRational subtractor)
{
    make_denominators_coherent(*this, subtractor);
    numerator_ = numerator_ - subtractor.numerator();
    reduce();
    return *this;
}

void textmidi::rational::TextmidiRational::reduce()
{
    const auto mped{most_positive_equal_divisor()};
    if (mped > 1)
    {
        numerator_   /= mped;
        denominator_ /= mped;
    }
}

std::int64_t TextmidiRational::most_positive_equal_divisor() const
{
    return most_positive_equal_divisor(numerator_, denominator_);
}

std::int64_t TextmidiRational::most_positive_equal_divisor(int64_t a, int64_t b) const
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

TextmidiRational textmidi::rational::operator+(TextmidiRational addend1, TextmidiRational addend2)
{
    addend1 += addend2;
    return addend1;
}

TextmidiRational textmidi::rational::operator-(TextmidiRational value, TextmidiRational subtrahend)
{
    value -= subtrahend;
    return value;
}

TextmidiRational& TextmidiRational::unreduced_product(const TextmidiRational& multiplier)
{
    numerator_   *= multiplier.numerator();
    denominator_ *= multiplier.denominator();
    return *this;
}

TextmidiRational& TextmidiRational::operator*=(TextmidiRational multiplier)
{
    unreduced_product(multiplier);
    reduce();
    return *this;
}

TextmidiRational textmidi::rational::operator*(TextmidiRational value, TextmidiRational multiplier)
{
    value *= multiplier;
    return value;
}

TextmidiRational& TextmidiRational::operator/=(TextmidiRational divisor)
{
    TextmidiRational w{divisor.denominator(), divisor.numerator()};
    numerator_ *= w.numerator();
    denominator_ *= w.denominator();
    reduce();
    return *this;
}

TextmidiRational textmidi::rational::operator/(TextmidiRational dividend, TextmidiRational divisor)
{
    dividend /= divisor;
    return dividend;
}

std::istream& textmidi::rational::operator>>(std::istream& is, TextmidiRational& tr)
{
    TextmidiRational rtn{};
    std::string instr{};
    is >> instr;
    int64_t n{}, d{1L};

    auto slashpos{instr.find('/')};
    if (instr.npos == slashpos)
    {
        stringstream numerator_ss(instr);
        numerator_ss >> n;
        rtn = TextmidiRational{};
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

textmidi::rational::TextmidiRational textmidi::rational::abs(TextmidiRational val)
{
    val.denominator(std::abs(val.denominator()));
    val.numerator(std::abs(val.numerator()));
    val.reduce();
    return val;
}

std::ostream& textmidi::rational::operator<<(std::ostream& os, TextmidiRational tr)
{
    tr.reduce();
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

textmidi::rational::TextmidiRational::operator bool() const
{
    return numerator_ != 0;
}

void textmidi::rational::TextmidiRational::make_denominators_coherent(TextmidiRational& a, TextmidiRational& b) const
{
    const auto mped{most_positive_equal_divisor(a.denominator(), b.denominator_)};
    const auto lcm{(a.denominator() / mped)* b.denominator()};
    const auto a_factor{lcm / a.denominator()};
    const auto b_factor{lcm / b.denominator()};
    a.unreduced_product(TextmidiRational{a_factor, a_factor});
    b.unreduced_product(TextmidiRational{b_factor, b_factor});
}


