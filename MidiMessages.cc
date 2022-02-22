//
// TextMIDITools Version 1.0.21
//
// textmidi 1.0.6
// Copyright © 2022 Thomas E. Janzen
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
uint64_t textmidi::MidiMessage::ticks_accumulated() const
{
    return ticks_accumulated_;
}

//
// set the accumulated MIDI ticks.
void textmidi::MidiMessage::ticks_accumulated(uint64_t ticks_accumulated)
{
    ticks_accumulated_ = ticks_accumulated;
}

//
// return the MIDI ticks to the next event.
uint64_t textmidi::MidiMessage::ticks_to_next_event() const
{
    return ticks_to_next_event_;
}

//
// set the MIDI ticks to the next event.
void textmidi::MidiMessage::ticks_to_next_event(uint64_t ticks_to_next_event)
{
    ticks_to_next_event_ = ticks_to_next_event;
}

uint64_t textmidi::MidiMessage::ticks_to_next_note_on() const
{
    return ticks_to_next_note_on_;
}

void textmidi::MidiMessage::ticks_to_next_note_on(uint64_t ticks_to_next_note_on)
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
    ++midiiter;
    int64_t len{variable_len_value(midiiter)};
    int64_t count{};
    do
    {
        sys_ex_vec_.push_back(*midiiter);
        ++count;
    } while ((*midiiter++ != end_of_sysex) && (count < len));
}

//
// Write the text version of the event to os.
ostream& textmidi::MidiSysExEvent::text(ostream& os) const
{
    os << '\n' << hex << "SYSEX";
    for (auto sys_ex_it(sys_ex_vec_.begin());
            (sys_ex_it != sys_ex_vec_.end()) && (*sys_ex_it != end_of_sysex);
            ++sys_ex_it)
    {
        os << ' ' << hex << "0x" << setw(2) << setfill('0')
           << static_cast<unsigned>(*sys_ex_it);
    }
    os << dec;
    return os;
}

void textmidi::MidiSysExRawEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    ++midiiter;
    const int64_t len{variable_len_value(midiiter)};
    for (unsigned b{}; b < len; ++b)
    {
        sys_ex_vec_.push_back(*midiiter++);
    };
}

ostream& textmidi::MidiSysExRawEvent::text(ostream& os) const
{
    os << '\n' << hex << "SYSEXRAW";
    for (auto sys_ex_it(sys_ex_vec_.begin());
            (sys_ex_it != sys_ex_vec_.end()) && (*sys_ex_it != end_of_sysex);
            ++sys_ex_it)
    {
        os << ' ' << hex << "0x"
           << setw(2) << setfill('0') << static_cast<unsigned>(*sys_ex_it);
    }
    os << dec;
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiSysExRawEvent& msg)
{
    return msg.text(os);
}

uint8_t textmidi::MidiChannelVoiceMessage::running_status() const
{
    return running_status_;
}

void textmidi::MidiChannelVoiceMessage::running_status(uint8_t running_status)
{
    running_status_ = running_status;
}

uint8_t textmidi::MidiChannelVoiceMessage::channel() const
{
    return channel_;
}

void textmidi::MidiChannelVoiceMessage::channel(uint8_t channel)
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
    key_ = *midiiter++;
    key_string_ = num_to_note(key_);
    velocity_ = static_cast<int>(*midiiter++);
}

uint8_t textmidi::MidiChannelVoiceNoteMessage::key() const
{
    return key_;
}

uint8_t textmidi::MidiChannelVoiceNoteMessage::velocity() const
{
    return velocity_;
}

string textmidi::MidiChannelVoiceNoteMessage::key_string() const
{
    return key_string_;
}

void textmidi::MidiChannelVoiceNoteMessage::key(uint8_t key)
{
    key_ = key;
}

void textmidi::MidiChannelVoiceNoteMessage::velocity(uint8_t velocity)
{
    velocity_ = velocity;
}

void textmidi::MidiChannelVoiceNoteMessage::key_string(string_view key_string)
{
    key_string_ = key_string;
}

ostream& textmidi::MidiChannelVoiceNoteMessage::text(ostream& os) const
{
    return os << dec << static_cast<unsigned>(channel()) << ' '
              << key_string() << ' ' << static_cast<unsigned>(velocity())
              << ' ';
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
    if (wholes_to_next_event())
    {
        os << "r ";
    }
    print_rhythm(os, wholes_to_next_event());
    if (wholes_to_next_event())
    {
        os << ' ';
    }
    return os;
}

void textmidi::MidiChannelVoiceNoteRestMessage
    ::wholes_to_noteoff(const RhythmRational& wholes_to_noteoff)
{
    wholes_to_noteoff_ = wholes_to_noteoff;
    wholes_to_noteoff_.reduce();
}

RhythmRational textmidi::MidiChannelVoiceNoteRestMessage::wholes_to_noteoff() const
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
    ::wholes_to_noteoff(const RhythmRational& wholes_to_noteoff)
{
    wholes_to_noteoff_ = wholes_to_noteoff;
    wholes_to_noteoff_.reduce();
}

RhythmRational textmidi::MidiChannelVoiceNoteOnMessage::wholes_to_noteoff() const
{
    return wholes_to_noteoff_;
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
    id_      = *midiiter++;
    value_   = *midiiter++;
}

ostream& textmidi::MidiChannelVoiceControlChangeMessage::text(ostream& os) const
{
    if (control_pan == id_)
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
    return os;
}

uint8_t textmidi::MidiChannelVoiceControlChangeMessage::id() const
{
    return id_;
}

void textmidi::MidiChannelVoiceControlChangeMessage::id(uint8_t id)
{
    id_ = id;
}

uint8_t textmidi::MidiChannelVoiceControlChangeMessage::value() const
{
    return value_;
}

void textmidi::MidiChannelVoiceControlChangeMessage::value(uint8_t value)
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
    program_ = *midiiter++;
}

ostream& textmidi::MidiChannelVoiceProgramChangeMessage::text(ostream& os) const
{
    os << "PROGRAM " << dec << static_cast<int>(channel()) << ' '
       << (static_cast<int>(program_) + 1);
    return os;
}

uint8_t textmidi::MidiChannelVoiceProgramChangeMessage::program() const
{
    return program_;
}

void textmidi::MidiChannelVoiceProgramChangeMessage::program(uint8_t program)
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
    pressure_ = *midiiter++;
}

ostream& textmidi::MidiChannelVoiceChannelPressureMessage::text(ostream& os) const
{
    return os;
    os << "CHANNEL_PRESSURE " << dec << static_cast<int>(channel())
       << ' ' << static_cast<int>(pressure_);
}

uint8_t textmidi::MidiChannelVoiceChannelPressureMessage::pressure() const
{
    return pressure_;
}

void textmidi::MidiChannelVoiceChannelPressureMessage::pressure(uint8_t pressure)
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
    uint8_t pitch_wheel_lsb{*midiiter++};
    uint8_t pitch_wheel_msb{*midiiter++};
    pitch_wheel_ = pitch_wheel_lsb | (pitch_wheel_msb << bits_per_byte);
}

ostream& textmidi::MidiChannelVoicePitchBendMessage::text(ostream& os) const
{
    return os << "PITCH_WHEEL " << dec << static_cast<int>(channel())
              << ' ' << static_cast<int>(pitch_wheel_);
}

uint8_t textmidi::MidiChannelVoicePitchBendMessage::pitch_wheel() const
{
    return pitch_wheel_;
}

void textmidi::MidiChannelVoicePitchBendMessage::pitch_wheel(uint8_t pitch_wheel)
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
    os << "CHANNEL_MODE " << static_cast<int>(channel())
       << ' ' << static_cast<int>(mode_);
    return os;
}

uint8_t textmidi::MidiChannelModeOmniPolyMessage::mode() const
{
    return mode_;
}

void textmidi::MidiChannelModeOmniPolyMessage::mode(uint8_t mode)
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
    ++midiiter;
    auto msb{*midiiter++};
    auto lsb{*midiiter++};
    sequence_number_ = (msb << bits_per_byte) | lsb;
}

ostream& textmidi::MidiFileMetaSequenceEvent::text(ostream& os) const
{
    os << "SEQUENCE " << sequence_number_;
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
    auto len{*midiiter};
    midiiter += len + 1;
}

ostream& textmidi::MidiFileMetaUnknown1Event::text(ostream& os) const
{
    os << "UNKNOWN META 0x11";
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSequenceEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaStringEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    int64_t len{variable_len_value(midiiter)};
    str_.insert(str_.begin(), midiiter, midiiter + len);
    midiiter += len;
}

ostream& textmidi::MidiFileMetaStringEvent::text(ostream& os) const
{
    os << "\"" << str_ << "\"" ;
    return os;
}

ostream& textmidi::MidiFileMetaTextEvent::text(ostream& os) const
{
    os << "TEXT ";
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
    ++midiiter;
    channel_ = *midiiter++;
}

ostream& textmidi::MidiFileMetaMidiChannelEvent::text(ostream& os) const
{
    os << "MIDI_CHANNEL " << channel_;
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
    ++midiiter;
}

ostream& textmidi::MidiFileMetaEndOfTrackEvent::text(ostream& os) const
{
    os << "END_OF_TRACK";
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaEndOfTrackEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaSetTempoEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    ++midiiter;
    tempo_ = *midiiter++;
    tempo_ <<= 8;
    tempo_ |= *midiiter++;
    tempo_ <<= 8;
    tempo_ |= *midiiter++;
}

ostream& textmidi::MidiFileMetaSetTempoEvent::text(ostream& os) const
{
    if (0 == tempo_)
    {
        os << "TEMPO 0";
    }
    else
    {
        os << "TEMPO " << ((60 * 1000000) / tempo_);
    }
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
    ++midiiter;
    hours_ = *midiiter++;
    minutes_ = *midiiter++;
    seconds_ = *midiiter++;
    fr_ = *midiiter++;
    ff_ = *midiiter++;
}

ostream& textmidi::MidiFileMetaSMPTEOffsetEvent::text(ostream& os) const
{
    os << "SMPTE_OFFSET " << hours_ << ':' << minutes_
       << ':' << seconds_ << ':' << fr_ << ':' << ff_;
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaSMPTEOffsetEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaMidiPortEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    // 0xFF 0x21 0x01 0xport
    ++midiiter; // skip 0x01
    midiport_ = *midiiter++;
}

ostream& textmidi::MidiFileMetaMidiPortEvent::text(ostream& os) const
{
    os << "MIDI_PORT " << midiport_;
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaMidiPortEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaTimeSignatureEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    ++midiiter;
    numerator_ = *midiiter++;
    denominator_ = *midiiter++;
    clocks_per_click_ = *midiiter++;
    thirtyseconds_per_quarter_= *midiiter++;
}

ostream& textmidi::MidiFileMetaTimeSignatureEvent::text(ostream& os) const
{
    os << "TIME_SIGNATURE " << numerator_ << ' '
       << (1 << denominator_)
       << ' ' << clocks_per_click_;
    // no one knows what thirtyseconds_per_quarter_ is for, so hide it.
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaTimeSignatureEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaKeySignatureEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    ++midiiter;
    accidentals_ = static_cast<signed char>(*midiiter++);
    minor_mode_ = (*midiiter++ != 0);
}

ostream& textmidi::MidiFileMetaKeySignatureEvent::text(ostream& os) const
{
    // -1 is one flat, 1, is one sharp, 0 is C or a.
    os << "KEY_SIGNATURE "
       << key_signature_map[KeySignatureMap::key_type{accidentals_, minor_mode_}];
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiFileMetaKeySignatureEvent& msg)
{
    return msg.text(os);
}

void textmidi::MidiFileMetaSequenceSpecificEvent
    ::consume_stream(MidiStreamIterator& midiiter)
{
    // skip 0x37 already found by switch case
    len_ = variable_len_value(midiiter);
    if (0 == *midiiter) // if manufacturer ID is 3 long
    {
        len_ -= 3;
        ++midiiter;
        manufacturer_ = *midiiter++ << 8;
        manufacturer_ = manufacturer_ | *midiiter++;
    }
    else
    {
        --len_;
        manufacturer_ = *midiiter++;
    }
    data_.resize(len_);
    copy(midiiter, midiiter + len_, data_.data());
    midiiter += len_;
}

ostream& textmidi::MidiFileMetaSequenceSpecificEvent::text(ostream& os) const
{
    os << "SEQUENCE_SPECIFIC " << manufacturer_ << ' ' << '{';
    for (unsigned l{0}; l < len_; ++l)
    {
        if (l > 0)
        {
            os << ',';
        }
        if (isprint(data_[l]))
        {
            os << data_[l];
        }
        else
        {
            os << "0x" << hex << static_cast<int>(data_[l]);
        }
    }
    os << '}' << dec << '\n';
    return os;
}

ostream& textmidi::operator<<(ostream& os,
        const MidiFileMetaSequenceSpecificEvent& msg)
{
    return msg.text(os);
}

MidiDelayMessagePair textmidi::MidiEventFactory::operator()(MidiStreamIterator& midiiter)
{
    auto midiitersave{midiiter};
    int64_t delaynum64{variable_len_value(midiiter)};
    MidiDelayMessagePair midi_delay_message_pair(delaynum64, nullptr);
    ticks_accumulated_ += delaynum64;

    uint8_t status{};
    switch (*midiiter)
    {
      case meta_prefix:
        {
            ++midiiter; // skip FF meta byte
            switch (*midiiter)
            {
               [[unlikely]] case sequence_prefix:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaSequenceEvent>();
                break;
               [[likely]] case text_prefix:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaTextEvent>();
                break;
              case copyright_prefix:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaCopyrightEvent>();
                break;
               [[unlikely]] case track_name_prefix:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaTrackEvent>();
                break;
               [[unlikely]] case instrument_name_prefix:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaInstrumentEvent>();
                break;
               [[unlikely]] case lyric_prefix:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaLyricEvent>();
                break;
               [[unlikely]] case marker_prefix:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaMarkerEvent>();
                break;
               [[unlikely]] case cue_point_prefix:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaCuePointEvent>();
                break;
               [[unlikely]] case unknown1_prefix[0]:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaUnknown1Event>();
                break;
               [[likely]] case midi_channel_prefix[0]:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaMidiChannelEvent>();
                break;
               [[likely]] case end_of_track_prefix[0]:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaEndOfTrackEvent>();
                break;
               [[likely]] case tempo_prefix[0]:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaSetTempoEvent>();
                break;
               [[unlikely]] case smpte_prefix[0]:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaSMPTEOffsetEvent>();
                break;
               [[likely]] case time_signature_prefix[0]:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaTimeSignatureEvent>();
                break;
               [[likely]] case key_signature_prefix[0]:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaKeySignatureEvent>();
                break;
              case midi_port_prefix[0]:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaMidiPortEvent>();
                break;
               [[unlikely]] case sequence_specific_prefix[0]:
                midi_delay_message_pair.second
                    = make_shared<MidiFileMetaSequenceSpecificEvent>();
                break;
              default:
                break;
            }
            ++midiiter; // skip meta event data length byte
        }
        break;
      [[unlikely]] case midi_time_code_quarter_frame:
        break;
      [[unlikely]] case song_position_pointer:
        break;
      [[unlikely]] case song_select:
        break;
      [[unlikely]] case tune_request:
        break;
      case start_of_sysex:
        midi_delay_message_pair.second = make_shared<MidiSysExEvent>();
        break;
      case end_of_sysex: // SYSEX Continuation and out-of-band messages
        midi_delay_message_pair.second = make_shared<MidiSysExRawEvent>();
        break;
      default:
        {
            if (*midiiter & event_flag)
            {
                status = *midiiter;
                running_status_ = status;
                ++midiiter;
            }
            else
            {
                status = running_status_;
            }
            switch (status & ~channel_mask)
            {
              [[likely]] case note_on:
                midi_delay_message_pair.second
                    = make_shared<MidiChannelVoiceNoteOnMessage>(status);
                break;
              [[likely]] case note_off:
                midi_delay_message_pair.second
                    = make_shared<MidiChannelVoiceNoteOffMessage>(status);
                break;
              [[likely]] case poly_key_pressure:
                midi_delay_message_pair.second
                    = make_shared
                    <MidiChannelVoicePolyphonicKeyPressureMessage>(status);
                break;
              [[likely]] case program:
                midi_delay_message_pair.second
                    = make_shared
                    <MidiChannelVoiceProgramChangeMessage >(status);
                break;
              [[likely]] case channel_pressure:
                midi_delay_message_pair.second
                    = make_shared
                    <MidiChannelVoiceChannelPressureMessage>(status);
                break;
              case pitch_wheel:
                midi_delay_message_pair.second
                    = make_shared<MidiChannelVoicePitchBendMessage>(status);
                break;
              case channel_mode:
                midi_delay_message_pair.second
                    = make_shared
                    <MidiChannelVoiceControlChangeMessage>(status);
                break;
              default: // is using running status.
                {
                    cerr << "UNKNOWN EVENT: " << hex << status << '\n';

                }
                break;
            }
        }
        break;
    }
    midi_delay_message_pair.second->consume_stream(midiiter);
    midi_delay_message_pair.second->ticks_accumulated(ticks_accumulated_);
    if (dynamic_cast<MidiFileMetaEndOfTrackEvent*>
            (midi_delay_message_pair.second.get()))
    {
        ticks_accumulated_ = 0LU;
    }
    return midi_delay_message_pair;
}

int64_t MidiEventFactory::ticks_accumulated_{};
int8_t MidiEventFactory::running_status_{};


void textmidi::ticks_of_note_on(MidiDelayMessagePairs&  midi_delay_message_pairs)
{
    for (auto delaymsgiter{midi_delay_message_pairs.begin()};
        (delaymsgiter != midi_delay_message_pairs.end())
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
                 (offiter != midi_delay_message_pairs.end())
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

void textmidi::ticks_to_next_note_on(MidiDelayMessagePairs& midi_delay_message_pairs)
{
    for (auto delaymsgiter{midi_delay_message_pairs.begin()};
        (delaymsgiter != midi_delay_message_pairs.end())
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
                 (offiter != midi_delay_message_pairs.end())
                         && (!dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                             (offiter->second.get()));
                 ++offiter)
            {
                auto next_note_on{dynamic_cast<MidiChannelVoiceNoteOnMessage*>
                    (offiter->second.get())};
                if (next_note_on && (next_note_on->velocity() != 0))
                {
                    if (*next_note_on == *note_on)
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

void textmidi::ticks_to_next_event(MidiDelayMessagePairs&  midi_delay_message_pairs)
{
    for (auto delaymsgiter{midi_delay_message_pairs.begin()};
        (delaymsgiter != midi_delay_message_pairs.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                      (delaymsgiter->second.get());
        ++delaymsgiter)
    {
        if ((delaymsgiter + 1) != midi_delay_message_pairs.end())
        {
            delaymsgiter->second->ticks_to_next_event(
                (delaymsgiter + 1)->second->ticks_accumulated()
                - delaymsgiter->second->ticks_accumulated());
        }
    }
}

void textmidi::value_to_next_event(MidiDelayMessagePairs&  midi_delay_message_pairs,
        const RhythmRational& quantum, uint32_t ticksperquarter)
{
    const auto ticksperquantum{quantum
        ? (quantum * rational::RhythmRational{QuartersPerWhole} * rational::RhythmRational{ticksperquarter}) : rational::RhythmRational{1L}};
    for (auto delaymsgiter{midi_delay_message_pairs.begin()};
        (delaymsgiter != midi_delay_message_pairs.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                     (delaymsgiter->second.get());
         ++delaymsgiter)
    {
        if ((delaymsgiter + 1) != midi_delay_message_pairs.end())
        {
            RhythmRational ratio_to_next_event{static_cast<std::int64_t>
                (delaymsgiter->second->ticks_to_next_event()),
                    QuartersPerWhole * ticksperquarter};
            delaymsgiter->second->wholes_to_next_event(
                    rational::snap(ratio_to_next_event, quantum));
            auto temp{delaymsgiter->second->wholes_to_next_event()};
            temp.reduce();
            delaymsgiter->second->wholes_to_next_event(temp);
        }
    }
}

void textmidi::value_of_note_on(MidiDelayMessagePairs&  midi_delay_message_pairs,
        const RhythmRational& quantum, uint32_t ticksperquarter)
{
    for (auto delaymsgiter{midi_delay_message_pairs.begin()};
        (delaymsgiter != midi_delay_message_pairs.end())
        && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                (delaymsgiter->second.get()); ++delaymsgiter)
    {
        auto note_on{dynamic_cast<MidiChannelVoiceNoteOnMessage*>
            (delaymsgiter->second.get())};
        if (note_on)
        {
            RhythmRational ratio_to_note_off{note_on->ticks_to_noteoff(),
                ticksperquarter * QuartersPerWhole};
            note_on->wholes_to_noteoff(
                rational::snap(ratio_to_note_off, quantum));
            RhythmRational ratio_past_note_off{note_on->ticks_past_noteoff(),
                ticksperquarter * QuartersPerWhole};
            note_on->wholes_past_noteoff(
                rational::snap(ratio_past_note_off, quantum));
        }
        auto rest{dynamic_cast<MidiChannelVoiceNoteRestMessage*>
            (delaymsgiter->second.get())};
        if (rest)
        {
            RhythmRational ratio_to_note_off{static_cast<int64_t>
                (rest->ticks_to_next_event()),
                    ticksperquarter * QuartersPerWhole};
            rest->wholes_to_noteoff(rational
                    ::snap(ratio_to_note_off, quantum));
        }
    }
}

void textmidi::PrintLazyEvent::operator()(ostream& os, MidiMessage* mm)
{
    const auto* no{dynamic_cast<MidiChannelVoiceNoteOnMessage*>(mm)};
    if (no)
    {
        if (!lazy_)
        {
            os << "LAZY ";
            lazy_ = true;
        }
        if (no->channel() != channel_)
        {
            channel_ = no->channel();
            os << "chan " << static_cast<int>(channel_) << '\n';
        }
        if (no->velocity())
        {
            if (dynamic_ != no->velocity())
            {
                if (dynamics_reverse_map.contains(no->velocity()))
                {
                    os << dynamics_reverse_map.at(no->velocity()) << '\n';
                } 
                else
                {
                    os << "vel " << no->velocity() << '\n';
                }
                dynamic_ = no->velocity();
            }
            os << no->key_string() << ' ';
        }
        if (no->wholes_to_noteoff())
        {
            if (no->wholes_to_next_event() != rational::RhythmRational{0L})
            {
                print_rhythm(os, no->wholes_to_noteoff()) << '\n';
                if (no->wholes_past_noteoff() > rational::RhythmRational{0L})
                {
                    os << "R ";
                    textmidi::rational::print_rhythm(os, no->wholes_past_noteoff());
                    os << ' ';
                }
            }
        }
        return;
    }
    const auto*
        rest{dynamic_cast<MidiChannelVoiceNoteRestMessage*>(mm)};
    if (rest)
    {
        if (rest->wholes_to_noteoff())
        {
            if (!lazy_)
            {
                os << "LAZY ";
                lazy_ = true;
            }
            os << rest->key_string() << ' ';
            if (rest->wholes_to_noteoff())
            {
                print_rhythm(os, rest->wholes_to_noteoff()) << '\n';
            }
        }
        return;
    }

    if (dynamic_cast<const MidiChannelVoiceNoteOffMessage*>(mm))
    {
        return;
    }
    if (lazy_)
    {
        os << "END_LAZY ";
        lazy_ = false;
    }
    os << *mm << ' ';
    print_rhythm(os, mm->wholes_to_next_event());
    os << '\n';
}

void textmidi::insert_rests(MidiDelayMessagePairs&  midi_delay_message_pairs)
{
    MidiDelayMessagePairs new_midi_delay_message_pairs;

    typedef std::bitset<MidiPitchQty> KeyBoard;
    typedef std::vector<KeyBoard> ChannelKeyBoards;

    ChannelKeyBoards channel_keyboards(MidiChannelQty);

    uint64_t rest_start_ticks_accumulated{};
    bool inrest{true};
    for (auto delaymsgiter{midi_delay_message_pairs.begin()};
              delaymsgiter != midi_delay_message_pairs.end();
            ++delaymsgiter)
    {
        new_midi_delay_message_pairs.push_back(*delaymsgiter);
        auto non{dynamic_cast<MidiChannelVoiceNoteOnMessage*>
            (delaymsgiter->second.get())};
        if (non)
        {
            channel_keyboards[non->channel() - 1][non->key()] = true;
            if (inrest)
            {
                const auto rest_ticks{
                    non->ticks_accumulated() - rest_start_ticks_accumulated};
                if (rest_ticks)
                {
                    auto rest{make_shared<MidiChannelVoiceNoteRestMessage>()};
                    rest->ticks_accumulated(rest_start_ticks_accumulated);
                    rest->ticks_to_next_event(rest_ticks);
                    MidiDelayMessagePair rest_pair{0, rest};
                    new_midi_delay_message_pairs.push_back(rest_pair);
                }
            }
            inrest = true;
            for (auto chkb : channel_keyboards)
            {
                inrest &= chkb.none();
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
                        rest_start_ticks_accumulated
                            = noff->ticks_accumulated();
                    }
                }
            }
        }
    }
    midi_delay_message_pairs = new_midi_delay_message_pairs;
}

int textmidi::PrintLazyEvent::dynamic_{128};


