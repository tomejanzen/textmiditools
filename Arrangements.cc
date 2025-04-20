//
// TextMIDITools Version 1.0.97
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#include <ranges>
#include <utility>

#if defined(TEXTMIDICGM_PRINT)
#include <iostream>
#include <iterator>
#endif

#include "Arrangements.h"

using std::unique_ptr, std::ranges::rotate;

int32_t arrangements::ArrangementsImpl::counter() const noexcept
{
    return counter_;
}

void arrangements::ArrangementsImpl::inc() noexcept
{
    ++counter_;
}

int32_t arrangements::Arrangements::counter() const noexcept
{
    return arrangements_impl_.counter();
}

void arrangements::Arrangements::inc() noexcept
{
    arrangements_impl_.inc();
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
    rotate(arrangement(), arrangement().begin() + (arrangement().size() - 1));
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
              it < (arrangement().end() - (arrangement().size() % 2UL));
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
    for (int32_t i{anchor}; std::cmp_less(i, arrangement().size() - (arrangement().size() % 2UL)); i += 2)
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

unique_ptr<arrangements::Arrangements> arrangements::ArrangementsFactory(
        PermutationEnum permutation_type, int32_t length) noexcept
{
    using std::make_unique;
    switch (permutation_type)
    {
      case PermutationEnum::Undefined:
      case PermutationEnum::Identity:
        return std::move(make_unique<arrangements::ArrangementsIdentity>(length));
        break;
      case PermutationEnum::LexicographicForward:
        return std::move(make_unique<arrangements
            ::ArrangementsLexicographicForward>(length));
        break;
      case PermutationEnum::LexicographicBackward:
        return std::move(make_unique<arrangements
            ::ArrangementsLexicographicBackward>(length));
        break;
      case PermutationEnum::RotateRight:
        return std::move(make_unique<arrangements::ArrangementsRotateRight>(length));
        break;
      case PermutationEnum::RotateLeft:
        return std::move(make_unique<arrangements::ArrangementsRotateLeft>(length));
        break;
      case PermutationEnum::Reverse:
        return std::move(make_unique<arrangements::ArrangementsReverse>(length));
        break;
      case PermutationEnum::SwapPairs:
        return std::move(make_unique<arrangements::ArrangementsSwapPairs>(length));
        break;
      case PermutationEnum::Skip:
        return std::move(make_unique<arrangements::ArrangementsSkip>(length));
        break;
      case PermutationEnum::Shuffle:
        return std::move(make_unique<arrangements::ArrangementsShuffle>(length));
        break;
      case PermutationEnum::Heaps:
        return std::move(make_unique<arrangements::ArrangementsHeaps>(length));
        break;
    }
    return std::move(make_unique<arrangements::ArrangementsIdentity>(length));
}
