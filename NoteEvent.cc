//
// TextMIDITools Version 1.0.50
//
// textmidi 1.0.6
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "NoteEvent.h"
#include "MIDIKeyString.h"

using namespace textmidi;
using namespace textmidi::cgm;

void NoteEvent::pitch(int pitch) noexcept
{
    pitch_ = pitch;
}

int NoteEvent::pitch() const noexcept
{
    return pitch_;
}

int NoteEvent::vel() const noexcept
{
    return vel_;
}

textmidi::rational::RhythmRational NoteEvent::musical_rhythm() const noexcept
{
    return musical_rhythm_;
}

//
// Write a note event as a textmidi text event.
std::ostream& cgm::operator<<(std::ostream& os, const NoteEvent& ne)
{
    auto flags{os.flags()};
    std::shared_ptr<bool> prefer_sharp{std::make_shared<bool>()};
    if (RestPitch == ne.pitch())
    {
        os << "R ";
    }
    else
    {
        const auto note_name(textmidi::num_to_note(ne.pitch(), prefer_sharp));
        os << note_name << ' ';
    }
    textmidi::rational::print_rhythm(os, ne.musical_rhythm());
    static_cast<void>(os.flags(flags));
    return os;
}

