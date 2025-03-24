//
// TextMIDITools Version 1.0.92
//
// textmidi 1.0.6
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <ostream>
#include <ranges>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <boost/lexical_cast.hpp>

#include "MidiEvents.h"
#include "MidiMaps.h"
#include "rational_support.h"
#include "MIDIKeyString.h"
#include "MidiString.h"
#include "Midi.h"

using std::string_view, std::string, std::ostream, std::shared_ptr,
      std::ostream_iterator;
using std::copy_n, std::ranges::for_each, std::ranges::find, std::hex,
      std::setw, std::setfill, std::dec, std::make_shared, std::cerr, std::list,
      std::tuple, std::int32_t, std::optional, std::tie, std::ostringstream,
      std::copy, std::equal;
using midi::MidiStreamRange, midi::MidiStreamIterator, midi::MidiStreamAtom,
      midi::variable_len_flag, midi::variable_len_shift,
      midi::variable_len_byte_mask, midi::variable_len_word_mask,
      midi::end_of_sysex, midi::meta_prefix, midi::key_signature_prefix,
      midi::variable_length_quantity_min_len, midi::RunningStatusStandard,
      midi::event_flag, midi::full_note_length, midi::program, midi::note_off,
      midi::byte7_mask, midi::byte7_shift, midi::note_on, midi::MaxDynamic,
      midi::channel_mask, midi::QuartersPerWhole;
using textmidi::MidiEvent,
      textmidi::rational::RhythmRational, textmidi::rational::print_rhythm,
      textmidi::MidiEventImpl,
      textmidi::MidiSysExEvent, textmidi::MidiSysExRawEvent,
      textmidi::MidiFileMetaEvent, textmidi::MidiFileMetaSequenceEvent,
      textmidi::MidiFileMetaUnknownEvent,
      textmidi::MidiFileMetaMidiChannelEvent,
      textmidi::MidiFileMetaSetTempoEvent,
      textmidi::MidiFileMetaSMPTEOffsetEvent,
      textmidi::MidiFileMetaMidiPortEvent,
      textmidi::MidiFileMetaTimeSignatureEvent,
      textmidi::MidiFileMetaKeySignatureEvent,
      textmidi::MidiFileMetaXmfPatchTypeEvent,
      textmidi::MidiFileMetaSequencerSpecificEvent,
      textmidi::MidiFileMetaEndOfTrackEvent, textmidi::MidiFileMetaStringEvent,
      textmidi::MidiFileMetaTextEvent, textmidi::MidiFileMetaProgramNameEvent,
      textmidi::MidiFileMetaDeviceNameEvent, textmidi::MidiFileMetaText0AEvent,
      textmidi::MidiFileMetaText0BEvent, textmidi::MidiFileMetaText0CEvent,
      textmidi::MidiFileMetaText0DEvent, textmidi::MidiFileMetaText0EEvent,
      textmidi::MidiFileMetaText0FEvent, textmidi::MidiFileMetaCopyrightEvent,
      textmidi::MidiFileMetaTrackNameEvent,
      textmidi::MidiFileMetaInstrumentEvent,
      textmidi::MidiFileMetaLyricEvent, textmidi::MidiFileMetaMarkerEvent,
      textmidi::MidiFileMetaCuePointEvent, textmidi::MidiChannelVoiceModeEvent,
      textmidi::MidiChannelVoiceNoteEvent,
      textmidi::MidiChannelVoiceNoteOnEvent,
      textmidi::MidiChannelVoiceNoteOffEvent,
      textmidi::MidiChannelVoiceProgramChangeEvent,
      textmidi::MidiChannelVoicePitchBendEvent,
      textmidi::MidiChannelVoiceControlChangeEvent,
      textmidi::MidiChannelVoiceChannelPressureEvent,
      textmidi::MidiChannelVoicePolyphonicKeyPressureEvent,
      textmidi::MidiChannelVoiceNoteRestEvent, textmidi::MidiEventFactory,
      textmidi::DelayEvent;

tuple<MidiStreamRange, int64_t> textmidi::variable_len_value(MidiStreamRange midi_stream_tail) noexcept
{
    uint64_t len{};
    auto midiiter{midi_stream_tail.begin()};
    while ((*midiiter) & midi::variable_len_flag)
    {
        len <<= midi::variable_len_shift;
        len |= (*(midiiter++) & midi::variable_len_byte_mask);
    }
    len <<= midi::variable_len_shift;
    len |= *midiiter;
    midiiter++;
    len &= midi::variable_len_word_mask;
    midi_stream_tail.advance(midiiter - midi_stream_tail.begin());
    return tuple(midi_stream_tail, len);
}

midi::MidiStreamAtom textmidi::channel_0_to_1(midi::MidiStreamAtom channel) noexcept
{
    ++channel;
    return channel;
}

namespace
{
    string dynamic_string(int32_t velocity) noexcept
    {
        using midi::dynamics_map;
        string rtn{};
        // If it matches the velocity map then use a symbol instead of vel nn
        if (dynamics_map.contains(velocity))
        {
            rtn = dynamics_map.at(velocity);
        }
        else
        {
            (rtn += "vel ") += boost::lexical_cast<string>(velocity);
        }
        rtn += ' ';
        return rtn;
    }
} // namespace

//
// Key names, major and minor.
// Based on MIDI Specification 1.1.
textmidi::KeySignatureMap textmidi::key_signature_map{
    {{ 0, false}, string_view{"C" }},
    {{-1, false}, string_view{"F" }},
    {{-2, false}, string_view{"Bb"}},
    {{-3, false}, string_view{"Eb"}},
    {{-4, false}, string_view{"Ab"}},
    {{-5, false}, string_view{"Db"}},
    {{-6, false}, string_view{"Gb"}},
    {{-7, false}, string_view{"Cb"}},
    {{ 1, false}, string_view{"G" }},
    {{ 2, false}, string_view{"D" }},
    {{ 3, false}, string_view{"A" }},
    {{ 4, false}, string_view{"E" }},
    {{ 5, false}, string_view{"B" }},
    {{ 6, false}, string_view{"F#"}},
    {{ 7, false}, string_view{"C#"}},
    {{ 0, true }, string_view{"a" }},
    {{-1, true }, string_view{"d" }},
    {{-2, true }, string_view{"g" }},
    {{-3, true }, string_view{"c" }},
    {{-4, true }, string_view{"f" }},
    {{-5, true }, string_view{"bb"}},
    {{-6, true }, string_view{"eb"}},
    {{-7, true }, string_view{"ab"}},
    {{ 1, true }, string_view{"e" }},
    {{ 2, true }, string_view{"b" }},
    {{ 3, true }, string_view{"f#"}},
    {{ 4, true }, string_view{"c#"}},
    {{ 5, true }, string_view{"g#"}},
    {{ 6, true }, string_view{"d#"}},
    {{ 7, true }, string_view{"a#"}}
};

//
// Create a variable-length value.

ostream& textmidi::operator<<(ostream& os, const DelayEvent& msg_pair)
{
    if (msg_pair.first > 0) [[likely]]
    {
        os << "DELAY " << dec << msg_pair.first << ' ';
    }
    return os << *msg_pair.second;
}

int32_t MidiEventImpl::events_to_next_note_on() const noexcept
{
    return events_to_next_note_on_;
}

void MidiEventImpl::events_to_next_note_on(int32_t events_to_next_note_on) noexcept
{
    events_to_next_note_on_ = events_to_next_note_on;
}

//
// return the accumulated MIDI ticks.
constexpr int64_t textmidi::MidiEvent::ticks_accumulated() const noexcept
{
    return midi_event_impl_.ticks_accumulated();
}

//
// set the accumulated MIDI ticks.
void textmidi::MidiEvent::ticks_accumulated(int64_t ticks_accumulated) noexcept
{
    midi_event_impl_.ticks_accumulated(ticks_accumulated);
}

//
// return the MIDI ticks to the next event.
constexpr int64_t textmidi::MidiEvent::ticks_to_next_event() const noexcept
{
    return midi_event_impl_.ticks_to_next_event();
}

//
// set the MIDI ticks to the next event.
void textmidi::MidiEvent::ticks_to_next_event(int64_t ticks_to_next_event)
    noexcept
{
    midi_event_impl_.ticks_to_next_event(ticks_to_next_event);
}

constexpr int64_t textmidi::MidiEvent::ticks_to_a_note_start() const noexcept
{
    return midi_event_impl_.ticks_to_a_note_start();
}

void textmidi::MidiEvent::ticks_to_a_note_start(int64_t ticks_to_a_note_start)
    noexcept
{
    midi_event_impl_.ticks_to_a_note_start(ticks_to_a_note_start);
}

constexpr RhythmRational textmidi::MidiEvent::wholes_to_next_event()
    const noexcept
{
    return midi_event_impl_.wholes_to_next_event();
}

int32_t textmidi::MidiEvent::events_to_next_note_on() const noexcept
{
    return midi_event_impl_.events_to_next_note_on();
}

void textmidi::MidiEvent::events_to_next_note_on(int32_t events_to_next_note_on)
    noexcept
{
    midi_event_impl_.events_to_next_note_on(events_to_next_note_on);
}

void textmidi::MidiEvent::wholes_to_next_event(
    const RhythmRational& wholes_to_next_event) noexcept
{
    midi_event_impl_.wholes_to_next_event(wholes_to_next_event);
    midi_event_impl_.reduce();
}

void textmidi::MidiEventImpl::reduce() noexcept
{
    wholes_to_next_event_.reduce();
}

//
// return the accumulated MIDI ticks.
constexpr int64_t textmidi::MidiEventImpl::ticks_accumulated() const noexcept
{
    return ticks_accumulated_;
}

//
// set the accumulated MIDI ticks.
void textmidi::MidiEventImpl::ticks_accumulated(int64_t ticks_accumulated)
    noexcept
{
    ticks_accumulated_ = ticks_accumulated;
}

//
// return the MIDI ticks to the next event.
constexpr int64_t textmidi::MidiEventImpl::ticks_to_next_event() const noexcept
{
    return ticks_to_next_event_;
}

//
// set the MIDI ticks to the next event.
void textmidi::MidiEventImpl::ticks_to_next_event(int64_t ticks_to_next_event)
    noexcept
{
    ticks_to_next_event_ = ticks_to_next_event;
}

constexpr int64_t textmidi::MidiEventImpl::ticks_to_a_note_start()
    const noexcept
{
    return ticks_to_a_note_start_;
}

void textmidi::MidiEventImpl::ticks_to_a_note_start(
    int64_t ticks_to_a_note_start) noexcept
{
    ticks_to_a_note_start_ = ticks_to_a_note_start;
}

constexpr RhythmRational textmidi::MidiEventImpl::wholes_to_next_event()
    const noexcept
{
    return wholes_to_next_event_;
}

void textmidi::MidiEventImpl::wholes_to_next_event(
        const RhythmRational& wholes_to_next_event) noexcept
{
    wholes_to_next_event_ = wholes_to_next_event;
    wholes_to_next_event_.reduce();
}

ostream& textmidi::operator<<(ostream& os, const MidiEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>>
    MidiSysExEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    // recall C++ can evaluate expressions in && in any order prior to testing.
    // Don't have side-effects in those expressions.
    if ((midi_stream_tail.size() >= midi::start_of_sysex.size())
        && (midi::start_of_sysex[0] == midi_stream_tail[0]))
    {
        auto evt = make_shared<MidiSysExEvent>();
        midi_stream_tail.advance(1);
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiSysExEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    int64_t len{};
    tie(midi_stream_tail, len) = variable_len_value(midi_stream_tail);
    int64_t count{};
    do
    {
        data_.push_back(midi_stream_tail[0]);
    } while ((midi_stream_tail.advance(1)[0] != midi::end_of_sysex[0]) && (++count < len));
    if (midi_stream_tail[0] == midi::end_of_sysex[0])
    {
        midi_stream_tail.advance(1);
    }
    return midi_stream_tail;
}

ostream& MidiSysExEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << hex << "SYSEX";
    int32_t i{};
    const auto subid = midi::sysex_subid_map(data_[i]);
    if (subid)
    {
        // subid
        os << ' ' << *subid;
        ++i;
        // device id
        const auto id = midi::sysex_device_id_map(data_[i]);
        if (id)
        {
            os << ' ' << *id;
        }
        else
        {
            os << ' ' << hex << "0x" << setw(2) << setfill('0')
               << static_cast<uint32_t>(data_[i]);
        }
        ++i;
        // subid1
        switch (data_[0])
        {
          case midi::sysex_subid_non_commercial[0]:
            break;
          case midi::sysex_subid_non_realtime[0]:
            {
                const auto id1 = midi::sysex_nonrt_id1_map(data_[i]);
                if (id1)
                {
                    os << ' ' << *id1;
                    ++i;
                }
            }
            break;
          case midi::sysex_subid_realtime[0]:
            {
                const auto id1 = midi::sysex_rt_id1_map(data_[i]);
                if (id1)
                {
                    os << ' ' << *id1;
                    ++i;
                }
            }
            break;
        }
    }
    if (static_cast<size_t>(i) < data_.size())
    {
        for (auto it(data_.cbegin() + i);
                (it != data_.cend()) && (*it != end_of_sysex[0]);
                ++it)
        {
            os << ' ' << hex << "0x" << setw(2) << setfill('0')
               << static_cast<uint32_t>(*it);
        }
    }
    os << dec;
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiSysExEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>>
    MidiSysExRawEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    int i{};
    if ((midi_stream_tail.size() >= static_cast<int64_t>(midi::end_of_sysex.size()))
        && (midi::end_of_sysex[0] == midi_stream_tail[i]))
    {
        midi_stream_tail.advance(1);
        auto evt = make_shared<MidiSysExRawEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiSysExRawEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    int64_t len{};
    tie(midi_stream_tail, len) = variable_len_value(midi_stream_tail);
    copy_n(midi_stream_tail.begin(), len, back_inserter(data_));
    midi_stream_tail.advance(len);
    return midi_stream_tail;
}

ostream& MidiSysExRawEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << hex << "SYSEXRAW";
    for (auto sys_ex_it(data_.cbegin());
            (sys_ex_it != data_.cend()) && (*sys_ex_it != end_of_sysex[0]);
            ++sys_ex_it)
    {
        os << ' ' << hex << "0x"
           << setw(2) << setfill('0') << static_cast<uint32_t>(*sys_ex_it);
    }
    os << dec;
    static_cast<void>(os.flags(flags));
    return os;
}

bool MidiFileMetaEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    return ((midi_stream_tail.size() >= static_cast<int64_t>(midi::meta_prefix.size()))
        && (midi::meta_prefix[0] == midi_stream_tail[0]));
}

ostream& textmidi::operator<<(ostream& os, const MidiSysExRawEvent& msg)
{
    return msg.print(os);
}

void textmidi::MidiFileMetaSequenceEvent::sequence_number(
    uint16_t sequence_number) noexcept
{
    sequence_number_ = sequence_number;
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>>
    MidiFileMetaSequenceEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::sequence_number_prefix;
    if ((midi_stream_tail.size() >= static_cast<int64_t>(midi::meta_prefix.size() + midi::sequence_number_prefix.size()))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && equal(sequence_number_prefix.cbegin(),
            sequence_number_prefix.cend(), midi_stream_tail.begin() + 1))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::sequence_number_prefix.size());
        auto evt = make_shared<MidiFileMetaSequenceEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaSequenceEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    auto msb{midi_stream_tail[0]};
    auto lsb{midi_stream_tail.advance(1)[0]};
    midi_stream_tail.advance(1);
    sequence_number_ = (msb << midi::bits_per_byte) | lsb;
    return midi_stream_tail;
}

ostream& MidiFileMetaSequenceEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << "SEQUENCE_NUMBER " << sequence_number_;
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSequenceEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaUnknownEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    if (((midi_stream_tail.size() >=
        (midi::meta_prefix.size() + midi::unknown_prefix.size()))
        && MidiFileMetaEvent::recognize(midi_stream_tail))
        && !midi::Initial_Meta.contains(midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size());
        auto evt = make_shared<MidiFileMetaUnknownEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaUnknownEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    meta_code_ = midi_stream_tail[0];
    midi_stream_tail.advance(1);
    int64_t len{};
    // get the length of the data
    tie(midi_stream_tail, len) = variable_len_value(midi_stream_tail);
    // copy the data
    copy_n(midi_stream_tail.begin(), len, back_inserter(data_));
    midi_stream_tail.advance(len);
    return midi_stream_tail;
}

ostream& MidiFileMetaUnknownEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << "UNKNOWN_META " << hex << setw(2) << setfill('0') << "0x"
        << static_cast<int32_t>(meta_code_);
    for_each(data_, [&](MidiStreamAtom msa) {
        os << " " << hex << "0x" << setw(2) << setfill('0')
           << static_cast<uint32_t>(msa);});
    os << dec;
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaUnknownEvent& msg)
{
    return msg.print(os);
}

void textmidi::MidiFileMetaMidiChannelEvent::channel(uint16_t channel) noexcept
{
    channel_ = channel;
}

constexpr uint16_t textmidi::MidiFileMetaMidiChannelEvent::channel() noexcept
{
    return channel_;
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>>
    MidiFileMetaMidiChannelEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::midi_channel_prefix;

    if ((midi_stream_tail.size() >= (midi::meta_prefix.size()
        + midi::midi_channel_prefix.size()))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && equal(midi_channel_prefix.cbegin(), midi_channel_prefix.cend(),
                midi_stream_tail.begin() + 1))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::midi_channel_prefix.size());
        auto evt = make_shared<MidiFileMetaMidiChannelEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaMidiChannelEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    channel_ = midi_stream_tail[0];
    channel_ = channel_0_to_1(channel_);
    midi_stream_tail.advance(1);
    if (channel_ > midi::MidiChannelQty)
    {
        const string errstr{(((string{"MIDI channel ("}
            += boost::lexical_cast<string>(static_cast<int32_t>(channel_)))
            += ") exceeded MIDI limit (")
            += boost::lexical_cast<string>(midi::MidiChannelQty))
            += ")\n"};
        cerr << errstr;
    }
    return midi_stream_tail;
}

ostream& MidiFileMetaMidiChannelEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << "MIDI_CHANNEL " << channel_;
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaMidiChannelEvent& msg)
{
    return msg.print(os);
}

void textmidi::MidiFileMetaSetTempoEvent::tempo(uint32_t tempo) noexcept
{
    tempo_ = tempo;
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaSetTempoEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::tempo_prefix;

    if ((midi_stream_tail.size() >= (midi::meta_prefix.size()
        + midi::tempo_prefix.size()))
       && MidiFileMetaEvent::recognize(midi_stream_tail)
       && equal(tempo_prefix.cbegin(), tempo_prefix.cend(), midi_stream_tail.begin() + 1))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::tempo_prefix.size());
        auto evt = make_shared<MidiFileMetaSetTempoEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaSetTempoEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    tempo_ = midi_stream_tail[0];
    tempo_ <<= 8;
    tempo_ |= midi_stream_tail.advance(1)[0];
    tempo_ <<= 8;
    tempo_ |= midi_stream_tail.advance(1)[0];
    midi_stream_tail.advance(1);
    return midi_stream_tail;
}

ostream& MidiFileMetaSetTempoEvent::print(ostream& os) const
{
    using std::setprecision;
    auto flags{os.flags()};
    if (0 == tempo_)
    {
        os << "TEMPO 0";
    }
    else
    {
        os << "TEMPO " << dec << setprecision(10)
           << ((midi::SecondsPerMinute * midi::UsecPerSecond)
           / static_cast<double>(tempo_));
    }
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSetTempoEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>>
    MidiFileMetaSMPTEOffsetEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::smpte_prefix;

    if ((midi_stream_tail.size() >= (midi::meta_prefix.size()
        + midi::smpte_prefix.size()))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && equal(smpte_prefix.cbegin(), smpte_prefix.cend(), midi_stream_tail.begin() + 1))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::smpte_prefix.size());
        auto evt = make_shared<MidiFileMetaSMPTEOffsetEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaSMPTEOffsetEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    hours_ = midi_stream_tail[0];
    fps_ = (hours_ >> midi::smpte_fps_shift) & midi::smpte_fps_mask;
    hours_ &= midi::smpte_hours_mask;
    minutes_ = midi_stream_tail.advance(1)[0];
    seconds_ = midi_stream_tail.advance(1)[0];
    fr_ = midi_stream_tail.advance(1)[0];
    ff_ = midi_stream_tail.advance(1)[0];
    midi_stream_tail.advance(1);
    if ((hours_ > 23) || (minutes_ > 59) || (seconds_ > 59) || (fr_ > 29)
        || (ff_ > 99))
    {
        cerr << "SMPTE Offset out of range: " << setfill('0') << setw(2)
             << hours_
             << ':' << setw(2) << minutes_ << ':' << setw(2) << seconds_ << ':'
             << setw(2) << fr_ << ':' << setw(2) << ff_ << '\n'
             << "but permitted ranges are : " << setfill('0') << setw(2)
             << midi::MaxSmpteHours
             << ':' << setw(2) << midi::MaxSmpteMinutes << ':' << setw(2)
             << midi::MaxSmpteSeconds << ':'
             << setw(2) << midi::MaxSmpteFrames << ':' << setw(2)
             << midi::MaxSmpteHundredths << '\n';
    }
    return midi_stream_tail;
}

ostream& MidiFileMetaSMPTEOffsetEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << "SMPTE_OFFSET ";
    const auto fps = midi::smpte_fps_map(fps_);
    if (fps)
    {
        os << *fps;
    }
    else
    {
        os << "SMPTE_24FPS";
    }
    os << ' ' << setfill('0') << setw(2) << hours_ << ':' << setw(2)
       << minutes_ << ':' << setw(2) << seconds_ << ':' << setw(2) << fr_
       << ':' << setw(2) << ff_;
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSMPTEOffsetEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaMidiPortEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::midi_port_prefix;

    if ((midi_stream_tail.size() >= static_cast<int64_t>((midi::meta_prefix.size()
        + midi::midi_port_prefix.size())))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && equal(midi_port_prefix.cbegin(), midi_port_prefix.cend(), midi_stream_tail.begin() + 1))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::midi_port_prefix.size());
        auto evt = make_shared<MidiFileMetaMidiPortEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaMidiPortEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    midiport_ = midi_stream_tail[0];
    midi_stream_tail.advance(1);
    return midi_stream_tail;
}

ostream& MidiFileMetaMidiPortEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << "MIDI_PORT " << midiport_;
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaMidiPortEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaTimeSignatureEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::time_signature_prefix;

    if ((midi_stream_tail.size() >= static_cast<int64_t>((midi::meta_prefix.size()
        + midi::time_signature_prefix.size())))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && equal(time_signature_prefix.cbegin(),
            time_signature_prefix.cend(), midi_stream_tail.begin() + 1))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::time_signature_prefix.size());
        auto evt = make_shared<MidiFileMetaTimeSignatureEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaTimeSignatureEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    numerator_ = midi_stream_tail[0];
    denominator_ = midi_stream_tail.advance(1)[0];
    clocks_per_click_ = midi_stream_tail.advance(1)[0];
    thirtyseconds_per_quarter_ = midi_stream_tail.advance(1)[0];
    midi_stream_tail.advance(1);
    return midi_stream_tail;
}

ostream& MidiFileMetaTimeSignatureEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << "TIME_SIGNATURE " << numerator_ << ' '
       << (1 << denominator_)
       << ' ' << clocks_per_click_;
    // no one knows what thirtyseconds_per_quarter_ is for, so hide it.
    static_cast<void>(os.flags(flags));
    return os;
}
ostream& textmidi::operator<<(ostream& os, const MidiFileMetaTimeSignatureEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaKeySignatureEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    bool recognized{};

    if ((midi_stream_tail.size() >= static_cast<int64_t>((midi::meta_prefix.size()
        + midi::key_signature_prefix.size())))
        && MidiFileMetaEvent::recognize(midi_stream_tail))
    {
        if (equal(midi::key_signature_prefix.cbegin(),
            midi::key_signature_prefix.cend(), midi_stream_tail.begin() + 1))
        {
            recognized = true;
            midi_stream_tail.advance(midi::meta_prefix.size()
                + sizeof midi::key_signature_prefix[0]);
        }
        else
        {
            if (equal(midi::key_signature_prefix_mode_missing.cbegin(),
                midi::key_signature_prefix_mode_missing.cend(),
                midi_stream_tail.begin() + 1))
            {
                // some programs that were obsolete when they were written
                // left off the mode (major/minor) byte, so the consume
                // bytes function has to see the length byte to know that.
                recognized = true;
                midi_stream_tail.advance(midi::meta_prefix.size()
                    + sizeof(midi::key_signature_prefix_mode_missing[0]));
            }
        }
    }
    if (recognized)
    {
        auto evt = make_shared<MidiFileMetaKeySignatureEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaKeySignatureEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    // Some programs that were obsolete when they were written
    // left off the mode byte,
    // so this consume_stream function has to see the length byte to know that.
    // So this consume_stream takes a bigger byte off the apple than others.
    const auto len{midi_stream_tail[0]};
    accidentals_ = static_cast<signed char>(midi_stream_tail.advance(1)[0]);
    if (2 == len)
    {
        minor_mode_ = (midi_stream_tail.advance(1)[0] != 0);
    }
    return midi_stream_tail.advance(1);
}

ostream& MidiFileMetaKeySignatureEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    // -1 is one flat, 1, is one sharp, 0 is C or a.

    if (key_signature_map.contains(KeySignatureMap::key_type{accidentals_,
        minor_mode_}))
    {
        os << "KEY_SIGNATURE "
           << key_signature_map[KeySignatureMap::
              key_type{accidentals_, minor_mode_}];
    }
    else
    {
        os << "KEY_SIGNATURE "
           << key_signature_map[KeySignatureMap::key_type{0, false}]
           << " ; illegal accidentals: " << accidentals_;
    }
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaKeySignatureEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>>
    MidiFileMetaXmfPatchTypeEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::xmf_patch_type_prefix;

    if ((midi_stream_tail.size() >= static_cast<int64_t>((midi::meta_prefix.size()
        + midi::xmf_patch_type_prefix.size())))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && equal(xmf_patch_type_prefix.cbegin(),
            xmf_patch_type_prefix.cend(), midi_stream_tail.begin() + 1))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + sizeof(xmf_patch_type_prefix[0]));
        auto evt = make_shared<MidiFileMetaXmfPatchTypeEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaXmfPatchTypeEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    // Some programs that were obsolete when they were written left off
    // the mode byte,
    // so this consume_stream function has to see the length byte to know that.
    // So this consume_stream takes a bigger byte off the apple than others.
    // Skipping a byte.  len = midi_stream_tail.advance(1)[0];
    xmf_patch_type_ = static_cast<signed char>(midi_stream_tail[0]);
    midi_stream_tail.advance(1);
    return midi_stream_tail;
}

ostream& MidiFileMetaXmfPatchTypeEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    // -1 is one flat, 1, is one sharp, 0 is C or a.
    os << "XMF_PATCH_TYPE "
       << midi::xmf_patch_type_map.at(
          static_cast<midi::XmfPatchTypeEnum>(xmf_patch_type_));
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaXmfPatchTypeEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>>
    MidiFileMetaSequencerSpecificEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::sequencer_specific_prefix;

    // page 10 of file format chapter.
    // FF 7F [variable_len] mgfID or 00 mfgid0 mfgid1
    if ((midi_stream_tail.size() >= static_cast<int64_t>((midi::meta_prefix.size()
        + midi::sequencer_specific_prefix.size())))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        &&  (sequencer_specific_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::sequencer_specific_prefix.size());
        auto evt = make_shared<MidiFileMetaSequencerSpecificEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaSequencerSpecificEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    int64_t len{};
    tie(midi_stream_tail, len) = variable_len_value(midi_stream_tail);
    int64_t count{};
    do
    {
        data_.push_back(midi_stream_tail[0]);
        midi_stream_tail.advance(1);
    } while (++count < len);
    return midi_stream_tail;
}

ostream& MidiFileMetaSequencerSpecificEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << '\n' << "SEQUENCER_SPECIFIC";
    for (auto it(data_.cbegin());
            (it != data_.cend()) && (*it != end_of_sysex[0]);
            ++it)
    {
        os << ' ' << hex << "0x" << setw(2) << setfill('0')
           << static_cast<uint32_t>(*it) << dec;
    }
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSequencerSpecificEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaEndOfTrackEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::end_of_track_prefix;

    if ((midi_stream_tail.size() >= static_cast<int64_t>
        (midi::meta_prefix.size() + midi::end_of_track_prefix.size()))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && equal(end_of_track_prefix.cbegin(), end_of_track_prefix.cend(),
                midi_stream_tail.begin() + 1))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::end_of_track_prefix.size());
        auto evt = make_shared<MidiFileMetaEndOfTrackEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiFileMetaEndOfTrackEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    return midi_stream_tail;
}

ostream& MidiFileMetaEndOfTrackEvent::print(ostream& os) const
{
    return os << "\nEND_OF_TRACK";
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaEndOfTrackEvent& msg)
{
    return msg.print(os);
}

MidiStreamRange MidiFileMetaStringEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    int64_t len{};
    tie(midi_stream_tail, len) = variable_len_value(midi_stream_tail);
    str_.insert(str_.begin(), &midi_stream_tail[0], &midi_stream_tail[len]);
    return midi_stream_tail.advance(len);
}
ostream& MidiFileMetaStringEvent::print(ostream& os) const
{
    string display_str{str_};
    textmidi::make_human_string(display_str);
    return os << '"' << display_str << '"';
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaStringEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaTextEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::text_prefix;

    if ((midi_stream_tail.size() >= static_cast<int64_t>((midi::meta_prefix.size()
        + midi::text_prefix.size() + midi::variable_length_quantity_min_len)))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::text_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::text_prefix.size());
        auto evt = make_shared<MidiFileMetaTextEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaTextEvent::print(ostream& os) const
{
    os << "TEXT ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaTextEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaProgramNameEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::program_name_prefix;

    if ((midi_stream_tail.size() >=
        (midi::meta_prefix.size() + midi::program_name_prefix.size()
        + midi::variable_length_quantity_min_len))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::program_name_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::program_name_prefix.size());
        auto evt = make_shared<MidiFileMetaProgramNameEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaProgramNameEvent::print(ostream& os) const
{
    os << "PROGRAM_NAME ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaProgramNameEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaDeviceNameEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::device_name_prefix;

    if ((midi_stream_tail.size() >= static_cast<int64_t>((midi::meta_prefix.size()
        + midi::device_name_prefix.size() + midi::variable_length_quantity_min_len)))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::device_name_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::device_name_prefix.size());
        auto evt = make_shared<MidiFileMetaDeviceNameEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaDeviceNameEvent::print(ostream& os) const
{
    os << "DEVICE_NAME ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaDeviceNameEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaText0AEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::text_0A_prefix;

    if ((midi_stream_tail.size() >= static_cast<int64_t>(midi::meta_prefix.size()
        + midi::text_0A_prefix.size() + midi::variable_length_quantity_min_len))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::text_0A_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::text_0A_prefix.size());
        auto evt = make_shared<MidiFileMetaText0AEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaText0AEvent::print(ostream& os) const
{
    os << "TEXT_0A ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaText0AEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaText0BEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::text_0B_prefix;

    if ((midi_stream_tail.size() >= static_cast<int64_t>(midi::meta_prefix.size()
        + midi::text_0B_prefix.size() + midi::variable_length_quantity_min_len))
        && (MidiFileMetaEvent::recognize(midi_stream_tail))
        && (midi::text_0B_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::text_0B_prefix.size());
        auto evt = make_shared<MidiFileMetaText0BEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaText0BEvent::print(ostream& os) const
{
    os << "TEXT_0B ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaText0BEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaText0CEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::text_0C_prefix;

    if ((midi_stream_tail.size() >= static_cast<int64_t>((midi::meta_prefix.size()
        + text_0C_prefix.size() + midi::variable_length_quantity_min_len)))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::text_0C_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::text_0C_prefix.size());
        auto evt = make_shared<MidiFileMetaText0CEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaText0CEvent::print(ostream& os) const
{
    os << "TEXT_0C ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaText0CEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaText0DEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::text_0D_prefix;

    if ((midi_stream_tail.size()
        >= static_cast<int64_t>(midi::meta_prefix.size()
        + midi::text_0D_prefix.size() + midi::variable_length_quantity_min_len))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::text_0D_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::text_0D_prefix.size());
        auto evt = make_shared<MidiFileMetaText0DEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaText0DEvent::print(ostream& os) const
{
    os << "TEXT_0D ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaText0DEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaText0EEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::text_0E_prefix;
    if ((midi_stream_tail.size() >= static_cast<int64_t>(midi::meta_prefix.size()
        + midi::text_0E_prefix.size() + midi::variable_length_quantity_min_len))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::text_0E_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::text_0E_prefix.size());
        auto evt = make_shared<MidiFileMetaText0EEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaText0EEvent::print(ostream& os) const
{
    os << "TEXT_0E ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaText0EEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaText0FEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::text_0F_prefix;
    if ((midi_stream_tail.size() >= static_cast<int64_t>(midi::meta_prefix.size()
        + midi::text_0F_prefix.size() + midi::variable_length_quantity_min_len))
    && MidiFileMetaEvent::recognize(midi_stream_tail)
            && (midi::text_0F_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::text_0F_prefix.size());
        auto evt = make_shared<MidiFileMetaText0FEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaText0FEvent::print(ostream& os) const
{
    os << "TEXT_0F ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaText0FEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaCopyrightEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::copyright_prefix;

    if ((midi_stream_tail.size()
        >= static_cast<int64_t>(midi::meta_prefix.size() + midi::copyright_prefix.size()
        + midi::variable_length_quantity_min_len))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::copyright_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::copyright_prefix.size());
        auto evt = make_shared<MidiFileMetaCopyrightEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaCopyrightEvent::print(ostream& os) const
{
    os << "COPYRIGHT ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaCopyrightEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaTrackNameEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::track_name_prefix;
    if ((midi_stream_tail.size()
        >= static_cast<int64_t>(midi::meta_prefix.size() + midi::track_name_prefix.size()
        + midi::variable_length_quantity_min_len))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::track_name_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::track_name_prefix.size());
        auto evt = make_shared<MidiFileMetaTrackNameEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaTrackNameEvent::print(ostream& os) const
{
    os << "TRACK ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaTrackNameEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaInstrumentEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::instrument_name_prefix;

    if (((midi_stream_tail.size()
        >= static_cast<int64_t>
        (midi::meta_prefix.size() + midi::instrument_name_prefix.size()
        + midi::variable_length_quantity_min_len))
        && MidiFileMetaEvent::recognize(midi_stream_tail))
        && (midi::instrument_name_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::instrument_name_prefix.size());
        auto evt = make_shared<MidiFileMetaInstrumentEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaInstrumentEvent::print(ostream& os) const
{
    os << "INSTRUMENT ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaInstrumentEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaLyricEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::lyric_prefix;

    if ((midi_stream_tail.size()
        >= static_cast<int64_t>(midi::meta_prefix.size() + midi::lyric_prefix.size()
        + midi::variable_length_quantity_min_len))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::lyric_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::lyric_prefix.size());
        auto evt = make_shared<MidiFileMetaLyricEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaLyricEvent::print(ostream& os) const
{
    os << "LYRIC ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaLyricEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaMarkerEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::marker_prefix;

    if ((midi_stream_tail.size()
        >= static_cast<int64_t>(midi::meta_prefix.size() + midi::marker_prefix.size()
        + midi::variable_length_quantity_min_len))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::marker_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::marker_prefix.size());
        auto evt = make_shared<MidiFileMetaMarkerEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaMarkerEvent::print(ostream& os) const
{
    os << "MARKER ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaMarkerEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiFileMetaCuePointEvent::recognize(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::cue_point_prefix;

    if ((midi_stream_tail.size()
        >= static_cast<int64_t>(midi::meta_prefix.size() + midi::cue_point_prefix.size()
        + midi::variable_length_quantity_min_len))
        && MidiFileMetaEvent::recognize(midi_stream_tail)
        && (midi::cue_point_prefix[0] == midi_stream_tail[1]))
    {
        midi_stream_tail.advance(midi::meta_prefix.size() + midi::cue_point_prefix.size());
        auto evt = make_shared<MidiFileMetaCuePointEvent>();
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiFileMetaCuePointEvent::print(ostream& os) const
{
    os << "CUE_POINT ";
    return MidiFileMetaStringEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaCuePointEvent& msg)
{
    return msg.print(os);
}

MidiStreamAtom textmidi::MidiChannelVoiceModeEvent::channel() const noexcept
{
    return channel_;
}

void textmidi::MidiChannelVoiceModeEvent::channel(MidiStreamAtom channel)
    noexcept
{
    channel_ = channel;
}

const RunningStatusStandard& MidiChannelVoiceModeEvent::running_status() const
    noexcept
{
    return running_status_;
}

const RunningStatusStandard& MidiChannelVoiceModeEvent::local_status() const
    noexcept
{
    return local_status_;
}
void MidiChannelVoiceModeEvent::local_status(const RunningStatusStandard& rs)
    noexcept
{
    local_status_ = rs;
}

RunningStatusStandard& MidiChannelVoiceModeEvent::local_status() noexcept
{
    return local_status_;
}

bool MidiChannelVoiceModeEvent::recognize(MidiStreamRange  midi_stream_tail,
    const midi::RunningStatusStandard& running_status) noexcept
{
    bool recognized{};

    // Recognize an event with running status.
    // All software evolves during maintenance to make all
    // data global and asymmetrically handled.
    // channel_pressure and control change have only 1 byte following
    // the command.
    // Check if these are data bytes and not commands.
    if ((running_status.command() == midi::channel_pressure[0])
        || (running_status.command() == midi::program[0]))
    {
        if (midi_stream_tail.size() >= 1)
        {
            // look for two bytes in a row with 1 << 8 clear.
            // This is how we look for data bytes with running status.
            if (!(midi_stream_tail[0] & midi::event_flag))
            {
                recognized = true;
            }
        }
    }
    else
    {
        if (midi_stream_tail.size() >=
            static_cast<int64_t>((midi::full_note_length - sizeof(midi::note_on[0]))))
        {
            // look for two bytes in a row with 1 << 8 clear.
            // This is how we look for data bytes with running status.
            if (!(midi_stream_tail[0] & midi::event_flag))
            {
                if (!(midi_stream_tail[1] & midi::event_flag))
                {
                    recognized = true;
                }
            }
        }
    }
    return recognized;
}

constexpr MidiStreamAtom textmidi::MidiChannelVoiceNoteEvent::key() const
    noexcept
{
    return key_;
}

void textmidi::MidiChannelVoiceNoteEvent::key(MidiStreamAtom key) noexcept
{
    key_ = key;
}

void textmidi::MidiChannelVoiceNoteEvent::key_string(string_view key_string)
    noexcept
{
    key_string_ = key_string;
}

string textmidi::MidiChannelVoiceNoteEvent::key_string() const noexcept
{
    return key_string_;
}

void textmidi::MidiChannelVoiceNoteEvent::velocity(MidiStreamAtom velocity)
    noexcept
{
    velocity_ = velocity;
}

constexpr MidiStreamAtom textmidi::MidiChannelVoiceNoteEvent::velocity() const
    noexcept
{
    return velocity_;
}

constexpr uint32_t textmidi::MidiChannelVoiceNoteEvent::ticks_per_whole() const
    noexcept
{
    return ticks_per_whole_;
}

constexpr bool textmidi::MidiChannelVoiceNoteEvent
    ::operator==(const MidiChannelVoiceNoteEvent& note) const noexcept
{
    return (key_ == note.key() && channel() == note.channel());
}

void textmidi::MidiChannelVoiceNoteEvent
    ::wholes_to_noteoff(const RhythmRational& wholes_to_noteoff) noexcept
{
    wholes_to_noteoff_ = wholes_to_noteoff;
    wholes_to_noteoff_.reduce();
}

RhythmRational textmidi::MidiChannelVoiceNoteEvent::wholes_to_noteoff() const
    noexcept
{
    return wholes_to_noteoff_;
}

MidiStreamRange MidiChannelVoiceNoteEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::MaxKeyboardKey;
    // Skip a beginning byte that is an event type byte.
    if (midi::event_flag & midi_stream_tail[0])
    {
        local_status().running_status(midi_stream_tail[0]);
        midi_stream_tail.advance(1);
    }
    else
    {
        local_status(running_status());
    }
    key_ = midi_stream_tail[0];
    midi_stream_tail.advance(1);
    if (key_ > MaxKeyboardKey)
    {
        const string errstr{((string("MIDI Key Number (")
            += boost::lexical_cast<string>(static_cast<int32_t>(key_)))
            += ") exceeded MIDI limit (")
            += boost::lexical_cast<string>(MaxKeyboardKey) };
        cerr << errstr;
    }
    channel(local_status().channel() + 1);
    key_string_ = textmidi::num_to_note(key_, prefer_sharp_);
    velocity_ = static_cast<int32_t>(midi_stream_tail[0]);
    midi_stream_tail.advance(1);
    if (velocity_ > midi::MaxDynamic)
    {
        const string errstr{(((string("MIDI velocity (")
            += boost::lexical_cast<string>(static_cast<int32_t>(velocity_)))
            += ") exceeded MIDI limit (")
            += boost::lexical_cast<string>(midi::MaxDynamic)) += ")\n"};
        cerr << errstr;
    }
    return midi_stream_tail;
}

ostream& MidiChannelVoiceNoteEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << dec << static_cast<uint32_t>(channel()) << ' '
       << key_string_ << ' ' << static_cast<uint32_t>(velocity_);
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteEvent& msg)
{
    return msg.print(os);
}

void textmidi::MidiChannelVoiceNoteOnEvent
    ::ticks_to_noteoff(int64_t ticks_to_noteoff) noexcept
{
    ticks_to_noteoff_ = ticks_to_noteoff;
}

constexpr int64_t textmidi::MidiChannelVoiceNoteOnEvent::ticks_to_noteoff()
    const noexcept
{
    return ticks_to_noteoff_;
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>>
    MidiChannelVoiceNoteOnEvent::recognize(MidiStreamRange midi_stream_tail,
    midi::RunningStatusStandard& running_status, shared_ptr<bool> prefer_sharp,
    uint32_t ticks_per_whole) noexcept
{
    if ((midi_stream_tail.size() >= midi::full_note_length)
        && ((midi_stream_tail[0] & ~midi::channel_mask) == midi::note_on[0]))
    {
        shared_ptr<MidiChannelVoiceNoteOnEvent> evt{
            make_shared<MidiChannelVoiceNoteOnEvent>(
            running_status, ticks_per_whole, prefer_sharp)};
        running_status.running_status(midi_stream_tail[0]);
        midi_stream_tail.advance(1);
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiChannelVoiceNoteOnEvent::print(ostream& os) const
{
    os << "NOTE_ON ";
    return MidiChannelVoiceNoteEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteOnEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiChannelVoiceNoteOffEvent::recognize(MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status, shared_ptr<bool> prefer_sharp, uint32_t ticks_per_whole) noexcept
{
    if ((midi_stream_tail.size() >= midi::full_note_length)
        && ((midi_stream_tail[0] & ~midi::channel_mask) == midi::note_off[0]))
    {
        shared_ptr<MidiChannelVoiceNoteOffEvent> evt
            = make_shared<MidiChannelVoiceNoteOffEvent>(running_status,
              ticks_per_whole, prefer_sharp);
        running_status.running_status(midi_stream_tail[0]);
        midi_stream_tail.advance(1);
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiChannelVoiceNoteOffEvent::print(ostream& os) const
{
    os << "NOTE_OFF ";
    return MidiChannelVoiceNoteEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteOffEvent& msg)
{
    return msg.print(os);
}

ostream& textmidi::operator <<(ostream& os, const MidiChannelVoicePitchBendEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiChannelVoicePitchBendEvent::recognize(MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status) noexcept
{
    using midi::pitch_wheel;
    if ((midi_stream_tail.size() >= midi::full_note_length)
        && ((midi_stream_tail[0] & ~midi::channel_mask) == pitch_wheel[0]))
    {
        auto evt{make_shared<MidiChannelVoicePitchBendEvent>(running_status)};
        running_status.running_status(midi_stream_tail[0]);
        midi_stream_tail.advance(1);
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiChannelVoicePitchBendEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    using midi::MaxPitchBend;
    local_status(running_status());
    midi::MidiStreamAtom pitch_wheel_lsb{midi_stream_tail[0]};
    midi_stream_tail.advance(1);
    if (pitch_wheel_lsb > MaxPitchBend)
    {
        const string errstr{((string("MIDI Pitch Bend LSB (")
            += boost::lexical_cast<string>
            (static_cast<int32_t>(pitch_wheel_lsb)))
            +=  ") exceeded MIDI limit (")
            += boost::lexical_cast<string>(MaxPitchBend) += ")\n"};
        cerr << errstr;
    }
    midi::MidiStreamAtom pitch_wheel_msb{midi_stream_tail[0]};
    midi_stream_tail.advance(1);
    if (pitch_wheel_msb > MaxPitchBend)
    {
        const string errstr{((string("MIDI Pitch Bend MSB (")
            += boost::lexical_cast<string>
            (static_cast<int32_t>(pitch_wheel_msb)))
            +=  ") exceeded MIDI limit (")
            += boost::lexical_cast<string>(MaxPitchBend) += ")\n"};
        cerr << errstr;
    }
    channel(local_status().channel() + 1);
    pitch_wheel_ = pitch_wheel_lsb | (pitch_wheel_msb << midi::byte7_shift);
    return midi_stream_tail;
}

ostream& MidiChannelVoicePitchBendEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << "PITCH_WHEEL " << dec << static_cast<int32_t>(channel())
       << ' ' << static_cast<int32_t>(pitch_wheel_);
    static_cast<void>(os.flags(flags));
    return os;
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiChannelVoiceControlChangeEvent::recognize(MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status) noexcept
{
    using midi::control;

    if ((midi_stream_tail.size() >= midi::full_note_length)
        && ((midi_stream_tail[0] & ~midi::channel_mask) == control[0]))
    {
        auto evt{make_shared<MidiChannelVoiceControlChangeEvent>(running_status)};
        running_status.running_status(midi_stream_tail[0]);
        midi_stream_tail.advance(1);
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiChannelVoiceControlChangeEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    using std::dec, std::hex, std::setw, std::setfill;
    local_status(running_status());
    channel(local_status().channel() + 1);
    id_      = midi_stream_tail[0];
    value_   = midi_stream_tail.advance(1)[0];
    midi_stream_tail.advance(1);
    if (value_ & ~midi::byte7_mask)
    {
        cerr << "Illegal 8-bit control value: "
             << hex << "0x" << setw(2) << setfill('0')
             << static_cast<int32_t>(value_) << dec << '\n';
    }
    return midi_stream_tail;
}

ostream& MidiChannelVoiceControlChangeEvent::print(ostream& os) const
{
    using std::set;
    auto flags{os.flags()};
    os << "CONTROL " << dec << static_cast<uint32_t>(channel()) << ' ';
    if (midi::control_pan[0] == id_)
    {
        int32_t temp_pan{value_};
        temp_pan -= midi::PanExcess64;
        if (!((midi::MinSignedPan <= temp_pan)
            && (temp_pan <= midi::MaxSignedPan)))
        {
            const string errstr{(((((string("Illegal pan value fails range: ")
                += boost::lexical_cast<string>(midi::MinSignedPan)) += " <= ")
                += boost::lexical_cast<string>(temp_pan)) += " <= ")
                += boost::lexical_cast<string>(midi::MaxSignedPan)) += '\n'};
            cerr << errstr;
        }
        string panstring{};
        const auto pan = midi::pan_map(temp_pan);
        panstring = (pan ? *pan : boost::lexical_cast<string>(temp_pan));
        os << "PAN " << ' ' << panstring << ' ';
    }
    else
    {
        const auto control_str = midi::control_function_map(id_);
        if (control_str)
        {
            os << *control_str;
        }
        else
        {
            if (static_cast<int32_t>(
                midi::RegisteredParameterMsbs::parameter_3d_msb) == id_)
            {
                os << hex << "0x" << setw(2) << setfill('0')
                   << static_cast<uint32_t>(id_) << dec;
            }
            else
            {
                os << hex << "0x" << setw(2) << setfill('0')
                    << static_cast<uint32_t>(id_) << dec;
            }
        }
        os << ' ';
        set<MidiStreamAtom> on_off_controls{midi::control_portamento_on_off[0],
            midi::control_legato_foot[0], midi::control_sostenuto[0],
            midi::control_local_control[0]};
        if (on_off_controls.contains(id_))
        {
            if (value_ >= midi::control_pedalthreshold)
            {
                // on
                os << "ON";
            }
            else
            {
                // off
                os << "OFF";
            }
        }
        else
        {
            os << hex << "0x" << setw(2) << setfill('0')
                << static_cast<uint32_t>(value_) << dec;
        }
    }
    os << ' ';
    static_cast<void>(os.flags(flags));
    return os;
}

void textmidi::MidiChannelVoiceControlChangeEvent::id(MidiStreamAtom id)
    noexcept
{
    id_ = id;
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiChannelVoiceProgramChangeEvent::recognize(MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status) noexcept
{
    if ((midi_stream_tail.size() >= midi::full_note_length)
        && ((midi_stream_tail[0] & ~midi::channel_mask) == midi::program[0]))
    {
        auto evt{make_shared<MidiChannelVoiceProgramChangeEvent>(running_status)};
        running_status.running_status(midi_stream_tail[0]);
        midi_stream_tail.advance(1);
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiChannelVoiceProgramChangeEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    local_status(running_status());
    channel(local_status().channel() + 1);
    program_ = midi_stream_tail[0];
    midi_stream_tail.advance(1);
    if (program_ & ~midi::byte7_mask)
    {
        cerr << "Illegal 8-bit control value: "
             << hex << static_cast<int32_t>(program_) << dec << '\n';
    }
    return midi_stream_tail;
}

ostream& MidiChannelVoiceProgramChangeEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << "PROGRAM " << dec << static_cast<int32_t>(channel()) << ' '
       << (static_cast<int32_t>(program_) + 1);
    static_cast<void>(os.flags(flags));
    return os;
}

MidiStreamAtom textmidi::MidiChannelVoiceControlChangeEvent::value() const
    noexcept
{
    return value_;
}

void textmidi::MidiChannelVoiceControlChangeEvent::value(MidiStreamAtom value)
    noexcept
{
    value_ = value;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceControlChangeEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>> MidiChannelVoiceChannelPressureEvent::recognize(MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status) noexcept
{
    if ((midi_stream_tail.size() >= midi::full_note_length)
        && ((midi_stream_tail[0] & ~midi::channel_mask) == midi::channel_pressure[0]))
    {
        auto evt{make_shared<MidiChannelVoiceChannelPressureEvent>(running_status)};
        running_status.running_status(midi_stream_tail[0]);
        midi_stream_tail.advance(1);
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

MidiStreamRange MidiChannelVoiceChannelPressureEvent::consume_stream(MidiStreamRange midi_stream_tail) noexcept
{
    local_status(running_status());
    channel(local_status().channel() + 1);
    pressure_ = midi_stream_tail[0];
    midi_stream_tail.advance(1);
    if (pressure_ > midi::MaxDynamic)
    {
        const string errstr{(((string("MIDI pressure (")
            += boost::lexical_cast<string>(static_cast<int32_t>(pressure_)))
            += ") exceeded MIDI limit (")
            += boost::lexical_cast<string>(midi::MaxDynamic))
            += ")\n" };
        cerr << errstr;
    }
    return midi_stream_tail;
}

ostream& MidiChannelVoiceChannelPressureEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    os << "CHANNEL_PRESSURE " << dec << static_cast<int32_t>(channel())
       << ' ' << static_cast<int32_t>(pressure_);
    static_cast<void>(os.flags(flags));
    return os;
}
ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceChannelPressureEvent& msg)
{
    return msg.print(os);
}

tuple<MidiStreamRange, optional<shared_ptr<MidiEvent>>>
    MidiChannelVoicePolyphonicKeyPressureEvent::recognize(MidiStreamRange midi_stream_tail,
    midi::RunningStatusStandard& running_status, shared_ptr<bool> prefer_sharp,
    uint32_t ticks_per_whole) noexcept
{
    if ((midi_stream_tail.size() >= midi::full_note_length)
    && ((midi_stream_tail[0] & ~midi::channel_mask) == midi::polyphonic_key_pressure[0]))
    {
        auto evt{make_shared<MidiChannelVoicePolyphonicKeyPressureEvent>(
            running_status, ticks_per_whole, prefer_sharp)};
        running_status.running_status(midi_stream_tail[0]);
        midi_stream_tail.advance(1);
        midi_stream_tail = evt->consume_stream(midi_stream_tail);
        return tuple(midi_stream_tail, evt);
    }
    else
    {
        return tuple(midi_stream_tail, optional<shared_ptr<MidiEvent>>{});
    }
}

ostream& MidiChannelVoicePolyphonicKeyPressureEvent::print(ostream& os) const
{
    os << "POLY_KEY_PRESSURE ";
    return MidiChannelVoiceNoteEvent::print(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoicePolyphonicKeyPressureEvent& msg)
{
    return msg.print(os);
}

ostream& MidiChannelVoiceNoteRestEvent::print(ostream& os) const
{
    auto flags{os.flags()};
    if (ticks_to_next_event())
    {
        os << "R ";
    }
    RhythmRational duration{ticks_to_next_event(), ticks_per_whole()};
    duration.reduce();
    (*print_rhythm)(os, duration);
    os << '\n';
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteRestEvent& msg)
{
    return msg.print(os);
}

void textmidi::MidiChannelVoiceProgramChangeEvent::
    program(MidiStreamAtom program) noexcept
{
    program_ = program;
}

constexpr MidiStreamAtom textmidi::MidiChannelVoiceProgramChangeEvent::
    program() const noexcept
{
    return program_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceProgramChangeEvent& msg)
{
    return msg.print(os);
}

bool textmidi::PrintLazyTrack::is_in_rest() const noexcept
{
    using std::ranges::find_if;
    auto inrest{true};
    for_each(channel_keyboards_,
        [&](const KeyBoard& chkb) { inrest = inrest
        && find_if(chkb, [](int32_t k) { return k > 0; } ) == chkb.end(); });
    return inrest;
}

void textmidi::PrintLazyTrack::ticks_to_note_stop() noexcept
{
    using std::numeric_limits;
    for (auto delay_event_iter   {delay_events_.begin()};
             (delay_event_iter != delay_events_.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>(
            delay_event_iter->second.get()); ++delay_event_iter)
    {
        // me is MidiEvent
        auto me{delay_event_iter->second.get()};
        auto note   {dynamic_cast<MidiChannelVoiceNoteEvent*>(me)};
        auto note_on_1{dynamic_cast<MidiChannelVoiceNoteOnEvent*>(note)};
        // If this is a note on but not a zero-velocity
        // which would be an ersatz note-off:
        if (note_on_1 && note->velocity())
        {
            // A second event iterator to look for the
            // corresponding note-off or note-on with velocity 0
            // that matches the current note-on.
            auto delay_event_iter2{delay_event_iter};
            // go to next event
            ++delay_event_iter2;
            // Look from the next event to the end-of-track.
            for ( ; (delay_event_iter2 != delay_events_.end())
                && (!dynamic_cast<MidiFileMetaEndOfTrackEvent*>(
                delay_event_iter2->second.get()));
                ++delay_event_iter2)
            {
                // If the delay is finite then this is not at the current chord
                auto me2{delay_event_iter2->second.get()};
                auto note2{dynamic_cast<MidiChannelVoiceNoteEvent*>(me2)};
                auto note_on_2{dynamic_cast<MidiChannelVoiceNoteOnEvent*>
                    (note2)};
                auto note_off2{dynamic_cast<MidiChannelVoiceNoteOffEvent*>
                    (note2)};
                auto note_poly_key_pressure2{dynamic_cast<const
                    MidiChannelVoicePolyphonicKeyPressureEvent* const>(note2)};
                if (note_on_2)
                {
                    if (!note_on_2->velocity() && (*note == *note2)
                        && !note_poly_key_pressure2)
                    {
                        // We have a note-on of velocity zero that
                        // is an ersatz note-off
                        // that has not been matched yet but has the same key
                        // and channel.
                        note_on_1->ticks_to_noteoff(
                            me2->ticks_accumulated()
                            - me->ticks_accumulated());

                        RhythmRational ratio_to_note_off{
                            note_on_1->ticks_to_noteoff(),
                            ticksperquarter_ * QuartersPerWhole};
                        note_on_1->wholes_to_noteoff(
                            rational::snap(ratio_to_note_off, quantum_));

                        note->set_matched();
                        note2->set_matched();
                        break;
                    }
                }
                else // handle a note-off
                {
                    if (note_off2 && !note_poly_key_pressure2)
                    {
                        if ((*note2 == *note) && !note2->matched())
                        {
                            note_on_1->ticks_to_noteoff(
                                me2->ticks_accumulated()
                                - me->ticks_accumulated());
                            RhythmRational ratio_to_note_off{
                                note_on_1->ticks_to_noteoff(),
                                ticksperquarter_ * QuartersPerWhole};
                            note_on_1->wholes_to_noteoff(
                                rational::snap(ratio_to_note_off, quantum_));
                            note->set_matched();
                            note_off2->set_matched();
                            break;
                        }
                    }
                    else
                    {
                        auto rest{dynamic_cast<MidiChannelVoiceNoteRestEvent*>
                            (delay_event_iter->second.get())};
                        if (rest)
                        {
                            RhythmRational ratio_to_note_off{
                                rest->ticks_to_next_event(),
                                    ticksperquarter_ * QuartersPerWhole};
                            ratio_to_note_off.reduce();
                            rest->wholes_to_noteoff(rational::
                                snap(ratio_to_note_off, quantum_));
                        }
                    }
                }
            }
            if (note_on_1->ticks_to_noteoff() == numeric_limits<int64_t>().max())
            {
                cerr << "no noteoff found, channel: "
                     << static_cast<int32_t>(note->channel()) << " key number: "
                     << static_cast<int32_t>(note->key()) << '\n';
            }
        }
    }
}

tuple<MidiStreamRange, optional<DelayEvent>>
MidiEventFactory::operator()(MidiStreamRange midi_stream_tail, int64_t& ticks_accumulated)
{
    using std::runtime_error;
    using midi::ChannelPressure, midi::PitchWheel, midi::NoteOn, midi::NoteOff,
         midi::PolyphonicKeyPressure, midi::Control, midi::Program;
    int64_t delaynum64{};
    tie(midi_stream_tail, delaynum64) = variable_len_value(midi_stream_tail);
    DelayEvent midi_delay_event_pair(delaynum64, nullptr);
    ticks_accumulated += delaynum64;

    bool clear_ticks_accumulated{};

    // return shared_ptr to the class..optionally
    // return dropped view, shortened by what had been consumed.
    optional<shared_ptr<MidiEvent>> recognition{};
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaSequenceEvent::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaTextEvent::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaProgramNameEvent::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaDeviceNameEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaText0AEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaText0BEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaText0CEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaText0DEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaText0EEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaText0FEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaCopyrightEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaTrackNameEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaInstrumentEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaLyricEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaMarkerEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaCuePointEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaUnknownEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaMidiChannelEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
    {
        tie(midi_stream_tail, recognition) = MidiFileMetaEndOfTrackEvent::recognize(midi_stream_tail);
        if (recognition.has_value())
        {
            clear_ticks_accumulated = true;
            running_status_.clear();
        }
    }
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaSetTempoEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaSMPTEOffsetEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaTimeSignatureEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaKeySignatureEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaXmfPatchTypeEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaMidiPortEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiFileMetaSequencerSpecificEvent
            ::recognize(midi_stream_tail);
#if 0
        break;
      [[unlikely]] case midi_time_code_quarter_frame[0]:
        break;
      [[unlikely]] case song_position_pointer[0]:
        break;
      [[unlikely]] case song_select[0]:
        break;
      [[unlikely]] case tune_request[0]:
        break;
#endif
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiSysExEvent
            ::recognize(midi_stream_tail);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiSysExRawEvent
            ::recognize(midi_stream_tail);

    if (!recognition.has_value())
    {
        optional<shared_ptr<MidiEvent>> evt{};
        bool is_voice_mode = MidiChannelVoiceModeEvent::recognize(midi_stream_tail, running_status_);
        if (is_voice_mode)
        {
            if (running_status_.running_status_valid())
            {
                switch (running_status_.command())
                {
                  case NoteOn:
                    {
                      auto note_on_evt = make_shared<MidiChannelVoiceNoteOnEvent>(
                          running_status_, ticks_per_whole_, prefer_sharp_);
                      midi_stream_tail = note_on_evt->consume_stream(midi_stream_tail);
                      recognition = note_on_evt;
                    }
                    break;
                  case NoteOff:
                    {
                      auto note_off_evt
                          = make_shared<MidiChannelVoiceNoteOffEvent>(
                          running_status_, ticks_per_whole_, prefer_sharp_);
                      midi_stream_tail = note_off_evt->consume_stream(midi_stream_tail);
                      recognition = note_off_evt;
                    }
                    break;
                  case PolyphonicKeyPressure:
                    {
                      auto polyphonic_key_pressure_evt
                          = make_shared<MidiChannelVoicePolyphonicKeyPressureEvent>
                          (running_status_, ticks_per_whole_, prefer_sharp_);
                      midi_stream_tail = polyphonic_key_pressure_evt->consume_stream(midi_stream_tail);
                      recognition = polyphonic_key_pressure_evt;
                    }
                    break;
                  case Control:
                    {
                      auto control_evt
                          = make_shared<MidiChannelVoiceControlChangeEvent>
                          (running_status_);
                      midi_stream_tail = control_evt->consume_stream(midi_stream_tail);
                      recognition = control_evt;
                    }
                    break;
                  case Program:
                    {
                      auto program_evt
                          = make_shared<MidiChannelVoiceProgramChangeEvent>
                          (running_status_);
                      midi_stream_tail = program_evt->consume_stream(midi_stream_tail);
                      recognition = program_evt;
                    }
                    break;
                  case ChannelPressure:
                    {
                      auto channel_pressure_evt
                          = make_shared<MidiChannelVoiceChannelPressureEvent>
                          (running_status_);
                      midi_stream_tail = channel_pressure_evt->consume_stream(midi_stream_tail);
                      recognition = channel_pressure_evt;
                    }
                    break;
                  case PitchWheel:
                    {
                      auto pitchwheel_evt
                          = make_shared<MidiChannelVoicePitchBendEvent>
                          (running_status_);
                      midi_stream_tail = pitchwheel_evt->consume_stream(midi_stream_tail);
                      recognition = pitchwheel_evt;
                    }
                    break;
                  default:
                    throw(runtime_error{"unknown bytes parsing"
                            " running status"});
                    break;
                }
            }
        }
    }
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiChannelVoiceNoteOnEvent
            ::recognize(midi_stream_tail, running_status_, prefer_sharp_,
            ticks_per_whole_);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiChannelVoiceNoteOffEvent
            ::recognize(midi_stream_tail, running_status_, prefer_sharp_,
            ticks_per_whole_);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiChannelVoicePolyphonicKeyPressureEvent
            ::recognize(midi_stream_tail, running_status_, prefer_sharp_,
            ticks_per_whole_);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiChannelVoiceProgramChangeEvent
            ::recognize(midi_stream_tail, running_status_);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiChannelVoiceChannelPressureEvent
            ::recognize(midi_stream_tail, running_status_);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiChannelVoicePitchBendEvent
            ::recognize(midi_stream_tail, running_status_);
    if (!recognition.has_value())
        tie(midi_stream_tail, recognition) = MidiChannelVoiceControlChangeEvent
            ::recognize(midi_stream_tail, running_status_);

    if (recognition.has_value())
    {
        recognition.value()->ticks_accumulated(ticks_accumulated);
        if (clear_ticks_accumulated)
        {
            ticks_accumulated = 0LU;
        }
        MidiChannelVoiceModeEvent*
            mcvm{dynamic_cast<MidiChannelVoiceModeEvent*>(recognition.value().get())};
        if (mcvm)
        {
            running_status_ = mcvm->local_status();
        }
        midi_delay_event_pair.second = recognition.value();
    }
    else
    {
        cerr << "Unrecognized byte sequence:\n";
        cerr << setfill('0') << setw(2) << hex;
        copy(&midi_stream_tail[0], &midi_stream_tail[32], ostream_iterator<int>(cerr, " "));
        cerr << dec << '\n';
        cerr << "seeking next command\n";
        size_t i{};
        while ((i < midi_stream_tail.size()) && ((midi_stream_tail[i] & event_flag) == 0))
        {
            ++i;
        }
        --i;
        auto second = make_shared<MidiFileMetaTextEvent>();
        midi_stream_tail.advance(i);
        second->consume_stream(midi_stream_tail);
        second->ticks_accumulated(ticks_accumulated);
        if (clear_ticks_accumulated)
        {
            ticks_accumulated = 0LU;
        }
        midi_delay_event_pair.second = second;
    }
    return tuple(midi_stream_tail, midi_delay_event_pair);
}

//
// We have to compute the ticks to the next note-on from a given note-on
// to know if the next note is in the same chord.  If it's in the same chord
// because it has zero ticks from a current note-on, then we shouldn't write
// a rhythm out yet; we want to wait until we have collected all the notes
// in a chord.  However if there is also another type of event in betweent
// then we have to write a 0/1 rhythm anyway in order to insert the other
// type of event.  This is what it takes to make LAZY mode equivalent to
// DETAIL mode of the textmidi language.
void textmidi::PrintLazyTrack::ticks_to_a_note_start() noexcept
{
    for (auto delay_event_iter{delay_events_.begin()};
        (delay_event_iter != delay_events_.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
        (delay_event_iter->second.get());
        ++delay_event_iter)
    {
        auto me{delay_event_iter->second.get()};
        auto note{dynamic_cast<MidiChannelVoiceNoteEvent*>(me)};
        auto note_on_1{dynamic_cast<MidiChannelVoiceNoteOnEvent*>(note)};
        if (note_on_1 && note->velocity())
        {
            auto delay_event_iter2{delay_event_iter};
            ++delay_event_iter2;
            for ( ;
                 (delay_event_iter2 != delay_events_.end())
                         && (!dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                             (delay_event_iter2->second.get()));
                 ++delay_event_iter2)
            {
                auto me2{delay_event_iter2->second.get()};
                auto next_note{dynamic_cast<MidiChannelVoiceNoteEvent*>
                    (delay_event_iter2->second.get())};
                auto next_note_on{dynamic_cast<
                    const MidiChannelVoiceNoteOnEvent* const>(next_note)};
                if (next_note_on && next_note->velocity())
                {
                    note_on_1->ticks_to_a_note_start(
                        me2->ticks_accumulated() - me->ticks_accumulated());
                    note_on_1->events_to_next_note_on(
                        distance(delay_event_iter, delay_event_iter2));
                    break;
                }
            }
        }
    }
}

void textmidi::PrintLazyTrack::ticks_to_next_event() noexcept
{
    using std::ranges::drop_view, std::ranges::transform,
          std::ranges::take_view;
    const auto len(delay_events_.size());
    auto lopped_end   = take_view(delay_events_, len - 1);
    auto lopped_begin = drop_view(delay_events_, 1);
    if (len > 1)
    {
        transform(lopped_end, lopped_begin, delay_events_.begin(),
            [this](const DelayEvent& mde1, const DelayEvent& mde2)
            { DelayEvent mde_rtn{mde1};
            mde_rtn.second->ticks_to_next_event(
                mde2.second->ticks_accumulated()
                - mde1.second->ticks_accumulated());
            RhythmRational ratio_to_next_event{
                mde_rtn.second->ticks_to_next_event(),
                QuartersPerWhole * ticksperquarter_};
            mde_rtn.second->wholes_to_next_event(rational::
                snap(ratio_to_next_event, quantum_));
            auto temp{mde_rtn.second->wholes_to_next_event()};
            temp.reduce();
            mde_rtn.second->wholes_to_next_event(temp);
            return mde_rtn; });
    }
}

void textmidi::PrintLazyTrack::print(ostream& os, DelayEvent& mdep) noexcept
{
    using std::ostringstream;
    int64_t rest_ticks{};
    auto me{mdep.second.get()};
    auto note{dynamic_cast<MidiChannelVoiceNoteEvent*>(me)};
    auto note_poly_key_pressure{dynamic_cast<
        const MidiChannelVoicePolyphonicKeyPressureEvent* const>(note)};
    // Define a function that determines if wholes_to_noteoff == 0.
    // A NoteOn with velocity == 0 is an ersatz note off.
    // If this is full NoteOn:
    if (note)
    {
        auto note_on_1{dynamic_cast<MidiChannelVoiceNoteOnEvent*>
            (mdep.second.get())};
        auto note_off_1{dynamic_cast<const MidiChannelVoiceNoteOffEvent* const>
            (mdep.second.get())};
        // Save the ticks of the start time of the new note.
        const RhythmRational wholes_to_event{
            mdep.second->ticks_to_next_event(), note->ticks_per_whole()};
        // If this is a new note with a non-zero velocity:
        if (note_on_1 && note->velocity())
        {
            // Add note to chord_.
            chord_.push_back(*note_on_1);
        }
        else // velocity == 0 or noteoff
        {
            // Test note_on_1 before note->velocity using short-circuited
            // evaluation.
            // not a note event but could be a rest or poly pressure which are
            // note events.
            if (!note_poly_key_pressure)
            {
                if (!(note_off_1 || (note_on_1 && (note->velocity() == 0))))
                {
                    const auto rest{dynamic_cast<
                        MidiChannelVoiceNoteRestEvent*>(mdep.second.get())};
                    if (rest)
                    {
                        RhythmRational duration{rest->wholes_to_next_event()};
                        duration.reduce();
                        if (duration > RhythmRational{})
                        {
                            os << lazy_string(true);
                            os << rest->key_string() << ' ';
                            (*print_rhythm)(os, duration);
                            os << '\n';
                        }
                        rest_ticks = rest->ticks_to_next_event();
                    }
                    else // poly key pressure (derived from NoteEvent) should
                         // land here along with all non-note events.
                    {
                        os << lazy_string(false);
                        os << *mdep.second << '\n';
                    }
                }
                else
                {
                    auto it{find(chord_, *note)};
                    if (it != chord_.end())
                    {
                        chord_.erase(it);
                    }
                }
            }
        }
    }
    else // not a note event
    {
        const auto rest{dynamic_cast<MidiChannelVoiceNoteRestEvent*>(
            mdep.second.get())};
        if (rest)
        {
            RhythmRational duration{rest->wholes_to_next_event()};
            duration.reduce();
            if (duration > RhythmRational{})
            {
                os << lazy_string(true);
                os << rest->key_string() << ' ';
                (*print_rhythm)(os, duration);
                os << '\n';
            }
        }
        else
        {
            os << lazy_string(false);
            os << *mdep.second << '\n';
        }
    }

    auto zero_rhythm_to_next_event{0 == me->ticks_to_next_event()};
    // Print out the chord.
    if (!chord_.empty())
    {
        auto chord_iter{chord_.begin()};
        {
            // timestamp of the current event.
            const auto now{me->ticks_accumulated() + rest_ticks};

            // Copy the MIDI events that have ticks_to_next_event after now.
            list<MidiChannelVoiceNoteOnEvent> notes_with_next_event_past_now{};
            copy_if(chord_.begin(), chord_.end(),
                back_inserter(notes_with_next_event_past_now),
                [now](const MidiEvent& eachme)
                { return ((eachme.ticks_accumulated()
                + eachme.ticks_to_next_event()) > now); } );
            // From events with ticks_to_next event that is after now,
            // Find the chord note with the smallest ticks_to_next_event.

            // Print out the chord_
            // if it is not the case that the next event is a note_on_1
            // starting at the same time.
            // i.e., if the next note starts at the same time
            // and is the next event...
            list<MidiChannelVoiceNoteOnEvent> not_tied_out_list{};
            if (!(!mdep.second->ticks_to_a_note_start()
                && (1 == mdep.second->events_to_next_note_on())))
            {
                // For tied in chord_
                for (auto& tied : chord_)
                {
                    // If the velocity changed, write a vel command or dynamic.
                    os << lazy_string(true);
                    if (dynamic_ != tied.velocity())
                    {
                        dynamic_ = tied.velocity();
                        os << dynamic_string(dynamic_);
                    }
                    // If the channel has changed, write out the chan command.
                    if (tied.channel() != channel_)
                    {
                        channel_ = tied.channel();
                        os << "chan " << static_cast<int32_t>(channel_) << ' ';
                    }
                    ostringstream oss;
                    // If the tied note started earlier,
                    // then tie it in from the previous chord.
                    if ((tied.ticks_accumulated() < now)
                            || (!note && !note_poly_key_pressure))
                    {
                        oss << '-';
                    }
                    // Write the note:
                    oss << tied.key_string();
                    // If this tied note has a tied.ticks_accumulated()
                    // + ticks_to_noteoff()
                    // is past the next event then tie out.
                    if ((tied.ticks_accumulated() + tied.ticks_to_noteoff())
                      > (me->ticks_accumulated() + me->ticks_to_next_event()))
                    {
                        oss << '-';
                    }
                    else
                    {
                        not_tied_out_list.push_back(tied);
                    }
                    oss << ' ';
                    if (!zero_rhythm_to_next_event)
                    {
                        os << oss.str();
                    }
                }
                if (me->ticks_to_next_event())
                {
                    (*print_rhythm)(os, RhythmRational{
                        me->ticks_to_next_event()
                        - rest_ticks, chord_iter->ticks_per_whole()});
                    os << '\n';
                }
                else // rhythm to next event == 0
                {
                    // If a note_on is more than one event away
                    // then print a zero rhythm.
                    if (mdep.second->events_to_next_note_on() > 1)
                    {
                        (*print_rhythm)(os,
                            mdep.second->wholes_to_next_event());
                        os << '\n';
                    }
                }
            }

            // Note perenniel MIDI issue that it is essentially impossible to
            // match up NOTE_ONs and NOTE_OFFs perfectly because
            // it is possible to have multiple concurrent NOTE_ONs of the
            // same key number.  Anyway we're removing one of them.
            // I can't use list::remove() because it would remove all of the
            // matching notes.
            // Throw out the notes that were in the chord but are not tied out.
            for_each(not_tied_out_list,
                [&](MidiChannelVoiceNoteOnEvent ntol) {
                auto ntol_it{find(chord_, ntol)};
                if (ntol_it != chord_.end()) { chord_.erase(ntol_it); } } );
        }
    }

    for (auto& tiednote : chord_)
    {
        if (tiednote.wholes_to_noteoff())
        {
            tiednote.wholes_to_noteoff(tiednote.wholes_to_noteoff()
                - mdep.second->wholes_to_next_event());
            tiednote.wholes_to_next_event(tiednote.wholes_to_next_event()
                - mdep.second->wholes_to_next_event());
        }
    }
    this->wholes_to_last_event(mdep.second->wholes_to_next_event());
}

string textmidi::PrintLazyTrack::lazy_string(bool lazy) noexcept
{
    if (lazy)
    {
        if (!lazy_)
        {
            lazy_ = true;
            return "LAZY ";
        }
    }
    else
    {
        if (lazy_)
        {
            lazy_ = false;
            return "END_LAZY ";
        }
    }
    return "";
}

void textmidi::PrintLazyTrack::insert_rests() noexcept
{
    // note there is no dimension in ChannelKeyboards for MIDI port.
    // But note-on/note-off pairs still have to pair, except when they don't.

    // This works on building a copy of events plus added rests
    // Because insereting the rests in situ would invalidate the iterators.
    //
    int64_t rest_start_ticks_accumulated{};
    for (auto delay_event_iter{delay_events_.begin()};
              delay_event_iter != delay_events_.end();
            ++delay_event_iter)
    {
        auto me{delay_event_iter->second.get()};
        auto note{dynamic_cast<MidiChannelVoiceNoteEvent*>(me)};
        auto note_on_1{dynamic_cast<
            const MidiChannelVoiceNoteOnEvent* const>(note)};
        auto note_off_1{dynamic_cast<
            const MidiChannelVoiceNoteOffEvent* const>(note)};
        // A note-on with zero velocity is a substitute note-off.
        const auto is_noteoff{(note_on_1 && !note->velocity()) || note_off_1};
        const auto is_noteon { note_on_1 &&  note->velocity()};

        if (is_in_rest())
        {
            // the rest's duration time is
            // the current event's time - rest_start_ticks_accumulated.
            const auto rest_ticks{me->ticks_accumulated()
                - rest_start_ticks_accumulated};
            // the rest's start time is the current event's time + length.
            if (rest_ticks)
            {
                // Construct a rest pseudo-event.
                auto rest{make_shared<MidiChannelVoiceNoteRestEvent>
                    (running_status_, QuartersPerWhole * ticksperquarter_,
                     prefer_sharp_)};
                rest->ticks_accumulated(rest_start_ticks_accumulated);
                rest->ticks_to_next_event(rest_ticks);
                RhythmRational wholes{rest_ticks,
                    QuartersPerWhole * ticksperquarter_};
                wholes.reduce();
                rest->wholes_to_next_event(wholes);
                DelayEvent rest_pair{rest_start_ticks_accumulated, rest};
                delay_events_.emplace(delay_event_iter, rest_pair);
#if 0
                // cppcheck pointed out redundancy
                rest_start_ticks_accumulated = me->ticks_accumulated()
                    + me->ticks_to_next_event();
#endif
                // advance rest_start_ticks_accumulated
                rest_start_ticks_accumulated = me->ticks_accumulated();
            }
        }
        if (is_noteon)
        {
            channel_keyboards_[note->channel() - 1][note->key()]++;
        }
        else
        {
            if (is_noteoff)
            {
                // Update channel keyboards.
                channel_keyboards_[note->channel() - 1][note->key()]--;
                if (channel_keyboards_[note->channel() - 1][note->key()] < 0)
                {
                    cerr << "key counter went negative on channel: "
                         << static_cast<int32_t>(note->channel())
                         << " and key number: "
                         << static_cast<int32_t>(note->key())
                         << ", too many note-offs\n";
                    channel_keyboards_[note->channel() - 1][note->key()] = 0;
                }
                // advance rest_start_ticks_accumulated
                // regardless of whether we are entering a rest period
                rest_start_ticks_accumulated = me->ticks_accumulated();
            }
            else // not a note-on or a note-off event,
                 // hence some other MIDI event
            {
                // advance rest_start_ticks_accumulated
                // regardless of whether we are entering a rest period
                rest_start_ticks_accumulated = me->ticks_accumulated();
            }
        }
    }
}
