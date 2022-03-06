//
// TextMIDITools Version 1.0.22
//
// smustextmidi 1.0.6
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(SMUSTRACKEVENT_H)
#    define  SMUSTRACKEVENT_H

#include <cstdint>

#include <memory>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "RhythmRational.h"

namespace smus
{

    struct SmusTrackEventFilePod
    {
        std::uint8_t decision;
        std::uint8_t data;
    };

    // decision:
    // 0-127 pitches
    // 128 rest
    // 129 Instrument Number (unknown)
    // 130 Time Signature
    // 131 key signature
    // 132 volume
    // 133 MIDI Channel
    // 134 MIDI Preset
    // 135 clef 0 treble , 1 bass, 2 alto , 3 tenor  (unknown)
    // 136 tempo
    // 255 end of track (unknown)
    class SmusTrackEventBase
    {
       public:
        SmusTrackEventBase()
          : decision_{},
            data_{},
            channel_{}
        {
        }
        explicit SmusTrackEventBase(const std::uint8_t* bytes)
          : decision_{bytes[0]},
            data_{bytes[1]},
            channel_{}
        {
        }
        SmusTrackEventBase& operator=(SmusTrackEventBase&& ) = delete;
        SmusTrackEventBase&& move(SmusTrackEventBase&& ) = delete;
        auto decision() const;
        auto data() const;
        void add_to_delay_accum(const textmidi::rational::RhythmRational& delay);
        textmidi::rational::RhythmRational delay_accum() const;
        void delay_accum(const textmidi::rational::RhythmRational& delay);
        std::uint8_t current_dynamic() const;
        void current_dynamic(std::uint8_t current_dynamic);
        std::uint8_t channel() const;
        void channel(std::uint8_t channel);
        std::string pre_rest();
        textmidi::rational::RhythmRational duration() const;
        static void flush();
        virtual std::string textmidi() = 0;
        virtual std::string textmidi_tempo() = 0;
        virtual ~SmusTrackEventBase() = default;
        static textmidi::rational::RhythmRational delay_accum_;
      private:
        std::uint8_t decision_;
        std::uint8_t data_;
        static std::uint8_t current_dynamic_;
        std::uint8_t channel_;

        bool is_dotted() const;
        textmidi::rational::RhythmRational dotted_multiplier() const;
        textmidi::rational::RhythmRational tuplet_multiplier() const;
    };

    // 0-127 pitches
    class SmusTrackEventPitch final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventPitch(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        SmusTrackEventPitch ()
          : SmusTrackEventBase{}
        {}
        std::string textmidi_tempo() override;
        std::string textmidi() override;
        bool is_tiedout() const;
        bool is_chorded() const;
        bool is_tied_back(std::uint8_t tp) const;
        void remove_from_tied();
        void add_to_tied();
        static void flush();
      private:
        static std::vector<std::uint8_t> tied_vec_;
    };

    // 128 rest
    class SmusTrackEventRest final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventRest(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // 129
    class SmusTrackEventInstrument final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventInstrument(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // 130 Time Signature
    class SmusTrackEventTimeSignature final : public SmusTrackEventBase
    {
      private:
        std::pair<unsigned, unsigned> time_signature() const;
      public:
        SmusTrackEventTimeSignature(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // 131 key signature
    class SmusTrackEventKeySignature final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventKeySignature(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
      private:
        std::string key() const;
    };

    // 132 volume
    class SmusTrackEventVolume final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventVolume(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // 133 volume
    class SmusTrackEventChannel final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventChannel(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // 134 Preset
    class SmusTrackEventPreset final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventPreset(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // 135
    class SmusTrackEventClef final : public SmusTrackEventBase
    {
      private:
          static std::map<std::uint8_t, std::string> clef_map;
      public:
        SmusTrackEventClef(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // 136 tempo
    class SmusTrackEventTempo final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventTempo(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // 255 end of track
    class SmusTrackEventEnd final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventEnd(const std::uint8_t* bytes)
          : SmusTrackEventBase{bytes}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    class SmusTrackEventFactory
    {
      public:
        SmusTrackEventFactory()
        {
        }
        std::unique_ptr<SmusTrackEventBase> operator()(const SmusTrackEventFilePod& te);
    };

}

#endif
