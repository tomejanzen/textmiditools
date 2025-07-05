//
// TextMIDITools Version 1.0.99
//
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(HEAPSALGORITHMTEMPLATE_H)
#    define  HEAPSALGORITHMTEMPLATE_H

#include <cstdint>

#include <algorithm>
#include <concepts>
#include <numeric>
#include <ranges>
#include <utility>
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
        using Array = boost::multi_array<std::int32_t, 1>;
        using ArrayRef = boost::multi_array_ref<std::int32_t, 1>;
        using Range = boost::multi_array_types::extent_range;
        using SwapPair = std::pair<std::int32_t, int32_t>;

        HeapsAlgorithmTemplate(std::int32_t N, ArrayLike& array_like) noexcept
          : N_{N},
            array_like_(array_like),
            A_{extents_[Range(1, N + 1)]},
            array_ref_(array_like.data(), extents_[Range(1, N_ + 1)])
        {
            A_[1] = 1;
        }

        bool next() noexcept
        {
            auto rtn{false};
            // while (!rtn)
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
                    std::int32_t X = M_;
                    std::int32_t Y{is_even(M_) ?  A_[M_] : 1};
                    std::swap(array_ref_[X], array_ref_[Y]);
                    M_ = 2;
                    rtn = true;
                }
            }
            return rtn;
        }

      private:
        bool done_{};
        std::int32_t M_{2};
        const std::int32_t N_;
        ArrayLike& array_like_;
        Array::extent_gen extents_{};
        Array A_;
        ArrayRef array_ref_;
        void clear() noexcept
        {
            done_ = false;
            M_ = 2;
            std::ranges::fill(A_, 0);
            A_[1] = 1;
        }
        constexpr bool is_even(std::int32_t x) noexcept
        {
            return (x % 2) == 0;
        }
    };
} // namespace heaps_algorithm
#endif

