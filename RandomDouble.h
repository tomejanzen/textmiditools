//
// TextMIDITools Version 1.0.84
//
// textmidicgm 1.0
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(RANDOMDOUBLE_H)
#    define  RANDOMDOUBLE_H

#include <random>
#include <cstdint>
#include <chrono>

namespace textmidi
{
    namespace cgm
    {

        class RandomDouble
        {
          public:
            using result_type = double;
            explicit RandomDouble(double low = 0.0, double high = 1.0)
              : re_{},
                dist_{low, high}
            {
                std::chrono::time_point<std::chrono::system_clock>
                    today(std::chrono::system_clock::now());
                std::int64_t count{today.time_since_epoch().count()};
                const std::int32_t seed((count & 0x00000000FFFFFFFF) ^ (count >> 32));

                re_.seed(static_cast<double>(seed));
            }
            double operator()()
            {
                return dist_(re_);
            }
          private:
            std::default_random_engine re_;
            std::uniform_real_distribution<> dist_;
        };

    }
}
#endif

