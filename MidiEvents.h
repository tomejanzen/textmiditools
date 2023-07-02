//
// TextMIDITools Version 1.0.61
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
// 2020-07-15 fix crash.  byte counting in MidiEvents.h
//
#if !defined(MIDIEVENTS_H)
#    define  MIDIEVENTS_H

#include <cstdint>
#include <cstdlib>

#include <bitset>
#include <iomanip>
#include <map>
#include <string>
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

    using KeySignatureMap = std::map<std::pair<std::int32_t, bool>, std::string_view>;
    extern KeySignatureMap key_signature_map;

    constexpr std::int64_t variable_len_value(midi::MidiStreamIterator& midiiter);

    //
    // A base class for MIDI events, such as key events.
    // This is a inherit implementation design.
    // Meant to be a bridge pattern.
    // MidiEventsABC is abstract base.
    // MidiEventsImpl has some musical time values.
    // MidiEvent has a member of MidiEventImpl and forwards calls to it.
    class MidiEventABC
    {
      public:
        MidiEventABC()
        {
        }

        // Rule of 5: If you delete or declare any of assign, copy, move,
        // move copy, d-tor, then delete or declare all of them.
        MidiEventABC(const MidiEventABC& ) = default;
        MidiEventABC(MidiEventABC&& ) = default;
        MidiEventABC& operator=(const MidiEventABC& ) = default;
        MidiEventABC& operator=(MidiEventABC&& ) = default;
        virtual ~MidiEventABC() = default;
        //
        // Write the textmidi version of a MIDI event to os.
        virtual std::ostream& text(std::ostream& ) const = 0;
    };

    class MidiEventImpl
    {
      public:
        MidiEventImpl()
          : ticks_accumulated_{},
            ticks_to_next_event_{},
            ticks_to_next_note_on_{},
            wholes_to_next_event_{}
        {
        }

        //
        // Report the accumulated number of ticks so far in this track.
        constexpr std::int64_t ticks_accumulated() const;
        void ticks_accumulated(std::int64_t );
        // Report the number of ticks before the next event.
        constexpr std::int64_t ticks_to_next_event() const;
        void ticks_to_next_event(std::int64_t );
        // Report the number of ticks before the next note-on.
        constexpr std::int64_t ticks_to_next_note_on() const;
        void ticks_to_next_note_on(std::int64_t );
        // Report the number of whole notes before the next event.
        constexpr rational::RhythmRational wholes_to_next_event() const;
        void wholes_to_next_event(const rational::RhythmRational& );
        void reduce();
      private:
        //
        // Interpret the bytes of the MIDI binary stream and save
        // the data in the class.
        std::int64_t ticks_accumulated_;
        std::int64_t ticks_to_next_event_;
        std::int64_t ticks_to_next_note_on_;
        rational::RhythmRational wholes_to_next_event_;
    };

    class MidiEvent : MidiEventABC
    {
      public:
        MidiEvent()
          : midi_event_impl_{}
        {
        }

        // Rule of 5: If you delete or declare any of assign, copy, move,
        // move copy, d-tor, then delete or declare all of them.
        MidiEvent(const MidiEvent& ) = default;
        MidiEvent(MidiEvent&& ) = default;
        MidiEvent& operator=(const MidiEvent& ) = default;
        MidiEvent& operator=(MidiEvent&& ) = default;
        virtual ~MidiEvent() = default;
        //
        // Write the textmidi version of a MIDI event to os.
        virtual std::ostream& text(std::ostream& ) const = 0;
        //
        // Report the accumulated number of ticks so far in this track.
        constexpr std::int64_t ticks_accumulated() const;
        void ticks_accumulated(std::int64_t );
        // Report the number of ticks before the next event.
        constexpr std::int64_t ticks_to_next_event() const;
        void ticks_to_next_event(std::int64_t );
        // Report the number of ticks before the next note-on.
        constexpr std::int64_t ticks_to_next_note_on() const;
        void ticks_to_next_note_on(std::int64_t );
        // Report the number of whole notes before the next event.
        constexpr rational::RhythmRational wholes_to_next_event() const;
        void wholes_to_next_event(const rational::RhythmRational& );
      private:
        MidiEventImpl midi_event_impl_;
        //
        // Interpret the bytes of the MIDI binary stream and save
        // the data in the class.
        virtual void consume_stream(midi::MidiStreamIterator& ) = 0;
    };

    std::ostream& operator<<(std::ostream& , const MidiEvent& );

    using MidiDelayEventPair = std::pair<std::int64_t, std::shared_ptr<MidiEvent> >;
    std::ostream& operator<<(std::ostream& , const MidiDelayEventPair& );

    using MidiDelayEventPairs = std::vector<MidiDelayEventPair>;

    class MidiSysExEvent final : public MidiEvent
    {
      public:
        explicit MidiSysExEvent(midi::MidiStreamIterator& midiiter)
            : MidiEvent{},
              data_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent>
            recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::vector<midi::MidiStreamAtom> data_;
    };

    // I don't know what this is; maybe it's in spec 1.1.
    // It's in the classical archives files.
    // It's an F7, length following this, incl. another f7, dah dit dah F7.
    // But the final F7 is optional and vendor-defined.
    class MidiSysExRawEvent final : public MidiEvent
    {
      public:
        explicit MidiSysExRawEvent(midi::MidiStreamIterator& midiiter)
            : MidiEvent{},
              data_{}
        {
            consume_stream(midiiter);
        }

        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::vector<midi::MidiStreamAtom> data_;
    };

    std::ostream& operator<<(std::ostream& , const MidiSysExRawEvent& );

    //
    // A base class for MIDI Meta Events.
    class MidiFileMetaEvent : public MidiEvent
    {
      public:
        explicit MidiFileMetaEvent()
        { }
        static const long prefix_len;
        static bool recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
    };

    //
    // MIDI Meta Sequence
    class MidiFileMetaSequenceEvent final : public MidiFileMetaEvent
    {
      public:
          explicit MidiFileMetaSequenceEvent(midi::MidiStreamIterator& midiiter)
            : MidiFileMetaEvent{},
              sequence_number_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        void sequence_number(std::uint16_t sequence_number);
        std::uint16_t sequence_number() const;
        static const long prefix_len;
        // Meant to recognize 0xFF 0x00 0x02
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::uint16_t sequence_number_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaSequenceEvent& );

    //
    // MIDI Meta Unknown event
    class MidiFileMetaUnknownEvent final : public MidiFileMetaEvent
    {
      public:
          explicit MidiFileMetaUnknownEvent(midi::MidiStreamIterator& midiiter)
            : MidiFileMetaEvent{},
              meta_code_{},
              data_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        // Meant to recognize 0xFF 0x00 0x02
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        midi::MidiStreamAtom meta_code_;
        midi::MidiStreamVector data_;
    };

    //
    // MIDI Channel event branch in class heirachy
    class MidiFileMetaMidiChannelEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaMidiChannelEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaEvent{},
            channel_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        void channel(std::uint16_t );
        constexpr std::uint16_t channel();
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::uint16_t channel_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaMidiChannelEvent& );

    class MidiFileMetaSetTempoEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaSetTempoEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaEvent{},
            tempo_{60}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        void tempo(std::uint32_t tempo);
        std::uint32_t tempo() const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::uint32_t tempo_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaSetTempoEvent& );

    class MidiFileMetaSMPTEOffsetEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaSMPTEOffsetEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaEvent{},
            fps_{midi::smpte_24fps},
            hours_{},
            minutes_{},
            seconds_{},
            fr_{},
            ff_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::uint16_t fps_;
        std::uint16_t hours_;
        std::uint16_t minutes_;
        std::uint16_t seconds_;
        std::uint16_t fr_;
        std::uint16_t ff_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaSMPTEOffsetEvent& );

    //
    // MIDI Port event
    class MidiFileMetaMidiPortEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaMidiPortEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaEvent{},
            midiport_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::uint16_t midiport_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaMidiPortEvent& );

    class MidiFileMetaTimeSignatureEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaTimeSignatureEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaEvent{},
            numerator_{},
            denominator_{},
            clocks_per_click_{},
            thirtyseconds_per_quarter_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::uint16_t numerator_;
        std::uint16_t denominator_;
        std::uint16_t clocks_per_click_;
        std::uint16_t thirtyseconds_per_quarter_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaTimeSignatureEvent& );

    class MidiFileMetaKeySignatureEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaKeySignatureEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaEvent{},
            accidentals_{},
            minor_mode_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::int32_t accidentals_;
        bool minor_mode_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaKeySignatureEvent& );

    class MidiFileMetaXmfPatchTypeEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaXmfPatchTypeEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaEvent{},
            xmf_patch_type_(static_cast<midi::MidiStreamAtom>(midi::XmfPatchTypeEnum::GM1))
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        midi::MidiStreamAtom xmf_patch_type_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaXmfPatchTypeEvent& );

    class MidiFileMetaSequencerSpecificEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaSequencerSpecificEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaEvent{},
            data_()
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        midi::MidiStreamVector data_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaSequencerSpecificEvent& );

    class MidiFileMetaEndOfTrackEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaEndOfTrackEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaEvent{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaEndOfTrackEvent& );

    //
    // MIDI string events creates a branch for MIDI TEXT, AUTHOR, COPYRIGHT, etc.
    //
    class MidiFileMetaStringEvent : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaStringEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaEvent{},
            str_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::string str_;
    };

    class MidiFileMetaTextEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaTextEvent(midi::MidiStreamIterator& midiiter) : MidiFileMetaStringEvent{midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaTextEvent& );

    class MidiFileMetaProgramNameEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaProgramNameEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaProgramNameEvent& );

    class MidiFileMetaDeviceNameEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaDeviceNameEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaDeviceNameEvent& );

    class MidiFileMetaText0AEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0AEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0AEvent& );

    class MidiFileMetaText0BEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0BEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0BEvent& );

    class MidiFileMetaText0CEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0CEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0CEvent& );

    class MidiFileMetaText0DEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0DEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0DEvent& );

    class MidiFileMetaText0EEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0EEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0EEvent& );

    class MidiFileMetaText0FEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0FEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0FEvent& );

    class MidiFileMetaCopyrightEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaCopyrightEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter},
            copyright_{}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        std::string copyright_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaCopyrightEvent& );

    //
    // Branch in class heirarchy for Track events
    class MidiFileMetaTrackNameEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaTrackNameEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter},
            name_{}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
      private:
        std::string name_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaTrackNameEvent& );

    class MidiFileMetaInstrumentEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaInstrumentEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
        std::ostream& text(std::ostream& ) const;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaInstrumentEvent& );

    class MidiFileMetaLyricEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaLyricEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
        std::ostream& text(std::ostream& ) const;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaLyricEvent& );

    class MidiFileMetaMarkerEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaMarkerEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter}
        {
        }
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
        std::ostream& text(std::ostream& ) const;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaMarkerEvent& );

    class MidiFileMetaCuePointEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaCuePointEvent(midi::MidiStreamIterator& midiiter)
          : MidiFileMetaStringEvent{midiiter},
            cue_point_{}
        {
        }
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end);
        std::ostream& text(std::ostream& ) const;
      private:
        std::string cue_point_;
    };

    std::ostream& operator<<(std::ostream& , const MidiFileMetaCuePointEvent& );

    //
    // Base class for voice events
    class MidiChannelVoiceModeEvent : public MidiEvent
    {
      public:
        explicit MidiChannelVoiceModeEvent(const midi::RunningStatus& running_status)
          : MidiEvent{},
            running_status_{running_status},
            local_status_{},
            channel_{static_cast<midi::MidiStreamAtom>(running_status.channel() + 1)}
        {
        }
        midi::MidiStreamAtom channel() const;
        void channel(midi::MidiStreamAtom );
        constexpr midi::RunningStatus& running_status();
        const midi::RunningStatus& running_status() const;
        const midi::RunningStatus& local_status() const;
        void local_status(const midi::RunningStatus& );
        midi::RunningStatus& local_status();
        static const long prefix_len;
        static bool recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end, midi::RunningStatus& running_status);
      private:
        midi::RunningStatus running_status_;
        midi::RunningStatus local_status_;
        midi::MidiStreamAtom channel_;
    };

    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceModeEvent& );

    //
    // Base class for MIDI voice note events, such as note on and note off.
    class MidiChannelVoiceNoteEvent : public MidiChannelVoiceModeEvent
    {
      public:
        MidiChannelVoiceNoteEvent(const midi::RunningStatus& running_status, std::shared_ptr<bool> prefer_sharp, midi::MidiStreamIterator& midiiter)
          : MidiChannelVoiceModeEvent{running_status},
            key_{},
            velocity_{},
            key_string_{},
            wholes_to_noteoff_{},
            prefer_sharp_{prefer_sharp}
        {
            consume_stream(midiiter);
        }
        // needed to make rests and have rests in the same hierarchy.
        MidiChannelVoiceNoteEvent(const midi::RunningStatus& running_status, std::shared_ptr<bool> prefer_sharp)
          : MidiChannelVoiceModeEvent{running_status},
            key_{},
            velocity_{},
            key_string_{},
            wholes_to_noteoff_{},
            prefer_sharp_{prefer_sharp}
        {
        }
        MidiChannelVoiceNoteEvent(const MidiChannelVoiceNoteEvent& running_status) = default;
        MidiChannelVoiceNoteEvent& operator=(const MidiChannelVoiceNoteEvent& running_status) = default;
        constexpr midi::MidiStreamAtom key() const;
        void key(midi::MidiStreamAtom key);
        void key_string(std::string_view );
        std::string key_string() const;
        void velocity(midi::MidiStreamAtom );
        constexpr midi::MidiStreamAtom velocity() const;
        std::ostream& text(std::ostream& ) const;
        constexpr bool operator==(const MidiChannelVoiceNoteEvent& ) const;
        void wholes_to_noteoff(const rational::RhythmRational& );
        rational::RhythmRational wholes_to_noteoff() const;
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        midi::MidiStreamAtom key_;
        midi::MidiStreamAtom velocity_;
        std::string key_string_;
        rational::RhythmRational wholes_to_noteoff_;
        std::shared_ptr<bool> prefer_sharp_;
    };

    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteEvent& );

    class MidiChannelVoiceNoteOnEvent final : public MidiChannelVoiceNoteEvent
    {
      public:
        MidiChannelVoiceNoteOnEvent(const midi::RunningStatus& running_status, std::uint32_t ticks_per_whole, std::shared_ptr<bool> prefer_sharp, midi::MidiStreamIterator& midiiter)
          : MidiChannelVoiceNoteEvent(running_status, prefer_sharp, midiiter),
            ticks_per_whole_{ticks_per_whole},
            ticks_to_noteoff_{},
            ticks_past_noteoff_{},
            wholes_past_noteoff_{}
        {
        }
        MidiChannelVoiceNoteOnEvent(const MidiChannelVoiceNoteOnEvent& ) = default;
        MidiChannelVoiceNoteOnEvent& operator=(const MidiChannelVoiceNoteOnEvent& ) = default;
        std::ostream& text(std::ostream& ) const;
        void ticks_to_noteoff(std::int64_t );
        constexpr std::int64_t ticks_to_noteoff() const;
        void ticks_past_noteoff(std::int64_t );
        constexpr std::int64_t ticks_past_noteoff() const;
        void wholes_past_noteoff(const rational::RhythmRational& );
        constexpr rational::RhythmRational wholes_past_noteoff() const;
        constexpr std::uint32_t ticks_per_whole() const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter,
            midi::MidiStreamIterator the_end, midi::RunningStatus& , std::shared_ptr<bool> prefer_sharp, std::uint32_t ticks_per_whole);
      private:
        std::uint32_t ticks_per_whole_;
        std::int64_t ticks_to_noteoff_;
        std::int64_t ticks_past_noteoff_;
        rational::RhythmRational wholes_past_noteoff_;
    };

    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteOnEvent& );

    class MidiChannelVoiceNoteOffEvent final : public MidiChannelVoiceNoteEvent
    {
      public:
        MidiChannelVoiceNoteOffEvent(midi::RunningStatus& running_status, std::shared_ptr<bool> prefer_sharp, midi::MidiStreamIterator& midiiter)
          : MidiChannelVoiceNoteEvent{running_status, prefer_sharp, midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter,
            midi::MidiStreamIterator the_end, midi::RunningStatus& , std::shared_ptr<bool> prefer_sharp);
    };

    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteOffEvent& );

    class MidiChannelVoiceProgramChangeEvent final : public MidiChannelVoiceModeEvent
    {
      public:
        explicit MidiChannelVoiceProgramChangeEvent(const midi::RunningStatus& running_status, midi::MidiStreamIterator& midiiter)
          : MidiChannelVoiceModeEvent{running_status},
            program_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        void program(midi::MidiStreamAtom );
        constexpr midi::MidiStreamAtom program() const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end, midi::RunningStatus& );
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        midi::MidiStreamAtom program_;
    };

    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceProgramChangeEvent& );

    class MidiChannelVoicePitchBendEvent final : public MidiChannelVoiceModeEvent
    {
      public:
        explicit MidiChannelVoicePitchBendEvent(const midi::RunningStatus& running_status, midi::MidiStreamIterator& midiiter)
          : MidiChannelVoiceModeEvent{running_status},
            pitch_wheel_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        void pitch_wheel(midi::MidiStreamAtom );
        midi::MidiStreamAtom pitch_wheel() const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end, midi::RunningStatus& );
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        std::uint16_t pitch_wheel_;
    };

    std::ostream& operator<<(std::ostream& , const MidiChannelVoicePitchBendEvent& );

    class MidiChannelVoiceControlChangeEvent final : public MidiChannelVoiceModeEvent
    {
      public:
        explicit MidiChannelVoiceControlChangeEvent(const midi::RunningStatus& running_status, midi::MidiStreamIterator& midiiter)
          : MidiChannelVoiceModeEvent(running_status),
            id_{},
            value_{}
        {
            consume_stream(midiiter);
        }
        std::ostream& text(std::ostream& ) const;
        midi::MidiStreamAtom id() const;
        void id(midi::MidiStreamAtom );
        midi::MidiStreamAtom value() const;
        void value(midi::MidiStreamAtom );
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end, midi::RunningStatus& );
      private:
        void consume_stream(midi::MidiStreamIterator& midiiter) override;
        midi::MidiStreamAtom id_;
        midi::MidiStreamAtom value_;
    };

    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceControlChangeEvent& );

    class MidiChannelVoiceChannelPressureEvent final : public MidiChannelVoiceModeEvent
    {
      public:
        explicit MidiChannelVoiceChannelPressureEvent(const midi::RunningStatus& running_status, midi::MidiStreamIterator& midiiter)
          : MidiChannelVoiceModeEvent(running_status),
            pressure_{}
        {
            consume_stream(midiiter);
        }

        std::ostream& text(std::ostream& ) const;
        void pressure(midi::MidiStreamAtom );
        midi::MidiStreamAtom pressure() const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end, midi::RunningStatus& );
      private:
        void consume_stream(midi::MidiStreamIterator& ) override;
        midi::MidiStreamAtom pressure_;
    };

    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceChannelPressureEvent& );

    class MidiChannelVoicePolyphonicKeyPressureEvent final : public MidiChannelVoiceNoteEvent
    {
      public:
        MidiChannelVoicePolyphonicKeyPressureEvent(const midi::RunningStatus& running_status, std::shared_ptr<bool> prefer_sharp, midi::MidiStreamIterator& midiiter)
          : MidiChannelVoiceNoteEvent{running_status, prefer_sharp, midiiter}
        {
        }
        std::ostream& text(std::ostream& ) const;
        static const long prefix_len;
        static std::shared_ptr<MidiEvent> recognize(midi::MidiStreamIterator& midiiter, midi::MidiStreamIterator the_end, midi::RunningStatus& , std::shared_ptr<bool> prefer_sharp);
    };

    std::ostream& operator<<(std::ostream& , const MidiChannelVoicePolyphonicKeyPressureEvent& );

    //
    // Rest Event.  This is not in the MIDI spec;
    // it is a fiction specifically crafted for textmidi.
    class MidiChannelVoiceNoteRestEvent final : public MidiChannelVoiceNoteEvent
    {
      public:
        explicit MidiChannelVoiceNoteRestEvent(midi::RunningStatus& running_status, std::uint32_t ticks_per_whole,
            std::shared_ptr<bool> prefer_sharp = std::shared_ptr<bool>{}, midi::MidiStreamIterator midiiter = midi::MidiStreamIterator{})
          : MidiChannelVoiceNoteEvent{running_status, prefer_sharp},
            ticks_per_whole_{ticks_per_whole}
        {
            key_string("R");
        }
        MidiChannelVoiceNoteRestEvent(const MidiChannelVoiceNoteRestEvent& ) = default;
        MidiChannelVoiceNoteRestEvent& operator=(const MidiChannelVoiceNoteRestEvent& ) = default;
        std::ostream& text(std::ostream& ) const;
      private:
        std::uint32_t ticks_per_whole_;
    };

    //
    // The factory that creates the classes above.
    class MidiEventFactory
    {
      public:
        explicit MidiEventFactory(midi::MidiStreamIterator midi_end, std::uint32_t ticks_per_whole)
          : ticks_per_whole_(ticks_per_whole),
            midi_end_(midi_end),
            running_status_{},
            prefer_sharp_{std::make_shared<bool>()}
        {
        }
        MidiDelayEventPair operator()(midi::MidiStreamIterator& midiiter, std::int64_t& ticks_accumulated_);
      private:
        const std::uint32_t ticks_per_whole_;
        midi::MidiStreamIterator midi_end_;
        midi::RunningStatus running_status_;
        std::shared_ptr<bool> prefer_sharp_;
    };

    // Print the text version of an event in LAZY mode.
    class PrintLazyTrack
    {
      public:
        PrintLazyTrack(
            MidiDelayEventPairs& midi_delay_event_pairs,
            const rational::RhythmRational quantum,
            std::uint32_t ticksperquarter)
          : channel_{},
            dynamic_{64},
            lazy_{},
            tied_list_{},
            midi_delay_event_pairs_{midi_delay_event_pairs},
            quantum_{quantum},
            ticksperquarter_{ticksperquarter},
            prefer_sharp_{std::make_shared<bool>()},
            running_status_{}
        {
            ticks_of_note_on();
            ticks_to_next_event();
            ticks_to_next_note_on();
            insert_rests();
            wholes_of_note_on();
            wholes_to_next_event();
        }
      private:
        void print(std::ostream& , MidiDelayEventPair& );
        void ticks_of_note_on();
        void ticks_to_next_event();
        void ticks_to_next_note_on();
        //
        // traverse the list of MIDI events and insert rest events
        // representing the delay after each event when there is no sounding note.
        void insert_rests();
        void wholes_of_note_on();
        void wholes_to_next_event();

        midi::MidiStreamAtom channel_;
        /* static */ int dynamic_;
        /* static */ bool lazy_;
        constexpr std::string_view lazy_string(bool lazy);
        std::list<MidiChannelVoiceNoteOnEvent> tied_list_;
        MidiDelayEventPairs& midi_delay_event_pairs_;
        rational::RhythmRational quantum_;
        std::uint32_t ticksperquarter_;
        std::shared_ptr<bool> prefer_sharp_;
        midi::RunningStatus running_status_;

        friend std::ostream& operator<<(std::ostream&, PrintLazyTrack& );
    };
    std::ostream& operator<<(std::ostream& , PrintLazyTrack& );
}

#endif // MIDI_EVENTS
