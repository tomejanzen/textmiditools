//
// TextMIDITools Version 1.0.95
//
// textmidicgm 1.0
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(TRACK_H)
#    define  TRACK_H

#include <cstdint>

#include <ostream>
#include <vector>
#include <chrono>

#include "NoteEvent.h"

namespace textmidi
{
    namespace cgm
    {
        inline constexpr std::int64_t TicksPerQuarter(240);
        // MIDI specifies tempo as per quarter note.
        const textmidi::rational::RhythmRational WholesPerBeat{1, 4};
        const textmidi::rational::RhythmRational QuarterPerWhole{1, 4};
        const textmidi::rational::RhythmRational QuartersPerWholeRat(4, 1);
        using TicksRatio = std::ratio<1L, TicksPerQuarter>;
        using TicksDuration = std::chrono::duration<std::int64_t, TicksRatio>;

        class Track
        {
          public:
            explicit Track(std::int32_t last_pitch_index = 0) noexcept
              : last_pitch_index_(last_pitch_index)
            {
            }
            TicksDuration the_next_time() const noexcept;
            std::int32_t last_pitch_index() const noexcept;
            void the_last_time(const TicksDuration the_last_time) noexcept;
            void the_next_time(const TicksDuration the_next_time) noexcept;
            void last_pitch_index(const std::int32_t last_pitch_index) noexcept;
          private:
            TicksDuration the_last_time_{};
            TicksDuration the_next_time_{};
            std::int32_t last_pitch_index_{};
        };
    } // namespace cgm
} // namespace textmidi

#endif
