//
// TextMIDITools Version 1.0.96
//
// Copyright © 2025 Thomas E. Janzen
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

using std::ostream;
using midi::MidiStreamAtom;

void midi::MidiHeader::swapbytes() noexcept
{
    chunk_len_ = htobe32(chunk_len_);
    format_    = static_cast<midi::MIDI_Format>(htobe16(static_cast<uint16_t>
                (format_)));
    ntrks_     = htobe16(ntrks_);
    division_  = htobe16(division_);
}

void midi::RunningStatusImpl
    ::running_status(MidiStreamAtom running_status_value)
    noexcept
{
    running_status_valid_ = true;
    running_status_value_ = running_status_value;
}

void midi::RunningStatusImpl::clear() noexcept
{
    running_status_valid_ = false;
    running_status_value_ = 0u;
}

bool midi::RunningStatusImpl::running_status_valid() const noexcept
{
    return running_status_valid_;
}

midi::MidiStreamAtom midi::RunningStatusImpl::running_status_value() const
noexcept
{
    return running_status_value_;
}

MidiStreamAtom midi::RunningStatusImpl::command() const noexcept
{
    return running_status_value_ & ~midi::channel_mask;
}

MidiStreamAtom midi::RunningStatusImpl::channel() const noexcept
{
    return running_status_value_ & midi::channel_mask;
}

void midi::RunningStatusStandard::operator()(MidiStreamAtom status_byte,
    MidiStreamVector& track) noexcept
{
    // "Meta and sysex events cancel any running status that was in effect."
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

void midi::RunningStatusNever::operator()(MidiStreamAtom status_byte,
    MidiStreamVector& track) noexcept
{
    // Used for testing using old MIDI SMF files that are mal-formed.
    const bool is_sysex{start_of_sysex[0]    == status_byte};

    this->clear();
    if (!is_sysex)
    {
        track.push_back(status_byte);
    }
}

void midi::RunningStatusPersistentAfterSysex
    ::operator()(MidiStreamAtom status_byte,
    MidiStreamVector& track) noexcept
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

void midi::RunningStatusPersistentAfterMeta
    ::operator()(MidiStreamAtom status_byte,
    MidiStreamVector& track) noexcept
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

void midi::RunningStatusPersistentAfterSysexOrMeta::operator()(MidiStreamAtom
    status_byte, MidiStreamVector& track) noexcept
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

std::unique_ptr<midi::RunningStatusBase> midi::RunningStatusFactory::operator()(
    midi::RunningStatusPolicy policy) noexcept
{
    using std::make_unique;
    std::unique_ptr<midi::RunningStatusBase> rsp
        = make_unique<midi::RunningStatusStandard>();
    switch (policy)
    {
      case midi::RunningStatusPolicy::Standard:
        rsp = make_unique<midi::RunningStatusStandard>();
        break;
      case midi::RunningStatusPolicy::Never:
        rsp = make_unique<midi::RunningStatusNever>();
        break;
      case midi::RunningStatusPolicy::PersistentAfterMeta:
        rsp = make_unique<midi::RunningStatusPersistentAfterMeta>();
        break;
      case midi::RunningStatusPolicy::PersistentAfterSysex:
        rsp = make_unique<midi::RunningStatusPersistentAfterSysex>();
        break;
      case midi::RunningStatusPolicy::PersistentAfterSysexOrMeta:
        rsp = make_unique<midi::RunningStatusPersistentAfterSysexOrMeta>();
        break;
    }
    return rsp;
}


