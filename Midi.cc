//
// TextMIDITools Version 1.0.34
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

#include "Midi.h"

using namespace textmidi;

void
textmidi::MidiHeader::to_bytes(std::uint8_t* bytes)
{
    MidiHeader* header_bytes{reinterpret_cast<MidiHeader*>(bytes)};
    *header_bytes = *this;
    header_bytes->swap();
}

void
textmidi::MidiHeader::swap()
{
    chunk_len_ = htobe32(chunk_len_);
    format_    = static_cast<MIDI_Format>(htobe16(static_cast<uint16_t>
                (format_)));
    ntrks_     = htobe16(ntrks_);
    division_  = htobe16(division_);
}

std::ostream&
textmidi::operator<<(std::ostream& os, textmidi::MIDI_Format mf)
{
    auto flags{os.flags()};
    textmidi::MIDI_Format mfcopy{mf};
    auto it{find_if(format_map.begin(), format_map.end(),
        [mfcopy](const FormatMap::value_type& sm)
        { return mfcopy == sm.second; })};
    if (it != format_map.end())
    {
        os << it->first;
    }
    else [[unlikely]]
    {
        os << "UNKNOWN FORMAT";
    }
    auto oldflags{os.flags(flags)};
    return os;
}

std::ostream&
textmidi::operator<<(std::ostream& os, const MidiHeader& mh)
{
    auto flags{os.flags()};
    os << mh.chunk_name_[0] << mh.chunk_name_[1] << mh.chunk_name_[2]
        << mh.chunk_name_[3] << ' ' << mh.chunk_len_ << ' ' << mh.format_
        << ' ' << mh.ntrks_ << ' ' << mh.division_;
    auto oldflags{os.flags(flags)};
    return os;
}

