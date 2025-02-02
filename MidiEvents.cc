//
// TextMIDITools Version 1.0.86
//
// textmidi 1.0.6
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cstdint>
#include <cstdlib>

#include <set>
#include <map>
#include <vector>
#include <string>
#include <ranges>
#include <algorithm>
#include <memory>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "MidiEvents.h"
#include "MidiMaps.h"
#include "rational_support.h"
#include "MIDIKeyString.h"
#include "MidiString.h"

using namespace std;
using namespace textmidi;
using namespace textmidi::rational;
using namespace midi;

namespace
{
    //
    // channel_0_to_1
    // transforms a MIDI binary channel in (0..15).
    // to a textmidi value (usu. channel or program) in (1..16)
    MidiStreamAtom channel_0_to_1(MidiStreamAtom channel) noexcept
    {
        ++channel;
        return channel;
    }

    string dynamic_string(int velocity) noexcept
    {
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
}
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
constexpr int64_t textmidi::variable_len_value(MidiStreamIterator& midiiter) noexcept
{
    uint64_t len{};
    while (*midiiter & variable_len_flag)
    {
        len <<= variable_len_shift;
        len |= (*midiiter++ & variable_len_byte_mask);
    }
    len <<= variable_len_shift;
    len |= *midiiter++;
    len &= variable_len_word_mask;
    return len;
}

ostream& textmidi::operator<<(ostream& os, const DelayEvent& msg_pair)
{
    if (msg_pair.first > 0) [[likely]]
    {
        os << "DELAY " << dec << msg_pair.first << ' ';
    }
    os << *msg_pair.second;
    return os;
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
void textmidi::MidiEvent::ticks_to_next_event(int64_t ticks_to_next_event) noexcept
{
    midi_event_impl_.ticks_to_next_event(ticks_to_next_event);
}

constexpr int64_t textmidi::MidiEvent::ticks_to_a_note_start() const noexcept
{
    return midi_event_impl_.ticks_to_a_note_start();
}

void textmidi::MidiEvent::ticks_to_a_note_start(int64_t ticks_to_a_note_start) noexcept
{
    midi_event_impl_.ticks_to_a_note_start(ticks_to_a_note_start);
}

constexpr RhythmRational textmidi::MidiEvent::wholes_to_next_event() const noexcept
{
    return midi_event_impl_.wholes_to_next_event();
}

int textmidi::MidiEvent::events_to_next_note_on() const noexcept
{
    return midi_event_impl_.events_to_next_note_on();
}

void textmidi::MidiEvent::events_to_next_note_on(int events_to_next_note_on) noexcept
{
    midi_event_impl_.events_to_next_note_on(events_to_next_note_on);
}

void textmidi::MidiEvent::wholes_to_next_event(const RhythmRational& wholes_to_next_event) noexcept
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
void textmidi::MidiEventImpl::ticks_accumulated(int64_t ticks_accumulated) noexcept
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
void textmidi::MidiEventImpl::ticks_to_next_event(int64_t ticks_to_next_event) noexcept
{
    ticks_to_next_event_ = ticks_to_next_event;
}

constexpr int64_t textmidi::MidiEventImpl::ticks_to_a_note_start() const noexcept
{
    return ticks_to_a_note_start_;
}

void textmidi::MidiEventImpl::ticks_to_a_note_start(int64_t ticks_to_a_note_start) noexcept
{
    ticks_to_a_note_start_ = ticks_to_a_note_start;
}

constexpr RhythmRational textmidi::MidiEventImpl::wholes_to_next_event() const noexcept
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
    return msg.text(os);
}

shared_ptr<MidiEvent> MidiSysExEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        recognized = (start_of_sysex[0] == *midiiter);
    }
    if (recognized)
    {
        ++midiiter;
    }
    return (recognized ? make_shared<MidiSysExEvent>(midiiter) : shared_ptr<MidiSysExEvent>{});
}

void textmidi::MidiSysExEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    int64_t len{variable_len_value(midiiter)};
    int64_t count{};
    do
    {
        data_.push_back(*midiiter);
    } while ((*(++midiiter) != end_of_sysex[0]) && (++count < len));
    if (*midiiter == end_of_sysex[0])
    {
        ++midiiter;
    }
}

//
// Write the text version of the event to os.
ostream& textmidi::MidiSysExEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << hex << "SYSEX";
    int i{};
    const auto subid = sysex_subid_map(data_[i]);
    if (subid)
    {
        // subid
        os << ' ' << *subid;
        ++i;
        // device id
        const auto id = sysex_device_id_map(data_[i]);
        if (id)
        {
            os << ' ' << *id;
        }
        else
        {
            os << ' ' << hex << "0x" << setw(2) << setfill('0')
               << static_cast<unsigned>(data_[i]);
        }
        ++i;
        // subid1
        switch (data_[0])
        {
          case sysex_subid_non_commercial[0]:
            break;
          case sysex_subid_non_realtime[0]:
            {
                const auto id1 = sysex_nonrt_id1_map(data_[i]);
                if (id1)
                {
                    os << ' ' << *id1;
                    ++i;
                }
            }
            break;
          case sysex_subid_realtime[0]:
            {
                const auto id1 = sysex_rt_id1_map(data_[i]);
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
               << static_cast<unsigned>(*it);
        }
    }
    os << dec;
    static_cast<void>(os.flags(flags));
    return os;
}


const long MidiSysExEvent::prefix_len{static_cast<long>(start_of_sysex.size())};

shared_ptr<MidiEvent> MidiSysExRawEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        recognized = (end_of_sysex[0] == *midiiter);
    }
    if (recognized)
    {
        ++midiiter;
    }
    return (recognized ? make_shared<MidiSysExRawEvent>(midiiter) : shared_ptr<MidiSysExRawEvent>{});
}

void textmidi::MidiSysExRawEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    const int64_t len{variable_len_value(midiiter)};
    ranges::copy_n(midiiter, len, back_inserter(data_));
    midiiter += len;
}

ostream& textmidi::MidiSysExRawEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << hex << "SYSEXRAW";
    for (auto sys_ex_it(data_.cbegin());
            (sys_ex_it != data_.cend()) && (*sys_ex_it != end_of_sysex[0]);
            ++sys_ex_it)
    {
        os << ' ' << hex << "0x"
           << setw(2) << setfill('0') << static_cast<unsigned>(*sys_ex_it);
    }
    os << dec;
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiSysExRawEvent& msg)
{
    return msg.text(os);
}

const long MidiSysExRawEvent::prefix_len{static_cast<long>(end_of_sysex.size())};

bool MidiFileMetaEvent::recognize(MidiStreamIterator& midiiter, MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        recognized = (meta_prefix[0] == *midiiter);
    }
    return recognized;
}

const long MidiFileMetaEvent::prefix_len{static_cast<long>(meta_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaSequenceEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            if (equal(sequence_number_prefix.cbegin(), sequence_number_prefix.cend(),
                        midiiter + 1))
            {
                recognized = true;
                midiiter += meta_prefix.size() + sequence_number_prefix.size();
            }
        }
    }
    return (recognized ? make_shared<MidiFileMetaSequenceEvent>(midiiter)
        : shared_ptr<MidiFileMetaSequenceEvent>{});
}

void textmidi::MidiFileMetaSequenceEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    auto msb{*midiiter++};
    auto lsb{*midiiter++};
    sequence_number_ = (msb << bits_per_byte) | lsb;
}

ostream& textmidi::MidiFileMetaSequenceEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "SEQUENCE_NUMBER " << sequence_number_;
    static_cast<void>(os.flags(flags));
    return os;
}

void textmidi::MidiFileMetaSequenceEvent::sequence_number(uint16_t sequence_number) noexcept
{
    sequence_number_ = sequence_number;
}

uint16_t textmidi::MidiFileMetaSequenceEvent::sequence_number() const noexcept
{
    return sequence_number_;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSequenceEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaSequenceEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(sequence_number_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaUnknownEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = !Initial_Meta.contains(*(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaUnknownEvent>(midiiter)
        : shared_ptr<MidiFileMetaUnknownEvent>{});
}

void textmidi::MidiFileMetaUnknownEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    meta_code_ = *midiiter++;
    int64_t len{variable_len_value(midiiter)};
    ranges::copy_n(midiiter, len, back_inserter(data_));
    midiiter += len;
}

ostream& textmidi::MidiFileMetaUnknownEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "UNKNOWN_META " << hex << setw(2) << setfill('0') << "0x" << static_cast<int>(meta_code_);
    ranges::for_each(data_, [&](MidiStreamAtom msa) {
        os << " " << hex << "0x" << setw(2) << setfill('0') << static_cast<unsigned>(msa);});
    os << dec;
    static_cast<void>(os.flags(flags));
    return os;
}

const long MidiFileMetaUnknownEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(unknown_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaMidiChannelEvent::recognize(
    MidiStreamIterator& midiiter, MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            if (equal(midi_channel_prefix.cbegin(), midi_channel_prefix.cend(), midiiter + 1))
            {
                recognized = true;
            }
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + midi_channel_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaMidiChannelEvent>(midiiter)
        : shared_ptr<MidiFileMetaMidiChannelEvent>{});
}

void textmidi::MidiFileMetaMidiChannelEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    channel_ = *midiiter++;
    channel_ = channel_0_to_1(channel_);
    if (channel_ > MidiChannelQty)
    {
        const string errstr{(((string{"MIDI channel ("}
            += boost::lexical_cast<string>(static_cast<int>(channel_)))
            += ") exceeded MIDI limit (") += boost::lexical_cast<string>(MidiChannelQty))
            += ")\n"};
        cerr << errstr;
    }
}

ostream& textmidi::MidiFileMetaMidiChannelEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "MIDI_CHANNEL " << channel_;
    static_cast<void>(os.flags(flags));
    return os;
}

void textmidi::MidiFileMetaMidiChannelEvent::channel(uint16_t channel) noexcept
{
    channel_ = channel;
}

constexpr uint16_t textmidi::MidiFileMetaMidiChannelEvent::channel() noexcept
{
    return channel_;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaMidiChannelEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaMidiChannelEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(midi_channel_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaSetTempoEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            if (equal(tempo_prefix.cbegin(), tempo_prefix.cend(), midiiter + 1))
            {
                recognized = true;
            }
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + tempo_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaSetTempoEvent>(midiiter)
        : shared_ptr<MidiFileMetaSetTempoEvent>{});
}

void textmidi::MidiFileMetaSetTempoEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    tempo_ = *midiiter++;
    tempo_ <<= 8;
    tempo_ |= *midiiter++;
    tempo_ <<= 8;
    tempo_ |= *midiiter++;
}

ostream& textmidi::MidiFileMetaSetTempoEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    if (0 == tempo_)
    {
        os << "TEMPO 0";
    }
    else
    {
        os << "TEMPO " << dec << setprecision(10)
           << ((SecondsPerMinute * UsecPerSecond) / static_cast<double>(tempo_));
    }
    static_cast<void>(os.flags(flags));
    return os;
}

void textmidi::MidiFileMetaSetTempoEvent::tempo(uint32_t tempo) noexcept
{
    tempo_ = tempo;
}

uint32_t textmidi::MidiFileMetaSetTempoEvent::tempo() const noexcept
{
    return tempo_;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSetTempoEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaSetTempoEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(tempo_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaSMPTEOffsetEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            if (equal(smpte_prefix.cbegin(), smpte_prefix.cend(), midiiter + 1))
            {
                recognized = true;
            }
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + smpte_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaSMPTEOffsetEvent>(midiiter)
        : shared_ptr<MidiFileMetaSMPTEOffsetEvent>{});
}

void textmidi::MidiFileMetaSMPTEOffsetEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    hours_ = *midiiter++;
    fps_ = (hours_ >> smpte_fps_shift) & smpte_fps_mask;
    hours_ &= smpte_hours_mask;
    minutes_ = *midiiter++;
    seconds_ = *midiiter++;
    fr_ = *midiiter++;
    ff_ = *midiiter++;
    if ((hours_ > 23) || (minutes_ > 59) || (seconds_ > 59) || (fr_ > 29) || (ff_ > 99))
    {
        cerr << "SMPTE Offset out of range: " << setfill('0') << setw(2) << hours_
             << ':' << setw(2) << minutes_ << ':' << setw(2) << seconds_ << ':'
             << setw(2) << fr_ << ':' << setw(2) << ff_ << '\n'
             << "but permitted ranges are : " << setfill('0') << setw(2) << MaxSmpteHours
             << ':' << setw(2) << MaxSmpteMinutes << ':' << setw(2) << MaxSmpteSeconds << ':'
             << setw(2) << MaxSmpteFrames << ':' << setw(2) << MaxSmpteHundredths << '\n';
    }
}

ostream& textmidi::MidiFileMetaSMPTEOffsetEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "SMPTE_OFFSET ";
    const auto fps = smpte_fps_map(fps_);
    if (fps)
    {
        os << *fps;
    }
    else
    {
        os << "SMPTE_24FPS";
    }
    os << ' ' << setfill('0') << setw(2) << hours_ << ':' << setw(2) << minutes_
       << ':' << setw(2) << seconds_ << ':' << setw(2) << fr_ << ':' << setw(2) << ff_;
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSMPTEOffsetEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaSMPTEOffsetEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(smpte_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaMidiPortEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            if (equal(midi_port_prefix.cbegin(), midi_port_prefix.cend(), midiiter + 1))
            {
                recognized = true;
            }
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + midi_port_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaMidiPortEvent>(midiiter)
        : shared_ptr<MidiFileMetaMidiPortEvent>{});
}

void textmidi::MidiFileMetaMidiPortEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    midiport_ = *midiiter++;
}

ostream& textmidi::MidiFileMetaMidiPortEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "MIDI_PORT " << midiport_;
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaMidiPortEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaMidiPortEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(midi_port_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaTimeSignatureEvent::recognize(
    MidiStreamIterator& midiiter, MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            if (equal(time_signature_prefix.cbegin(), time_signature_prefix.cend(),
                midiiter + 1))
            {
                recognized = true;
            }
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + time_signature_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaTimeSignatureEvent>(midiiter)
        : shared_ptr<MidiFileMetaTimeSignatureEvent>{});
}

void textmidi::MidiFileMetaTimeSignatureEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    numerator_ = *midiiter++;
    denominator_ = *midiiter++;
    clocks_per_click_ = *midiiter++;
    thirtyseconds_per_quarter_= *midiiter++;
}

ostream& textmidi::MidiFileMetaTimeSignatureEvent::text(ostream& os) const
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
    return msg.text(os);
}

const long MidiFileMetaTimeSignatureEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(time_signature_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaKeySignatureEvent::recognize(
    MidiStreamIterator& midiiter, MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            if (equal(key_signature_prefix.cbegin(), key_signature_prefix.cend(), midiiter + 1))
            {
                recognized = true;
                midiiter += meta_prefix.size() + sizeof(key_signature_prefix[0]);
            }
            else
            {
                if (equal(key_signature_prefix_mode_missing.cbegin(), key_signature_prefix_mode_missing.cend(), midiiter + 1))
                {
                    // some programs that were obsolete when they were written left
                    // off the mode byte, so the consume
                    // bytes function has to see the length byte to know that.
                    recognized = true;
                    midiiter += meta_prefix.size() + sizeof(key_signature_prefix_mode_missing[0]);
                }
            }
        }
    }
    return (recognized ? make_shared<MidiFileMetaKeySignatureEvent>(midiiter)
        : shared_ptr<MidiFileMetaKeySignatureEvent>{});
}

void textmidi::MidiFileMetaKeySignatureEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    // Some programs that were obsolete when they were written left off the mode byte,
    // so this consume_stream function has to see the length byte to know that.
    // So this consume_stream takes a bigger byte off the apple than others.
    const auto len{*midiiter++};
    accidentals_ = static_cast<signed char>(*midiiter++);
    if (2 == len)
    {
        minor_mode_ = (*midiiter++ != 0);
    }
}

ostream& textmidi::MidiFileMetaKeySignatureEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    // -1 is one flat, 1, is one sharp, 0 is C or a.

    if (key_signature_map.contains(KeySignatureMap::key_type{accidentals_, minor_mode_}))
    {
        os << "KEY_SIGNATURE "
           << key_signature_map[KeySignatureMap::key_type{accidentals_, minor_mode_}];
    }
    else
    {
        os << "KEY_SIGNATURE "
           << key_signature_map[KeySignatureMap::key_type{0, false}] << " ; illegal accidentals: " << accidentals_;
    }
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaKeySignatureEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaKeySignatureEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(key_signature_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaXmfPatchTypeEvent::recognize(
    MidiStreamIterator& midiiter, MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            if (equal(xmf_patch_type_prefix.cbegin(), xmf_patch_type_prefix.cend(), midiiter + 1))
            {
                recognized = true;
                midiiter += meta_prefix.size() + sizeof(xmf_patch_type_prefix[0]);
            }
        }
    }
    return (recognized ? make_shared<MidiFileMetaXmfPatchTypeEvent>(midiiter)
        : shared_ptr<MidiFileMetaXmfPatchTypeEvent>{});
}

void textmidi::MidiFileMetaXmfPatchTypeEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    // Some programs that were obsolete when they were written left off the mode byte,
    // so this consume_stream function has to see the length byte to know that.
    // So this consume_stream takes a bigger byte off the apple than others.
    // Skipping a byte.  len = *midiiter++;
    *midiiter++;
    xmf_patch_type_ = static_cast<signed char>(*midiiter++);
}

ostream& textmidi::MidiFileMetaXmfPatchTypeEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    // -1 is one flat, 1, is one sharp, 0 is C or a.
    os << "XMF_PATCH_TYPE "
       << xmf_patch_type_map.at(static_cast<XmfPatchTypeEnum>(xmf_patch_type_));
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaXmfPatchTypeEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaXmfPatchTypeEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(xmf_patch_type_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaSequencerSpecificEvent::recognize(MidiStreamIterator& midiiter,
        MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    // page 10 of file format chapter.
    // FF 7F [variable_len] mgfID or 00 mfgid0 mfgid1
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (sequencer_specific_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + sequencer_specific_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaSequencerSpecificEvent>(midiiter)
        : shared_ptr<MidiFileMetaSequencerSpecificEvent>{});
}

void textmidi::MidiFileMetaSequencerSpecificEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    int64_t len{variable_len_value(midiiter)};
    int64_t count{};
    do
    {
        data_.push_back(*midiiter++);
    } while (++count < len);
}

ostream& textmidi::MidiFileMetaSequencerSpecificEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << '\n' << "SEQUENCER_SPECIFIC";
    for (auto it(data_.cbegin());
            (it != data_.cend()) && (*it != end_of_sysex[0]);
            ++it)
    {
        os << ' ' << hex << "0x" << setw(2) << setfill('0')
           << static_cast<unsigned>(*it) << dec;
    }
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSequencerSpecificEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaSequencerSpecificEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(sequencer_specific_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaEndOfTrackEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            if (equal(end_of_track_prefix.cbegin(), end_of_track_prefix.cend(), midiiter + 1))
            {
                recognized = true;
            }
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + end_of_track_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaEndOfTrackEvent>(midiiter)
        : shared_ptr<MidiFileMetaEndOfTrackEvent>{});
}

void textmidi::MidiFileMetaEndOfTrackEvent::consume_stream(MidiStreamIterator& ) noexcept
{
}

ostream& textmidi::MidiFileMetaEndOfTrackEvent::text(ostream& os) const
{
    os << "\nEND_OF_TRACK";
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaEndOfTrackEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaEndOfTrackEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(end_of_track_prefix.size())};

void textmidi::MidiFileMetaStringEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    int64_t len{variable_len_value(midiiter)};
    str_.insert(str_.begin(), midiiter, midiiter + len);
    midiiter += len;
}

ostream& textmidi::MidiFileMetaStringEvent::text(ostream& os) const
{
    string display_str{str_};

    textmidi::make_human_string(display_str);
    os << '"' << display_str << '"';
    return os;
}

ostream& textmidi::MidiFileMetaTextEvent::text(ostream& os) const
{
    os << "TEXT ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaTextEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (text_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + text_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaTextEvent>(midiiter)
        : shared_ptr<MidiFileMetaTextEvent>{});
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaTextEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaTextEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(text_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaProgramNameEvent::text(ostream& os) const
{
    os << "PROGRAM_NAME ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaProgramNameEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (program_name_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + program_name_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaProgramNameEvent>(midiiter)
        : shared_ptr<MidiFileMetaProgramNameEvent>{});
}

const long MidiFileMetaProgramNameEvent::prefix_len{static_cast<long>(
    meta_prefix.size()) + static_cast<long>(program_name_prefix.size())
    + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaDeviceNameEvent::text(ostream& os) const
{
    os << "DEVICE_NAME ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaDeviceNameEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (device_name_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + device_name_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaDeviceNameEvent>(midiiter)
        : shared_ptr<MidiFileMetaDeviceNameEvent>{});
}


const long MidiFileMetaDeviceNameEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(device_name_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaText0AEvent::text(ostream& os) const
{
    os << "TEXT_0A ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaText0AEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (text_0A_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + text_0A_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaText0AEvent>(midiiter)
        : shared_ptr<MidiFileMetaText0AEvent>{});
}

const long MidiFileMetaText0AEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(text_0A_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaText0BEvent::text(ostream& os) const
{
    os << "TEXT_0B ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaText0BEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (text_0B_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + text_0B_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaText0BEvent>(midiiter)
        : shared_ptr<MidiFileMetaText0BEvent>{});
}

const long MidiFileMetaText0BEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(text_0B_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaText0CEvent::text(ostream& os) const
{
    os << "TEXT_0C ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaText0CEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (text_0C_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + text_0C_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaText0CEvent>(midiiter)
        : shared_ptr<MidiFileMetaText0CEvent>{});
}

const long MidiFileMetaText0CEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(text_0C_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaText0DEvent::text(ostream& os) const
{
    os << "TEXT_0D ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaText0DEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (text_0D_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + text_0D_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaText0DEvent>(midiiter)
        : shared_ptr<MidiFileMetaText0DEvent>{});
}

const long MidiFileMetaText0DEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(text_0D_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaText0EEvent::text(ostream& os) const
{
    os << "TEXT_0E ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaText0EEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (text_0E_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + text_0E_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaText0EEvent>(midiiter)
        : shared_ptr<MidiFileMetaText0EEvent>{});
}

const long MidiFileMetaText0EEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(text_0E_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaText0FEvent::text(ostream& os) const
{
    os << "TEXT_0F ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaText0FEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (text_0F_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + text_0F_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaText0FEvent>(midiiter)
        : shared_ptr<MidiFileMetaText0FEvent>{});
}

const long MidiFileMetaText0FEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(text_0F_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaCopyrightEvent::text(ostream& os) const
{
    os << "COPYRIGHT ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaCopyrightEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (copyright_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + copyright_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaCopyrightEvent>(midiiter)
        : shared_ptr<MidiFileMetaCopyrightEvent>{});
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaCopyrightEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaCopyrightEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(copyright_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaTrackNameEvent::text(ostream& os) const
{
    os << "TRACK ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaTrackNameEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (track_name_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + track_name_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaTrackNameEvent>(midiiter)
        : shared_ptr<MidiFileMetaTrackNameEvent>{});
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaTrackNameEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaTrackNameEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(track_name_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaInstrumentEvent::text(ostream& os) const
{
    os << "INSTRUMENT ";
    return MidiFileMetaStringEvent::text(os);
    return os;
}

shared_ptr<MidiEvent> MidiFileMetaInstrumentEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (instrument_name_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + instrument_name_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaInstrumentEvent>(midiiter)
        : shared_ptr<MidiFileMetaInstrumentEvent>{});
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaInstrumentEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaInstrumentEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(instrument_name_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaLyricEvent::text(ostream& os) const
{
    os << "LYRIC ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaLyricEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (lyric_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + lyric_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaLyricEvent>(midiiter)
        : shared_ptr<MidiFileMetaLyricEvent>{});
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaLyricEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaLyricEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(lyric_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaMarkerEvent::text(ostream& os) const
{
    os << "MARKER ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaMarkerEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (marker_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + marker_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaMarkerEvent>(midiiter)
        : shared_ptr<MidiFileMetaMarkerEvent>{});
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaMarkerEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaMarkerEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(marker_prefix.size()) + variable_length_quantity_min_len};

ostream& textmidi::MidiFileMetaCuePointEvent::text(ostream& os) const
{
    os << "CUE_POINT ";
    return MidiFileMetaStringEvent::text(os);
}

shared_ptr<MidiEvent> MidiFileMetaCuePointEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        if (MidiFileMetaEvent::recognize(midiiter, the_end))
        {
            recognized = (cue_point_prefix[0] == *(midiiter + 1));
        }
    }
    if (recognized)
    {
        midiiter += meta_prefix.size() + cue_point_prefix.size();
    }
    return (recognized ? make_shared<MidiFileMetaCuePointEvent>(midiiter)
        : shared_ptr<MidiFileMetaCuePointEvent>{});
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaCuePointEvent& msg)
{
    return msg.text(os);
}

const long MidiFileMetaCuePointEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(cue_point_prefix.size()) + variable_length_quantity_min_len};

bool MidiChannelVoiceModeEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end, RunningStatusStandard& running_status) noexcept
{
    bool recognized{};
    // Recognize an event with running status.
    // All software evolves during maintenance to make all
    // data global and asymmetrically handled.
    // channel_pressure and control change have only 1 byte following the command.
    // Check if these are data bytes and not commands.
    if ((running_status.command() == channel_pressure[0]) || (running_status.command() == program[0]))
    {
        if (distance(midiiter, the_end) >= 1)
        {
            // look for two bytes in a row with 1 << 8 clear.
            // This is how we look for data bytes with running status.
            if (!(*midiiter & event_flag))
            {
                recognized = true;
            }
        }
    }
    else
    {
        if (distance(midiiter, the_end) >= prefix_len)
        {
            // look for two bytes in a row with 1 << 8 clear.
            // This is how we look for data bytes with running status.
            if (!(*midiiter & event_flag))
            {
                if (!(*(midiiter + 1) & event_flag))
                {
                    recognized = true;
                }
            }
        }
    }
    return recognized;
}

const long MidiChannelVoiceModeEvent::prefix_len{
    full_note_length - static_cast<long>(sizeof(note_on[0]))};

MidiStreamAtom textmidi::MidiChannelVoiceModeEvent::channel() const noexcept
{
    return channel_;
}

void textmidi::MidiChannelVoiceModeEvent::channel(MidiStreamAtom channel) noexcept
{
    channel_ = channel;
}

constexpr RunningStatusStandard& MidiChannelVoiceModeEvent::running_status() noexcept
{
    return running_status_;
}

const RunningStatusStandard& MidiChannelVoiceModeEvent::running_status() const noexcept
{
    return running_status_;
}

const RunningStatusStandard& MidiChannelVoiceModeEvent::local_status() const noexcept
{
    return local_status_;
}
void MidiChannelVoiceModeEvent::local_status(const RunningStatusStandard& rs) noexcept
{
    local_status_ = rs;
}
RunningStatusStandard& MidiChannelVoiceModeEvent::local_status() noexcept
{
    return local_status_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceModeEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiChannelVoiceNoteEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    // Skip a beginning byte that is an event type byte.
    if (event_flag & *midiiter)
    {
        local_status().running_status(*midiiter);
        ++midiiter;
    }
    else
    {
        local_status(running_status());
    }
    key_ = *midiiter++;
    if (key_ > MaxKeyboardKey)
    {
        const string errstr{((string("MIDI Key Number (")
            += boost::lexical_cast<string>(static_cast<int>(key_)))
            += ") exceeded MIDI limit (") += boost::lexical_cast<string>(MaxKeyboardKey) };
        cerr << errstr;
    }
    channel(local_status().channel() + 1);
    key_string_ = num_to_note(key_, prefer_sharp_);
    velocity_ = static_cast<int>(*midiiter++);
    if (velocity_ > MaxDynamic)
    {
        const string errstr{(((string("MIDI velocity (")
            += boost::lexical_cast<string>(static_cast<int>(velocity_)))
            += ") exceeded MIDI limit (") += boost::lexical_cast<string>(MaxDynamic)) += ")\n"};
        cerr << errstr;
    }
}

constexpr MidiStreamAtom textmidi::MidiChannelVoiceNoteEvent::key() const noexcept
{
    return key_;
}

void textmidi::MidiChannelVoiceNoteEvent::key(MidiStreamAtom key) noexcept
{
    key_ = key;
}

void textmidi::MidiChannelVoiceNoteEvent::key_string(string_view key_string) noexcept
{
    key_string_ = key_string;
}

string textmidi::MidiChannelVoiceNoteEvent::key_string() const noexcept
{
    return key_string_;
}

void textmidi::MidiChannelVoiceNoteEvent::velocity(MidiStreamAtom velocity) noexcept
{
    velocity_ = velocity;
}

constexpr MidiStreamAtom textmidi::MidiChannelVoiceNoteEvent::velocity() const noexcept
{
    return velocity_;
}

ostream& textmidi::MidiChannelVoiceNoteEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << dec << static_cast<unsigned>(channel()) << ' '
       << key_string() << ' ' << static_cast<unsigned>(velocity());
    static_cast<void>(os.flags(flags));
    return os;
}

constexpr uint32_t textmidi::MidiChannelVoiceNoteEvent::ticks_per_whole() const noexcept
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

RhythmRational textmidi::MidiChannelVoiceNoteEvent::wholes_to_noteoff() const noexcept
{
    return wholes_to_noteoff_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteEvent& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiChannelVoiceNoteOnEvent::text(ostream& os) const
{
    os << "NOTE_ON ";
    return MidiChannelVoiceNoteEvent::text(os);
}

void textmidi::MidiChannelVoiceNoteOnEvent
    ::ticks_to_noteoff(int64_t ticks_to_noteoff) noexcept
{
    ticks_to_noteoff_ = ticks_to_noteoff;
}

constexpr int64_t textmidi::MidiChannelVoiceNoteOnEvent::ticks_to_noteoff() const noexcept
{
    return ticks_to_noteoff_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteOnEvent& msg)
{
    return msg.text(os);
}

shared_ptr<MidiEvent> MidiChannelVoiceNoteOnEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end, RunningStatusStandard& running_status, shared_ptr<bool> prefer_sharp, uint32_t ticks_per_whole) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        recognized = ((*midiiter & ~channel_mask) == note_on[0]);
    }
    if (recognized)
    {
        running_status.running_status(*midiiter);
        ++midiiter;
    }
    return (recognized ? make_shared<MidiChannelVoiceNoteOnEvent>(running_status,
        ticks_per_whole, prefer_sharp, midiiter)
        : shared_ptr<MidiChannelVoiceNoteOnEvent>{});
}

const long MidiChannelVoiceNoteOnEvent::prefix_len{full_note_length};

shared_ptr<MidiEvent> MidiChannelVoiceNoteOffEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end, RunningStatusStandard& running_status, shared_ptr<bool> prefer_sharp, uint32_t ticks_per_whole) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        recognized  = ((*midiiter & ~channel_mask) == note_off[0]);
    }
    if (recognized)
    {
        running_status.running_status(*midiiter);
        ++midiiter;
    }
    return (recognized ? make_shared<MidiChannelVoiceNoteOffEvent>(running_status, ticks_per_whole, prefer_sharp, midiiter)
        : shared_ptr<MidiChannelVoiceNoteOffEvent>{});
}

const long MidiChannelVoiceNoteOffEvent::prefix_len{full_note_length};

shared_ptr<MidiEvent> MidiChannelVoicePitchBendEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end, RunningStatusStandard& running_status) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        recognized = ((*midiiter & ~channel_mask) == midi::pitch_wheel[0]);
    }
    if (recognized)
    {
        running_status.running_status(*midiiter);
        ++midiiter;
    }
    return (recognized ? make_shared<MidiChannelVoicePitchBendEvent>(running_status, midiiter)
        : shared_ptr<MidiChannelVoicePitchBendEvent>{});
}

void textmidi::MidiChannelVoicePitchBendEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    local_status(running_status());
    MidiStreamAtom pitch_wheel_lsb{*midiiter++};
    if (pitch_wheel_lsb > MaxPitchBend)
    {
        const string errstr{((string("MIDI Pitch Bend LSB (")
            += boost::lexical_cast<string>(static_cast<int>(pitch_wheel_lsb)))
            +=  ") exceeded MIDI limit (")
            += boost::lexical_cast<string>(MaxPitchBend) += ")\n"};
        cerr << errstr;
    }
    MidiStreamAtom pitch_wheel_msb{*midiiter++};
    if (pitch_wheel_msb > MaxPitchBend)
    {
        const string errstr{((string("MIDI Pitch Bend MSB (")
            += boost::lexical_cast<string>(static_cast<int>(pitch_wheel_msb)))
            +=  ") exceeded MIDI limit (")
            += boost::lexical_cast<string>(MaxPitchBend) += ")\n"};
        cerr << errstr;
    }
    channel(local_status().channel() + 1);
    pitch_wheel_ = pitch_wheel_lsb | (pitch_wheel_msb << byte7_shift);
}

ostream& textmidi::MidiChannelVoicePitchBendEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "PITCH_WHEEL " << dec << static_cast<int>(channel())
       << ' ' << static_cast<int>(pitch_wheel_);
    static_cast<void>(os.flags(flags));
    return os;
}

void textmidi::MidiChannelVoicePitchBendEvent::pitch_wheel(MidiStreamAtom pitch_wheel) noexcept
{
    pitch_wheel_ = pitch_wheel;
}

MidiStreamAtom textmidi::MidiChannelVoicePitchBendEvent::pitch_wheel() const noexcept
{
    return pitch_wheel_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoicePitchBendEvent& msg)
{
    return msg.text(os);
}

const long MidiChannelVoicePitchBendEvent::prefix_len{full_note_length};

shared_ptr<MidiEvent> MidiChannelVoiceControlChangeEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end, RunningStatusStandard& running_status) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        recognized = ((*midiiter & ~channel_mask) == control[0]);
    }
    if (recognized)
    {
        running_status.running_status(*midiiter);
        ++midiiter;
    }
    return (recognized ? make_shared<MidiChannelVoiceControlChangeEvent>(running_status, midiiter)
        : shared_ptr<MidiChannelVoiceControlChangeEvent>{});
}

void textmidi::MidiChannelVoiceControlChangeEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    local_status(running_status());
    channel(local_status().channel() + 1);
    id_      = *midiiter++;
    value_   = *midiiter++;
    if (value_ & ~byte7_mask)
    {
        cerr << "Illegal 8-bit control value: "
             << hex << "0x" << setw(2) << setfill('0') << static_cast<int>(value_) << dec << '\n';
    }
}

ostream& textmidi::MidiChannelVoiceControlChangeEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "CONTROL " << dec << static_cast<unsigned>(channel()) << ' ';
    if (control_pan[0] == id_)
    {
        int32_t temp_pan{value_};
        temp_pan -= PanExcess64;
        if (!((MinSignedPan <= temp_pan) && (temp_pan <= MaxSignedPan)))
        {
            const string errstr{(((((string("Illegal pan value fails range: ")
                += boost::lexical_cast<string>(MinSignedPan)) += " <= ")
                += boost::lexical_cast<string>(temp_pan)) += " <= ")
                += boost::lexical_cast<string>(MaxSignedPan)) += '\n'};
            cerr << errstr;
        }
        string panstring{};
        const auto pan = pan_map(temp_pan);
        panstring = (pan ? *pan : boost::lexical_cast<string>(temp_pan));
        os << "PAN " << ' ' << panstring << ' ';
    }
    else
    {
        const auto control_str = control_function_map(id_);
        if (control_str)
        {
            os << *control_str;
        }
        else
        {
            if (static_cast<int>(RegisteredParameterMsbs::parameter_3d_msb) == id_)
            {
                os << hex << "0x" << setw(2) << setfill('0') << static_cast<unsigned>(id_) << dec;
            }
            else
            {
                os << hex << "0x" << setw(2) << setfill('0') << static_cast<unsigned>(id_) << dec;
            }
        }
        os << ' ';
        set<MidiStreamAtom> on_off_controls{control_portamento_on_off[0],
            control_legato_foot[0], control_sostenuto[0], control_local_control[0]};
        if (on_off_controls.contains(id_))
        {
            if (value_ >= control_pedalthreshold)
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
            os << hex << "0x" << setw(2) << setfill('0') << static_cast<unsigned>(value_) << dec;
        }
    }
    os << ' ';
    static_cast<void>(os.flags(flags));
    return os;
}

MidiStreamAtom textmidi::MidiChannelVoiceControlChangeEvent::id() const noexcept
{
    return id_;
}

void textmidi::MidiChannelVoiceControlChangeEvent::id(MidiStreamAtom id) noexcept
{
    id_ = id;
}

MidiStreamAtom textmidi::MidiChannelVoiceControlChangeEvent::value() const noexcept
{
    return value_;
}

void textmidi::MidiChannelVoiceControlChangeEvent::value(MidiStreamAtom value) noexcept
{
    value_ = value;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceControlChangeEvent& msg)
{
    return msg.text(os);
}

const long MidiChannelVoiceControlChangeEvent::prefix_len{full_note_length};

shared_ptr<MidiEvent> MidiChannelVoiceChannelPressureEvent::recognize(
    MidiStreamIterator& midiiter, MidiStreamIterator the_end, RunningStatusStandard& running_status) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        recognized = ((*midiiter & ~channel_mask) == channel_pressure[0]);
    }
    if (recognized)
    {
        running_status.running_status(*midiiter);
        ++midiiter;
    }
    return (recognized ? make_shared<MidiChannelVoiceChannelPressureEvent>
        (running_status, midiiter) : shared_ptr<MidiChannelVoiceChannelPressureEvent>{});
}

void textmidi::MidiChannelVoiceChannelPressureEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    local_status(running_status());
    channel(local_status().channel() + 1);
    pressure_ = *midiiter++;
    if (pressure_ > MaxDynamic)
    {
        const string errstr{(((string("MIDI pressure (")
            += boost::lexical_cast<string>(static_cast<int>(pressure_)))
            += ") exceeded MIDI limit (") += boost::lexical_cast<string>(MaxDynamic))
            += ")\n" };
        cerr << errstr;
    }
}

ostream& textmidi::MidiChannelVoiceChannelPressureEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "CHANNEL_PRESSURE " << dec << static_cast<int>(channel())
       << ' ' << static_cast<int>(pressure_);
    static_cast<void>(os.flags(flags));
    return os;
}

void textmidi::MidiChannelVoiceChannelPressureEvent::pressure(MidiStreamAtom pressure) noexcept
{
    pressure_ = pressure;
}

MidiStreamAtom textmidi::MidiChannelVoiceChannelPressureEvent::pressure() const noexcept
{
    return pressure_;
}

ostream& textmidi::operator<<(ostream& os,
        const MidiChannelVoiceChannelPressureEvent& msg)
{
    return msg.text(os);
}

const long MidiChannelVoiceChannelPressureEvent::prefix_len{full_note_length};

ostream& textmidi::MidiChannelVoicePolyphonicKeyPressureEvent::text(ostream& os) const
{
    os << "POLY_KEY_PRESSURE ";
    return MidiChannelVoiceNoteEvent::text(os);
}

shared_ptr<MidiEvent> MidiChannelVoicePolyphonicKeyPressureEvent
    ::recognize(MidiStreamIterator& midiiter, MidiStreamIterator the_end,
    RunningStatusStandard& running_status, shared_ptr<bool> prefer_sharp, uint32_t ticks_per_whole) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        recognized = ((*midiiter & ~channel_mask) == polyphonic_key_pressure[0]);
    }
    if (recognized)
    {
        running_status.running_status(*midiiter);
        ++midiiter;
    }
    return (recognized ? make_shared<MidiChannelVoicePolyphonicKeyPressureEvent>
        (running_status, ticks_per_whole, prefer_sharp, midiiter) : shared_ptr<MidiChannelVoicePolyphonicKeyPressureEvent>{});
}

ostream& textmidi::operator<<(ostream& os,
        const MidiChannelVoicePolyphonicKeyPressureEvent& msg)
{
    return msg.text(os);
}

const long MidiChannelVoicePolyphonicKeyPressureEvent::prefix_len{full_note_length};

ostream& textmidi::MidiChannelVoiceNoteRestEvent::text(ostream& os) const
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

ostream& textmidi::MidiChannelVoiceNoteOffEvent::text(ostream& os) const
{
    os << "NOTE_OFF ";
    return MidiChannelVoiceNoteEvent::text(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteOffEvent& msg)
{
    return msg.text(os);
}

const size_t prefix_len{full_note_length};

shared_ptr<MidiEvent> MidiChannelVoiceProgramChangeEvent
   ::recognize(MidiStreamIterator& midiiter, MidiStreamIterator the_end, RunningStatusStandard& running_status) noexcept
{
    bool recognized{};
    if (distance(midiiter, the_end) >= prefix_len)
    {
        recognized = ((*midiiter & ~channel_mask) == midi::program[0]);
    }
    if (recognized)
    {
        running_status.running_status(*midiiter);
        ++midiiter;
    }
    return (recognized ? make_shared<MidiChannelVoiceProgramChangeEvent>(running_status, midiiter)
        : shared_ptr<MidiChannelVoiceProgramChangeEvent>{});
}

void textmidi::MidiChannelVoiceProgramChangeEvent::consume_stream(MidiStreamIterator& midiiter) noexcept
{
    local_status(running_status());
    channel(local_status().channel() + 1);
    program_ = *midiiter++;
    if (program_ & ~byte7_mask)
    {
        cerr << "Illegal 8-bit control value: "
             << hex << static_cast<int>(program_) << dec << '\n';
    }
}

ostream& textmidi::MidiChannelVoiceProgramChangeEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "PROGRAM " << dec << static_cast<int>(channel()) << ' '
       << (static_cast<int>(program_) + 1);
    static_cast<void>(os.flags(flags));
    return os;
}

void textmidi::MidiChannelVoiceProgramChangeEvent::program(MidiStreamAtom program) noexcept
{
    program_ = program;
}

constexpr MidiStreamAtom textmidi::MidiChannelVoiceProgramChangeEvent::program() const noexcept
{
    return program_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceProgramChangeEvent& msg)
{
    return msg.text(os);
}

const long MidiChannelVoiceProgramChangeEvent::prefix_len{full_note_length};

DelayEvent textmidi::MidiEventFactory::operator()(MidiStreamIterator& midiiter, int64_t& ticks_accumulated)
{
    int64_t delaynum64{variable_len_value(midiiter)};
    DelayEvent midi_delay_event_pair(delaynum64, nullptr);
    ticks_accumulated += delaynum64;

    bool clear_ticks_accumulated{};
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaSequenceEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaTextEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaProgramNameEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaDeviceNameEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaText0AEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaText0BEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaText0CEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaText0DEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaText0EEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaText0FEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaCopyrightEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaTrackNameEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaInstrumentEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaLyricEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaMarkerEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaCuePointEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaUnknownEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaMidiChannelEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
    {
        if (midi_delay_event_pair.second = MidiFileMetaEndOfTrackEvent::recognize(midiiter, midi_end_))
        {
            clear_ticks_accumulated = true;
            running_status_.clear();
        }
    }
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaSetTempoEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaSMPTEOffsetEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaTimeSignatureEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaKeySignatureEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaXmfPatchTypeEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaMidiPortEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiFileMetaSequencerSpecificEvent::recognize(midiiter, midi_end_);
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
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiSysExEvent::recognize(midiiter, midi_end_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiSysExRawEvent::recognize(midiiter, midi_end_);

    if (!midi_delay_event_pair.second)
    {
        if (MidiChannelVoiceModeEvent::recognize(midiiter, midi_end_, running_status_))
        {
            if (running_status_.running_status_valid())
            {
                switch (running_status_.command())
                {
                  case NoteOn:
                    midi_delay_event_pair.second = make_shared<MidiChannelVoiceNoteOnEvent>(running_status_, ticks_per_whole_, prefer_sharp_, midiiter);
                    break;
                  case NoteOff:
                    midi_delay_event_pair.second = make_shared<MidiChannelVoiceNoteOffEvent>(running_status_, ticks_per_whole_, prefer_sharp_, midiiter);
                    break;
                  case PolyphonicKeyPressure:
                    midi_delay_event_pair.second = make_shared<MidiChannelVoicePolyphonicKeyPressureEvent>(running_status_, ticks_per_whole_, prefer_sharp_, midiiter);
                    break;
                  case Control:
                    midi_delay_event_pair.second = make_shared<MidiChannelVoiceControlChangeEvent>(running_status_, midiiter);
                    break;
                  case Program:
                    midi_delay_event_pair.second = make_shared<MidiChannelVoiceProgramChangeEvent>(running_status_, midiiter);
                    break;
                  case ChannelPressure:
                    midi_delay_event_pair.second = make_shared<MidiChannelVoiceChannelPressureEvent>(running_status_, midiiter);
                    break;
                  case PitchWheel:
                    midi_delay_event_pair.second = make_shared<MidiChannelVoicePitchBendEvent >(running_status_, midiiter);
                    break;
                  default:
                    throw(runtime_error{"unknown bytes parsing running status"});
                    break;
                }
            }
        }
    }
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoiceNoteOnEvent::recognize(midiiter, midi_end_, running_status_, prefer_sharp_, ticks_per_whole_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoiceNoteOffEvent::recognize(midiiter, midi_end_, running_status_, prefer_sharp_, ticks_per_whole_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoicePolyphonicKeyPressureEvent::recognize(midiiter, midi_end_, running_status_, prefer_sharp_, ticks_per_whole_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoiceProgramChangeEvent::recognize(midiiter, midi_end_, running_status_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoiceChannelPressureEvent::recognize(midiiter, midi_end_, running_status_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoicePitchBendEvent::recognize(midiiter, midi_end_, running_status_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoiceControlChangeEvent::recognize(midiiter, midi_end_, running_status_);

    if (midi_delay_event_pair.second)
    {
        midi_delay_event_pair.second->ticks_accumulated(ticks_accumulated);
        if (clear_ticks_accumulated)
        {
            ticks_accumulated = 0LU;
            clear_ticks_accumulated = false;
        }
        MidiChannelVoiceModeEvent* mcvm{dynamic_cast<MidiChannelVoiceModeEvent*>
            (midi_delay_event_pair.second.get())};
        if (mcvm)
        {
            running_status_ = mcvm->local_status();
        }
    }
    else
    {
        cerr << "Unrecognized byte sequence:\n";
        for (auto i{0}; i < 32; ++i)
        {
            cerr << setfill('0') << setw(2) << hex << static_cast<int>(*midiiter++) << ' ' << dec;
        }
        cerr << '\n';
        cerr << "seeking next command\n";
        while ((midiiter != midi_end_) && ((*midiiter & event_flag) == 0))
        {
            ++midiiter;
        }
        --midiiter; // find any delay
        midi_delay_event_pair.second = make_shared<MidiFileMetaTextEvent>(midiiter);
        midi_delay_event_pair.second->ticks_accumulated(ticks_accumulated);
        if (clear_ticks_accumulated)
        {
            ticks_accumulated = 0LU;
            clear_ticks_accumulated = false;
        }
    }
    return midi_delay_event_pair;
}

bool textmidi::PrintLazyTrack::is_in_rest() const noexcept
{
    auto inrest{true};
    ranges::for_each(channel_keyboards_,
        [&](const KeyBoard& chkb) { inrest = inrest
        && ranges::find_if(chkb, [](int k) { return k > 0; } ) == chkb.end(); });
    return inrest;
}

void textmidi::PrintLazyTrack::ticks_to_note_stop() noexcept
{
    for (auto delay_event_iter   {delay_events_.begin()};
             (delay_event_iter != delay_events_.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>(delay_event_iter->second.get()); ++delay_event_iter)
    {
        // me is MidiEvent
        auto me{delay_event_iter->second.get()};
        auto note   {dynamic_cast<MidiChannelVoiceNoteEvent*>(me)};
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>(note)};
        // If this is a note on but not a zero-velocity
        // which would be an ersatz note-off:
        if (note_on && note->velocity())
        {
            // A second event iterator to look for the
            // corresponding note-off or note-on with velocity 0
            // that matches the current note-on.
            auto delay_event_iter2{delay_event_iter};
            // go to next event
            ++delay_event_iter2;
            // Look from the next event to the end-of-track.
            for ( ; (delay_event_iter2 != delay_events_.end())
                     && (!dynamic_cast<MidiFileMetaEndOfTrackEvent*>(delay_event_iter2->second.get()));
                 ++delay_event_iter2)
            {
                // If the delay is finite then this is not at the current chord.
                auto me2{delay_event_iter2->second.get()};
                auto note2{dynamic_cast<MidiChannelVoiceNoteEvent*>(me2)};
                auto note_on2{dynamic_cast<MidiChannelVoiceNoteOnEvent*>(note2)};
                auto note_off2{dynamic_cast<MidiChannelVoiceNoteOffEvent*>(note2)};
                auto note_poly_key_pressure2{dynamic_cast<MidiChannelVoicePolyphonicKeyPressureEvent*>(note2)};
                if (note_on2)
                {
                    if (!note_on2->velocity() && (*note == *note2) && !note_poly_key_pressure2)
                    {
                        // We have a note-on of velocity zero that is an ersatz note-off
                        // that has not been matched yet but has the same key and channel.
                        note_on->ticks_to_noteoff(
                            me2->ticks_accumulated() - me->ticks_accumulated());

                        RhythmRational ratio_to_note_off{note_on->ticks_to_noteoff(),
                            ticksperquarter_ * QuartersPerWhole};
                        note_on->wholes_to_noteoff(
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
                            note_on->ticks_to_noteoff(
                                me2->ticks_accumulated() - me->ticks_accumulated());
                            RhythmRational ratio_to_note_off{note_on->ticks_to_noteoff(),
                                ticksperquarter_ * QuartersPerWhole};
                            note_on->wholes_to_noteoff(
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
                                rest->ticks_to_next_event(), ticksperquarter_ * QuartersPerWhole};
                            ratio_to_note_off.reduce();
                            rest->wholes_to_noteoff(rational::snap(ratio_to_note_off, quantum_));
                        }
                    }
                }
            }
            if (note_on->ticks_to_noteoff() == numeric_limits<int64_t>().max())
            {
                cerr << "no noteoff found, channel: "
                     << static_cast<int>(note->channel()) << " key number: "
                     << static_cast<int>(note->key()) << '\n';
            }
        }
    }
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
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>(delay_event_iter->second.get());
        ++delay_event_iter)
    {
        auto me{delay_event_iter->second.get()};
        auto note{dynamic_cast<MidiChannelVoiceNoteEvent*>(me)};
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>(note)};
        if (note_on && note->velocity())
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
                auto next_note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>(next_note)};
                if (next_note_on && next_note->velocity())
                {
                    note_on->ticks_to_a_note_start(
                        me2->ticks_accumulated() - me->ticks_accumulated());
                    note_on->events_to_next_note_on(distance(delay_event_iter, delay_event_iter2));
                    break;
                }
            }
        }
    }
}

void textmidi::PrintLazyTrack::ticks_to_next_event() noexcept
{
    const auto len(delay_events_.size());
    auto lopped_end   = ranges::take_view(delay_events_, len - 1);
    auto lopped_begin = ranges::drop_view(delay_events_, 1);
    if (len > 1)
    {
        ranges::transform(lopped_end, lopped_begin, delay_events_.begin(),
            [this](const DelayEvent& mde1, const DelayEvent& mde2)
            { DelayEvent mde_rtn{mde1};
            mde_rtn.second->ticks_to_next_event(
                mde2.second->ticks_accumulated() - mde1.second->ticks_accumulated());
            RhythmRational ratio_to_next_event{
                mde_rtn.second->ticks_to_next_event(), QuartersPerWhole * ticksperquarter_};
            mde_rtn.second->wholes_to_next_event(rational::snap(ratio_to_next_event, quantum_));
            auto temp{mde_rtn.second->wholes_to_next_event()};
            temp.reduce();
            mde_rtn.second->wholes_to_next_event(temp);
            return mde_rtn; });
    }
}

void textmidi::PrintLazyTrack::print(ostream& os, DelayEvent& mdep) noexcept
{
    //
    // cast to the base and two final classes.
    // TODO: have NoteOn and NoteOff take care of themselves without
    // revealing their respective typea.
    int64_t rest_ticks{};
    auto me{mdep.second.get()};
    auto note{dynamic_cast<MidiChannelVoiceNoteEvent*>(me)};
    auto note_poly_key_pressure{dynamic_cast<MidiChannelVoicePolyphonicKeyPressureEvent*>(note)};
    // Define a function that determines if wholes_to_noteoff == 0.
    // A NoteOn with velocity == 0 is an ersatz note off.
    // If this is full NoteOn:
    if (note)
    {
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>(mdep.second.get())};
        auto note_off{dynamic_cast<MidiChannelVoiceNoteOffEvent*>(mdep.second.get())};
        // Save the ticks of the start time of the new note.
        const RhythmRational wholes_to_event{mdep.second->ticks_to_next_event(), note->ticks_per_whole()};
        // If this is a new note with a non-zero velocity:
        if (note_on && note->velocity())
        {
            // Add note to chord_.
            chord_.push_back(*note_on);
        }
        else // velocity == 0 or noteoff
        {
            // Test note_on before note->velocity using short-circuited evaluation.
            // not a note event but could be a rest or poly pressure which are note events.
            if (!note_poly_key_pressure)
            {
                if (!(note_off || (note_on && (note->velocity() == 0))))
                {
                    const auto rest{dynamic_cast<MidiChannelVoiceNoteRestEvent*>(mdep.second.get())};
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
                    else // poly key pressure (derived from NoteEvent) should land here
                         // along with all non-note events.
                    {
                        os << lazy_string(false);
                        os << *mdep.second << '\n';
                    }
                }
                else
                {
                    auto it{ranges::find(chord_, *note)};
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
        const auto rest{dynamic_cast<MidiChannelVoiceNoteRestEvent*>(mdep.second.get())};
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
            const auto now{me->ticks_accumulated() + rest_ticks}; // timestamp of the current event.

            // Copy the MIDI events that that have ticks_to_next_event after now.
            list<MidiChannelVoiceNoteOnEvent> notes_with_next_event_past_now{};
            copy_if(chord_.begin(), chord_.end(), back_inserter(notes_with_next_event_past_now),
                [now](const MidiEvent& eachme)
                { return ((eachme.ticks_accumulated() + eachme.ticks_to_next_event()) > now); } );
            // From events with ticks_to_next event that is after now,
            // Find the chord note with the smallest ticks_to_next_event.

            // Print out the chord_
            // if it is not the case that the next event is a note_on starting at the same time.
            // i.e., if the next note starts at the same time and is the next event...
            list<MidiChannelVoiceNoteOnEvent> not_tied_out_list{};
            if (!(!mdep.second->ticks_to_a_note_start() && (1 == mdep.second->events_to_next_note_on())))
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
                        os << "chan " << static_cast<int>(channel_) << ' ';
                    }
                    ostringstream oss;
                    // If the tied note started earlier, then tie it in from the previous chord.
                    if ((tied.ticks_accumulated() < now) || (!note && !note_poly_key_pressure))
                    {
                        oss << '-';
                    }
                    // Write the note:
                    oss << tied.key_string();
                    // If this tied note has a tied.ticks_accumulated() + ticks_to_noteoff()
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
                    (*print_rhythm)(os, RhythmRational{me->ticks_to_next_event()
                        - rest_ticks, chord_iter->ticks_per_whole()});
                    os << '\n';
                }
                else // rhythm to next event == 0
                {
                    // If a note_on is more than one event away then print a zero rhythm.
                    if (mdep.second->events_to_next_note_on() > 1)
                    {
                        (*print_rhythm)(os, mdep.second->wholes_to_next_event());
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
            ranges::for_each(not_tied_out_list,
                [&](MidiChannelVoiceNoteOnEvent ntol) { auto ntol_it{ranges::find(chord_, ntol)};
                    if (ntol_it != chord_.end()) { chord_.erase(ntol_it); } } );
        }
    }

    for (auto& tiednote : chord_)
    {
        if (tiednote.wholes_to_noteoff())
        {
            tiednote.wholes_to_noteoff(tiednote.wholes_to_noteoff() - mdep.second->wholes_to_next_event());
            tiednote.wholes_to_next_event(tiednote.wholes_to_next_event() - mdep.second->wholes_to_next_event());
        }
    }
    this->wholes_to_last_event(mdep.second->wholes_to_next_event());
}

ostream& textmidi::operator<<(ostream& os, PrintLazyTrack& print_lazy_track)
{
    for (auto& mp : print_lazy_track.delay_events_)
    {
        print_lazy_track.print(os, mp);
    }
    return os;
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
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>(note)};
        auto note_off{dynamic_cast<MidiChannelVoiceNoteOffEvent*>(note)};
        // A note-on with zero velocity is a substitute note-off.
        const auto is_noteoff{(note_on && !note->velocity()) || note_off};
        const auto is_noteon { note_on &&  note->velocity()};

        if (is_in_rest())
        {
            // the rest's duration time is the current event's time - rest_start_ticks_accumulated.
            const auto rest_ticks{me->ticks_accumulated() - rest_start_ticks_accumulated};
            // the rest's start time is the current event's time + length.
            if (rest_ticks)
            {
                // Construct a rest pseudo-event.
                auto rest{make_shared<MidiChannelVoiceNoteRestEvent>
                    (running_status_, QuartersPerWhole * ticksperquarter_, prefer_sharp_)};
                rest->ticks_accumulated(rest_start_ticks_accumulated);
                rest->ticks_to_next_event(rest_ticks);
                RhythmRational wholes{rest_ticks, QuartersPerWhole * ticksperquarter_};
                wholes.reduce();
                rest->wholes_to_next_event(wholes);
                DelayEvent rest_pair{rest_start_ticks_accumulated, rest};
                delay_events_.emplace(delay_event_iter, rest_pair);
                rest_start_ticks_accumulated = me->ticks_accumulated()
                    + me->ticks_to_next_event();
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
                         << static_cast<int>(note->channel()) << " and key number: "
                         << static_cast<int>(note->key()) << ", too many note-offs\n";
                    channel_keyboards_[note->channel() - 1][note->key()] = 0;
                }
                // advance rest_start_ticks_accumulated
                // regardless of whether we are entering a rest period
                rest_start_ticks_accumulated = me->ticks_accumulated();
            }
            else // not a note-on or a note-off event, hence some other MIDI event
            {
                // advance rest_start_ticks_accumulated
                // regardless of whether we are entering a rest period
                rest_start_ticks_accumulated = me->ticks_accumulated();
            }
        }
    }
}

