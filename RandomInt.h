//
// TextMIDITools Version 1.0.24
//
// textmidicgm 1.0
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(RANDOMINT)
#    define  RANDOMINT
#include <iostream>
#include <limits>
#include <random>
#include <chrono>

namespace textmidi
{
    namespace cgm
    {
        // Stroustrup Tour of C++ 2nd Ed p 191
        class RandomInt
        {
          public:
            typedef std::uint64_t result_type;
            RandomInt(std::uint64_t low = 0LU,
                      std::uint64_t high = std::numeric_limits<std::uint64_t>().max())
              : re_{},
                dist_{low, high},
                low_{low},
                high_{high}
            {
                std::chrono::time_point<std::chrono::system_clock>
                    today(std::chrono::system_clock::now());
                std::int64_t count{today.time_since_epoch().count()};
                const std::int32_t seed((count & 0x00000000FFFFFFFF) ^ (count >> 32));
                re_.seed(static_cast<double>(seed));
            }
            std::uint64_t operator()()
            {
                return dist_(re_);
            }
            std::uint64_t min() const
            {
                return low_;
            }
            std::uint64_t max() const
            {
                return high_ + 1;
            }
          private:
            std::default_random_engine re_;
            std::uniform_int_distribution<std::uint64_t> dist_;
            std::uint64_t low_;
            std::uint64_t high_;
        };
    }
}
#endif

