//
// TextMIDITools Version 1.0.73
//
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <endian.h>

#include <set>
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

void RunningStatusImpl::running_status(MidiStreamAtom running_status_value)
{
    running_status_valid_ = true;
    running_status_value_ = running_status_value;
}

void RunningStatusImpl::clear()
{
    running_status_valid_ = false;
    running_status_value_ = 0u;
}

bool RunningStatusImpl::running_status_valid() const
{
    return running_status_valid_;
}

midi::MidiStreamAtom RunningStatusImpl::running_status_value() const
{
    return running_status_value_;
}

MidiStreamAtom RunningStatusImpl::command() const
{
    return running_status_value_ & ~midi::channel_mask;
}

MidiStreamAtom RunningStatusImpl::channel() const
{
    return running_status_value_ & midi::channel_mask;
}

void RunningStatusStandard::operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
{
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    const bool is_same{this->running_status_value() == status_byte};
    const bool is_meta{ meta_prefix[0]       == status_byte};
    const bool is_sysex{start_of_sysex[0]    == status_byte};
    const bool is_end_of_track{end_of_track_prefix[0] == status_byte};

    if (is_end_of_track || is_meta || is_sysex)
    {
        this->clear();
    }
    // SYSEX messages are built whole in a separate function,
    // but META messages are appended to the 0xFF.
    if ((!is_same && !is_sysex) || is_meta)
    {
        track.push_back(status_byte);
    }
    if (!is_sysex || !is_meta)
    {
        running_status(status_byte);
    }
}

void RunningStatusNever::operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
{
    // Used for testing using old MIDI SMF files that are mal-formed.
    const bool is_sysex{start_of_sysex[0]    == status_byte};

    this->clear();
    if (!is_sysex)
    {
        track.push_back(status_byte);
    }
}

void RunningStatusPersistentAfterSysex::operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
{
    // Supporting broken old MIDI files for testing purposes.
    const bool is_same{this->running_status_value() == status_byte};
    const bool is_meta{ meta_prefix[0]       == status_byte};
    const bool is_sysex{start_of_sysex[0]    == status_byte};
    const bool is_end_of_track{end_of_track_prefix[0] == status_byte};

    if (is_end_of_track || is_meta)
    {
        this->clear();
    }
    // SYSEX messages are built whole in a separate function,
    // but META messages are appended to the 0xFF.
    if ((!is_same && !is_sysex) || is_meta)
    {
        track.push_back(status_byte);
    }
    // Never save sysex or meta as running status
    if (!is_sysex || !is_meta)
    {
        running_status(status_byte);
    }
}

void RunningStatusPersistentAfterMeta::operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
{
    // Supporting broken old MIDI files for testing purposes.
    const bool is_same{this->running_status_value() == status_byte};
    const bool is_meta{ meta_prefix[0]       == status_byte};
    const bool is_sysex{start_of_sysex[0]    == status_byte};
    const bool is_end_of_track{end_of_track_prefix[0] == status_byte};

    if (is_end_of_track || is_sysex)
    {
        this->clear();
    }
    if ((!is_same && !is_sysex) || is_meta)
    {
        track.push_back(status_byte);
    }
    // Never save sysex or meta as running status
    if (!is_sysex || !is_meta)
    {
        running_status(status_byte);
    }
}

void RunningStatusPersistentAfterSysexOrMeta::operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
{
    // Supporting broken old MIDI files for testing purposes.
    const bool is_same{this->running_status_value() == status_byte};
    const bool is_meta{ meta_prefix[0]       == status_byte};
    const bool is_sysex{start_of_sysex[0]    == status_byte};
    const bool is_end_of_track{end_of_track_prefix[0] == status_byte};

    if (is_end_of_track)
    {
        this->clear();
    }
    if ((!is_same && !is_sysex) || is_meta)
    {
        track.push_back(status_byte);
    }
    // Never save sysex or meta as running status
    if (!is_sysex || !is_meta)
    {
        running_status(status_byte);
    }
}

RunningStatusBase* RunningStatusFactory::operator()(RunningStatusPolicy policy)
{
    switch (policy)
    {
      case RunningStatusPolicy::Standard:
        return new RunningStatusStandard();
        break;
      case RunningStatusPolicy::Never:
        return new RunningStatusNever();
        break;
      case RunningStatusPolicy::PersistentAfterMeta:
        return new RunningStatusPersistentAfterMeta();
        break;
      case RunningStatusPolicy::PersistentAfterSysex:
        return new RunningStatusPersistentAfterSysex();
        break;
      case RunningStatusPolicy::PersistentAfterSysexOrMeta:
        return new RunningStatusPersistentAfterSysexOrMeta();
        break;
      default:
        return new RunningStatusStandard();
        break;
    }
}


