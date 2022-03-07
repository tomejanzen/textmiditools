//
// TextMIDITools Version 1.0.23
//
// textmidi 1.0.6
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// NoteEvent is used by textmidicgm.
//
#if !defined(NOTEVENT)
#    define  NOTEVENT

#include <cstdint>

#include "RhythmRational.h"

namespace textmidi
{
    namespace cgm
    {
    
    constexpr int RestPitch{129};
    
    class NoteEvent
    {
      public:
        NoteEvent(int pitch, int vel, textmidi::rational::RhythmRational musical_rhythm)
          : pitch_{pitch},
            vel_{vel},
            musical_rhythm_{musical_rhythm}
        {
        }
        NoteEvent() = default;
        NoteEvent(const NoteEvent& ) = default;
        explicit NoteEvent(NoteEvent&& ) = delete;
        NoteEvent& operator=(const NoteEvent& ) = default;
        NoteEvent& operator=(NoteEvent&& ) = delete;
    
        void pitch(int pitch) noexcept
        {
            pitch_ = pitch;
        }
        int pitch() const noexcept
        {
            return pitch_;
        }
        int vel() const noexcept
        {
            return vel_;
        }
        textmidi::rational::RhythmRational musical_rhythm() const noexcept
        {
            return musical_rhythm_;
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
