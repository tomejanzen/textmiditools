// TextMIDITools Version 1.0.92
//
// textmidi 1.0.6
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// 2021-03-02 don't write zero delays
//
// 2020-04-03 rewrite mididisasm Thomas Janzen
//
// 2020-07-15 fix crash.  byte counting in MidiEvents.h
//
#if !defined(MIDIEVENTS_H)
#    define  MIDIEVENTS_H

#include <cstdint>
#include <cstdlib>

#include <bitset>
#include <iomanip>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/preprocessor.hpp>

#include "RhythmRational.h"
#include "Midi.h"
#include "MIDIKeyString.h"

namespace textmidi
{
    //
    // channel_0_to_1
    // transforms a MIDI binary channel in (0..15).
    // to a textmidi value (usu. channel or program) in (1..16)
    midi::MidiStreamAtom channel_0_to_1(midi::MidiStreamAtom channel) noexcept;

    using KeySignatureMap = std::map<std::pair<std::int32_t, bool>,
        std::string_view>;
    using std::cout;
    extern KeySignatureMap key_signature_map;

    std::tuple<midi::MidiStreamRange, int64_t>
        variable_len_value(midi::MidiStreamRange midi_stream_tail) noexcept;

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
        MidiEventABC() = default;

        // Rule of 5: If you delete or declare any of assign, copy, move,
        // move copy, d-tor, then delete or declare all of them.
        MidiEventABC(const MidiEventABC& ) = default;
        MidiEventABC(MidiEventABC&& ) = default;
        MidiEventABC& operator=(const MidiEventABC& ) = default;
        MidiEventABC& operator=(MidiEventABC&& ) = default;
        virtual ~MidiEventABC() = default;
        virtual std::ostream& print(std::ostream& ) const = 0;
    };

    class MidiEventImpl
    {
      public:
        MidiEventImpl() = default;

        //
        // Report the accumulated number of ticks so far in this track.
        constexpr std::int64_t ticks_accumulated() const noexcept;
        void ticks_accumulated(std::int64_t ) noexcept;
        // Report the number of ticks before the next event.
        constexpr std::int64_t ticks_to_next_event() const noexcept;
        void ticks_to_next_event(std::int64_t ) noexcept;
        // Report the number of ticks before the next note-on.
        constexpr std::int64_t ticks_to_a_note_start() const noexcept;
        void ticks_to_a_note_start(std::int64_t ) noexcept;
        // Report the number of whole notes before the next event.
        constexpr rational::RhythmRational wholes_to_next_event() const
            noexcept;
        void wholes_to_next_event(const rational::RhythmRational& ) noexcept;
        void reduce() noexcept;
        std::int32_t events_to_next_note_on() const noexcept;
        void events_to_next_note_on(std::int32_t events_to_next_note_on) noexcept;
      private:
        //
        // Interpret the bytes of the MIDI binary stream and save
        // the data in the class.
        std::int64_t ticks_accumulated_{};
        std::int64_t ticks_to_next_event_{};
        std::int64_t ticks_to_a_note_start_{};
        rational::RhythmRational wholes_to_next_event_{};
        std::int32_t events_to_next_note_on_{};
    };

    class MidiEvent : public MidiEventABC
    {
      public:
        MidiEvent() = default;

        // Rule of 5: If you delete or declare any of assign, copy, move,
        // move copy, d-tor, then delete or declare all of them.
        MidiEvent(const MidiEvent& ) = default;
        MidiEvent(MidiEvent&& ) = default;
        MidiEvent& operator=(const MidiEvent& ) = default;
        MidiEvent& operator=(MidiEvent&& ) = default;
        virtual ~MidiEvent() = default;
        // Report the accumulated number of ticks so far in this track.
        constexpr std::int64_t ticks_accumulated() const noexcept;
        void ticks_accumulated(std::int64_t ) noexcept;
        // Report the number of ticks before the next event.
        constexpr std::int64_t ticks_to_next_event() const noexcept;
        void ticks_to_next_event(std::int64_t ) noexcept;
        // Report the number of ticks before the next note-on.
        constexpr std::int64_t ticks_to_a_note_start() const noexcept;
        void ticks_to_a_note_start(std::int64_t ) noexcept;
        // Report the number of whole notes before the next event.
        constexpr rational::RhythmRational wholes_to_next_event() const
            noexcept;
        void wholes_to_next_event(const rational::RhythmRational& ) noexcept;
        std::int32_t events_to_next_note_on() const noexcept;
        void events_to_next_note_on(std::int32_t events_to_next_note_on) noexcept;
        virtual midi::MidiStreamRange
            consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept = 0;
      private:
        MidiEventImpl midi_event_impl_{};
      friend std::ostream& operator<<(std::ostream& , const MidiEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiEvent& );

    using DelayEvent = std::pair<std::int64_t, std::shared_ptr<MidiEvent> >;
    using OptionalEvent = std::optional<std::shared_ptr<MidiEvent>>;
    using StreamEvent = std::tuple<midi::MidiStreamRange, OptionalEvent>;
    std::ostream& operator<<(std::ostream& , const DelayEvent& );

    using DelayEvents = std::list<DelayEvent>;

    class MidiSysExEvent final : public MidiEvent
    {
      public:
        explicit MidiSysExEvent() noexcept
            : MidiEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        midi::MidiStreamVector data_{};
      friend std::ostream& operator<<(std::ostream& , const MidiSysExEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiSysExEvent& );


    // I don't know what this is; maybe it's in spec 1.1.
    // It's in the classical archives files.
    // It's an F7, length following this, incl. another f7, dah dit dah F7.
    // But the final F7 is optional and vendor-defined.
    class MidiSysExRawEvent final : public MidiEvent
    {
      public:
        explicit MidiSysExRawEvent() noexcept
            : MidiEvent{}
        {
        }

        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;

        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        midi::MidiStreamVector data_{};
      friend std::ostream& operator<<(std::ostream& , const MidiSysExRawEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiSysExRawEvent& );

    //
    // A base class for MIDI Meta Events.
    class MidiFileMetaEvent : public MidiEvent
    {
      public:
        MidiFileMetaEvent() = default;
        static bool recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
    };

    //
    // MIDI Meta Sequence
    class MidiFileMetaSequenceEvent final : public MidiFileMetaEvent
    {
      public:
          explicit MidiFileMetaSequenceEvent() noexcept
            : MidiFileMetaEvent{}
        {
        }
        void sequence_number(std::uint16_t sequence_number) noexcept;
        std::uint16_t sequence_number() const noexcept;
        // Meant to recognize 0xFF 0x00 0x02
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::uint16_t sequence_number_{};
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaSequenceEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaSequenceEvent& );

    //
    // MIDI Meta Unknown event
    class MidiFileMetaUnknownEvent final : public MidiFileMetaEvent
    {
      public:
          explicit MidiFileMetaUnknownEvent() noexcept
            : MidiFileMetaEvent{}
        {
        }
        // Meant to recognize 0xFF 0x00 0x02
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        midi::MidiStreamAtom meta_code_{};
        midi::MidiStreamVector data_{};
      friend std::ostream& operator<<(std::ostream& os, const MidiFileMetaUnknownEvent& msg);
    };
    std::ostream& operator<<(std::ostream& os, const MidiFileMetaUnknownEvent& msg);

    //
    // MIDI Channel event branch in class heirachy
    class MidiFileMetaMidiChannelEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaMidiChannelEvent() noexcept
          : MidiFileMetaEvent{},
            channel_{}
        {
        }
        void channel(std::uint16_t ) noexcept;
        constexpr std::uint16_t channel() noexcept;
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::uint16_t channel_;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaMidiChannelEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaMidiChannelEvent& );

    // OK
    class MidiFileMetaSetTempoEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaSetTempoEvent() noexcept
          : MidiFileMetaEvent{},
            tempo_{60}
        {
        }
        void tempo(std::uint32_t tempo) noexcept;
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::uint32_t tempo_;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaSetTempoEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaSetTempoEvent& );

    // OK
    class MidiFileMetaSMPTEOffsetEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaSMPTEOffsetEvent() noexcept
          : MidiFileMetaEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::uint16_t fps_{midi::smpte_24fps};
        std::uint16_t hours_{};
        std::uint16_t minutes_{};
        std::uint16_t seconds_{};
        std::uint16_t fr_{};
        std::uint16_t ff_{};
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaSMPTEOffsetEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaSMPTEOffsetEvent& );

    //
    // MIDI Port event
    // OK
    class MidiFileMetaMidiPortEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaMidiPortEvent() noexcept
          : MidiFileMetaEvent{},
            midiport_{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::uint16_t midiport_;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaMidiPortEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaMidiPortEvent& );

    // OK
    class MidiFileMetaTimeSignatureEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaTimeSignatureEvent() noexcept
          : MidiFileMetaEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::uint16_t numerator_{};
        std::uint16_t denominator_{};
        std::uint16_t clocks_per_click_{};
        std::uint16_t thirtyseconds_per_quarter_{};
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaTimeSignatureEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaTimeSignatureEvent& );

    // OK
    class MidiFileMetaKeySignatureEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaKeySignatureEvent() noexcept
          : MidiFileMetaEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::int32_t accidentals_{};
        bool minor_mode_{};
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaKeySignatureEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaKeySignatureEvent& );

    class MidiFileMetaXmfPatchTypeEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaXmfPatchTypeEvent() noexcept
          : MidiFileMetaEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        midi::MidiStreamAtom xmf_patch_type_{
            static_cast<midi::MidiStreamAtom>(midi::XmfPatchTypeEnum::GM1)};
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaXmfPatchTypeEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaXmfPatchTypeEvent& );

    // OK
    class MidiFileMetaSequencerSpecificEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaSequencerSpecificEvent() noexcept
          : MidiFileMetaEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      private:
        midi::MidiStreamVector data_{};
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaSequencerSpecificEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaSequencerSpecificEvent& );

    class MidiFileMetaEndOfTrackEvent final : public MidiFileMetaEvent
    {
      public:
        explicit MidiFileMetaEndOfTrackEvent() noexcept
          : MidiFileMetaEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaEndOfTrackEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaEndOfTrackEvent& );

    //
    // MIDI string events creates a branch
    // for MIDI TEXT, AUTHOR, COPYRIGHT, etc.
    //
    class MidiFileMetaStringEvent : public MidiFileMetaEvent
    {
      public:
        MidiFileMetaStringEvent() noexcept
          : MidiFileMetaEvent{}
        {
        }
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const override;
      private:
        std::string str_{};
      friend std::ostream& operator<<(std::ostream& os, const MidiFileMetaStringEvent& msg);
    };
    std::ostream& operator<<(std::ostream& os, const MidiFileMetaStringEvent& msg);

    class MidiFileMetaTextEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaTextEvent() noexcept : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaTextEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaTextEvent& );

    class MidiFileMetaProgramNameEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaProgramNameEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaProgramNameEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaProgramNameEvent& );

    class MidiFileMetaDeviceNameEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaDeviceNameEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaDeviceNameEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaDeviceNameEvent& );

    class MidiFileMetaText0AEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0AEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaText0AEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0AEvent& );

    class MidiFileMetaText0BEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0BEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaText0BEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0BEvent& );

    class MidiFileMetaText0CEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0CEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaText0CEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0CEvent& );

    class MidiFileMetaText0DEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0DEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaText0DEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0DEvent& );

    class MidiFileMetaText0EEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0EEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaText0EEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0EEvent& );

    class MidiFileMetaText0FEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaText0FEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaText0FEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaText0FEvent& );

    class MidiFileMetaCopyrightEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaCopyrightEvent() noexcept
          : MidiFileMetaStringEvent{},
            copyright_{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::string copyright_;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaCopyrightEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaCopyrightEvent& );

    //
    // Branch in class heirarchy for Track events
    // OK
    class MidiFileMetaTrackNameEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaTrackNameEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::string name_{};
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaTrackNameEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaTrackNameEvent& );

    class MidiFileMetaInstrumentEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaInstrumentEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaInstrumentEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaInstrumentEvent& );

    class MidiFileMetaLyricEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaLyricEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaLyricEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaLyricEvent& );

    class MidiFileMetaMarkerEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaMarkerEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaMarkerEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaMarkerEvent& );

    class MidiFileMetaCuePointEvent final : public MidiFileMetaStringEvent
    {
      public:
        explicit MidiFileMetaCuePointEvent() noexcept
          : MidiFileMetaStringEvent{}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail) noexcept;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::string cue_point_{};
      friend std::ostream& operator<<(std::ostream& , const MidiFileMetaCuePointEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiFileMetaCuePointEvent& );

    //
    // Base class for voice events
    class MidiChannelVoiceModeEvent : public MidiEvent
    {
      public:
        explicit MidiChannelVoiceModeEvent(
            const midi::RunningStatusStandard& running_status) noexcept
          : MidiEvent{},
            running_status_{running_status},
            channel_{static_cast<midi::MidiStreamAtom>
                (running_status.channel() + 1)}
        {
        }
        midi::MidiStreamAtom channel() const noexcept;
        void channel(midi::MidiStreamAtom ) noexcept;
        constexpr midi::RunningStatusStandard& running_status() noexcept
        {
            return running_status_;
        }
        const midi::RunningStatusStandard& running_status() const noexcept;
        const midi::RunningStatusStandard& local_status() const noexcept;
        void local_status(const midi::RunningStatusStandard& ) noexcept;
        midi::RunningStatusStandard& local_status() noexcept;
        static bool recognize(midi::MidiStreamRange  midi_stream_tail,
            const midi::RunningStatusStandard& running_status) noexcept;
      private:
        midi::RunningStatusStandard running_status_;
        midi::RunningStatusStandard local_status_{};
        midi::MidiStreamAtom channel_;
        bool operator==(const MidiChannelVoiceModeEvent& mcvme) const
        {
            return mcvme.local_status().running_status_value()
                == this->local_status().running_status_value();
        }
    };

    //
    // Base class for MIDI voice note events, such as note on and note off.
    class MidiChannelVoiceNoteEvent : public MidiChannelVoiceModeEvent
    {
      public:
        explicit MidiChannelVoiceNoteEvent(
            const midi::RunningStatusStandard& running_status,
            std::uint32_t ticks_per_whole, std::shared_ptr<bool> prefer_sharp)
          : MidiChannelVoiceModeEvent{running_status},
            ticks_per_whole_{ticks_per_whole},
            prefer_sharp_{prefer_sharp}
        {
        }
        MidiChannelVoiceNoteEvent(const MidiChannelVoiceNoteEvent& ) = default;
        MidiChannelVoiceNoteEvent&
            operator=(const MidiChannelVoiceNoteEvent& ) = default;
        constexpr midi::MidiStreamAtom key() const noexcept;
        void key(midi::MidiStreamAtom key) noexcept;
        void key_string(std::string_view ) noexcept;
        std::string key_string() const noexcept;
        void velocity(midi::MidiStreamAtom ) noexcept;
        constexpr midi::MidiStreamAtom velocity() const noexcept;
        constexpr bool operator==(const MidiChannelVoiceNoteEvent& ) const
            noexcept;
        void wholes_to_noteoff(const rational::RhythmRational& ) noexcept;
        rational::RhythmRational wholes_to_noteoff() const noexcept;
        constexpr std::uint32_t ticks_per_whole() const noexcept;
        constexpr bool matched() const noexcept
        {
            return matched_;
        }
        void set_matched() noexcept
        {
            matched_ = true;
        }
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept override;
        std::ostream& print(std::ostream& os) const override;
      private:
        std::uint32_t ticks_per_whole_;
        midi::MidiStreamAtom key_{};
        midi::MidiStreamAtom velocity_{};
        std::string key_string_{};
        rational::RhythmRational wholes_to_noteoff_{};
        std::shared_ptr<bool> prefer_sharp_;
        bool matched_{};
      friend std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteEvent& );

    class MidiChannelVoiceNoteOffEvent;

    // OK
    class MidiChannelVoiceNoteOnEvent final : public MidiChannelVoiceNoteEvent
    {
      public:
        explicit MidiChannelVoiceNoteOnEvent(
            const midi::RunningStatusStandard& running_status,
            std::uint32_t ticks_per_whole, std::shared_ptr<bool> prefer_sharp)
          : MidiChannelVoiceNoteEvent
            (running_status, ticks_per_whole, prefer_sharp)
        {
        }
        MidiChannelVoiceNoteOnEvent(const MidiChannelVoiceNoteOnEvent& )
            = default;
        MidiChannelVoiceNoteOnEvent& operator=(const MidiChannelVoiceNoteOnEvent& ) = default;
        void ticks_to_noteoff(std::int64_t ) noexcept;
        constexpr std::int64_t ticks_to_noteoff() const noexcept;

        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status,
            std::shared_ptr<bool> prefer_sharp, uint32_t ticks_per_whole) noexcept;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::int64_t ticks_to_noteoff_{std::numeric_limits<std::int64_t>().max()};
      friend std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteOnEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteOnEvent& );

    class MidiChannelVoiceNoteOffEvent final : public MidiChannelVoiceNoteEvent
    {
      public:
        explicit MidiChannelVoiceNoteOffEvent(
            midi::RunningStatusStandard& running_status,
            std::uint32_t ticks_per_whole,
            std::shared_ptr<bool> prefer_sharp) noexcept
          : MidiChannelVoiceNoteEvent{running_status,
            ticks_per_whole, prefer_sharp}
        {
        }
        static StreamEvent recognize(midi::MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status, std::shared_ptr<bool> prefer_sharp, uint32_t ticks_per_whole) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteOffEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteOffEvent& );

    // OK
    class MidiChannelVoiceProgramChangeEvent final
        : public MidiChannelVoiceModeEvent
    {
      public:
        explicit MidiChannelVoiceProgramChangeEvent(
           const midi::RunningStatusStandard& running_status) noexcept
          : MidiChannelVoiceModeEvent{running_status}
        {
        }
        void program(midi::MidiStreamAtom ) noexcept;
        constexpr midi::MidiStreamAtom program() const noexcept;
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept final;
        std::ostream& print(std::ostream& os) const final;
      private:
        midi::MidiStreamAtom program_{};
      friend std::ostream& operator<<(std::ostream& , const MidiChannelVoiceProgramChangeEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceProgramChangeEvent& );

    class MidiChannelVoicePitchBendEvent final
        : public MidiChannelVoiceModeEvent
    {
      public:
        explicit MidiChannelVoicePitchBendEvent(
            const midi::RunningStatusStandard& running_status) noexcept
          : MidiChannelVoiceModeEvent{running_status}
        {
        }
        static StreamEvent recognize(midi::MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept final;
        std::ostream& print(std::ostream& os) const final;
      private:
        std::uint16_t pitch_wheel_{};
      friend std::ostream& operator<<(std::ostream& , const MidiChannelVoicePitchBendEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiChannelVoicePitchBendEvent& );

    // OK
    class MidiChannelVoiceControlChangeEvent final
        : public MidiChannelVoiceModeEvent
    {
      public:
        explicit MidiChannelVoiceControlChangeEvent(
            const midi::RunningStatusStandard& running_status) noexcept
          : MidiChannelVoiceModeEvent(running_status)
        {
        }
        void id(midi::MidiStreamAtom ) noexcept;
        midi::MidiStreamAtom value() const noexcept;
        void value(midi::MidiStreamAtom ) noexcept;
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept final;
        std::ostream& print(std::ostream& os) const final;
      private:
        midi::MidiStreamAtom id_{};
        midi::MidiStreamAtom value_{};
      friend std::ostream& operator<<(std::ostream& , const MidiChannelVoiceControlChangeEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceControlChangeEvent& );

    class MidiChannelVoiceChannelPressureEvent final
        : public MidiChannelVoiceModeEvent
    {
      public:
        explicit MidiChannelVoiceChannelPressureEvent(
            const midi::RunningStatusStandard& running_status) noexcept
          : MidiChannelVoiceModeEvent(running_status)
        {
        }
        static StreamEvent recognize(midi::MidiStreamRange midi_stream_tail, midi::RunningStatusStandard& running_status) noexcept;
        midi::MidiStreamRange consume_stream(midi::MidiStreamRange midi_stream_tail) noexcept final;
        std::ostream& print(std::ostream& os) const final;
      private:
        midi::MidiStreamAtom pressure_{};
      friend std::ostream& operator<<(std::ostream& , const MidiChannelVoiceChannelPressureEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceChannelPressureEvent& );

    class MidiChannelVoicePolyphonicKeyPressureEvent final : public MidiChannelVoiceNoteEvent
    {
      public:
        explicit MidiChannelVoicePolyphonicKeyPressureEvent(
            const midi::RunningStatusStandard& running_status,
            std::uint32_t ticks_per_whole,
            std::shared_ptr<bool> prefer_sharp) noexcept
          : MidiChannelVoiceNoteEvent{running_status,
              ticks_per_whole, prefer_sharp}
        {
        }
        static StreamEvent
            recognize(midi::MidiStreamRange midi_stream_tail,
            midi::RunningStatusStandard& running_status, std::shared_ptr<bool> prefer_sharp,
            uint32_t ticks_per_whole) noexcept;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiChannelVoicePolyphonicKeyPressureEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiChannelVoicePolyphonicKeyPressureEvent& );

    //
    // Rest Event.  This is not in the MIDI spec;
    // it is a fiction specifically crafted for textmidi.
    class MidiChannelVoiceNoteRestEvent final : public MidiChannelVoiceNoteEvent
    {
      public:
        explicit MidiChannelVoiceNoteRestEvent(
            midi::RunningStatusStandard& running_status,
            std::uint32_t ticks_per_whole,
            std::shared_ptr<bool> prefer_sharp = std::shared_ptr<bool>{},
            midi::MidiStreamRange midi_stream_tail = midi::MidiStreamRange{}) noexcept
          : MidiChannelVoiceNoteEvent{running_status,
              ticks_per_whole, prefer_sharp}
        {
            key_string("R");
        }
        MidiChannelVoiceNoteRestEvent(const MidiChannelVoiceNoteRestEvent& )
            = default;
        MidiChannelVoiceNoteRestEvent&
            operator=(const MidiChannelVoiceNoteRestEvent& ) = default;
        std::ostream& print(std::ostream& os) const final;
      friend std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteRestEvent& );
    };
    std::ostream& operator<<(std::ostream& , const MidiChannelVoiceNoteRestEvent& );

    //
    // The factory that creates the classes above.
    class MidiEventFactory
    {
      public:
        explicit MidiEventFactory(std::uint32_t ticks_per_whole) noexcept
          : ticks_per_whole_(ticks_per_whole)
        {
        }
        std::tuple<midi::MidiStreamRange, std::optional<DelayEvent>>
            operator()(midi::MidiStreamRange midi_stream_tail, int64_t& ticks_accumulated);
      private:
        const std::uint32_t ticks_per_whole_;
        midi::RunningStatusStandard running_status_{};
        std::shared_ptr<bool> prefer_sharp_{std::make_shared<bool>()};
    };

    // Print the text version of an event in LAZY mode.
    class PrintLazyTrack
    {
      public:
        PrintLazyTrack(
            DelayEvents& delay_events,
            const rational::RhythmRational quantum,
            std::uint32_t ticksperquarter) noexcept
          : delay_events_{delay_events},
            quantum_{quantum},
            ticksperquarter_{ticksperquarter}
        {
            ticks_to_note_stop();
            ticks_to_next_event();
            ticks_to_a_note_start();
            insert_rests();
        }
      private:
        void print(std::ostream& , DelayEvent& ) noexcept;
        void ticks_to_note_stop() noexcept;
        void ticks_to_next_event() noexcept;
        void ticks_to_a_note_start() noexcept;
        //
        // traverse the list of MIDI events and insert rest events
        // representing the delay after each event
        // when there is no sounding note.
        void insert_rests() noexcept;
        rational::RhythmRational wholes_to_last_event() const noexcept
        {
            return wholes_to_last_event_;
        }
        void wholes_to_last_event(rational::RhythmRational rr) noexcept
        {
            wholes_to_last_event_ = rr;
        }
        using KeyBoard = std::vector<std::int32_t>;
        using ChannelKeyboards = std::vector<KeyBoard>;

        bool is_in_rest() const noexcept;

        midi::MidiStreamAtom channel_{};
        std::int32_t dynamic_{128};
        bool lazy_{};
        std::string lazy_string(bool lazy) noexcept;
        std::list<MidiChannelVoiceNoteOnEvent> chord_{};
        DelayEvents& delay_events_;
        rational::RhythmRational quantum_;
        std::uint32_t ticksperquarter_;
        std::shared_ptr<bool> prefer_sharp_{std::make_shared<bool>()};
        midi::RunningStatusStandard running_status_{};
        rational::RhythmRational wholes_to_last_event_{};
        ChannelKeyboards channel_keyboards_{midi::MidiChannelQty,
            KeyBoard(midi::MidiPitchQty, 0)};

      friend std::ostream& operator<<(std::ostream& os,
            PrintLazyTrack& print_lazy_track)
      {
          for (auto& mp : print_lazy_track.delay_events_)
          {
              print_lazy_track.print(os, mp);
          }
          return os;
      }
    };
    std::ostream& operator<<(std::ostream& , PrintLazyTrack& );
} // namespace textmidi

#endif // MIDI_EVENTS
