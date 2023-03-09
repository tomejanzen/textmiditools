//
// TextMIDITools Version 1.0.46
//
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <endian.h>

#include <algorithm>
#include <ranges>

#include "Midi.h"

using namespace std;
using namespace midi;

void midi::MidiHeader::to_bytes(MidiStreamAtom* bytes)
{
    MidiHeader* header_bytes{reinterpret_cast<MidiHeader*>(bytes)};
    *header_bytes = *this;
    header_bytes->swapbytes();
}

void midi::MidiHeader::swapbytes()
{
    chunk_len_ = htobe32(chunk_len_);
    format_    = static_cast<MIDI_Format>(htobe16(static_cast<uint16_t>
                (format_)));
    ntrks_     = htobe16(ntrks_);
    division_  = htobe16(division_);
}



