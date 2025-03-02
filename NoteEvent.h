//
// TextMIDITools Version 1.0.91
//
// textmidi 1.0.6
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

    inline constexpr int RestPitch{129};

    class NoteEvent
    {
      public:
        constexpr NoteEvent(int pitch, int vel, textmidi::rational::RhythmRational musical_rhythm) noexcept
          : pitch_{pitch},
            vel_{vel},
            musical_rhythm_{musical_rhythm}
        {
        }
        void pitch(int pitch) noexcept;
        int pitch() const noexcept;
        int vel() const noexcept;
        textmidi::rational::RhythmRational musical_rhythm() const noexcept;
        void musical_rhythm(textmidi::rational::RhythmRational mr) noexcept
        {
            musical_rhythm_ = mr;
        }
      private:
        int pitch_;
        int vel_;
        textmidi::rational::RhythmRational musical_rhythm_;
    };

    std::ostream& operator<<(std::ostream& os, const NoteEvent& ne);

    }
}

#endif
