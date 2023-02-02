//
// TextMIDITools Version 1.0.29
//
// textmidi 1.0.6
// Copyright © 2023 Thomas E. Janzen
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
#include <stdexcept>
#include <utility>
#include <vector>
#include <list>

#include <boost/lexical_cast.hpp>
#include <boost/preprocessor.hpp>

#include "RhythmRational.h"

#include "Midi.h"

namespace textmidi
{
    //
    // Holds binary standard MIDI file format data.
    typedef std::uint8_t MidiStreamAtom;
    typedef std::vector<MidiStreamAtom> MidiStreamVector;
    typedef MidiStreamVector::iterator MidiStreamIterator;
    typedef MidiStreamVector::const_iterator MidiStreamConstIterator;
    class RunningStatus
    {
      public:
        RunningStatus()
          : running_status_state_(),
            running_status_value_{}
        {
        }
        RunningStatus(const RunningStatus& ) = default;
        RunningStatus& operator=(const RunningStatus& ) = default;
        void running_status(MidiStreamAtom running_status_value)
        {
            running_status_state_ = true;
            running_status_value_ = running_status_value;
        }

        void clear()
        {
            running_status_state_ = false;
            running_status_value_ = 0u;
        }
        bool running_status_state() const
        {
            return running_status_state_;
        }
        MidiStreamAtom running_status_value() const
        {
            return running_status_value_;
        }
        // returns a zero-based channel
        MidiStreamAtom channel() const
        {
            return running_status_value_ & textmidi::channel_mask;
        }
        MidiStreamAtom command() const
        {
            return running_status_value_ & ~textmidi::channel_mask;
        }
      private:
        bool running_status_state_;
        MidiStreamAtom running_status_value_;
    };

    typedef std::map<std::pair<std::int32_t, bool>, std::string_view> KeySignatureMap;
    extern KeySignatureMap key_signature_map;

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
        MidiMessage(const MidiMessage& ) = default;
        MidiMessage&& move(MidiMessage&& ) = delete;
        MidiMessage& operator=(MidiMessage&& ) = default;
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
        std::int64_t ticks_accumulated() const;
        void ticks_accumulated(std::int64_t ticks_accumulated);
        // Report the number of ticks before the next event.
        std::int64_t ticks_to_next_event() const;
        void ticks_to_next_event(std::int64_t ticks_to_next_event);
        // Report the number of ticks before the next note-on.
        std::int64_t ticks_to_next_note_on() const;
        void ticks_to_next_note_on(std::int64_t ticks_to_next_note_on);
        // Report the number of whole notes before the next event.
        rational::RhythmRational wholes_to_next_event() const;
        void wholes_to_next_event(const rational::RhythmRational& wholes_to_next_event);
      private:
        std::int64_t ticks_accumulated_;
        std::int64_t ticks_to_next_event_;
        std::int64_t ticks_to_next_note_on_;
        rational::RhythmRational wholes_to_next_event_;
    };

    std::ostream& operator<<(std::ostream& os, const MidiMessage& msg);

    typedef std::pair<std::int64_t, std::shared_ptr<MidiMessage> >
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
              data_{}
        {}
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(start_of_sysex[0])};
        static constexpr bool recognize(const MidiStreamConstIterator& chi, const MidiStreamConstIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return start_of_sysex[0] == *chi;
            }
            return false;
        }
      private:
        std::vector<MidiStreamAtom> data_;
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
              data_{}
        {}

        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(end_of_sysex[0])};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return end_of_sysex[0] == *chi;
            }
            return false;
        }
      private:
        std::vector<MidiStreamAtom> data_;
    };
    std::ostream& operator<<(std::ostream& os, const MidiSysExRawEvent& msg);

    //
    // Base class for voice messages
    class MidiChannelVoiceMessage : public MidiMessage
    {
      public:
        MidiChannelVoiceMessage(const RunningStatus& running_status)
          : MidiMessage{},
            running_status_{running_status},
            local_status_{},
            channel_{running_status.channel() + 1}
        {
        }
        MidiStreamAtom channel() const;
        void channel(MidiStreamAtom channel);
        RunningStatus& running_status()
        {
            return running_status_;
        }
        const RunningStatus& running_status() const
        {
            return running_status_;
        }
        const RunningStatus& local_status() const
        {
            return local_status_;
        }
        void local_status(const RunningStatus& rs)
        {
            local_status_ = rs;
        }
        RunningStatus& local_status()
        {
            return local_status_;
        }
        static constexpr long int prefix_len{full_note_length - sizeof(note_on)};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end, MidiStreamAtom cmd)
        {
            // Recognize an event with running status.
            // All software evolves during maintenance to make all
            // data global and asymmetrically handled.
            // channel_pressure and control change have only 1 byte following the command.
            // Check if these are data bytes and not commands.
            if ((cmd == channel_pressure[0]) || (cmd == program[0]))
            {
                return (*chi & event_flag) == 0;
            }
            else
            {
                if (std::distance(chi, the_end) >= prefix_len)
                {
                    // look for two bytes in a row with 1 << 8 clear.
                    if ((*chi & event_flag) == 0)
                    {
                        ++chi;
                        return (*chi & event_flag) == 0;
                    }
                    return false;
                }
            }
            return false;
        }
      private:
        RunningStatus running_status_;
        RunningStatus local_status_;
        MidiStreamAtom channel_;
    };
    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceMessage& msg);

    //
    // Base class for MIDI voice note messages, such as note on and note off.
    class MidiChannelVoiceNoteMessage : public MidiChannelVoiceMessage
    {
      public:
        MidiChannelVoiceNoteMessage(const RunningStatus& running_status)
          : MidiChannelVoiceMessage{running_status},
            key_{},
            velocity_{},
            key_string_{},
            wholes_to_noteoff_{}
        {
        }

        void consume_stream(MidiStreamIterator& midiiter) override;
        MidiStreamAtom key() const;
        MidiStreamAtom velocity() const;
        std::string key_string() const;
        void key(MidiStreamAtom key);
        void velocity(MidiStreamAtom velocity);
        void key_string(std::string_view key_string);
        std::ostream& text(std::ostream& os) const;
        bool operator==(const MidiChannelVoiceNoteMessage& note) const;
        void wholes_to_noteoff(const rational::RhythmRational& wholes_to_noteoff);
        rational::RhythmRational wholes_to_noteoff() const;
      private:
        MidiStreamAtom key_;
        MidiStreamAtom velocity_;
        std::string key_string_;
        rational::RhythmRational wholes_to_noteoff_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceNoteMessage& msg);

    //
    // Rest Message.  This is not in the MIDI spec;
    // it is a fiction specifically crafted for textmidi.
    class MidiChannelVoiceNoteRestMessage final : public MidiChannelVoiceNoteMessage
    {
      public:
        MidiChannelVoiceNoteRestMessage(const RunningStatus& running_status = RunningStatus{})
          : MidiChannelVoiceNoteMessage(running_status)
        {
            key_string("R");
        }
        std::ostream& text(std::ostream& os) const;
    };

    //
    // MIDI Note Off support.
    class MidiChannelVoiceNoteOffMessage final : public MidiChannelVoiceNoteMessage
    {
      public:
        MidiChannelVoiceNoteOffMessage(RunningStatus& running_status)
          : MidiChannelVoiceNoteMessage{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == note_off[0];
            }
            return false;
        }
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceNoteOffMessage& msg);

    //
    // MIDI Note On support
    class MidiChannelVoiceNoteOnMessage final : public MidiChannelVoiceNoteMessage
    {
      public:
        MidiChannelVoiceNoteOnMessage(const RunningStatus& running_status, std::uint32_t ticks_per_whole)
          : MidiChannelVoiceNoteMessage(running_status),
            ticks_per_whole_{ticks_per_whole},
            ticks_to_noteoff_{},
            ticks_past_noteoff_{},
            wholes_past_noteoff_{}
        {
        }
        std::ostream& text(std::ostream& os) const;
        void ticks_to_noteoff(std::int64_t ticks_to_noteoff);
        std::int64_t ticks_to_noteoff() const;
        void ticks_past_noteoff(std::int64_t ticks_past_noteoff);
        std::int64_t ticks_past_noteoff() const;
        void wholes_past_noteoff(const rational::RhythmRational& wholes_past_noteoff);
        rational::RhythmRational wholes_past_noteoff() const;
        std::uint32_t ticks_per_whole() const;
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == note_on[0];
            }
            return false;
        }
      private:
        std::uint32_t ticks_per_whole_;
        std::int64_t ticks_to_noteoff_;
        std::int64_t ticks_past_noteoff_;
        rational::RhythmRational wholes_past_noteoff_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceNoteOnMessage& msg);

    //
    // MIDI poly key pressure support.
    class MidiChannelVoicePolyphonicKeyPressureMessage
        final : public MidiChannelVoiceNoteMessage
    {
      public:
        MidiChannelVoicePolyphonicKeyPressureMessage(const RunningStatus& running_status)
          : MidiChannelVoiceNoteMessage{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == polyphonic_key_pressure[0];
            }
            return false;
        }
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoicePolyphonicKeyPressureMessage& msg);

    //
    // MIDI control change support
    class MidiChannelVoiceControlChangeMessage
        final : public MidiChannelVoiceMessage
    {
      public:
        MidiChannelVoiceControlChangeMessage(const RunningStatus& running_status)
          : MidiChannelVoiceMessage(running_status),
            id_{},
            value_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        MidiStreamAtom id() const;
        void id(MidiStreamAtom id);
        MidiStreamAtom value() const;
        void value(MidiStreamAtom value);
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == control[0];
            }
            return false;
        }
      private:
        MidiStreamAtom id_;
        MidiStreamAtom value_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceControlChangeMessage& msg);

    //
    // MIDI program change support.
    class MidiChannelVoiceProgramChangeMessage
        final : public MidiChannelVoiceMessage
    {
      public:
        MidiChannelVoiceProgramChangeMessage(const RunningStatus& running_status)
          : MidiChannelVoiceMessage{running_status},
            program_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        MidiStreamAtom program() const;
        void program(MidiStreamAtom program);
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == textmidi::program[0];
            }
            return false;
        }
      private:
        MidiStreamAtom program_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceProgramChangeMessage& msg);

    //
    // MIDI Channel Pressure support
    class MidiChannelVoiceChannelPressureMessage
        final : public MidiChannelVoiceMessage
    {
      public:
        MidiChannelVoiceChannelPressureMessage(const RunningStatus& running_status)
          : MidiChannelVoiceMessage(running_status),
            pressure_{}
        {
        }

        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        MidiStreamAtom pressure() const;
        void pressure(MidiStreamAtom pressure);
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == channel_pressure[0];
            }
            return false;
        }
      private:
        MidiStreamAtom pressure_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelVoiceChannelPressureMessage& msg);

    //
    // MIDI pitch bend support
    class MidiChannelVoicePitchBendMessage final : public MidiChannelVoiceMessage
    {
      public:
        MidiChannelVoicePitchBendMessage(const RunningStatus& running_status)
          : MidiChannelVoiceMessage{running_status},
            pitch_wheel_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        MidiStreamAtom pitch_wheel() const;
        void pitch_wheel(MidiStreamAtom pitch_wheel);
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == textmidi::pitch_wheel[0];
            }
            return false;
        }
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
        MidiStreamAtom mode() const;
        void mode(MidiStreamAtom mode);
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == control_onmi_on[0];
            }
            return false;
        }
      private:
        MidiStreamAtom mode_;
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
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == control_mono_on[0];
            }
            return false;
        }
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
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == control_poly_on[0];
            }
            return false;
        }
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
        static constexpr long int prefix_len{full_note_length};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (*chi & ~channel_mask) == control_mono_on[0];
            }
            return false;
        }
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiChannelModeChannelMonoMessage& msg);

    //
    // A base class for MIDI Meta Events.
    class MidiFileMetaEvent : public MidiMessage
    {
      public:
        MidiFileMetaEvent(RunningStatus& running_status)
        {
#if defined(CLEAR_RUNNING_STATUS)
            running_status.clear();
#endif
        }
        static constexpr long int prefix_len{sizeof(meta_prefix)};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                return (meta_prefix[0] == *chi);
            }
            return false;
        }

    };

    //
    // MIDI Meta Sequence
    class MidiFileMetaSequenceEvent final : public MidiFileMetaEvent
    {
      public:
          MidiFileMetaSequenceEvent(RunningStatus& running_status)
            : MidiFileMetaEvent{running_status},
              sequence_number_{}
        {}
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        void sequence_number(std::uint16_t sequence_number);
        std::uint16_t sequence_number() const;
        static constexpr long int prefix_len{sizeof(meta_prefix) + sizeof(sequence_number_prefix)};
        // Meant to recognize 0xFF 0x00 0x02
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    if (sequence_number_prefix[0] == *chi++)
                    {
                        return sequence_number_prefix[1] == *chi;
                    }
                }
            }
            return false;
        }
      private:
        std::uint16_t sequence_number_;
    };

    //
    // MIDI Meta Unknown event
    class MidiFileMetaUnknown1Event final : public MidiFileMetaEvent
    {
      public:
          MidiFileMetaUnknown1Event(RunningStatus& running_status)
            : MidiFileMetaEvent{running_status}
        {}
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(unknown1_prefix)};
        // Meant to recognize 0xFF 0x00 0x02
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return unknown1_prefix[0] == *chi++;
                }
                else
                {
                    return false;
                }
            }
            return false;
        }
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaSequenceEvent& msg);

    //
    // MIDI string events creates a branch
    // for MIDI TEXT, AUTHOR, COPYRIGHT, etc.
    class MidiFileMetaStringEvent : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaStringEvent(RunningStatus& running_status)
          : MidiFileMetaEvent{running_status},
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
        MidiFileMetaTextEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(text_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return text_prefix[0] == *chi++;
                }
            }
            return false;
        }
    };
    class MidiFileMetaText08Event final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaText08Event(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(text_08_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return text_08_prefix[0] == *chi++;
                }
            }
            return false;
        }
    };
    class MidiFileMetaText09Event final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaText09Event(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(text_09_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return text_09_prefix[0] == *chi++;
                }
            }
            return false;
        }
    };
    class MidiFileMetaText0AEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaText0AEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(text_0A_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return text_0A_prefix[0] == *chi++;
                }
            }
            return false;
        }
    };
    class MidiFileMetaText0BEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaText0BEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(text_0B_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return text_0B_prefix[0] == *chi++;
                }
            }
            return false;
        }
    };

    class MidiFileMetaText0CEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaText0CEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(text_0C_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return text_0C_prefix[0] == *chi++;
                }
            }
            return false;
        }
    };

    class MidiFileMetaText0DEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaText0DEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(text_0D_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return text_0D_prefix[0] == *chi++;
                }
            }
            return false;
        }
    };

    class MidiFileMetaText0EEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaText0EEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(text_0E_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return text_0E_prefix[0] == *chi++;
                }
            }
            return false;
        }
    };

    class MidiFileMetaText0FEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaText0FEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(text_0F_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return text_0F_prefix[0] == *chi++;
                }
            }
            return false;
        }
    };

    std::ostream& operator<<(std::ostream& os, const MidiFileMetaTextEvent& msg);

    //
    // MIDI Copyright support
    class MidiFileMetaCopyrightEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaCopyrightEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status},
            copyright_{}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(copyright_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return copyright_prefix[0] == *chi++;
                }
            }
            return false;
        }
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
        MidiFileMetaTrackEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status},
            name_{}
        {
        }
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(track_name_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return track_name_prefix[0] == *chi++;
                }
            }
            return false;
        }
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
        MidiFileMetaInstrumentEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(instrument_name_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return instrument_name_prefix[0] == *chi++;
                }
            }
            return false;
        }
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaInstrumentEvent& msg);

    //
    // MIDI Lyric support
    class MidiFileMetaLyricEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaLyricEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(lyric_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return lyric_prefix[0] == *chi++;
                }
            }
            return false;
        }
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaLyricEvent& msg);

    //
    // MIDI Marker support
    class MidiFileMetaMarkerEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaMarkerEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status}
        {
        }
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(marker_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return marker_prefix[0] == *chi++;
                }
            }
            return false;
        }
        std::ostream& text(std::ostream& os) const;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaMarkerEvent& msg);

    //
    // MIDI cue point support
    class MidiFileMetaCuePointEvent final : public MidiFileMetaStringEvent
    {
      public:
        MidiFileMetaCuePointEvent(RunningStatus& running_status)
          : MidiFileMetaStringEvent{running_status},
            cue_point_{}
        {
        }
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(cue_point_prefix) + variable_length_quantity_min_len};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return cue_point_prefix[0] == *chi++;
                }
            }
            return false;
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
        MidiFileMetaMidiChannelEvent(RunningStatus& running_status)
          : MidiFileMetaEvent{running_status},
            channel_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        std::uint16_t channel();
        void channel(std::uint16_t channel);
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(midi_channel_prefix)};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    if (midi_channel_prefix[0] == *chi++)
                    {
                        return midi_channel_prefix[1] == *chi++;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            return false;
        }
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
        MidiFileMetaEndOfTrackEvent(RunningStatus& running_status)
          : MidiFileMetaEvent{running_status}
        {
        }
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(end_of_track_prefix)};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    if (end_of_track_prefix[0] == *chi++)
                    {
                        return end_of_track_prefix[1] == *chi++;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            return false;
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
        MidiFileMetaSetTempoEvent(RunningStatus& running_status)
          : MidiFileMetaEvent{running_status},
            tempo_{60}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        void tempo(std::uint32_t tempo);
        std::uint32_t tempo() const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(tempo_prefix)};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    if (tempo_prefix[0] == *chi++)
                    {
                        return tempo_prefix[1] == *chi++;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            return false;
        }
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
        MidiFileMetaSMPTEOffsetEvent(RunningStatus& running_status)
          : MidiFileMetaEvent{running_status},
            hours_{},
            minutes_{},
            seconds_{},
            fr_{},
            ff_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(smpte_prefix)};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    if (smpte_prefix[0] == *chi++)
                    {
                        return smpte_prefix[1] == *chi++;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            return false;
        }
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
        MidiFileMetaMidiPortEvent(RunningStatus& running_status)
          : MidiFileMetaEvent{running_status},
            midiport_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(midi_port_prefix)};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    if (midi_port_prefix[0] == *chi++)
                    {
                        return midi_port_prefix[1] == *chi++;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            return false;
        }
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
        MidiFileMetaTimeSignatureEvent(RunningStatus& running_status)
          : MidiFileMetaEvent{running_status},
            numerator_{},
            denominator_{},
            clocks_per_click_{},
            thirtyseconds_per_quarter_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(time_signature_prefix)};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    if (time_signature_prefix[0] == *chi++)
                    {
                        return time_signature_prefix[1] == *chi++;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            return false;
        }
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
        MidiFileMetaKeySignatureEvent(RunningStatus& running_status)
          : MidiFileMetaEvent{running_status},
            accidentals_{},
            minor_mode_{}
        {
        }
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(key_signature_prefix)};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    if (key_signature_prefix[0] == *chi++)
                    {
                        return key_signature_prefix[1] == *chi++;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            return false;
        }
      private:
        std::int32_t accidentals_;
        bool minor_mode_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaKeySignatureEvent& msg);

    //
    // MIDI sequence specific event support
    class MidiFileMetaSequencerSpecificEvent final : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaSequencerSpecificEvent(RunningStatus& running_status)
          : MidiFileMetaEvent{running_status},
            data_()
        {}
        void consume_stream(MidiStreamIterator& midiiter) override;
        std::ostream& text(std::ostream& os) const;
        static constexpr long int prefix_len{sizeof(meta_prefix[0]) + sizeof(sequencer_specific_prefix)};
        static constexpr bool recognize(MidiStreamIterator chi, MidiStreamIterator the_end)
        {
            // page 10 of file format chapter.
            // FF 7F [variable_len] mgfID or 00 mfgid0 mfgid1 
            if (std::distance(chi, the_end) >= prefix_len)
            {
                if (MidiFileMetaEvent::recognize(chi, the_end))
                {
                    ++chi;
                    return (sequencer_specific_prefix[0] == *chi);
                }
            }
            return false;
        }
        private:
            std::vector<uint8_t> data_;
    };

    std::ostream& operator<<(std::ostream& os,
        const MidiFileMetaSequencerSpecificEvent& msg);

    //
    // The factory that creates the classes above.
    class MidiEventFactory
    {
      public:
        MidiEventFactory(MidiStreamIterator midi_end)
          : midi_end_(midi_end),
            running_status_{}
        {
        }
        MidiDelayMessagePair operator()(MidiStreamIterator& midiiter);
        static std::int64_t ticks_accumulated_;
        static std::uint32_t ticks_per_whole_;
      private:
        MidiStreamIterator midi_end_;
        RunningStatus running_status_;
    };

    // Print the text version of an event in LAZY mode.
    class PrintLazyTrack
    {
      public:
        PrintLazyTrack(
            MidiDelayMessagePairs& midi_delay_message_pairs,
            const rational::RhythmRational quantum,
            std::uint32_t ticksperquarter)
          : channel_{0},
            tied_list_{},
            midi_delay_message_pairs_{midi_delay_message_pairs},
            quantum_{quantum},
            ticksperquarter_{ticksperquarter}
        {
            ticks_of_note_on();
            ticks_to_next_event();
            ticks_to_next_note_on();
            insert_rests();
            wholes_of_note_on();
            wholes_to_next_event();
        }
      private:
        void print(std::ostream& , MidiDelayMessagePair& );
        void ticks_of_note_on();
        void ticks_to_next_event();
        void ticks_to_next_note_on();
        //
        // traverse the list of MIDI messages and insert rest events
        // representing the delay after each event when there is no sounding note.
        void insert_rests();
        void wholes_of_note_on();
        void wholes_to_next_event();

        MidiStreamAtom channel_;
        static int dynamic_;
        static bool lazy_;
        static std::string lazy_string(bool lazy);
        std::list<MidiChannelVoiceNoteOnMessage> tied_list_;
        MidiDelayMessagePairs& midi_delay_message_pairs_;
        rational::RhythmRational quantum_;
        std::uint32_t ticksperquarter_;

        friend std::ostream& operator<<(std::ostream&, PrintLazyTrack& );
    };
    std::ostream& operator<<(std::ostream& , PrintLazyTrack& );
}

#endif // MIDI_EVENTS
