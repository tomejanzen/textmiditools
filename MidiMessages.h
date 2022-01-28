//
// TextMIDITools Version 1.0.11
//
// textmidi 1.0.6
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// 2021-03-02 don't write zero delays
//
// 2020-04-03 rewrite mididisasm Thomas Janzen Maynard, MA
//
// 2020-07-15 fix crash.  byte counting in MidiMessages.h
//
#if !defined(MIDI_EVENTS)
#    define  MIDI_EVENTS

#include <cstdint>
#include <cstdlib>
#include <bitset>
#include <iomanip>
#include <map>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/rational.hpp>

#include "Midi.h"

namespace textmidi
{
    //
    // Holds binary standard MIDI file format data.
    typedef std::vector<uint8_t> MidiStreamVector;
    typedef std::vector<uint8_t>::iterator MidiStreamIterator;

    typedef boost::rational<int64_t> Ratio64;

    typedef std::map<std::pair<std::int32_t, bool>, std::string_view> KeySignatureMap;
    extern KeySignatureMap key_signature_map;

    std::ostream& print_lazy_value(std::ostream& os, const Ratio64& ratio64);

    std::int64_t variable_len_value(MidiStreamIterator& midiiter);

    //
    // A base class for MIDI messages, such as key events.
    // This is a inherit implementation design.
    class MidiMessage
    {
      public:
        MidiMessage()
          : ticks_accumulated_{},
            ticks_to_next_event_{},
            ticks_to_next_note_on_{},
            wholes_to_next_event_{}
        {
        }
        MidiMessage&& move(MidiMessage&& ) = delete;
        MidiMessage& operator=(MidiMessage&& ) = delete;
        //
        // Inspect the bytes of MIDI binary stream and save
        // the data in the class.
        virtual void consume_stream(MidiStreamIterator& midiiter) = 0;
        //
        // Write the textmidi version of a MIDI event to os.
        virtual std::ostream& text(std::ostream& ) const = 0;
        virtual ~MidiMessage() = default;
        //
        // Report the accumulated number of ticks so far in this track.
        std::uint64_t ticks_accumulated() const;
        void ticks_accumulated(std::uint64_t ticks_accumulated);
        // Report the number of ticks before the next event.
        std::uint64_t ticks_to_next_event() const;
        void ticks_to_next_event(std::uint64_t ticks_to_next_event);
        // Report the number of ticks before the next note-on.
        std::uint64_t ticks_to_next_note_on() const;
        void ticks_to_next_note_on(std::uint64_t ticks_to_next_note_on);
        // Report the number of whole notes before the next event.
        Ratio64 wholes_to_next_event() const;
        void wholes_to_next_event(const Ratio64& wholes_to_next_event);
      private:
        std::uint64_t ticks_accumulated_;
        std::uint64_t ticks_to_next_event_;
        std::uint64_t ticks_to_next_note_on_;
        Ratio64 wholes_to_next_event_;
    };

    std::ostream& operator<<(std::ostream& os, const MidiMessage& msg);

    typedef std::pair<std::uint64_t, std::shared_ptr<MidiMessage> >
        MidiDelayMessagePair;
    std::ostream& operator<<(std::ostream& os,
        const MidiDelayMessagePair& msg_pair);

    typedef std::vector<MidiDelayMessagePair> MidiDelayMessagePairs;

    //
    // MIDI System Exclusive support.
    class MidiSysExEvent final : public MidiMessage
    {
      public:
        MidiSysExEvent()
            : MidiMessage{},
              sys_ex_vec_{}
        {}
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
      private:
        std::vector<std::uint8_t> sys_ex_vec_;
    };

    // I don't know what this is; maybe it's in spec 1.1.
    // It's in the classical archives files.
    // It's an F7, length following this, incl. another f7, dah dit dah F7.
    // But the final F7 is optional and vendor-defined.
    class MidiSysExRawEvent final : public MidiMessage
    {
      public:
        MidiSysExRawEvent()
            : MidiMessage{},
              sys_ex_vec_{}
        {}

        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
      private:
        std::vector<std::uint8_t> sys_ex_vec_;
    };
    std::ostream& operator<<(std::ostream& os, const MidiSysExRawEvent& msg);

    //
    // Base class for voice messages
    class MidiChannelVoiceMessage : public MidiMessage
    {
      public:
        MidiChannelVoiceMessage(std::uint8_t status)
          : MidiMessage{},
            running_status_{status},
            channel_{static_cast<decltype(channel_)>
                (((status & channel_mask) + 1) & 0xFF)}
        {
        }
        void consume_stream(MidiStreamIterator& )
        {
        }
        std::uint8_t running_status() const;
        void running_status(std::uint8_t running_status);
        std::uint8_t channel() const;
        void channel(std::uint8_t channel);
      private:
        std::uint8_t running_status_;
        std::uint8_t channel_;
    };
    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceMessage& msg);

    //
    // Base class for MIDI voice note messages, such as note on and note off.
    class MidiChannelVoiceNoteMessage : public MidiChannelVoiceMessage
    {
      public:
        MidiChannelVoiceNoteMessage(std::uint8_t status)
          : MidiChannelVoiceMessage(status),
            key_{},
            velocity_{},
            key_string_{}
        {
        }

        void consume_stream(MidiStreamIterator& midiiter) override;
        std::uint8_t key() const;
        std::uint8_t velocity() const;
        std::string key_string() const;
        void key(std::uint8_t key);
        void velocity(std::uint8_t velocity);
        void key_string(std::string_view key_string);
        std::ostream& text(std::ostream& os) const;
        bool operator==(const MidiChannelVoiceNoteMessage& note) const;
      private:
        std::uint8_t key_;
        std::uint8_t velocity_;
        std::string key_string_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceNoteMessage& msg);

    //
    // Rest Message.  This is not in the MIDI spec;
    // it is a fiction crafted for textmidi.
    class MidiChannelVoiceNoteRestMessage final : public MidiChannelVoiceNoteMessage
    {
      public:
        MidiChannelVoiceNoteRestMessage(std::uint8_t status = 0)
          : MidiChannelVoiceNoteMessage(status),
            wholes_to_noteoff_{}
        {
            key_string("r");
        }
        std::ostream& text(std::ostream& os) const;
        void wholes_to_noteoff(const Ratio64& wholes_to_noteoff);
        Ratio64 wholes_to_noteoff() const;
      private:
        Ratio64 wholes_to_noteoff_;
    };

    //
    // MIDI Note Off support.
    class MidiChannelVoiceNoteOffMessage final : public MidiChannelVoiceNoteMessage
    {
      public:
        MidiChannelVoiceNoteOffMessage(std::uint8_t status)
          : MidiChannelVoiceNoteMessage(status)
        {
        }
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceNoteOffMessage& msg);

    //
    // MIDI Note On support
    class MidiChannelVoiceNoteOnMessage final : public MidiChannelVoiceNoteMessage
    {
      public:
        MidiChannelVoiceNoteOnMessage(std::uint8_t status)
          : MidiChannelVoiceNoteMessage(status),
            ticks_to_noteoff_{},
            ticks_past_noteoff_{},
            wholes_to_noteoff_{},
            wholes_past_noteoff_{}
        {
        }
        std::ostream& text(std::ostream& os) const;
        void ticks_to_noteoff(std::int64_t ticks_to_noteoff);
        std::int64_t ticks_to_noteoff() const;
        void ticks_past_noteoff(std::int64_t ticks_past_noteoff);
        std::int64_t ticks_past_noteoff() const;
        void wholes_to_noteoff(const Ratio64& wholes_to_noteoff);
        Ratio64 wholes_to_noteoff() const;
        void wholes_past_noteoff(const Ratio64& wholes_past_noteoff);
        Ratio64 wholes_past_noteoff() const;
      private:
        std::int64_t ticks_to_noteoff_;
        std::int64_t ticks_past_noteoff_;
        Ratio64 wholes_to_noteoff_;
        Ratio64 wholes_past_noteoff_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceNoteOnMessage& msg);

    //
    // MIDI poly key pressure support.
    class MidiChannelVoicePolyphonicKeyPressureMessage
        final : public MidiChannelVoiceNoteMessage
    {
      public:
        MidiChannelVoicePolyphonicKeyPressureMessage(std::uint8_t status)
          : MidiChannelVoiceNoteMessage(status)
        {
        }
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoicePolyphonicKeyPressureMessage& msg);

    //
    // MIDI control change support
    class MidiChannelVoiceControlChangeMessage
        final : public MidiChannelVoiceMessage
    {
      public:
        MidiChannelVoiceControlChangeMessage(std::uint8_t status)
          : MidiChannelVoiceMessage(status),
            id_{},
            value_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        std::uint8_t id() const;
        void id(std::uint8_t id);
        std::uint8_t value() const;
        void value(std::uint8_t value);
      private:
        std::uint8_t id_;
        std::uint8_t value_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceControlChangeMessage& msg);

    //
    // MIDI program change support.
    class MidiChannelVoiceProgramChangeMessage
        final : public MidiChannelVoiceMessage
    {
      public:
        MidiChannelVoiceProgramChangeMessage(std::uint8_t status)
          : MidiChannelVoiceMessage{status},
            program_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        std::uint8_t program() const;
        void program(std::uint8_t program);
      private:
        std::uint8_t program_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceProgramChangeMessage& msg);

    //
    // MIDI Channel Pressure support
    class MidiChannelVoiceChannelPressureMessage
        final : public MidiChannelVoiceMessage
    {
      public:
        MidiChannelVoiceChannelPressureMessage(std::uint8_t status)
          : MidiChannelVoiceMessage(status),
            pressure_{}
        {
        }

        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        std::uint8_t pressure() const;
        void pressure(std::uint8_t pressure);
      private:
        std::uint8_t pressure_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceChannelPressureMessage& msg);

    //
    // MIDI pitch bend support
    class MidiChannelVoicePitchBendMessage final : public MidiChannelVoiceMessage
    {
      public:
        MidiChannelVoicePitchBendMessage(std::uint8_t status)
          : MidiChannelVoiceMessage{status},
            pitch_wheel_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        std::uint8_t pitch_wheel() const;
        void pitch_wheel(std::uint8_t pitch_wheel);
      private:
        std::uint16_t pitch_wheel_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoicePitchBendMessage& msg);

    //
    // MIDI treats Mode messages differently
    // but that is a concept and not existential.
    class MidiChannelModeMessage : public MidiMessage
    {
    };

    //
    // MIDI Mode Omni/Poly support
    class MidiChannelModeOmniPolyMessage final : public MidiChannelVoiceMessage
    {
      public:
        void consume_stream(MidiStreamIterator& ) override;
        std::ostream& text(std::ostream& os) const;
        std::uint8_t mode() const;
        void mode(std::uint8_t mode);
      private:
        std::uint8_t mode_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelModeOmniPolyMessage& msg);

    //
    // MIDI Mode Omni/Mono support
    class MidiChannelModeOmniMonoMessage final : public MidiChannelModeMessage
    {
      public:
        void consume_stream(MidiStreamIterator& )
        {
        }
        std::ostream& text(std::ostream& os) const override;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelModeOmniMonoMessage& msg);

    //
    // MIDI Mode Poly support
    class MidiChannelModeChannelPolyMessage final : public MidiChannelModeMessage
    {
      public:
        void consume_stream(MidiStreamIterator& )
        {
        }
        std::ostream& text(std::ostream& os) const override;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelModeChannelPolyMessage& msg);

    //
    // MIDI Mono support
    class MidiChannelModeChannelMonoMessage final : public MidiChannelModeMessage
    {
      public:
        void consume_stream(MidiStreamIterator& )
        {
        }
        std::ostream& text(std::ostream& os) const override;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelModeChannelMonoMessage& msg);

    //
    // An empty class to create a branch  for MIDI Meta Events.
    class MidiFileMetaEvent : public MidiMessage
    {
    };

    //
    // MIDI Meta Sequence
    class MidiFileMetaSequenceEvent final : public MidiFileMetaEvent
    {
      public:
          MidiFileMetaSequenceEvent()
            : MidiFileMetaEvent{},
              sequence_number_{}
        {}
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        void sequence_number(std::uint16_t sequence_number);
        std::uint16_t sequence_number() const;
      private:
        std::uint16_t sequence_number_;
    };

    //
    // MIDI Meta Unknown event
    class MidiFileMetaUnknown1Event final : public MidiFileMetaEvent
    {
      public:
          MidiFileMetaUnknown1Event()
            : MidiFileMetaEvent{}
        {}
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaSequenceEvent& msg);

    //
    // MIDI string events creates a branch
    // for MIDI TEXT, AUTHOR, COPYRIGHT, etc.
    class MidiFileMetaStringEvent : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaStringEvent()
          : MidiFileMetaEvent{},
            str_{}
        {}
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
      private:
        std::string str_;
    };

    //
    // MIDI Text support
    class MidiFileMetaTextEvent final : public MidiFileMetaStringEvent
    {
      public:
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaTextEvent& msg);

    //
    // MIDI Copyright support
    class MidiFileMetaCopyrightEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaCopyrightEvent()
          : MidiFileMetaStringEvent{},
            copyright_{}
        {}
        std::ostream& text(std::ostream& os) const;
      private:
        std::string copyright_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaCopyrightEvent& msg);

    //
    // Branch in class heirarchy for Track events
    class MidiFileMetaTrackEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaTrackEvent()
          : MidiFileMetaStringEvent{},
            name_{}
        {
        }
        std::ostream& text(std::ostream& os) const;
      private:
        std::string name_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaTrackEvent& msg);

    //
    // MIDI Instrument event support
    class MidiFileMetaInstrumentEvent final : public MidiFileMetaStringEvent
    {
      public:
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaInstrumentEvent& msg);

    //
    // MIDI Lyric support
    class MidiFileMetaLyricEvent final : public MidiFileMetaStringEvent
    {
      public:
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaLyricEvent& msg);

    //
    // MIDI Marker support
    class MidiFileMetaMarkerEvent final : public MidiFileMetaStringEvent
    {
      public:
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaMarkerEvent& msg);

    //
    // MIDI cue point support
    class MidiFileMetaCuePointEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaCuePointEvent()
          : MidiFileMetaStringEvent{},
            cue_point_{}
        {
        }
        std::ostream& text(std::ostream& os) const;
      private:
        std::string cue_point_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaCuePointEvent& msg);

    //
    // MIDI Channel event branch in class heirachy
    class MidiFileMetaMidiChannelEvent final : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaMidiChannelEvent()
          : MidiFileMetaEvent{},
            channel_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        std::uint16_t channel();
        void channel(std::uint16_t channel);
      private:
        std::uint16_t channel_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaMidiChannelEvent& msg);

    //
    // MIDI END OF TRACK support
    class MidiFileMetaEndOfTrackEvent final : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaEndOfTrackEvent()
          : MidiFileMetaEvent{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaEndOfTrackEvent& msg);

    //
    // MIDI Tempo event support
    class MidiFileMetaSetTempoEvent final : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaSetTempoEvent()
          : MidiFileMetaEvent{},
            tempo_{60}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        void tempo(std::uint32_t tempo);
        std::uint32_t tempo() const;
      private:
        std::uint32_t tempo_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaSetTempoEvent& msg);

    //
    // MIDI SMPTE OFFSET support
    class MidiFileMetaSMPTEOffsetEvent final : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaSMPTEOffsetEvent()
          : MidiFileMetaEvent{},
            hours_{},
            minutes_{},
            seconds_{},
            fr_{},
            ff_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
      private:
        std::uint16_t hours_;
        std::uint16_t minutes_;
        std::uint16_t seconds_;
        std::uint16_t fr_;
        std::uint16_t ff_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaSMPTEOffsetEvent& msg);

    //
    // MIDI Port event
    class MidiFileMetaMidiPortEvent final : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaMidiPortEvent()
          : MidiFileMetaEvent{},
            midiport_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
      private:
        std::uint16_t midiport_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaMidiPortEvent& msg);

    //
    // MIDI Time Signature support
    class MidiFileMetaTimeSignatureEvent final : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaTimeSignatureEvent()
          : MidiFileMetaEvent{},
            numerator_{},
            denominator_{},
            clocks_per_click_{},
            thirtyseconds_per_quarter_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
      private:
        std::uint16_t numerator_;
        std::uint16_t denominator_;
        std::uint16_t clocks_per_click_;
        std::uint16_t thirtyseconds_per_quarter_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaTimeSignatureEvent& msg);

    //
    // MIDI Key Signature support
    class MidiFileMetaKeySignatureEvent final : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaKeySignatureEvent()
          : MidiFileMetaEvent{},
            accidentals_{},
            minor_mode_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
      private:
        std::int32_t accidentals_;
        bool minor_mode_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaKeySignatureEvent& msg);

    //
    // MIDI sequence specific event support
    class MidiFileMetaSequenceSpecificEvent final : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaSequenceSpecificEvent()
          : MidiFileMetaEvent{},
            len_{},
            manufacturer_{},
            data_()
        {}
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        private:
            std::size_t len_;
            std::uint16_t manufacturer_;
            std::vector<uint8_t> data_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaSequenceSpecificEvent& msg);

    //
    // The factory that creates the classes above.
    class MidiEventFactory
    {
        public:
        MidiDelayMessagePair operator()(MidiStreamIterator& midiiter);
        static std::int64_t ticks_accumulated_;
        static std::int8_t  running_status_;
    };

    void ticks_of_note_on(MidiDelayMessagePairs&  midi_delay_message_pairs);
    void ticks_to_next_note_on(MidiDelayMessagePairs&
        midi_delay_message_pairs);
    void ticks_to_next_event(MidiDelayMessagePairs&
        midi_delay_message_pairs);

    void value_to_next_event(MidiDelayMessagePairs&  midi_delay_message_pairs,
            const Ratio64& quantum, std::uint32_t ticksperquarter);
    void value_of_note_on(MidiDelayMessagePairs&  midi_delay_message_pairs,
            const Ratio64& quantum, std::uint32_t ticksperquarter);

    // Print the text version of an event in LAZY mode.
    class PrintLazyEvent
    {
      public:
        PrintLazyEvent(bool lazy = false, std::uint8_t channel = 0)
          : lazy_(lazy),
            channel_(channel)
        {}
        void operator()(std::ostream& os, MidiMessage* mm);
      private:
        bool lazy_;
        std::uint8_t channel_;
    };

    //
    // traverse the list of MIDI messages and insert rest events
    // representing the delay after each event when there is no sounding note.
    void insert_rests(MidiDelayMessagePairs&  midi_delay_message_pairs);

}

#endif // MIDI_EVENTS
