//
// TextMIDITools Version 1.0.77
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
        ArrangementsABC()
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
        ArrangementsImpl() :
            counter_{}
        {
        }
        int counter() const;
        void counter(int counter);
        void inc();
        bool counter_finite() const;
      private:
        int counter_;
    };

    class Arrangements : public ArrangementsABC
    {
      public:
        Arrangements() :
            ArrangementsABC(), arrangements_impl_{}
        {
        }
        int counter() const;
        void counter(int counter);
        void inc();
        bool counter_finite() const;
        virtual void next() = 0;
        virtual const Arrangement& arrangement() = 0;
      private:
        ArrangementsImpl arrangements_impl_;
    };

    class ArrangementsInSitu : public Arrangements
    {
        public:
            explicit ArrangementsInSitu(int length)
              : Arrangements(), arrangement_()
            {
                auto counting = std::views::iota(0, length);
                std::ranges::copy(counting, std::back_inserter(arrangement_));
            }
            const Arrangement& arrangement() const;
            Arrangement& arrangement();
        private:
           Arrangement arrangement_;
    };

    class ArrangementsSequence : public Arrangements
    {
        public:
            explicit ArrangementsSequence(int length)
                : Arrangements(), arrangements_(), length_(length)
            {
            }
            const ArrangementVec& arrangements() const;
            ArrangementVec& arrangements();
            int length() const;
        private:
            ArrangementVec arrangements_;
            int length_;
    };

    class ArrangementsIdentity : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsIdentity(int length) : ArrangementsInSitu(length)
        {
        }
        const Arrangement& arrangement() const;
        void next()
        { }
    };
    class ArrangementsLexicographicForward : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsLexicographicForward(int length) : ArrangementsInSitu(length)
        {
        }
        void next();
    };

    class ArrangementsLexicographicBackward : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsLexicographicBackward(int length) : ArrangementsInSitu(length)
        {
        }
        void next();
    };

    class ArrangementsRotateRight : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsRotateRight(int length) : ArrangementsInSitu(length)
        {
        }
        void next();
    };

    class ArrangementsRotateLeft : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsRotateLeft(int length) : ArrangementsInSitu(length)
        {
        }
        void next();
    };

    class ArrangementsReverse : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsReverse(int length) : ArrangementsInSitu(length)
        {
        }
        void next();
    };

    class ArrangementsSwapPairs : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsSwapPairs(int length) : ArrangementsInSitu(length)
        {
        }
        void next();
    };

    class ArrangementsSkip : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsSkip(int length) : ArrangementsInSitu(length)
        {
        }
        void next();
    };

    class ArrangementsShuffle : public ArrangementsInSitu
    {
      public:
        explicit ArrangementsShuffle(int length) 
          : ArrangementsInSitu(length), random_dev_(), generator_(random_dev_())
        {
        }
        void next();
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
        explicit ArrangementsHeaps(int length = 1)
          : ArrangementsInSitu(length),
            heaps_algorithm_template_{length, arrangement()}
        {
        }

        void next();
      private:
        heaps_algorithm::HeapsAlgorithmTemplate<std::vector<int> > heaps_algorithm_template_;
    };

    std::shared_ptr<Arrangements> ArrangementsFactory(PermutationEnum permutation_type, int length);

}

#endif
