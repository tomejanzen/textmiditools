//
// TextMIDITools Version 1.0.99
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(TRACK_H)
#    define  TRACK_H

#include <cstdint>

#include <chrono>
#include <ostream>
#include <vector>

#include "Midi.h"
#include "NoteEvent.h"

namespace textmidi
{
    namespace cgm
    {
        // MIDI specifies tempo as per quarter note.
        const textmidi::rational::RhythmRational WholesPerBeat{1, 4};
        const textmidi::rational::RhythmRational QuartersPerWholeRat(4, 1);

        class Track
        {
          public:
            explicit Track(std::int32_t last_pitch_index = 0) noexcept
              : last_pitch_index_(last_pitch_index)
            {
            }
            rational::RhythmRational the_next_time() const noexcept;
            std::int32_t last_pitch_index() const noexcept;
            void the_last_time(const rational::RhythmRational the_last_time) noexcept;
            void the_next_time(const rational::RhythmRational the_next_time) noexcept;
            void last_pitch_index(const std::int32_t last_pitch_index) noexcept;
          private:
            rational::RhythmRational the_last_time_{};
            rational::RhythmRational the_next_time_{};
            std::int32_t last_pitch_index_{};
        };
    } // namespace cgm
} // namespace textmidi

#endif
