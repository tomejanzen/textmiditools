//
// TextMIDITools Version 1.0.99
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// NoteEvent is used by textmidicgm.
//
#if !defined(NOTEVENT_H)
#    define  NOTEVENT_H

#include <cstdint>

#include "RhythmRational.h"

namespace textmidi
{
    namespace cgm
    {

    inline constexpr std::int32_t RestPitch{129};

    class NoteEvent
    {
      public:
        constexpr NoteEvent(std::int32_t pitch, std::int32_t vel,
            textmidi::rational::RhythmRational rhythm) noexcept
          : pitch_{pitch},
            vel_{vel},
            rhythm_{rhythm}
        {
        }
        void pitch(std::int32_t pitch) noexcept;
        std::int32_t pitch() const noexcept;
        std::int32_t vel() const noexcept;
        textmidi::rational::RhythmRational rhythm() const noexcept;
        void rhythm(textmidi::rational::RhythmRational mr) noexcept
        {
            rhythm_ = mr;
        }
      private:
        std::int32_t pitch_;
        std::int32_t vel_;
        textmidi::rational::RhythmRational rhythm_;
      friend std::ostream& operator<<(std::ostream& , const NoteEvent& );
    };

    std::ostream& operator<<(std::ostream& os, const NoteEvent& ne);

    } // namespace cgm
} // namespace textmidi

#endif
