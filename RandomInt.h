//
// TextMIDITools Version 1.0.14
//
// textmidicgm 1.0
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#include <iostream>
#include <limits>
#include <random>
#include <chrono>

namespace cgm
{
    // Stroustrup Tour of C++ 2nd Ed p 191
    class RandomInt
    {
      public:
        RandomInt(std::uint64_t low = 0LU,
                  std::uint64_t high = std::numeric_limits<std::uint64_t>().max())
          : re_{},
            dist_{low, high}
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
      private:
        std::default_random_engine re_;
        std::uniform_int_distribution<std::uint64_t> dist_;
    };
}
