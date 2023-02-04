//
// TextMIDITools Version 1.0.33
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

//
// Write a note event as a textmidi text event.
std::ostream& cgm::operator<<(std::ostream& os, const NoteEvent& ne)
{
    auto flags{os.flags()};
    if (RestPitch == ne.pitch())
    {
        os << "R ";
    }
    else
    {
        const auto note_name(textmidi::num_to_note(ne.pitch()));
        os << note_name << ' ';
    }
    textmidi::rational::print_rhythm(os, ne.musical_rhythm());
    auto oldflags{os.flags(flags)};
    return os;
}

