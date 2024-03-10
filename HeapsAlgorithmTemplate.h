//
// TextMIDITools Version 1.0.72
//
// textmidi 1.0.6
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(HEAPSALGORITHMTEMPLATE_H)
#    define  HEAPSALGORITHMTEMPLATE_H

#include <ranges>
#include <numeric>
#include <algorithm>
#include <concepts>
#include <vector>

#include <boost/multi_array.hpp>

namespace heaps_algorithm
{

    template<typename S>
    concept MultiArrayable = requires (S a) {
      {a.data() } -> std::same_as<typename S::pointer >;
    };

    //
    // Heap, B. R. "Permutations by interchanges"
    // The Computer Journal (6)3. 293-4.  1963.
    // The British Compuer Society. London.
    template <MultiArrayable ArrayLike>
    class HeapsAlgorithmTemplate
    {
      public:
        // boost:: multi_array is used to permit
        // the use of 1-based indexing as in Heap.
        typedef boost::multi_array<int, 1> Array;
        typedef boost::multi_array_ref<int, 1> ArrayRef;
        typedef boost::multi_array_types::extent_range Range;
        typedef std::pair<int, int> SwapPair;

        HeapsAlgorithmTemplate(int N, ArrayLike& array_like)
          : done_{},
            M_{2},
            N_{N},
            array_like_(array_like),
            extents_{},
            A_{extents_[Range(1, N + 1)]},
            array_ref_(array_like.data(), extents_[Range(1, N_ + 1)])
        {
            A_[1] = 1;
        }

        bool next()
        {
            auto rtn{false};
            //while (!rtn)
            {
                ++A_[M_];
                if (A_[M_] == M_)
                {
                    A_[M_] = 0;
                    ++M_;
                    if ((done_ = (M_ > N_)))
                    {
                        clear();
                    }
                }
                else
                {
                    int X = M_;
                    int Y{is_even(M_) ?  A_[M_] : 1};
                    std::swap(array_ref_[X], array_ref_[Y]);
                    M_ = 2;
                    rtn = true;
                }
            }
            return rtn;
        }

        const ArrayLike& array_like() const
        {
            return array_like_;
        }
      private:
        bool done_;
        int M_;
        const int N_;
        ArrayLike& array_like_;
        Array::extent_gen extents_;
        Array A_;
        ArrayRef array_ref_;
        void clear()
        {
            done_ = false;
            M_ = 2;
            std::ranges::fill(A_, 0);
            A_[1] = 1;
        }
        constexpr bool is_even(int x)
        {
            return (x % 2) == 0;
        }
    };
}
#endif

