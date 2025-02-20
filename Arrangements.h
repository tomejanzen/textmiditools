//
// TextMIDITools Version 1.0.89
//
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(ARRANGEMENTS_H)
#    define  ARRANGEMENTS_H

#include <algorithm>
#include <vector>
#include <iterator>
#include <memory>
#include <cstdint>
#include <ranges>
#include <random>
#include <numeric>

#include "HeapsAlgorithmTemplate.h"

namespace arrangements
{
    using Arrangement = std::vector<int>;
    using ArrangementVec = std::vector<Arrangement>;
    enum class PermutationEnum : int
    {
        Undefined,
        Identity,
        LexicographicForward,
        LexicographicBackward,
        RotateRight,
        RotateLeft,
        Reverse,
        SwapPairs,
        Skip,
        Shuffle,
        Heaps,
    };

    class ArrangementsABC
    {
      public:
        ArrangementsABC() = default;
        virtual int counter() const = 0;
        virtual void counter(int counter) = 0;
        virtual void inc() = 0;
        virtual bool counter_finite() const noexcept = 0;
        virtual void next() = 0;
        virtual const Arrangement& arrangement() const noexcept = 0;
        virtual Arrangement& arrangement() noexcept = 0;
        ArrangementsABC(const ArrangementsABC& ) = default;
        ArrangementsABC(ArrangementsABC&& ) = default;
        ArrangementsABC& operator=(const ArrangementsABC& ) = default;
        ArrangementsABC& operator=(ArrangementsABC&& ) = default;
        virtual ~ArrangementsABC() = default;
    };

    class ArrangementsImpl
    {
      public:
        int counter() const noexcept;
        void counter(int counter) noexcept;
        void inc() noexcept;
        bool counter_finite() const noexcept;
      private:
        int counter_{};
    };

    class Arrangements : public ArrangementsABC
    {
      public:
        Arrangements()  = default;
        int counter() const noexcept final;
        void counter(int counter) noexcept final;
        void inc() noexcept final;
        bool counter_finite() const noexcept final;
      private:
        ArrangementsImpl arrangements_impl_{};
    };

    class ArrangementsInSitu : public Arrangements
    {
        public:
            explicit ArrangementsInSitu(int length) noexcept
              : Arrangements(), arrangement_()
            {
                auto counting = std::views::iota(0, length);
                std::ranges::copy(counting, std::back_inserter(arrangement_));
            }
            const Arrangement& arrangement() const noexcept override;
            Arrangement& arrangement() noexcept override;
        private:
           Arrangement arrangement_;
    };

    class ArrangementsIdentity final : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsIdentity(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        const Arrangement& arrangement() const noexcept final;
        void next() noexcept final
        { }
    };
    class ArrangementsLexicographicForward final : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsLexicographicForward(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept final;
    };

    class ArrangementsLexicographicBackward final : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsLexicographicBackward(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept final;
    };

    class ArrangementsRotateRight final : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsRotateRight(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept final;
    };

    class ArrangementsRotateLeft final : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsRotateLeft(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept final;
    };

    class ArrangementsReverse final : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsReverse(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept final;
    };

    class ArrangementsSwapPairs final : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsSwapPairs(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept final;
    };

    class ArrangementsSkip final : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsSkip(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept final;
    };

    class ArrangementsShuffle final : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsShuffle(int length) noexcept
          : ArrangementsInSitu(length), random_dev_(), generator_(random_dev_())
        {
        }
        void next() noexcept final;
      private:
        std::random_device random_dev_;
        std::mt19937 generator_;
    };

    // https://en.wikipedia.com: Heap's Algorithm
    // First caller passes in full length of array.
    // https://en.wikipedia.org/wiki/Heap's_algorithm
    class ArrangementsHeaps final : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsHeaps(int length = 1) noexcept
          : ArrangementsInSitu(length),
            heaps_algorithm_template_(length, arrangement())
        {
        }

        void next() noexcept final;
      private:
        heaps_algorithm::HeapsAlgorithmTemplate<std::vector<int> > heaps_algorithm_template_;
    };

    std::shared_ptr<Arrangements> ArrangementsFactory(PermutationEnum permutation_type, int length) noexcept;

}

#endif
