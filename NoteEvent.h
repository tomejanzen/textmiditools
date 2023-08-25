//
// TextMIDITools Version 1.0.65
//
// textmidi 1.0.6
// Copyright © 2023 Thomas E. Janzen
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
        constexpr NoteEvent(int pitch, int vel, textmidi::rational::RhythmRational musical_rhythm)
          : pitch_{pitch},
            vel_{vel},
            musical_rhythm_{musical_rhythm}
        {
        }
        void pitch(int pitch) noexcept;
        int pitch() const noexcept;
        int vel() const noexcept;
        constexpr textmidi::rational::RhythmRational musical_rhythm() const noexcept;
      private:
        int pitch_;
        int vel_;
        textmidi::rational::RhythmRational musical_rhythm_;
    };

    std::ostream& operator<<(std::ostream& os, const NoteEvent& ne);

    }
}

#endif
