//
// TextMIDITools Version 1.0.86
//
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#include <ranges>
#undef TEXTMIDICGM_PRINT
#if defined(TEXTMIDICGM_PRINT)
#include <iostream>
#include <iterator>
#endif

using namespace std;

#include "Arrangements.h"

using namespace arrangements;

int ArrangementsImpl::counter() const noexcept
{
    return counter_;
}
void ArrangementsImpl::counter(int counter) noexcept
{
    counter_ = counter;
}
void ArrangementsImpl::inc() noexcept
{
    ++counter_;
}
bool ArrangementsImpl::counter_finite() const noexcept
{
    return counter_ != 0;
}

int Arrangements::counter() const noexcept
{
    return arrangements_impl_.counter();
}
void Arrangements::counter(int counter) noexcept
{
    arrangements_impl_.counter(counter);
}
void Arrangements::inc() noexcept
{
    arrangements_impl_.inc();
}
bool Arrangements::counter_finite() const noexcept
{
    return arrangements_impl_.counter_finite();
}

const Arrangement& ArrangementsInSitu::arrangement() const noexcept
{
    return arrangement_;
}

Arrangement& ArrangementsInSitu::arrangement() noexcept
{
    return arrangement_;
}

const ArrangementVec& ArrangementsSequence::arrangements() const noexcept
{
    return arrangements_;
}
ArrangementVec& ArrangementsSequence::arrangements() noexcept
{
    return arrangements_;
}
int ArrangementsSequence::length() const noexcept
{
    return length_;
}

const Arrangement& ArrangementsIdentity::arrangement() const noexcept
{
    return ArrangementsInSitu::arrangement();
}

void ArrangementsLexicographicForward::next() noexcept
{
    ranges::next_permutation(arrangement());
    inc();
}

void ArrangementsLexicographicBackward::next() noexcept
{
    ranges::prev_permutation(arrangement());
}

void ArrangementsRotateRight::next() noexcept
{
    ranges::rotate(arrangement(),
        arrangement().begin() + (arrangement().size() - 1));
#if defined(TEXTMIDICGM_PRINT)
    ranges::copy(arrangement(), ostream_iterator<int>(cout, " ")); cout << '\n';
#endif
    inc();
}

void ArrangementsRotateLeft::next() noexcept
{
    ranges::rotate(arrangement(), arrangement().begin() + 1);
    inc();
}

void ArrangementsReverse::next() noexcept
{
    ranges::reverse(arrangement());
    inc();
}

void ArrangementsSwapPairs::next() noexcept
{
    for (auto it{arrangement().begin()};
              it < (arrangement().end() - (arrangement().size() % 2LU));
              it += 2)
    {
        iter_swap(it, it + 1);
    }
    inc();
}

void ArrangementsSkip::next() noexcept
{
    const int anchor{counter() & 1};
    for (int i{anchor}; i < static_cast<int>(arrangement().size()
        - (arrangement().size() % 2LU)); i += 2)
    {
        swap(arrangement()[i],
            arrangement()[(i + 1) % arrangement().size()]);
    }
    inc();
}

void ArrangementsShuffle::next() noexcept
{
    ranges::shuffle(arrangement(), generator_);
    inc();
}

void ArrangementsHeaps::next() noexcept
{
    inc();
    while (!heaps_algorithm_template_.next()) {};
}

shared_ptr<Arrangements> arrangements::ArrangementsFactory(PermutationEnum permutation_type, int length) noexcept
{
    switch (permutation_type)
    {
      case PermutationEnum::Identity:
        return make_shared<ArrangementsIdentity>(length);
        break;
      case PermutationEnum::LexicographicForward:
        return make_shared<ArrangementsLexicographicForward>(length);
        break;
      case PermutationEnum::LexicographicBackward:
        return make_shared<ArrangementsLexicographicBackward>(length);
        break;
      case PermutationEnum::RotateRight:
        return make_shared<ArrangementsRotateRight>(length);
        break;
      case PermutationEnum::RotateLeft:
        return make_shared<ArrangementsRotateLeft>(length);
        break;
      case PermutationEnum::Reverse:
        return make_shared<ArrangementsReverse>(length);
        break;
      case PermutationEnum::SwapPairs:
        return make_shared<ArrangementsSwapPairs>(length);
        break;
      case PermutationEnum::Skip:
        return make_shared<ArrangementsSkip>(length);
        break;
      case PermutationEnum::Shuffle:
        return make_shared<ArrangementsShuffle>(length);
        break;
      case PermutationEnum::Heaps:
        return make_shared<ArrangementsHeaps>(length);
        break;
      default:
        break;
    }
    return make_shared<ArrangementsIdentity>(length);
}

