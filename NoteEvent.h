//
// TextMIDITools Version 1.0.12
//
// textmidi 1.0.6
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// NoteEvent is used by textmidicgm.
//
#if !defined(NOTEVENT)
#    define  NOTEVENT

#include <cstdint>

#include <boost/rational.hpp>

namespace cgm
{

typedef boost::rational<int64_t> MusicalRhythm;
constexpr int RestPitch{129};

class NoteEvent
{
  public:
    NoteEvent(int pitch, int vel,
                       double rhythm, MusicalRhythm musical_rhythm)
      : pitch_{pitch},
        vel_{vel},
        rhythm_{rhythm},
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
    double rhythm() const noexcept
    {
        return rhythm_;
    }
    MusicalRhythm musical_rhythm() const noexcept
    {
        return musical_rhythm_;
    }
  private:
    int pitch_;
    int vel_;
    double rhythm_;
    MusicalRhythm musical_rhythm_;
};

std::ostream& operator<<(std::ostream& os, const NoteEvent& ne);

}

#endif
