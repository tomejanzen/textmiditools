//
// TextMIDITools Version 1.0.58
//
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#undef TEXTMIDICGM_PRINT
#if defined(TEXTMIDICGM_PRINT)
#include <ranges>
#include <iostream>
#include <iterator>

using namespace std;
#endif

#include "Arrangements.h"

using namespace arrangements;

int ArrangementsImpl::counter() const
{
    return counter_;
}
void ArrangementsImpl::counter(int counter)
{
    counter_ = counter;
}
void ArrangementsImpl::inc()
{
    ++counter_;
}
bool ArrangementsImpl::counter_finite() const
{
    return counter_ != 0;
}

int Arrangements::counter() const
{
    return arrangements_impl_.counter();
}
void Arrangements::counter(int counter)
{
    arrangements_impl_.counter(counter);
}
void Arrangements::inc()
{
    arrangements_impl_.inc();
}
bool Arrangements::counter_finite() const
{
    return arrangements_impl_.counter_finite();
}

const Arrangement& ArrangementsInSitu::arrangement() const
{
    return arrangement_;
}
Arrangement& ArrangementsInSitu::arrangement()
{
    return arrangement_;
}

const ArrangementVec& ArrangementsSequence::arrangements() const
{
    return arrangements_;
}
ArrangementVec& ArrangementsSequence::arrangements()
{
    return arrangements_;
}
int ArrangementsSequence::length() const
{
    return length_;
}

const Arrangement& ArrangementsIdentity::arrangement() const
{
    return arrangement();
}

void ArrangementsLexicographicForward::next()
{
    std::ranges::next_permutation(arrangement());
    inc();
}

void ArrangementsLexicographicBackward::next()
{
    std::ranges::prev_permutation(arrangement());
}

void ArrangementsRotateRight::next()
{
    std::ranges::rotate(arrangement(),
        arrangement().begin() + (arrangement().size() - 1));
#if defined(TEXTMIDICGM_PRINT)
    ranges::copy(arrangement(), ostream_iterator<int>(cout, " ")); cout << '\n';
#endif
    inc();
}

void ArrangementsRotateLeft::next()
{
    std::ranges::rotate(arrangement(), arrangement().begin() + 1);
    inc();
}

void ArrangementsReverse::next()
{
    std::ranges::reverse(arrangement());
    inc();
}

void ArrangementsSwapPairs::next()
{
    for (int i{}; i < static_cast<int>(arrangement().size()
        - (arrangement().size() % 2LU)); i += 2)
    {
        std::swap(arrangement()[i], arrangement()[i + 1]);
    }
    inc();
}

void ArrangementsSkip::next()
{
    const int anchor{counter() & 1};
    for (int i{anchor}; i < static_cast<int>(arrangement().size()
        - (arrangement().size() % 2LU)); i += 2)
    {
        std::swap(arrangement()[i],
            arrangement()[(i + 1) % arrangement().size()]);
    }
    inc();
}

void ArrangementsShuffle::next()
{
    std::ranges::shuffle(arrangement(), generator_);
    inc();
}

void ArrangementsHeaps::next()
{
    inc();
    while (!heaps_algorithm_template_.next()) {};
}

std::shared_ptr<Arrangements> arrangements::ArrangementsFactory(PermutationEnum permutation_type, int length)
{
    switch (permutation_type)
    {
      case PermutationEnum::Identity:
        return std::make_shared<ArrangementsIdentity>(length);
        break;
      case PermutationEnum::LexicographicForward:
        return std::make_shared<ArrangementsLexicographicForward>(length);
        break;
      case PermutationEnum::LexicographicBackward:
        return std::make_shared<ArrangementsLexicographicBackward>(length);
        break;
      case PermutationEnum::RotateRight:
        return std::make_shared<ArrangementsRotateRight>(length);
        break;
      case PermutationEnum::RotateLeft:
        return std::make_shared<ArrangementsRotateLeft>(length);
        break;
      case PermutationEnum::Reverse:
        return std::make_shared<ArrangementsReverse>(length);
        break;
      case PermutationEnum::SwapPairs:
        return std::make_shared<ArrangementsSwapPairs>(length);
        break;
      case PermutationEnum::Skip:
        return std::make_shared<ArrangementsSkip>(length);
        break;
      case PermutationEnum::Shuffle:
        return std::make_shared<ArrangementsShuffle>(length);
        break;
      case PermutationEnum::Heaps:
        return std::make_shared<ArrangementsHeaps>(length);
        break;
      default:
        break;
    }
    return std::make_shared<ArrangementsIdentity>(length);
}

