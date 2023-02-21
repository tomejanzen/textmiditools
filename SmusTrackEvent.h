//
// TextMIDITools Version 1.0.39
//
// smustextmidi 1.0.6
// Copyright © 2023 Thomas E. Janzen
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
    constexpr int Rest{128};
    constexpr int InstrumentNumber{129};
    constexpr int TimeSignature{130};
    constexpr int KeySignature{131};
    constexpr int Volume{132};
    constexpr int Channel{133};
    constexpr int Preset{134};
    constexpr int Clef{135}; // 0 treble , 1 bass, 2 alto , 3 tenor  (unknown)
    constexpr int Tempo{136};
    constexpr int EndOfTrack{255};

    class SmusTrackEventBase
    {
       public:
        SmusTrackEventBase()
          : decision_{},
            data_{}
        {
        }
        explicit SmusTrackEventBase(const SmusTrackEventFilePod evt)
          : decision_{evt.decision},
            data_{evt.data}
        {
        }
        SmusTrackEventBase& operator=(SmusTrackEventBase&& ) = delete;
        SmusTrackEventBase&& move(SmusTrackEventBase&& ) = delete;
        auto decision() const;
        auto data() const;
        void add_to_delay_accum(const textmidi::rational::RhythmRational& delay);
        textmidi::rational::RhythmRational delay_accum() const;
        void delay_accum(const textmidi::rational::RhythmRational& delay);
        static int current_dynamic();
        static void current_dynamic(int current_dynamic);
        static bool i_am_lazy();
        static void i_am_lazy(bool i_am_lazy);
        static std::string i_am_lazy_string(bool i_am_lazy);
        static int channel();
        static void channel(int channel);
        std::string pre_rest();
        textmidi::rational::RhythmRational duration() const;
        static void flush();
        virtual std::string textmidi() = 0;
        virtual std::string textmidi_tempo() = 0;
        virtual ~SmusTrackEventBase() = default;
        static textmidi::rational::RhythmRational delay_accum_;
      private:
        int decision_;
        int data_;
        static int current_dynamic_;
        static bool i_am_lazy_;
        static int channel_;

        bool is_dotted() const;
        textmidi::rational::RhythmRational dotted_multiplier() const;
        textmidi::rational::RhythmRational tuplet_multiplier() const;
    };

    // 0-127 pitches
    class SmusTrackEventPitch final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventPitch(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
        {
        }
        SmusTrackEventPitch ()
          : SmusTrackEventBase{}
        {}
        std::string textmidi_tempo() override;
        std::string textmidi() override;
        bool is_tiedout() const;
        bool is_chorded() const;
        bool is_tied_back(int tp) const;
        void remove_from_tied();
        void add_to_tied();
        static void flush();
      private:
        static std::vector<int> tied_vec_;
    };

    // Rest
    class SmusTrackEventRest final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventRest(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // InstrumentNumber
    class SmusTrackEventInstrument final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventInstrument(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // TimeSignature
    class SmusTrackEventTimeSignature final : public SmusTrackEventBase
    {
      private:
        std::pair<unsigned, unsigned> time_signature() const;
      public:
        SmusTrackEventTimeSignature(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // KeySignature
    class SmusTrackEventKeySignature final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventKeySignature(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
      private:
        std::string key() const;
    };

    // Volume
    class SmusTrackEventVolume final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventVolume(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // Channel
    class SmusTrackEventChannel final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventChannel(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // Preset
    class SmusTrackEventPreset final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventPreset(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // Clef
    class SmusTrackEventClef final : public SmusTrackEventBase
    {
      private:
          static std::map<int, std::string> clef_map;
      public:
        SmusTrackEventClef(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // Tempo
    class SmusTrackEventTempo final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventTempo(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() override;
        std::string textmidi() override;
    };

    // EndOfTrack
    class SmusTrackEventEnd final : public SmusTrackEventBase
    {
      public:
        SmusTrackEventEnd(const SmusTrackEventFilePod evt)
          : SmusTrackEventBase{evt}
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
        std::unique_ptr<SmusTrackEventBase> operator()(const SmusTrackEventFilePod& evt);
    };

}

#endif
