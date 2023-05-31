//
// TextMIDITools Version 1.0.58
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

void RunningStatus::policy(RunningStatusPolicy policy)
{
    policy_ = policy;
}

void RunningStatus::running_status(MidiStreamAtom running_status_value)
{
    running_status_valid_ = true;
    running_status_value_ = running_status_value;
}

void RunningStatus::clear()
{
    running_status_valid_ = false;
    running_status_value_ = 0u;
}

bool RunningStatus::running_status_valid() const
{
    return running_status_valid_;
}

midi::MidiStreamAtom RunningStatus::running_status_value() const
{
    return running_status_value_;
}

MidiStreamAtom RunningStatus::channel() const
{
    return running_status_value_ & midi::channel_mask;
}

MidiStreamAtom RunningStatus::command() const
{
    return running_status_value_ & ~midi::channel_mask;
}

void RunningStatus::operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
{
    const bool same{running_status_value_ == status_byte};
    if ((!same) || (RunningStatusPolicy::Never == policy_))
    {
        this->running_status(status_byte);
        track.push_back(status_byte);
    }
}

