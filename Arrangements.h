//
// TextMIDITools Version 1.0.86
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
        ArrangementsABC() noexcept
        {
        }
        virtual int counter() const = 0;
        virtual void counter(int counter) = 0;
        virtual void inc() = 0;
        virtual bool counter_finite() const = 0;
        virtual void next() = 0;
        virtual const Arrangement& arrangement() = 0;
        ArrangementsABC(const ArrangementsABC& ) = default;
        ArrangementsABC(ArrangementsABC&& ) = default;
        ArrangementsABC& operator=(const ArrangementsABC& ) = default;
        ArrangementsABC& operator=(ArrangementsABC&& ) = default;
        virtual ~ArrangementsABC() = default;
    };

    class ArrangementsImpl
    {
      public:
        ArrangementsImpl() = default;
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
        Arrangements() :
            ArrangementsABC(), arrangements_impl_{}
        {
        }
        int counter() const noexcept override;
        void counter(int counter) noexcept override;
        void inc() noexcept;
        bool counter_finite() const noexcept;
        virtual void next() = 0;
        virtual const Arrangement& arrangement() const = 0;
      private:
        ArrangementsImpl arrangements_impl_;
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

    class ArrangementsSequence : public Arrangements
    {
        public:
            explicit ArrangementsSequence(int length) noexcept
              : Arrangements(), arrangements_(), length_(length)
            {
            }
            const ArrangementVec& arrangements() const noexcept;
            ArrangementVec& arrangements() noexcept;
            int length() const noexcept;
        private:
            ArrangementVec arrangements_;
            int length_;
    };

    class ArrangementsIdentity : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsIdentity(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        const Arrangement& arrangement() const noexcept override;
        void next() noexcept override
        { }
    };
    class ArrangementsLexicographicForward : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsLexicographicForward(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept override;
    };

    class ArrangementsLexicographicBackward : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsLexicographicBackward(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept override;
    };

    class ArrangementsRotateRight : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsRotateRight(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept override;
    };

    class ArrangementsRotateLeft : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsRotateLeft(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept override;
    };

    class ArrangementsReverse : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsReverse(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept override;
    };

    class ArrangementsSwapPairs : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsSwapPairs(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept override;
    };

    class ArrangementsSkip : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsSkip(int length) noexcept
          : ArrangementsInSitu(length)
        {
        }
        void next() noexcept override;
    };

    class ArrangementsShuffle : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsShuffle(int length) noexcept
          : ArrangementsInSitu(length), random_dev_(), generator_(random_dev_())
        {
        }
        void next() noexcept override;
      private:
        std::random_device random_dev_;
        std::mt19937 generator_;
    };

    // https://en.wikipedia.com: Heap's Algorithm
    // First caller passes in full length of array.
    // https://en.wikipedia.org/wiki/Heap's_algorithm
    class ArrangementsHeaps : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsHeaps(int length = 1) noexcept
          : ArrangementsInSitu(length),
            heaps_algorithm_template_{length, arrangement()}
        {
        }

        void next() noexcept override;
      private:
        heaps_algorithm::HeapsAlgorithmTemplate<std::vector<int> > heaps_algorithm_template_;
    };

    std::shared_ptr<Arrangements> ArrangementsFactory(PermutationEnum permutation_type, int length) noexcept;

}

#endif
