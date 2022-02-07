//
// TextMIDITools Version 1.0.15
//
// textmidi 1.0.6
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "NoteEvent.h"
#include "MIDIKeyString.h"

//
// Write a note event as a textmidi text event.
std::ostream& cgm::operator<<(std::ostream& os, const NoteEvent& ne)
{
    if (RestPitch == ne.pitch())
    {
        os << "R ";
    }
    else
    {
        const auto note_name(textmidi::num_to_note(ne.pitch()));
        os << note_name << ' ';
    }
    if ((1 == ne.musical_rhythm().numerator())
       && (ne.musical_rhythm().denominator() != 0))
    {
        os << ne.musical_rhythm().denominator();
    }
    else
    {
        // boost rational prints "2/1" and not "2", like we need.
        os << ne.musical_rhythm();
    }
    return os;
}

