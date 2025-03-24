//
// TextMIDITools Version 1.0.94
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#include <utility>
#include <ranges>
#undef TEXTMIDICGM_PRINT
#if defined(TEXTMIDICGM_PRINT)
#include <iostream>
#include <iterator>
#endif

#include "Arrangements.h"

using std::shared_ptr,
    std::ranges::rotate;

int32_t arrangements::ArrangementsImpl::counter() const noexcept
{
    return counter_;
}
void arrangements::ArrangementsImpl::counter(int32_t counter) noexcept
{
    counter_ = counter;
}
void arrangements::ArrangementsImpl::inc() noexcept
{
    ++counter_;
}
bool arrangements::ArrangementsImpl::counter_finite() const noexcept
{
    return counter_ != 0;
}

int32_t arrangements::Arrangements::counter() const noexcept
{
    return arrangements_impl_.counter();
}
void arrangements::Arrangements::counter(int32_t counter) noexcept
{
    arrangements_impl_.counter(counter);
}
void arrangements::Arrangements::inc() noexcept
{
    arrangements_impl_.inc();
}
bool arrangements::Arrangements::counter_finite() const noexcept
{
    return arrangements_impl_.counter_finite();
}

const arrangements::Arrangement& arrangements::ArrangementsInSitu::arrangement()
    const noexcept
{
    return arrangement_;
}

arrangements::Arrangement& arrangements::ArrangementsInSitu::arrangement()
    noexcept
{
    return arrangement_;
}

const arrangements::Arrangement& arrangements::ArrangementsIdentity
    ::arrangement() const noexcept
{
    return arrangements::ArrangementsInSitu::arrangement();
}

void arrangements::ArrangementsLexicographicForward::next() noexcept
{
    using std::ranges::next_permutation;
    next_permutation(arrangement());
    inc();
}

void arrangements::ArrangementsLexicographicBackward::next() noexcept
{
    using std::ranges::prev_permutation;
    prev_permutation(arrangement());
}

void arrangements::ArrangementsRotateRight::next() noexcept
{
    rotate(arrangement(),
        arrangement().begin() + (arrangement().size() - 1));
#if defined(TEXTMIDICGM_PRINT)
    {
        using std::ranges::copy;
        copy(arrangement(), ostream_iterator<int32_t>(cout, " "));
        cout << '\n';
    }
#endif
    inc();
}

void arrangements::ArrangementsRotateLeft::next() noexcept
{
    rotate(arrangement(), arrangement().begin() + 1);
    inc();
}

void arrangements::ArrangementsReverse::next() noexcept
{
    using std::ranges::reverse;
    reverse(arrangement());
    inc();
}

void arrangements::ArrangementsSwapPairs::next() noexcept
{
    for (auto it{arrangement().begin()};
              it < (arrangement().end() - (arrangement().size() % 2LU));
              it += 2)
    {
        iter_swap(it, it + 1);
    }
    inc();
}

void arrangements::ArrangementsSkip::next() noexcept
{
    using std::swap;
    const int32_t anchor{counter() & 1};
    for (int32_t i{anchor}; i < static_cast<int32_t>(arrangement().size()
        - (arrangement().size() % 2LU)); i += 2)
    {
        swap(arrangement()[i],
            arrangement()[(i + 1) % arrangement().size()]);
    }
    inc();
}

void arrangements::ArrangementsShuffle::next() noexcept
{
    using std::ranges::shuffle;
    shuffle(arrangement(), generator_);
    inc();
}

void arrangements::ArrangementsHeaps::next() noexcept
{
    inc();
    while (!heaps_algorithm_template_.next()) {}
}

shared_ptr<arrangements::Arrangements> arrangements::ArrangementsFactory(
        PermutationEnum permutation_type, int32_t length) noexcept
{
    using std::make_shared;
    switch (permutation_type)
    {
      case PermutationEnum::Undefined:
      case PermutationEnum::Identity:
        return make_shared<arrangements::ArrangementsIdentity>(length);
        break;
      case PermutationEnum::LexicographicForward:
        return make_shared<arrangements
            ::ArrangementsLexicographicForward>(length);
        break;
      case PermutationEnum::LexicographicBackward:
        return make_shared<arrangements
            ::ArrangementsLexicographicBackward>(length);
        break;
      case PermutationEnum::RotateRight:
        return make_shared<arrangements::ArrangementsRotateRight>(length);
        break;
      case PermutationEnum::RotateLeft:
        return make_shared<arrangements::ArrangementsRotateLeft>(length);
        break;
      case PermutationEnum::Reverse:
        return make_shared<arrangements::ArrangementsReverse>(length);
        break;
      case PermutationEnum::SwapPairs:
        return make_shared<arrangements::ArrangementsSwapPairs>(length);
        break;
      case PermutationEnum::Skip:
        return make_shared<arrangements::ArrangementsSkip>(length);
        break;
      case PermutationEnum::Shuffle:
        return make_shared<arrangements::ArrangementsShuffle>(length);
        break;
      case PermutationEnum::Heaps:
        return make_shared<arrangements::ArrangementsHeaps>(length);
        break;
    }
    return make_shared<arrangements::ArrangementsIdentity>(length);
}
