//
// TextMIDITools Version 1.0.30
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
#include <iomanip>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "MidiMessages.h"
#include "rational_support.h"
#include "MIDIKeyString.h"

using namespace std;
using namespace textmidi;
using namespace textmidi::rational;

//
// Key names, major and minor.
// Based on MIDI Specification 1.1.
textmidi::KeySignatureMap textmidi::key_signature_map{
    {{0, false}, string_view{"C"}},
    {{-1, false}, string_view{"F"}},
    {{-2, false}, string_view{"Bb"}},
    {{-3, false}, string_view{"Eb"}},
    {{-4, false}, string_view{"Ab"}},
    {{-5, false}, string_view{"Db"}},
    {{-6, false}, string_view{"Gb"}},
    {{-7, false}, string_view{"Cb"}},
    {{1, false}, string_view{"G"}},
    {{2, false}, string_view{"D"}},
    {{3, false}, string_view{"A"}},
    {{4, false}, string_view{"E"}},
    {{5, false}, string_view{"B"}},
    {{6, false}, string_view{"F#"}},
    {{7, false}, string_view{"C#"}},
    {{0, true}, string_view{"a"}},
    {{-1, true}, string_view{"d"}},
    {{-2, true}, string_view{"g"}},
    {{-3, true}, string_view{"c"}},
    {{-4, true}, string_view{"f"}},
    {{-5, true}, string_view{"bb"}},
    {{-6, true}, string_view{"eb"}},
    {{-7, true}, string_view{"ab"}},
    {{1, true}, string_view{"e"}},
    {{2, true}, string_view{"b"}},
    {{3, true}, string_view{"f#"}},
    {{4, true}, string_view{"c#"}},
    {{5, true}, string_view{"g#"}},
    {{6, true}, string_view{"d#"}},
    {{7, true}, string_view{"a#"}}
};


//
// Create a variable-length value.
int64_t textmidi::variable_len_value(MidiStreamIterator& midiiter)
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

//
// return the accumulated MIDI ticks.
int64_t textmidi::MidiMessage::ticks_accumulated() const
{
    return ticks_accumulated_;
}

//
// set the accumulated MIDI ticks.
void textmidi::MidiMessage::ticks_accumulated(int64_t ticks_accumulated)
{
    ticks_accumulated_ = ticks_accumulated;
}

//
// return the MIDI ticks to the next event.
int64_t textmidi::MidiMessage::ticks_to_next_event() const
{
    return ticks_to_next_event_;
}

//
// set the MIDI ticks to the next event.
void textmidi::MidiMessage::ticks_to_next_event(int64_t ticks_to_next_event)
{
    ticks_to_next_event_ = ticks_to_next_event;
}

int64_t textmidi::MidiMessage::ticks_to_next_note_on() const
{
    return ticks_to_next_note_on_;
}

void textmidi::MidiMessage::ticks_to_next_note_on(int64_t ticks_to_next_note_on)
{
    ticks_to_next_note_on_ = ticks_to_next_note_on;
}

RhythmRational textmidi::MidiMessage::wholes_to_next_event() const
{
    return wholes_to_next_event_;
}

void textmidi::MidiMessage::wholes_to_next_event(
        const RhythmRational& wholes_to_next_event)
{
    wholes_to_next_event_ = wholes_to_next_event;
    wholes_to_next_event_.reduce();
}

ostream& textmidi::operator<<(ostream& os, const MidiMessage& msg)
{
    return msg.text(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiDelayMessagePair& msg_pair)
{
    if (msg_pair.first > 0) [[likely]]
    {
        os << "DELAY " << dec << msg_pair.first << ' ';
    }
    os << *msg_pair.second;
    return os;
}

void textmidi::MidiSysExEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (start_of_sysex[0] == *midiiter)
    {
        ++midiiter;
    }

    int64_t len{variable_len_value(midiiter)};
    int64_t count{};
    do
    {
        data_.push_back(*midiiter);
    } while ((*midiiter++ != end_of_sysex[0]) && (++count < len));
}

//
// Write the text version of the event to os.
ostream& textmidi::MidiSysExEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << '\n' << hex << "SYSEX";
    for (auto sys_ex_it(data_.begin());
            (sys_ex_it != data_.end()) && (*sys_ex_it != end_of_sysex[0]);
            ++sys_ex_it)
    {
        os << ' ' << hex << "0x" << setw(2) << setfill('0')
           << static_cast<unsigned>(*sys_ex_it);
    }
    auto oldflags{os.flags(flags)};
    return os;
}

void textmidi::MidiSysExRawEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    ++midiiter;
    const int64_t len{variable_len_value(midiiter)};
    for (unsigned b{}; b < len; ++b)
    {
        data_.push_back(*midiiter++);
    };
}

ostream& textmidi::MidiSysExRawEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << '\n' << hex << "SYSEXRAW";
    for (auto sys_ex_it(data_.begin());
            (sys_ex_it != data_.end()) && (*sys_ex_it != end_of_sysex[0]);
            ++sys_ex_it)
    {
        os << ' ' << hex << "0x"
           << setw(2) << setfill('0') << static_cast<unsigned>(*sys_ex_it);
    }
    auto oldflags{os.flags(flags)};
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiSysExRawEvent& msg)
{
    return msg.text(os);
}

MidiStreamAtom textmidi::MidiChannelVoiceMessage::channel() const
{
    return channel_;
}

void textmidi::MidiChannelVoiceMessage::channel(MidiStreamAtom channel)
{
    channel_ = channel;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceMessage& msg)
{
    return msg.text(os);
}

void textmidi::MidiChannelVoiceNoteMessage
    ::consume_stream(MidiStreamIterator& midiiter)
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
    channel(local_status().channel() + 1);
    key_string_ = num_to_note(key_);
    velocity_ = static_cast<int>(*midiiter++);
}

MidiStreamAtom textmidi::MidiChannelVoiceNoteMessage::key() const
{
    return key_;
}

MidiStreamAtom textmidi::MidiChannelVoiceNoteMessage::velocity() const
{
    return velocity_;
}

string textmidi::MidiChannelVoiceNoteMessage::key_string() const
{
    return key_string_;
}

void textmidi::MidiChannelVoiceNoteMessage::key(MidiStreamAtom key)
{
    key_ = key;
}

void textmidi::MidiChannelVoiceNoteMessage::velocity(MidiStreamAtom velocity)
{
    velocity_ = velocity;
}

void textmidi::MidiChannelVoiceNoteMessage::key_string(string_view key_string)
{
    key_string_ = key_string;
}

ostream& textmidi::MidiChannelVoiceNoteMessage::text(ostream& os) const
{
    auto flags{os.flags()};
    os << dec << static_cast<unsigned>(channel()) << ' '
       << key_string() << ' ' << static_cast<unsigned>(velocity());
    auto oldflags{os.flags(flags)};
    return os;
}

bool textmidi::MidiChannelVoiceNoteMessage
    ::operator==(const MidiChannelVoiceNoteMessage& note) const
{
    return (key_ == note.key() && channel() == note.channel());
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteMessage& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiChannelVoiceNoteRestMessage::text(ostream& os) const
{
    auto flags{os.flags()};
    if (ticks_to_next_event())
    {
        os << "R ";
    }
    RhythmRational duration{ticks_to_next_event(), MidiEventFactory::ticks_per_whole_};
    duration.reduce();
    print_rhythm(os, duration);
    os << '\n';
    auto oldflags{os.flags(flags)};
    return os;
}

void textmidi::MidiChannelVoiceNoteMessage
    ::wholes_to_noteoff(const RhythmRational& wholes_to_noteoff)
{
    wholes_to_noteoff_ = wholes_to_noteoff;
    wholes_to_noteoff_.reduce();
}

RhythmRational textmidi::MidiChannelVoiceNoteMessage::wholes_to_noteoff() const
{
    return wholes_to_noteoff_;
}

ostream& textmidi::MidiChannelVoiceNoteOffMessage::text(ostream& os) const
{
    os << "NOTE_OFF ";
    return MidiChannelVoiceNoteMessage::text(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteOffMessage& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiChannelVoiceNoteOnMessage::text(ostream& os) const
{
    os << "NOTE_ON ";
    return MidiChannelVoiceNoteMessage::text(os);
}

void textmidi::MidiChannelVoiceNoteOnMessage
    ::ticks_to_noteoff(int64_t ticks_to_noteoff)
{
    ticks_to_noteoff_ = ticks_to_noteoff;
}

int64_t textmidi::MidiChannelVoiceNoteOnMessage::ticks_to_noteoff() const
{
    return ticks_to_noteoff_;
}

uint32_t textmidi::MidiChannelVoiceNoteOnMessage::ticks_per_whole() const
{
    return ticks_per_whole_;
}

void textmidi::MidiChannelVoiceNoteOnMessage
    ::ticks_past_noteoff(int64_t ticks_past_noteoff)
{
    ticks_past_noteoff_ = ticks_past_noteoff;
}

int64_t textmidi::MidiChannelVoiceNoteOnMessage::ticks_past_noteoff() const
{
    return ticks_past_noteoff_;
}

void textmidi::MidiChannelVoiceNoteOnMessage
    ::wholes_past_noteoff(const RhythmRational& wholes_past_noteoff)
{
    wholes_past_noteoff_ = wholes_past_noteoff;
    wholes_past_noteoff_.reduce();
}

RhythmRational textmidi::MidiChannelVoiceNoteOnMessage::wholes_past_noteoff() const
{
    return wholes_past_noteoff_;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoiceNoteOnMessage& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiChannelVoicePolyphonicKeyPressureMessage
    ::text(ostream& os) const
{
    os << "POLY_KEY_PRESSURE ";
    return MidiChannelVoiceNoteMessage::text(os);
}

ostream& textmidi::operator<<(ostream& os,
        const MidiChannelVoicePolyphonicKeyPressureMessage& msg)
{
    return msg.text(os);
}

void textmidi::MidiChannelVoiceControlChangeMessage
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if ((*midiiter & ~channel_mask) == control[0])
    {
        local_status().running_status(*midiiter);
        ++midiiter;
    }
    else
    {
        local_status(running_status());
    }
    channel(local_status().channel() + 1);
    id_      = *midiiter++;
#if 0
    // MSB of 14 bit controllers
    if ((id_ >= 0) && (id_ <= 31))
    {
    }
    // LSB of 14 bit controllers
    if ((id_ >= 32) && (id_ <= 63))
    {
    }
    // "Additional" single-byte controllers
    if ((id_ >= 64) && (id_ <= 95))
    {
    }
    // increment and decrement and parameter numbers
    if ((id_ >= 96) && (id_ <= 101))
    {
    }
    // Undefined single-byte
    if ((id_ >= 102) && (id_ <= 119))
    {
    }
#endif
    value_   = *midiiter++;
    if (value_ & ~byte7_mask)
    {
        cerr << "Illegal 8-bit control value: " 
             << hex << static_cast<int>(value_) << dec << '\n';
    }
}

ostream& textmidi::MidiChannelVoiceControlChangeMessage::text(ostream& os) const
{
    auto flags{os.flags()};
    if (control_pan[0] == id_)
    {
        int32_t tempvalue{value_};
        tempvalue -= PanExcess64;
        string panstring{};
        switch (tempvalue)
        {
          case -64:
            panstring = "left";
            break;
          case   0:
            panstring = "center";
            break;
          case  63:
            panstring = "right";
            break;
          default: [[likely]]
            panstring = boost::lexical_cast<string>(tempvalue);
            break;
        }
        os << "PAN " << dec << static_cast<unsigned>(channel())
           << ' ' << panstring;
    }
    else
    {
        os << "CONTROL " << dec << static_cast<unsigned>(channel())
            << ' ' << static_cast<unsigned>(id_)
            << ' ' << static_cast<unsigned>(value_);
    }
    auto oldflags{os.flags(flags)};
    return os;
}

MidiStreamAtom textmidi::MidiChannelVoiceControlChangeMessage::id() const
{
    return id_;
}

void textmidi::MidiChannelVoiceControlChangeMessage::id(MidiStreamAtom id)
{
    id_ = id;
}

MidiStreamAtom textmidi::MidiChannelVoiceControlChangeMessage::value() const
{
    return value_;
}

void textmidi::MidiChannelVoiceControlChangeMessage::value(MidiStreamAtom value)
{
    value_ = value;
}

ostream& textmidi::operator<<(ostream& os,
        const MidiChannelVoiceControlChangeMessage& msg)
{
    return msg.text(os);
}

void textmidi::MidiChannelVoiceProgramChangeMessage
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (textmidi::program[0] == (~channel_mask & *midiiter))
    {
        local_status().running_status(*midiiter);
        ++midiiter;
    }
    else
    {
        local_status(running_status());
    }
    channel(local_status().channel() + 1);
    program_ = *midiiter++;
}

ostream& textmidi::MidiChannelVoiceProgramChangeMessage::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "PROGRAM " << dec << static_cast<int>(channel()) << ' '
       << (static_cast<int>(program_) + 1);
    auto oldflags{os.flags(flags)};
    return os;
}

MidiStreamAtom textmidi::MidiChannelVoiceProgramChangeMessage::program() const
{
    return program_;
}

void textmidi::MidiChannelVoiceProgramChangeMessage::program(MidiStreamAtom program)
{
    program_ = program;
}

ostream& textmidi::operator<<(ostream& os,
        const MidiChannelVoiceProgramChangeMessage& msg)
{
    return msg.text(os);
}

void textmidi::MidiChannelVoiceChannelPressureMessage
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (channel_pressure[0] == (~channel_mask & *midiiter))
    {
        local_status().running_status(*midiiter);
        ++midiiter;
    }
    else
    {
        local_status(running_status());
    }
    channel(local_status().channel() + 1);
    pressure_ = *midiiter++;
}

ostream& textmidi::MidiChannelVoiceChannelPressureMessage::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "CHANNEL_PRESSURE " << dec << static_cast<int>(channel())
       << ' ' << static_cast<int>(pressure_);
    auto oldflags{os.flags(flags)};
    return os;
}

MidiStreamAtom textmidi::MidiChannelVoiceChannelPressureMessage::pressure() const
{
    return pressure_;
}

void textmidi::MidiChannelVoiceChannelPressureMessage::pressure(MidiStreamAtom pressure)
{
    pressure_ = pressure;
}

ostream& textmidi::operator<<(ostream& os,
        const MidiChannelVoiceChannelPressureMessage& msg)
{
    return msg.text(os);
}

void textmidi::MidiChannelVoicePitchBendMessage
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (textmidi::pitch_wheel[0] == (~channel_mask & *midiiter))
    {
        local_status().running_status(*midiiter);
        ++midiiter;
    }
    else
    {
        local_status(running_status());
    }
    MidiStreamAtom pitch_wheel_lsb{*midiiter++};
    MidiStreamAtom pitch_wheel_msb{*midiiter++};
    channel(local_status().channel() + 1);
    pitch_wheel_ = pitch_wheel_lsb | (pitch_wheel_msb << byte7_shift);
}

ostream& textmidi::MidiChannelVoicePitchBendMessage::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "PITCH_WHEEL " << dec << static_cast<int>(channel())
       << ' ' << static_cast<int>(pitch_wheel_);
    auto oldflags{os.flags(flags)};
    return os;
}

MidiStreamAtom textmidi::MidiChannelVoicePitchBendMessage::pitch_wheel() const
{
    return pitch_wheel_;
}

void textmidi::MidiChannelVoicePitchBendMessage::pitch_wheel(MidiStreamAtom pitch_wheel)
{
    pitch_wheel_ = pitch_wheel;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelVoicePitchBendMessage& msg)
{
    return msg.text(os);
}

void textmidi::MidiChannelModeOmniPolyMessage
    ::consume_stream(MidiStreamIterator& )
{
}

ostream& textmidi::MidiChannelModeOmniPolyMessage::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "CHANNEL_MODE " << static_cast<int>(channel())
       << ' ' << static_cast<int>(mode_);
    auto oldflags{os.flags(flags)};
    return os;
}

MidiStreamAtom textmidi::MidiChannelModeOmniPolyMessage::mode() const
{
    return mode_;
}

void textmidi::MidiChannelModeOmniPolyMessage::mode(MidiStreamAtom mode)
{
    mode_ = mode;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelModeOmniPolyMessage& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiChannelModeOmniMonoMessage::text(ostream& os) const
{
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiChannelModeOmniMonoMessage& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiChannelModeChannelPolyMessage::text(ostream& os) const
{
    return os;
}

ostream& textmidi::operator<<(ostream& os,
        const MidiChannelModeChannelPolyMessage& msg)
{
    return msg.text(os);
}

ostream& MidiChannelModeChannelMonoMessage::text(ostream& os) const
{
    return os;
}

ostream& textmidi::operator<<(ostream& os,
        const MidiChannelModeChannelMonoMessage& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaSequenceEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (sequence_number_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (sequence_number_prefix[1] == *midiiter)
    {
        ++midiiter;
    }
    auto msb{*midiiter++};
    auto lsb{*midiiter++};
    sequence_number_ = (msb << bits_per_byte) | lsb;
}

ostream& textmidi::MidiFileMetaSequenceEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "SEQUENCE_NUMBER " << sequence_number_;
    auto oldflags{os.flags(flags)};
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

void textmidi::MidiFileMetaUnknown1Event
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (unknown1_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    auto len{*midiiter};
    midiiter += len + 1;
}

ostream& textmidi::MidiFileMetaUnknown1Event::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "UNKNOWN META 0x11";
    auto oldflags{os.flags(flags)};
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSequenceEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaStringEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    ++midiiter; // skip whatever type of string it is
    int64_t len{variable_len_value(midiiter)};
    str_.insert(str_.begin(), midiiter, midiiter + len);
    midiiter += len;
}

ostream& textmidi::MidiFileMetaStringEvent::text(ostream& os) const
{
    string display_str{str_};
    string::size_type pos{};

    while (0x0 == display_str[display_str.size() - 1])
    {
        display_str.resize(display_str.size() - 1);
    }
    pos = 0;
    while ((pos < display_str.size()) && (pos = display_str.find('\a', pos)) != str_.npos)
    {
        const char control_string[]{R"(\a)"};
        display_str.replace(pos, string::size_type{1}, &control_string[0], string::size_type{2}); 
        ++pos;
    }
    pos = 0;
    while ((pos < display_str.size()) && (pos = display_str.find('\b', pos)) != str_.npos)
    {
        const char control_string[]{R"(\b)"};
        display_str.replace(pos, string::size_type{1}, &control_string[0], string::size_type{2}); 
        ++pos;
    }
    pos = 0;
    while ((pos < display_str.size()) && (pos = display_str.find('\t', pos)) != str_.npos)
    {
        const char control_string[]{R"(\t)"};
        display_str.replace(pos, string::size_type{1}, &control_string[0], string::size_type{2}); 
        ++pos;
    }
    pos = 0;
    while ((pos < display_str.size()) && (pos = display_str.find('\n')) != str_.npos)
    {
        display_str.replace(pos, 1, R"(\n)", 2); 
        ++pos;
    }
    pos = 0;
    while ((pos < display_str.size()) && (pos = display_str.find('\v')) != str_.npos)
    {
        display_str.replace(pos, 1, R"(\v)", 2); 
        ++pos;
    }
    pos = 0;
    while ((pos < display_str.size()) && (pos = display_str.find('\f')) != str_.npos)
    {
        display_str.replace(pos, 1, R"(\f)", 2); 
        ++pos;
    }
    pos = 0;
    while ((pos < display_str.size()) && (pos = display_str.find('\r', pos)) != str_.npos)
    {
        const char control_string[]{R"(\r)"};
        display_str.replace(pos, string::size_type{1}, &control_string[0], string::size_type{2}); 
        ++pos;
    }
    pos = 0;
    while ((pos < display_str.size()) && (pos = display_str.find('\"', pos)) != str_.npos)
    {
        const char control_string[]{R"(\")"};
        display_str.replace(pos, string::size_type{1}, &control_string[0], string::size_type{2}); 
        pos += 2;
    }

    os << '"' << display_str << '"';
    return os;
}

ostream& textmidi::MidiFileMetaTextEvent::text(ostream& os) const
{
    os << "TEXT ";
    return MidiFileMetaStringEvent::text(os);
}
ostream& textmidi::MidiFileMetaText08Event::text(ostream& os) const
{
    os << "TEXT08 ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::MidiFileMetaText09Event::text(ostream& os) const
{
    os << "TEXT09 ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::MidiFileMetaText0AEvent::text(ostream& os) const
{
    os << "TEXT0A ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::MidiFileMetaText0BEvent::text(ostream& os) const
{
    os << "TEXT0B ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::MidiFileMetaText0CEvent::text(ostream& os) const
{
    os << "TEXT0C ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::MidiFileMetaText0DEvent::text(ostream& os) const
{
    os << "TEXT0D ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::MidiFileMetaText0EEvent::text(ostream& os) const
{
    os << "TEXT0E ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::MidiFileMetaText0FEvent::text(ostream& os) const
{
    os << "TEXT0F ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaTextEvent& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiFileMetaCopyrightEvent::text(ostream& os) const
{
    os << "COPYRIGHT ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaCopyrightEvent& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiFileMetaTrackEvent::text(ostream& os) const
{
    os << "TRACK ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaTrackEvent& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiFileMetaInstrumentEvent::text(ostream& os) const
{
    os << "INSTRUMENT ";
    return MidiFileMetaStringEvent::text(os);
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaInstrumentEvent& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiFileMetaLyricEvent::text(ostream& os) const
{
    os << "LYRIC ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaLyricEvent& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiFileMetaMarkerEvent::text(ostream& os) const
{
    os << "MARKER ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaMarkerEvent& msg)
{
    return msg.text(os);
}

ostream& textmidi::MidiFileMetaCuePointEvent::text(ostream& os) const
{
    os << "CUE_POINT ";
    return MidiFileMetaStringEvent::text(os);
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaCuePointEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaMidiChannelEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (midi_channel_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (midi_channel_prefix[1] == *midiiter)
    {
        ++midiiter;
    }
    channel_ = *midiiter + 1;
    ++midiiter;
}

ostream& textmidi::MidiFileMetaMidiChannelEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "MIDI_CHANNEL " << channel_;
    auto oldflags{os.flags(flags)};
    return os;
}

uint16_t textmidi::MidiFileMetaMidiChannelEvent::channel()
{
    return channel_;
}

void textmidi::MidiFileMetaMidiChannelEvent::channel(uint16_t channel)
{
    channel_ = channel;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaMidiChannelEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaEndOfTrackEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (end_of_track_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (end_of_track_prefix[1] == *midiiter)
    {
        ++midiiter;
    }
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

void textmidi::MidiFileMetaSetTempoEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (tempo_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (tempo_prefix[1] == *midiiter)
    {
        ++midiiter;
    }
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
        os << "TEMPO " << dec << setprecision(10) << ((60.0 * 1000000.0) / static_cast<double>(tempo_));
    }
    auto oldflags{os.flags(flags)};
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

void textmidi::MidiFileMetaSMPTEOffsetEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (smpte_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (smpte_prefix[1] == *midiiter)
    {
        ++midiiter;
    }
    hours_ = *midiiter++;
    minutes_ = *midiiter++;
    seconds_ = *midiiter++;
    fr_ = *midiiter++;
    ff_ = *midiiter++;
}

ostream& textmidi::MidiFileMetaSMPTEOffsetEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "SMPTE_OFFSET " << setfill('0') << setw(2) << hours_ << ':' << setw(2) << minutes_
       << ':' << setw(2) << seconds_ << ':' << setw(2) << fr_ << ':' << setw(2) << ff_;
    auto oldflags{os.flags(flags)};
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSMPTEOffsetEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaMidiPortEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (midi_port_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (midi_port_prefix[1] == *midiiter)
    {
        ++midiiter;
    }
    // 0xFF 0x21 0x01 0xport
    midiport_ = *midiiter++;
}

ostream& textmidi::MidiFileMetaMidiPortEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    os << "MIDI_PORT " << midiport_;
    auto oldflags{os.flags(flags)};
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaMidiPortEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaTimeSignatureEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (time_signature_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (time_signature_prefix[1] == *midiiter)
    {
        ++midiiter;
    }
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
    auto oldflags{os.flags(flags)};
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaTimeSignatureEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaKeySignatureEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (key_signature_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (key_signature_prefix[1] == *midiiter)
    {
        ++midiiter;
    }
    accidentals_ = static_cast<signed char>(*midiiter++);
    minor_mode_ = (*midiiter++ != 0);
}

ostream& textmidi::MidiFileMetaKeySignatureEvent::text(ostream& os) const
{
    auto flags{os.flags()};
    // -1 is one flat, 1, is one sharp, 0 is C or a.
    os << "KEY_SIGNATURE "
       << key_signature_map[KeySignatureMap::key_type{accidentals_, minor_mode_}];
    auto oldflags{os.flags(flags)};
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaKeySignatureEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaSequencerSpecificEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    // FF 7F [variable_len] mgfID or 00 mfgid0 mfgid1 
    if (meta_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
    if (sequencer_specific_prefix[0] == *midiiter)
    {
        ++midiiter;
    }
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
    for (auto it(data_.begin());
            (it != data_.end()) && (*it != end_of_sysex[0]);
            ++it)
    {
        os << ' ' << hex << "0x" << setw(2) << setfill('0')
           << static_cast<unsigned>(*it);
    }
    auto oldflags{os.flags(flags)};
    return os;
}

ostream& textmidi::operator<<(ostream& os,
        const MidiFileMetaSequencerSpecificEvent& msg)
{
    return msg.text(os);
}

MidiDelayMessagePair textmidi::MidiEventFactory::operator()(MidiStreamIterator& midiiter)
{
    int64_t delaynum64{variable_len_value(midiiter)};
    MidiDelayMessagePair midi_delay_message_pair(delaynum64, nullptr);
    ticks_accumulated_ += delaynum64;

    bool clear_ticks_accumulated{};
    if (MidiFileMetaSequenceEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaSequenceEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaTextEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaTextEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaCopyrightEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaCopyrightEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaTrackEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaTrackEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaInstrumentEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaInstrumentEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaLyricEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaLyricEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaMarkerEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaMarkerEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaCuePointEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaCuePointEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaUnknown1Event::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaUnknown1Event>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaMidiChannelEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaMidiChannelEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaEndOfTrackEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaEndOfTrackEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
        clear_ticks_accumulated = true;
    }
    if (MidiFileMetaSetTempoEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaSetTempoEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaSMPTEOffsetEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaSMPTEOffsetEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaTimeSignatureEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaTimeSignatureEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaKeySignatureEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaKeySignatureEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaMidiPortEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaMidiPortEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiFileMetaSequencerSpecificEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiFileMetaSequencerSpecificEvent>(running_status_);
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
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
    if (MidiSysExEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiSysExEvent>();
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }
    if (MidiSysExRawEvent::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiSysExRawEvent>();
#if defined(CLEAR_RUNNING_STATUS)
        running_status_.clear();
#endif
    }

    if (MidiChannelVoiceMessage::recognize(midiiter, midi_end_, running_status_.command()))
    {
        if (running_status_.running_status_state())
        {
            switch(running_status_.command())
            {
              case note_on[0]:
                midi_delay_message_pair.second = make_shared<MidiChannelVoiceNoteOnMessage>(running_status_, ticks_per_whole_);
                break;
              case note_off[0]:
                midi_delay_message_pair.second = make_shared<MidiChannelVoiceNoteOffMessage>(running_status_);
                break;
              case polyphonic_key_pressure[0]:
                midi_delay_message_pair.second = make_shared<MidiChannelVoicePolyphonicKeyPressureMessage>(running_status_);
                break;
              case control[0]:
                midi_delay_message_pair.second = make_shared<MidiChannelVoiceControlChangeMessage>(running_status_);
                break;
              case program[0]:
                midi_delay_message_pair.second = make_shared<MidiChannelVoiceProgramChangeMessage>(running_status_);
                break;
              case channel_pressure[0]:
                midi_delay_message_pair.second = make_shared<MidiChannelVoiceChannelPressureMessage>(running_status_);
                break;
              case pitch_wheel[0]:
                midi_delay_message_pair.second = make_shared<MidiChannelVoicePitchBendMessage >(running_status_);
                break;
              default:
                throw(runtime_error{"unknown bytes parsing running status"});
                break;
            }
        }
    }
    if (MidiChannelVoiceNoteOnMessage::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiChannelVoiceNoteOnMessage>(running_status_, ticks_per_whole_);
    }
    if (MidiChannelVoiceNoteOffMessage::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiChannelVoiceNoteOffMessage>(running_status_);
    }
    if (MidiChannelVoicePolyphonicKeyPressureMessage::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiChannelVoicePolyphonicKeyPressureMessage>(running_status_);
    }
    if (MidiChannelVoiceProgramChangeMessage::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared <MidiChannelVoiceProgramChangeMessage>(running_status_);
    }
    if (MidiChannelVoiceChannelPressureMessage::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiChannelVoiceChannelPressureMessage>(running_status_);
    }
    if (MidiChannelVoicePitchBendMessage::recognize(midiiter, midi_end_))
    {
        midi_delay_message_pair.second = make_shared<MidiChannelVoicePitchBendMessage>(running_status_);
    }
    if (MidiChannelVoiceControlChangeMessage::recognize(midiiter, midi_end_)) 
    {
        midi_delay_message_pair.second = make_shared<MidiChannelVoiceControlChangeMessage>(running_status_);
    }

#undef      DEBUG_MIDITEXT (1)
#if defined(DEBUG_MIDITEXT)
    if (!midi_delay_message_pair.second)
    {
        for (auto i{0}; i < 32; ++i)
        {
            cout << setfill('0') << setw(2) << hex << static_cast<int>(*midiiter++) << ' ';
        }
        exit(0);
    }
    else
    {
#endif
    midi_delay_message_pair.second->consume_stream(midiiter);
    midi_delay_message_pair.second->ticks_accumulated(ticks_accumulated_);
    if (clear_ticks_accumulated)
    {
        ticks_accumulated_ = 0LU;
        clear_ticks_accumulated = false;
    }
    MidiChannelVoiceMessage* mcvm{dynamic_cast<MidiChannelVoiceMessage*>(midi_delay_message_pair.second.get())};
    if (mcvm)
    {
        running_status_ = mcvm->local_status();
    }
#if defined(DEBUG_MIDITEXT)
    }
#endif
    return midi_delay_message_pair;
}

int64_t  MidiEventFactory::ticks_accumulated_{};
uint32_t MidiEventFactory::ticks_per_whole_{};

void textmidi::PrintLazyTrack::ticks_of_note_on()
{
    for (auto delaymsgiter{midi_delay_message_pairs_.begin()};
        (delaymsgiter != midi_delay_message_pairs_.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
              (delaymsgiter->second.get()); ++delaymsgiter)
    {
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnMessage*>
                                 (delaymsgiter->second.get())};
        if (note_on && (note_on->velocity() != 0))
        {
            bool same_chord{true};
            auto offiter{delaymsgiter};
            ++offiter;
            for ( ;
                 (offiter != midi_delay_message_pairs_.end())
                         && (!dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                                 (offiter->second.get()));
                 ++offiter)
            {
                if (offiter->first != 0)
                {
                    same_chord = false;
                }
                auto note_on_zero{dynamic_cast<MidiChannelVoiceNoteOnMessage*>
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
                    auto note_off{dynamic_cast<MidiChannelVoiceNoteOffMessage*>
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
    for (auto delaymsgiter{midi_delay_message_pairs_.begin()};
        (delaymsgiter != midi_delay_message_pairs_.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                (delaymsgiter->second.get()); ++delaymsgiter)
    {
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnMessage*>
            (delaymsgiter->second.get())};
        if (note_on && (note_on->velocity() != 0))
        {
            auto offiter{delaymsgiter};
            ++offiter;
            for ( ;
                 (offiter != midi_delay_message_pairs_.end())
                         && (!dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                             (offiter->second.get()));
                 ++offiter)
            {
                auto next_note_on{dynamic_cast<MidiChannelVoiceNoteOnMessage*>
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
    for (auto delaymsgiter{midi_delay_message_pairs_.begin()};
        (delaymsgiter != midi_delay_message_pairs_.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                      (delaymsgiter->second.get());
        ++delaymsgiter)
    {
        if ((delaymsgiter + 1) != midi_delay_message_pairs_.end())
        {
            delaymsgiter->second->ticks_to_next_event(
                (delaymsgiter + 1)->second->ticks_accumulated()
                - delaymsgiter->second->ticks_accumulated());
        }
    }
}

void textmidi::PrintLazyTrack::wholes_to_next_event()
{
    const auto ticksperquantum{quantum_
        ? (quantum_ * rational::RhythmRational{QuartersPerWhole} * rational::RhythmRational{ticksperquarter_}) : rational::RhythmRational{1L}};
    for (auto delaymsgiter{midi_delay_message_pairs_.begin()};
        (delaymsgiter != midi_delay_message_pairs_.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                     (delaymsgiter->second.get());
         ++delaymsgiter)
    {
        if ((delaymsgiter + 1) != midi_delay_message_pairs_.end())
        {
            RhythmRational ratio_to_next_event{static_cast<std::int64_t>
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
    for (auto delaymsgiter{midi_delay_message_pairs_.begin()};
        (delaymsgiter != midi_delay_message_pairs_.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                (delaymsgiter->second.get()); ++delaymsgiter)
    {
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnMessage*>
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
        auto rest{dynamic_cast<MidiChannelVoiceNoteRestMessage*>
            (delaymsgiter->second.get())};
        if (rest)
        {
            RhythmRational ratio_to_note_off{static_cast<int64_t>
                (rest->ticks_to_next_event()),
                    MidiEventFactory::ticks_per_whole_};
            ratio_to_note_off.reduce();
            rest->wholes_to_noteoff(rational::snap(ratio_to_note_off, quantum_));
        }
    }
}

void textmidi::PrintLazyTrack::print(ostream& os, MidiDelayMessagePair& mdmp)
{
    MidiMessage* mm{mdmp.second.get()};
    auto* no{dynamic_cast<MidiChannelVoiceNoteOnMessage*>(mm)};
    if (no)
    {
        if (no->velocity())
        {
            if (dynamic_ != no->velocity())
            {
                os << lazy_string(true);
                if (dynamics_reverse_map.contains(no->velocity()))
                {
                    os << dynamics_reverse_map.at(no->velocity()) << '\n';
                }
                else
                {
                    os << "vel " << static_cast<int>(no->velocity()) << ' ';
                }
                dynamic_ = no->velocity();
            }
            if (no->channel() != channel_)
            {
                os << lazy_string(true);
                channel_ = no->channel();
                os << "chan " << static_cast<int>(channel_) << ' ';
            }
            // find out if there are notes that are tied in.

            // There are tied-in notes.
            // Find the soonest thing to happen.
            //
            // First write the notes that are tied into the present.
            //
            auto min_wholes_to_noteoff{no->wholes_to_noteoff()};
            auto min_wholes_to_next_event{no->wholes_to_next_event()};
            if (!tied_list_.empty())
            {
                for (auto& tiednote : tied_list_)
                {
                    if (tiednote.wholes_to_noteoff())
                    {
                        min_wholes_to_noteoff
                            = std::min(tiednote.wholes_to_noteoff(), min_wholes_to_noteoff);
                    }
                    if (tiednote.wholes_to_next_event())
                    {
                        min_wholes_to_next_event
                            = std::min(tiednote.wholes_to_next_event(), min_wholes_to_next_event);
                    }
                }
                for (auto& tiednote : tied_list_)
                {
                    os << lazy_string(true);
                    os << '-';
                    os << tiednote.key_string();
                    if (tiednote.wholes_to_noteoff() > min_wholes_to_next_event)
                    {
                        os << "- ";
                    }
                    else
                    {
                        remove_if(tied_list_.begin(), tied_list_.end(),
                            [tiednote](const MidiChannelVoiceNoteOnMessage& mcvnom)
                            { return tiednote.local_status().running_status_state() == mcvnom.local_status().running_status_state(); } );
                        os << ' ';
                    }
                    tiednote.wholes_to_next_event(tiednote.wholes_to_next_event() - min_wholes_to_next_event);
                    const auto temp{tiednote.wholes_to_next_event() * RhythmRational{MidiEventFactory::ticks_per_whole_, 1L}};
                    tiednote.ticks_to_next_event(temp.numerator() / temp.denominator());

                    tiednote.wholes_to_noteoff(tiednote.wholes_to_noteoff()       - min_wholes_to_next_event);
                    const auto temp2{tiednote.wholes_to_noteoff() * RhythmRational{MidiEventFactory::ticks_per_whole_, 1L}};
                    tiednote.ticks_to_noteoff(temp.numerator() / temp.denominator());
                }
                remove_if(tied_list_.begin(), tied_list_.end(),
                    [](MidiChannelVoiceNoteOnMessage mcvnom) { return RhythmRational{} == mcvnom.wholes_to_noteoff(); });
            }

            os << no->key_string();
            if ((min_wholes_to_next_event > RhythmRational{}) && (no->wholes_to_noteoff() > min_wholes_to_next_event))
            {
                os << "- ";
            }
            else
            {
                os << ' ';
            }
            RhythmRational duration{min_wholes_to_next_event};
            if (duration > RhythmRational{})
            {
                print_rhythm(os, duration);
                os << '\n';
            }
            no->wholes_to_next_event(no->wholes_to_next_event() - min_wholes_to_next_event);
            no->wholes_to_noteoff(no->wholes_to_noteoff()       - min_wholes_to_next_event);
            if ((min_wholes_to_next_event > RhythmRational{}) &&(no->wholes_to_noteoff() > min_wholes_to_next_event))
            {
                tied_list_.push_back(*no);
            }

        }
    }
    else // not a note-on
    {
        const auto* noff{dynamic_cast<MidiChannelVoiceNoteOffMessage*>(mm)};
        if (!noff)
        {
            const auto* rest{dynamic_cast<MidiChannelVoiceNoteRestMessage*>(mm)};
            if (rest)
            {
                RhythmRational duration{rest->wholes_to_next_event()};
                duration.reduce();
                if (duration > RhythmRational{})
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
    }
}

std::ostream& textmidi::operator<<(std::ostream& os, PrintLazyTrack& print_lazy_track)
{
    for (auto& mp : print_lazy_track.midi_delay_message_pairs_)
    {
        print_lazy_track.print(os, mp);
    }
    return os;
}

string textmidi::PrintLazyTrack::lazy_string(bool lazy)
{
    string str{};
    if (lazy)
    {
        if (!lazy_)
        {
            lazy_ = lazy;
            return string{"LAZY "};
        }
    }
    else
    {
        if (lazy_)
        {
            lazy_ = lazy;
            return string{"END_LAZY "};
        }
    }
    return str;

}

void textmidi::PrintLazyTrack::insert_rests()
{
    // building a new sequence of MidiDelayMessagePair to replace the old one.
    MidiDelayMessagePairs new_midi_delay_message_pairs;

    // Create a separate bit keyboard for each channel.
    typedef std::bitset<MidiPitchQty> KeyBoard;
    typedef std::vector<KeyBoard> ChannelKeyBoards;
    ChannelKeyBoards channel_keyboards(MidiChannelQty);

    int64_t rest_start_ticks_accumulated{};
    bool inrest{false};
    for (auto delaymsgiter{midi_delay_message_pairs_.begin()};
              delaymsgiter != midi_delay_message_pairs_.end();
            ++delaymsgiter)
    {
        auto non{dynamic_cast<MidiChannelVoiceNoteOnMessage*>
            (delaymsgiter->second.get())};
        if (non)
        {
            if (inrest && (0L < non->velocity()))
            {
                const auto rest_ticks{
                    non->ticks_accumulated() - rest_start_ticks_accumulated};
                rest_start_ticks_accumulated
                    = non->ticks_accumulated() + non->ticks_to_next_event();
                if (rest_ticks)
                {
                    auto rest{make_shared<MidiChannelVoiceNoteRestMessage>()};
                    rest->ticks_accumulated(rest_start_ticks_accumulated);
                    rest->ticks_to_next_event(rest_ticks);
                    RhythmRational wholes{rest_ticks, MidiEventFactory::ticks_per_whole_};
                    wholes.reduce();
                    if (wholes > RhythmRational{})
                    {
                        rest->wholes_to_next_event(wholes);
                        rest->wholes_to_noteoff(wholes);
                        MidiDelayMessagePair rest_pair{0, rest};
                        new_midi_delay_message_pairs.push_back(rest_pair);
                        rest_start_ticks_accumulated = non->ticks_accumulated() + non->ticks_to_noteoff();
                    }
                }
            }
            channel_keyboards[non->channel() - 1][non->key()] = true;
            new_midi_delay_message_pairs.push_back(*delaymsgiter);
            const auto thisnoteoff{(0L == non->velocity())};

            if (thisnoteoff)
            {
                // Update channel keyboards.
                channel_keyboards[non->channel() - 1][non->key()] = false;
                if (!inrest)
                {
                    // This is a state machine.
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
                        rest_start_ticks_accumulated = non->ticks_accumulated();
                    }
                }
            }
        }
        else
        {
            auto noff{dynamic_cast<MidiChannelVoiceNoteOffMessage*>
                (delaymsgiter->second.get())};
            if (noff)
            {
                // Update channel keyboards.
                channel_keyboards[noff->channel() - 1][noff->key()] = false;
                if (!inrest)
                {
                    // This is a state machine.
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
                        rest_start_ticks_accumulated = noff->ticks_accumulated();
                    }
                }
                new_midi_delay_message_pairs.push_back(*delaymsgiter);
            }
            else
            {
                new_midi_delay_message_pairs.push_back(*delaymsgiter);
                auto rest{make_shared<MidiChannelVoiceNoteRestMessage>()};
                rest->ticks_accumulated(delaymsgiter->second.get()->ticks_accumulated());
                delaymsgiter->second.get()->ticks_accumulated(0L);
                rest->ticks_to_next_event(delaymsgiter->second.get()->ticks_to_next_event());
                delaymsgiter->second.get()->ticks_to_next_event(0L);
                rest->ticks_to_next_note_on(delaymsgiter->second.get()->ticks_to_next_note_on());
                delaymsgiter->second.get()->ticks_to_next_note_on(0L);
                if (rest->ticks_to_next_event() > 0)
                {
                    rest->wholes_to_next_event(RhythmRational{rest->ticks_to_next_event(), MidiEventFactory::ticks_per_whole_});
                    rest->wholes_to_noteoff(rest->wholes_to_next_event());
                    MidiDelayMessagePair rest_pair{rest->ticks_accumulated(), rest};
                    new_midi_delay_message_pairs.push_back(rest_pair);
                }
                inrest = true;
            }
        }
    }
    midi_delay_message_pairs_ = new_midi_delay_message_pairs;
}

int textmidi::PrintLazyTrack::dynamic_{128};
bool textmidi::PrintLazyTrack::lazy_{false};


