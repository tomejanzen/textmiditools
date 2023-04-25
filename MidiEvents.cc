//
// TextMIDITools Version 1.0.55
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
    MidiStreamAtom channel_0_to_1(MidiStreamAtom channel)
    {
        ++channel;
        return channel;
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
constexpr int64_t textmidi::variable_len_value(MidiStreamIterator& midiiter)
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

ostream& textmidi::operator<<(ostream& os, const MidiDelayEventPair& msg_pair)
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
constexpr int64_t textmidi::MidiEvent::ticks_accumulated() const
{
    return ticks_accumulated_;
}

//
// set the accumulated MIDI ticks.
void textmidi::MidiEvent::ticks_accumulated(int64_t ticks_accumulated)
{
    ticks_accumulated_ = ticks_accumulated;
}

//
// return the MIDI ticks to the next event.
constexpr int64_t textmidi::MidiEvent::ticks_to_next_event() const
{
    return ticks_to_next_event_;
}

//
// set the MIDI ticks to the next event.
void textmidi::MidiEvent::ticks_to_next_event(int64_t ticks_to_next_event)
{
    ticks_to_next_event_ = ticks_to_next_event;
}

constexpr int64_t textmidi::MidiEvent::ticks_to_next_note_on() const
{
    return ticks_to_next_note_on_;
}

void textmidi::MidiEvent::ticks_to_next_note_on(int64_t ticks_to_next_note_on)
{
    ticks_to_next_note_on_ = ticks_to_next_note_on;
}

constexpr RhythmRational textmidi::MidiEvent::wholes_to_next_event() const
{
    return wholes_to_next_event_;
}

void textmidi::MidiEvent::wholes_to_next_event(
        const RhythmRational& wholes_to_next_event)
{
    wholes_to_next_event_ = wholes_to_next_event;
    wholes_to_next_event_.reduce();
}

ostream& textmidi::operator<<(ostream& os, const MidiEvent& msg)
{
    return msg.text(os);
}

shared_ptr<MidiEvent> MidiSysExEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end)
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

void textmidi::MidiSysExEvent::consume_stream(MidiStreamIterator& midiiter)
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

    if (i < data_.size())
    {
        for (auto it(data_.cbegin() + i);
                (it != data_.cend()) && (*it != end_of_sysex[0]);
                ++it)
        {
            os << ' ' << hex << "0x" << setw(2) << setfill('0')
               << static_cast<unsigned>(*it);
        }
    }
    static_cast<void>(os.flags(flags));
    return os;
}


const long MidiSysExEvent::prefix_len{static_cast<long>(start_of_sysex.size())};

shared_ptr<MidiEvent> MidiSysExRawEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end)
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

void textmidi::MidiSysExRawEvent::consume_stream(MidiStreamIterator& midiiter)
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
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiSysExRawEvent& msg)
{
    return msg.text(os);
}

const long MidiSysExRawEvent::prefix_len{static_cast<long>(end_of_sysex.size())};

bool MidiFileMetaEvent::recognize(MidiStreamIterator& midiiter, MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaSequenceEvent::consume_stream(MidiStreamIterator& midiiter)
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

void textmidi::MidiFileMetaSequenceEvent::sequence_number(uint16_t sequence_number)
{
    sequence_number_ = sequence_number;
}

uint16_t textmidi::MidiFileMetaSequenceEvent::sequence_number() const
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
    MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaUnknownEvent::consume_stream(MidiStreamIterator& midiiter)
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
    for (const auto it : data_)
    {
        os << ' ' << hex << "0x" << setw(2) << setfill('0') << static_cast<unsigned>(it);
    }
    static_cast<void>(os.flags(flags));
    return os;
}

const long MidiFileMetaUnknownEvent::prefix_len{static_cast<long>(meta_prefix.size())
    + static_cast<long>(unknown_prefix.size())};

shared_ptr<MidiEvent> MidiFileMetaMidiChannelEvent::recognize(
    MidiStreamIterator& midiiter, MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaMidiChannelEvent::consume_stream(MidiStreamIterator& midiiter)
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

void textmidi::MidiFileMetaMidiChannelEvent::channel(uint16_t channel)
{
    channel_ = channel;
}

constexpr uint16_t textmidi::MidiFileMetaMidiChannelEvent::channel()
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
    MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaSetTempoEvent::consume_stream(MidiStreamIterator& midiiter)
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

void textmidi::MidiFileMetaSetTempoEvent::tempo(uint32_t tempo)
{
    tempo_ = tempo;
}

uint32_t textmidi::MidiFileMetaSetTempoEvent::tempo() const
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
    MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaSMPTEOffsetEvent::consume_stream(MidiStreamIterator& midiiter)
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
    MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaMidiPortEvent::consume_stream(MidiStreamIterator& midiiter)
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
    MidiStreamIterator& midiiter, MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaTimeSignatureEvent::consume_stream(MidiStreamIterator& midiiter)
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
    MidiStreamIterator& midiiter, MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaKeySignatureEvent::consume_stream(MidiStreamIterator& midiiter)
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
    os << "KEY_SIGNATURE "
       << key_signature_map[KeySignatureMap::key_type{accidentals_, minor_mode_}];
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
    MidiStreamIterator& midiiter, MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaXmfPatchTypeEvent::consume_stream(MidiStreamIterator& midiiter)
{
    // Some programs that were obsolete when they were written left off the mode byte,
    // so this consume_stream function has to see the length byte to know that.
    // So this consume_stream takes a bigger byte off the apple than others.
    const auto len{*midiiter++};
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
        MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaSequencerSpecificEvent::consume_stream(MidiStreamIterator& midiiter)
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
           << static_cast<unsigned>(*it);
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
    MidiStreamIterator the_end)
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

void textmidi::MidiFileMetaEndOfTrackEvent::consume_stream(MidiStreamIterator& )
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

void textmidi::MidiFileMetaStringEvent::consume_stream(MidiStreamIterator& midiiter)
{
    int64_t len{variable_len_value(midiiter)};
    str_.insert(str_.begin(), midiiter, midiiter + len);
    midiiter += len;
}

ostream& textmidi::MidiFileMetaStringEvent::text(ostream& os) const
{
    string display_str{str_};
    string::size_type pos{};

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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end)
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
    MidiStreamIterator the_end, RunningStatus& running_status)
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

MidiStreamAtom textmidi::MidiChannelVoiceModeEvent::channel() const
{
    return channel_;
}

void textmidi::MidiChannelVoiceModeEvent::channel(MidiStreamAtom channel)
{
    channel_ = channel;
}

constexpr RunningStatus& MidiChannelVoiceModeEvent::running_status()
{
    return running_status_;
}

const RunningStatus& MidiChannelVoiceModeEvent::running_status() const
{
    return running_status_;
}

const RunningStatus& MidiChannelVoiceModeEvent::local_status() const
{
    return local_status_;
}
void MidiChannelVoiceModeEvent::local_status(const RunningStatus& rs)
{
    local_status_ = rs;
}
RunningStatus& MidiChannelVoiceModeEvent::local_status()
{
    return local_status_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceModeEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiChannelVoiceNoteEvent::consume_stream(MidiStreamIterator& midiiter)
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

constexpr MidiStreamAtom textmidi::MidiChannelVoiceNoteEvent::key() const
{
    return key_;
}

void textmidi::MidiChannelVoiceNoteEvent::key(MidiStreamAtom key)
{
    key_ = key;
}

void textmidi::MidiChannelVoiceNoteEvent::key_string(string_view key_string)
{
    key_string_ = key_string;
}

string textmidi::MidiChannelVoiceNoteEvent::key_string() const
{
    return key_string_;
}

void textmidi::MidiChannelVoiceNoteEvent::velocity(MidiStreamAtom velocity)
{
    velocity_ = velocity;
}

constexpr MidiStreamAtom textmidi::MidiChannelVoiceNoteEvent::velocity() const
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

constexpr bool textmidi::MidiChannelVoiceNoteEvent
    ::operator==(const MidiChannelVoiceNoteEvent& note) const
{
    return (key_ == note.key() && channel() == note.channel());
}

void textmidi::MidiChannelVoiceNoteEvent
    ::wholes_to_noteoff(const RhythmRational& wholes_to_noteoff)
{
    wholes_to_noteoff_ = wholes_to_noteoff;
    wholes_to_noteoff_.reduce();
}

RhythmRational textmidi::MidiChannelVoiceNoteEvent::wholes_to_noteoff() const
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
    ::ticks_to_noteoff(int64_t ticks_to_noteoff)
{
    ticks_to_noteoff_ = ticks_to_noteoff;
}

constexpr int64_t textmidi::MidiChannelVoiceNoteOnEvent::ticks_to_noteoff() const
{
    return ticks_to_noteoff_;
}

void textmidi::MidiChannelVoiceNoteOnEvent
    ::ticks_past_noteoff(int64_t ticks_past_noteoff)
{
    ticks_past_noteoff_ = ticks_past_noteoff;
}

constexpr int64_t textmidi::MidiChannelVoiceNoteOnEvent::ticks_past_noteoff() const
{
    return ticks_past_noteoff_;
}

void textmidi::MidiChannelVoiceNoteOnEvent
    ::wholes_past_noteoff(const RhythmRational& wholes_past_noteoff)
{
    wholes_past_noteoff_ = wholes_past_noteoff;
    wholes_past_noteoff_.reduce();
}

constexpr RhythmRational textmidi::MidiChannelVoiceNoteOnEvent::wholes_past_noteoff() const
{
    return wholes_past_noteoff_;
}

constexpr uint32_t textmidi::MidiChannelVoiceNoteOnEvent::ticks_per_whole() const
{
    return ticks_per_whole_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteOnEvent& msg)
{
    return msg.text(os);
}

shared_ptr<MidiEvent> MidiChannelVoiceNoteOnEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end, RunningStatus& running_status, shared_ptr<bool> prefer_sharp, uint32_t ticks_per_whole)
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
    MidiStreamIterator the_end, RunningStatus& running_status, shared_ptr<bool> prefer_sharp)
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
    return (recognized ? make_shared<MidiChannelVoiceNoteOffEvent>(running_status, prefer_sharp, midiiter)
        : shared_ptr<MidiChannelVoiceNoteOffEvent>{});
}

const long MidiChannelVoiceNoteOffEvent::prefix_len{full_note_length};

shared_ptr<MidiEvent> MidiChannelVoicePitchBendEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end, RunningStatus& running_status)
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

void textmidi::MidiChannelVoicePitchBendEvent::consume_stream(MidiStreamIterator& midiiter)
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

void textmidi::MidiChannelVoicePitchBendEvent::pitch_wheel(MidiStreamAtom pitch_wheel)
{
    pitch_wheel_ = pitch_wheel;
}

MidiStreamAtom textmidi::MidiChannelVoicePitchBendEvent::pitch_wheel() const
{
    return pitch_wheel_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoicePitchBendEvent& msg)
{
    return msg.text(os);
}

const long MidiChannelVoicePitchBendEvent::prefix_len{full_note_length};

shared_ptr<MidiEvent> MidiChannelVoiceControlChangeEvent::recognize(MidiStreamIterator& midiiter,
    MidiStreamIterator the_end, RunningStatus& running_status)
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

void textmidi::MidiChannelVoiceControlChangeEvent::consume_stream(MidiStreamIterator& midiiter)
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
            os << hex << "0x" << setw(2) << setfill('0') << static_cast<unsigned>(value_);
        }
    }
    os << ' ';
    static_cast<void>(os.flags(flags));
    return os;
}

MidiStreamAtom textmidi::MidiChannelVoiceControlChangeEvent::id() const
{
    return id_;
}

void textmidi::MidiChannelVoiceControlChangeEvent::id(MidiStreamAtom id)
{
    id_ = id;
}

MidiStreamAtom textmidi::MidiChannelVoiceControlChangeEvent::value() const
{
    return value_;
}

void textmidi::MidiChannelVoiceControlChangeEvent::value(MidiStreamAtom value)
{
    value_ = value;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceControlChangeEvent& msg)
{
    return msg.text(os);
}

const long MidiChannelVoiceControlChangeEvent::prefix_len{full_note_length};

shared_ptr<MidiEvent> MidiChannelVoiceChannelPressureEvent::recognize(
    MidiStreamIterator& midiiter, MidiStreamIterator the_end, RunningStatus& running_status)
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

void textmidi::MidiChannelVoiceChannelPressureEvent::consume_stream(MidiStreamIterator& midiiter)
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

void textmidi::MidiChannelVoiceChannelPressureEvent::pressure(MidiStreamAtom pressure)
{
    pressure_ = pressure;
}

MidiStreamAtom textmidi::MidiChannelVoiceChannelPressureEvent::pressure() const
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
    RunningStatus& running_status, shared_ptr<bool> prefer_sharp)
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
        (running_status, prefer_sharp, midiiter) : shared_ptr<MidiChannelVoicePolyphonicKeyPressureEvent>{});
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
    RhythmRational duration{ticks_to_next_event(), ticks_per_whole_};
    duration.reduce();
    print_rhythm(os, duration);
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
   ::recognize(MidiStreamIterator& midiiter, MidiStreamIterator the_end, RunningStatus& running_status)
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

void textmidi::MidiChannelVoiceProgramChangeEvent::consume_stream(MidiStreamIterator& midiiter)
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

void textmidi::MidiChannelVoiceProgramChangeEvent::program(MidiStreamAtom program)
{
    program_ = program;
}

constexpr MidiStreamAtom textmidi::MidiChannelVoiceProgramChangeEvent::program() const
{
    return program_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceProgramChangeEvent& msg)
{
    return msg.text(os);
}

const long MidiChannelVoiceProgramChangeEvent::prefix_len{full_note_length};

MidiDelayEventPair textmidi::MidiEventFactory::operator()(MidiStreamIterator& midiiter, int64_t& ticks_accumulated)
{
    int64_t delaynum64{variable_len_value(midiiter)};
    MidiDelayEventPair midi_delay_event_pair(delaynum64, nullptr);
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
                    midi_delay_event_pair.second = make_shared<MidiChannelVoiceNoteOffEvent>(running_status_, prefer_sharp_, midiiter);
                    break;
                  case PolyphonicKeyPressure:
                    midi_delay_event_pair.second = make_shared<MidiChannelVoicePolyphonicKeyPressureEvent>(running_status_, prefer_sharp_, midiiter);
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
        midi_delay_event_pair.second = MidiChannelVoiceNoteOffEvent::recognize(midiiter, midi_end_, running_status_, prefer_sharp_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoicePolyphonicKeyPressureEvent::recognize(midiiter, midi_end_, running_status_, prefer_sharp_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoiceProgramChangeEvent::recognize(midiiter, midi_end_, running_status_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoiceChannelPressureEvent::recognize(midiiter, midi_end_, running_status_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoicePitchBendEvent::recognize(midiiter, midi_end_, running_status_);
    if (!midi_delay_event_pair.second)
        midi_delay_event_pair.second = MidiChannelVoiceControlChangeEvent::recognize(midiiter, midi_end_, running_status_);

    if (!midi_delay_event_pair.second)
    {
        cerr << "Unrecognized byte sequence:\n";
        for (auto i{0}; i < 32; ++i)
        {
            cerr << setfill('0') << setw(2) << hex << static_cast<int>(*midiiter++) << ' ';
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
    else
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
    return midi_delay_event_pair;
}

void textmidi::PrintLazyTrack::ticks_of_note_on()
{
    for (auto delaymsgiter{midi_delay_event_pairs_.cbegin()};
        (delaymsgiter != midi_delay_event_pairs_.cend())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
              (delaymsgiter->second.get()); ++delaymsgiter)
    {
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>
                                 (delaymsgiter->second.get())};
        if (note_on && (note_on->velocity() != 0))
        {
            bool same_chord{true};
            auto offiter{delaymsgiter};
            ++offiter;
            for ( ;
                 (offiter != midi_delay_event_pairs_.cend())
                         && (!dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                                 (offiter->second.get()));
                 ++offiter)
            {
                if (offiter->first != 0)
                {
                    same_chord = false;
                }
                auto note_on_zero{dynamic_cast<MidiChannelVoiceNoteOnEvent*>
                    (offiter->second.get())};
                if (note_on_zero)
                {
                    if (note_on_zero->velocity() == 0)  // Is ersatz Note Off?
                    {
                        // Are same pitch and channel?
                        if (*note_on_zero == *note_on)
                        {
                            note_on->ticks_to_noteoff(
                                note_on_zero->ticks_accumulated()
                                - note_on->ticks_accumulated());
                            auto nextiter(offiter);
                            ++nextiter;
                            note_on->ticks_past_noteoff(
                              nextiter->second.get()->ticks_accumulated()
                                  - note_on_zero->ticks_accumulated());
                            break;
                        }
                    }
                    else
                    {
                        if (same_chord && (0 == offiter->first))
                        {
                        }
                    }
                }
                else
                {
                    auto note_off{dynamic_cast<MidiChannelVoiceNoteOffEvent*>
                        (offiter->second.get())};
                    if (note_off)
                    {
                        if (*note_off == *note_on)
                        {
                            note_on->ticks_to_noteoff(
                                note_off->ticks_accumulated()
                                - note_on->ticks_accumulated());
                            auto nextiter(offiter);
                            ++nextiter;
                            note_on->ticks_past_noteoff(
                                nextiter->second.get()->ticks_accumulated()
                                - note_off->ticks_accumulated());
                            break;
                        }
                    }
                }
            }
        }
    }
}

void textmidi::PrintLazyTrack::ticks_to_next_note_on()
{
    for (auto delaymsgiter{midi_delay_event_pairs_.begin()};
        (delaymsgiter != midi_delay_event_pairs_.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                (delaymsgiter->second.get()); ++delaymsgiter)
    {
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>
            (delaymsgiter->second.get())};
        if (note_on && (note_on->velocity() != 0))
        {
            auto offiter{delaymsgiter};
            ++offiter;
            for ( ;
                 (offiter != midi_delay_event_pairs_.end())
                         && (!dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                             (offiter->second.get()));
                 ++offiter)
            {
                auto next_note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>
                    (offiter->second.get())};
                if (next_note_on && (next_note_on->velocity() != 0))
                {
                    if (!(*next_note_on == *note_on))
                    {
                        note_on->ticks_to_next_note_on(
                            next_note_on->ticks_accumulated()
                                - note_on->ticks_accumulated());
                        break;
                    }
                }
            }
        }
    }
}

void textmidi::PrintLazyTrack::ticks_to_next_event()
{
    const auto len(midi_delay_event_pairs_.size());
    auto lopped_end   = ranges::take_view(midi_delay_event_pairs_, len - 1);
    auto lopped_begin = ranges::drop_view(midi_delay_event_pairs_, 1);
    if (len > 1)
    {
        ranges::transform(lopped_end, lopped_begin, midi_delay_event_pairs_.begin(),
            [](const MidiDelayEventPair& mde1, const MidiDelayEventPair& mde2)
            { MidiDelayEventPair mde_rtn{mde1};
            mde_rtn.second->ticks_to_next_event(
                mde2.second->ticks_accumulated() - mde1.second->ticks_accumulated());
            return mde_rtn; });
    }
}

void textmidi::PrintLazyTrack::wholes_to_next_event()
{
    const auto ticksperquantum{quantum_
        ? (quantum_ * rational::RhythmRational{QuartersPerWhole} * rational::RhythmRational{ticksperquarter_})
        : rational::RhythmRational{1L}};
    for (auto delaymsgiter{midi_delay_event_pairs_.cbegin()};
        (delaymsgiter != midi_delay_event_pairs_.cend())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                     (delaymsgiter->second.get());
         ++delaymsgiter)
    {
        if ((delaymsgiter + 1) != midi_delay_event_pairs_.end())
        {
            RhythmRational ratio_to_next_event{static_cast<int64_t>
                (delaymsgiter->second->ticks_to_next_event()),
                    QuartersPerWhole * ticksperquarter_};
            delaymsgiter->second->wholes_to_next_event(
                    rational::snap(ratio_to_next_event, quantum_));
            auto temp{delaymsgiter->second->wholes_to_next_event()};
            temp.reduce();
            delaymsgiter->second->wholes_to_next_event(temp);
        }
    }
}

void textmidi::PrintLazyTrack::wholes_of_note_on()
{
    for (auto delaymsgiter{midi_delay_event_pairs_.cbegin()};
        (delaymsgiter != midi_delay_event_pairs_.cend())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                (delaymsgiter->second.get()); ++delaymsgiter)
    {
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>
            (delaymsgiter->second.get())};
        if (note_on)
        {
            RhythmRational ratio_to_note_off{note_on->ticks_to_noteoff(),
                ticksperquarter_ * QuartersPerWhole};
            note_on->wholes_to_noteoff(
                rational::snap(ratio_to_note_off, quantum_));
            RhythmRational ratio_past_note_off{note_on->ticks_past_noteoff(),
                ticksperquarter_ * QuartersPerWhole};
            note_on->wholes_past_noteoff(
                rational::snap(ratio_past_note_off, quantum_));
        }
        auto rest{dynamic_cast<MidiChannelVoiceNoteRestEvent*>
            (delaymsgiter->second.get())};
        if (rest)
        {
            RhythmRational ratio_to_note_off{static_cast<int64_t>
                (rest->ticks_to_next_event()), ticksperquarter_ * QuartersPerWhole};
            ratio_to_note_off.reduce();
            rest->wholes_to_noteoff(rational::snap(ratio_to_note_off, quantum_));
        }
    }
}

void textmidi::PrintLazyTrack::print(ostream& os, MidiDelayEventPair& mdmp)
{
    MidiEvent* mm{mdmp.second.get()};
    auto* note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>(mm)};
    auto rem_if_zero_wholes_to_noteoff =
        [](MidiChannelVoiceNoteOnEvent mcvnom) { return RhythmRational{}
        == mcvnom.wholes_to_noteoff(); };
    if (note_on)
    {
        if (note_on->velocity())
        {
            // Write the velocity value if it has changed.
            if (dynamic_ != note_on->velocity())
            {
                os << lazy_string(true);
                const auto dynamic = dynamics_map(note_on->velocity());
                if (dynamic)
                {
                    os << *dynamic << '\n';
                }
                else
                {
                    os << "vel " << static_cast<int>(note_on->velocity()) << ' ';
                }
                dynamic_ = note_on->velocity();
            }
            // Write the channel value if it has changed.
            if (note_on->channel() != channel_)
            {
                os << lazy_string(true);
                channel_ = note_on->channel();
                os << "chan " << static_cast<int>(channel_) << ' ';
            }
            // find out if there are notes that are tied in.

            // There are tied-in notes.
            // Find the soonest thing to happen.
            //
            // First write the notes that are tied into the present.
            //
            if (!tied_list_.empty())
            {
                for (auto& tiednote : tied_list_)
                {
                    os << lazy_string(true);
                    os << '-';
                    os << tiednote.key_string();
                    if (tiednote.wholes_to_noteoff())
                    {
                        os << "- ";
                    }
                    else
                    {
                        auto rem_if_same_note_event
                            = [tiednote](const MidiChannelVoiceNoteOnEvent& mcvnom)
                            { return tiednote.local_status().running_status_value()
                              == mcvnom.local_status().running_status_value(); } ;
                        ranges::remove_if(tied_list_, rem_if_same_note_event);
                        os << ' ';
                    }
                }
                ranges::remove_if(tied_list_, rem_if_zero_wholes_to_noteoff);
            }
            os << lazy_string(true);

            os << note_on->key_string();
            if ((note_on->wholes_to_next_event())
                && (note_on->wholes_to_noteoff() > note_on->wholes_to_next_event()))
            {
                tied_list_.push_back(*note_on);
                os << "- ";
            }
            else
            {
                os << ' ';
            }
            // May print a zero-long delay if there was a non-note event
            // immediately after a note-on.  The delay will be written
            // after the non-note event.  You may want to edit the
            // output of ./miditext --lazy, which was the point in any case.
            print_rhythm(os, note_on->wholes_to_next_event());
            os << '\n';
        }
        else
        {
            ranges::remove_if(tied_list_, rem_if_zero_wholes_to_noteoff);
            const auto* rest{dynamic_cast<MidiChannelVoiceNoteRestEvent*>(mm)};
            if (rest)
            {
                RhythmRational duration{rest->wholes_to_next_event()};
                duration.reduce();
                if (duration)
                {
                    os << lazy_string(true);
                    os << rest->key_string() << ' ';
                    print_rhythm(os, duration);
                    os << '\n';
                }
            }
        }
    }
    else // not a note-on
    {
        if (!dynamic_cast<MidiChannelVoiceNoteOffEvent*>(mm))
        {
            const auto* rest{dynamic_cast<MidiChannelVoiceNoteRestEvent*>(mm)};
            if (rest)
            {
                RhythmRational duration{rest->wholes_to_next_event()};
                duration.reduce();
                if (duration)
                {
                    os << lazy_string(true);
                    os << rest->key_string() << ' ';
                    print_rhythm(os, duration);
                    os << '\n';
                }
            }
            else
            {
                os << lazy_string(false);
                os << *mm << '\n';
            }
        }
        else
        {
            ranges::remove_if(tied_list_, rem_if_zero_wholes_to_noteoff);
        }
    }
    for (auto& tiednote : tied_list_)
    {
        if (tiednote.wholes_to_noteoff())
        {
            tiednote.wholes_to_noteoff(tiednote.wholes_to_noteoff()       - mm->wholes_to_next_event());
        }
    }
}

ostream& textmidi::operator<<(ostream& os, PrintLazyTrack& print_lazy_track)
{
    for (auto& mp : print_lazy_track.midi_delay_event_pairs_)
    {
        print_lazy_track.print(os, mp);
    }
    return os;
}

constexpr string_view textmidi::PrintLazyTrack::lazy_string(bool lazy)
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

void textmidi::PrintLazyTrack::insert_rests()
{
    // building a new sequence of MidiDelayEventPair to replace the old one.
    MidiDelayEventPairs new_midi_delay_event_pairs;
    // Add * 2 pad for added rests.
    new_midi_delay_event_pairs.reserve(midi_delay_event_pairs_.size() * 2);

    // Create a separate bit keyboard for each channel.
    using KeyBoard = bitset<MidiPitchQty>;
    using ChannelKeyBoards = vector<KeyBoard>;
    ChannelKeyBoards channel_keyboards(MidiChannelQty);

    // This works on building a copy of events plus added rests
    // Because insereting the rests in situ would invalidate the iterators.
    //
    int64_t rest_start_ticks_accumulated{};
    bool inrest{false};
    for (auto delaymsgiter{midi_delay_event_pairs_.cbegin()};
              delaymsgiter != midi_delay_event_pairs_.cend();
            ++delaymsgiter)
    {
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnEvent*>
            (delaymsgiter->second.get())};
        if (note_on)
        {
            if (inrest && (0L < note_on->velocity()))
            {
                // the rest's duration time is the current event's time - rest_start_ticks_accumulated.
                const auto rest_ticks{
                    note_on->ticks_accumulated() - rest_start_ticks_accumulated};
                // the rest's start time is the current event's time + length.
                rest_start_ticks_accumulated
                    = note_on->ticks_accumulated() + note_on->ticks_to_next_event();
                if (rest_ticks)
                {
                    // Construct a rest pseudo-event.
                    auto rest{make_shared<MidiChannelVoiceNoteRestEvent>
                        (running_status_, QuartersPerWhole * ticksperquarter_, prefer_sharp_)};
                    rest->ticks_accumulated(rest_start_ticks_accumulated);
                    rest->ticks_to_next_event(rest_ticks);
                    RhythmRational wholes{rest_ticks, QuartersPerWhole * ticksperquarter_};
                    wholes.reduce();
                    if (wholes)
                    {
                        rest->wholes_to_next_event(wholes);
                        rest->wholes_to_noteoff(wholes);
                        MidiDelayEventPair rest_pair{0, rest};
                        new_midi_delay_event_pairs.push_back(rest_pair);
                        rest_start_ticks_accumulated = note_on->ticks_accumulated()
                            + note_on->ticks_to_noteoff();
                    }
                }
            }
            channel_keyboards[note_on->channel() - 1][note_on->key()] = true;
            // add the note-on to the new track event sequence.
            new_midi_delay_event_pairs.push_back(*delaymsgiter);
            // A note-on with zero velocity is a substitute note-off.
            const auto thisnoteoff{(0L == note_on->velocity())};

            if (thisnoteoff)
            {
                // Update channel keyboards.
                channel_keyboards[note_on->channel() - 1][note_on->key()] = false;
                if (!inrest)
                {
                    // This is a state mamidiiterne.
                    // We came in during a note, but are now going to check if
                    // all notes are off.
                    // That transition triggers a start of a rest.

                    // Check if all notes are off.
                    inrest = true;
                    for (auto chkb : channel_keyboards)
                    {
                        inrest &= chkb.none();
                    }
                    // end update inrest

                    // We transitioned from not-in-a-rest to in-a-rest.
                    if (inrest)
                    {
                        rest_start_ticks_accumulated = note_on->ticks_accumulated();
                    }
                }
            }
        }
        else
        {
            auto note_off{dynamic_cast<MidiChannelVoiceNoteOffEvent*>
                (delaymsgiter->second.get())};
            if (note_off)
            {
                // Update channel keyboards.
                channel_keyboards[note_off->channel() - 1][note_off->key()] = false;
                if (!inrest)
                {
                    // This is a state mamidiiterne.
                    // We came in during a note, but are now going to check if
                    // all notes are off.
                    // That transition triggers a start of a rest.

                    // Check if all notes are off.
                    inrest = true;
                    for (auto chkb : channel_keyboards)
                    {
                        inrest &= chkb.none();
                    }
                    // end update inrest

                    // We transitioned from not-in-a-rest to in-a-rest.
                    if (inrest)
                    {
                        rest_start_ticks_accumulated = note_off->ticks_accumulated();
                    }
                }
                new_midi_delay_event_pairs.push_back(*delaymsgiter);
            }
            else
            {
                // Make a rest pseudo-event.
                auto rest{make_shared<MidiChannelVoiceNoteRestEvent>
                    (running_status_, QuartersPerWhole * ticksperquarter_, prefer_sharp_)};
                // Move the ticks accumulated, to next event and to next note_on to the rest.
                rest->ticks_accumulated    (delaymsgiter->second.get()->ticks_accumulated());
                rest->ticks_to_next_event  (delaymsgiter->second.get()->ticks_to_next_event());
                rest->ticks_to_next_note_on(delaymsgiter->second.get()->ticks_to_next_note_on());
                // Clear the current pre-rest event.
                delaymsgiter->second.get()->ticks_accumulated(0L);
                delaymsgiter->second.get()->ticks_to_next_event(0L);
                delaymsgiter->second.get()->ticks_to_next_note_on(0L);
                // Add the non-note_on and non-note_off event onto the new events.
                new_midi_delay_event_pairs.push_back(*delaymsgiter);
                // prepare to add the rest to the new event sequence.
                if (rest->ticks_to_next_event() > 0)
                {
                    rest->wholes_to_next_event(RhythmRational{rest->ticks_to_next_event(),
                        QuartersPerWhole * ticksperquarter_});
                    rest->wholes_to_noteoff(rest->wholes_to_next_event());
                    MidiDelayEventPair rest_pair{rest->ticks_accumulated(), rest};
                    // this rest is needed for when there are no notes, e.g., just meta events.
                    rest_start_ticks_accumulated
                        = rest->ticks_accumulated() + rest->ticks_to_next_event();
                    new_midi_delay_event_pairs.push_back(rest_pair);
                }
                inrest = true;
            }
        }
    }
    midi_delay_event_pairs_ = move(new_midi_delay_event_pairs);
}
