//
// TextMIDITools Version 1.1.0
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(RANDOMINT_H)
#    define  RANDOMINT_H

#include <chrono>
#include <iostream>
#include <limits>
#include <random>

namespace textmidi
{
    namespace cgm
    {
        // Stroustrup Tour of C++ 2nd Ed p 191
        class RandomInt
        {
          public:
            using result_type = std::uint64_t;
            explicit RandomInt(std::uint64_t low = 0UL,
                std::uint64_t high
                = std::numeric_limits<std::uint64_t>().max()) noexcept
              : dist_{low, high},
                low_{low},
                high_{high}
            {
                std::chrono::time_point<std::chrono::system_clock>
                    today(std::chrono::system_clock::now());
                std::int64_t count{today.time_since_epoch().count()};
                const std::int32_t seed((count & 0x00000000FFFFFFFF)
                    ^ (count >> 32));
                re_.seed(static_cast<double>(seed));
            }

            std::uint64_t operator()() noexcept
            {
                return dist_(re_);
            }

            std::uint64_t min() const noexcept
            {
                return low_;
            }

            std::uint64_t max() const noexcept
            {
                return high_ + 1;
            }

          private:
            std::default_random_engine re_{};
            std::uniform_int_distribution<std::uint64_t> dist_;
            std::uint64_t low_;
            std::uint64_t high_;
        };
    } // namespace cgm
} // namespace textmidi
#endif

