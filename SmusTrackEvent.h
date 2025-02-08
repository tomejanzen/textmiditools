//
// TextMIDITools Version 1.0.87
//
// smustextmidi 1.0.6
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(SMUSTRACKEVENT_H)
#    define  SMUSTRACKEVENT_H

#include <cstdint>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Midi.h"
#include "MidiMaps.h"
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
    inline constexpr int Rest{128};
    inline constexpr int InstrumentNumber{129};
    inline constexpr int TimeSignature{130};
    inline constexpr int KeySignature{131};
    inline constexpr int Volume{132};
    inline constexpr int Channel{133};
    inline constexpr int Preset{134};
    inline constexpr int Clef{135}; // 0 treble , 1 bass, 2 alto , 3 tenor  (unknown)
    inline constexpr int Tempo{136};
    inline constexpr int EndOfTrack{255};

    class SmusTrackEventBase
    {
       public:
        SmusTrackEventBase() = default;
        explicit SmusTrackEventBase(const SmusTrackEventFilePod evt) noexcept
          : decision_{evt.decision},
            data_{evt.data}
        {
        }
        auto decision() const noexcept;
        auto data() const noexcept;
        void add_to_delay_accum(const textmidi::rational::RhythmRational& delay) noexcept;
        textmidi::rational::RhythmRational delay_accum() const noexcept;
        void delay_accum(const textmidi::rational::RhythmRational& delay) noexcept;
        static int current_dynamic() noexcept;
        static void current_dynamic(int current_dynamic) noexcept;
        static bool i_am_lazy() noexcept;
        static void i_am_lazy(bool i_am_lazy) noexcept;
        static std::string i_am_lazy_string(bool i_am_lazy) noexcept;
        static int channel() noexcept;
        static void channel(int channel) noexcept;
        std::string pre_rest() noexcept;
        textmidi::rational::RhythmRational duration() const noexcept;
        static void flush() noexcept;
        virtual std::string textmidi() = 0;
        virtual std::string textmidi_tempo() = 0;
        // Rule of 5: Abstract base classes with virtual d-tor should get
        // defaults.
        SmusTrackEventBase(const SmusTrackEventBase& ) = default;
        SmusTrackEventBase(SmusTrackEventBase&& ) = default;
        SmusTrackEventBase& operator=(const SmusTrackEventBase& ) = default;
        SmusTrackEventBase& operator=(SmusTrackEventBase&& ) = default;
        virtual ~SmusTrackEventBase() = default;
        static textmidi::rational::RhythmRational delay_accum_;
      private:
        int decision_{};
        int data_{};
        static int current_dynamic_;
        static bool i_am_lazy_;
        static int channel_;

        bool is_dotted() const noexcept;
        textmidi::rational::RhythmRational dotted_multiplier() const noexcept;
        textmidi::rational::RhythmRational tuplet_multiplier() const noexcept;
    };

    // 0-127 pitches
    class SmusTrackEventPitch final : public SmusTrackEventBase
    {
      public:
        explicit SmusTrackEventPitch(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        SmusTrackEventPitch () noexcept
          : SmusTrackEventBase{}
        {}
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
        bool is_tiedout() const noexcept;
        bool is_chorded() const noexcept;
        bool is_tied_back(int tp) const noexcept;
        void remove_from_tied() noexcept;
        void add_to_tied() noexcept;
        static void flush() noexcept;
      private:
        static std::vector<int> tied_vec_;
    };

    // Rest
    class SmusTrackEventRest final : public SmusTrackEventBase
    {
      public:
        explicit SmusTrackEventRest(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
    };

    // InstrumentNumber
    class SmusTrackEventInstrument final : public SmusTrackEventBase
    {
      public:
        explicit SmusTrackEventInstrument(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
    };

    // TimeSignature
    class SmusTrackEventTimeSignature final : public SmusTrackEventBase
    {
      private:
        std::pair<unsigned, unsigned> time_signature() const noexcept;
      public:
        explicit SmusTrackEventTimeSignature(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
    };

    // KeySignature
    class SmusTrackEventKeySignature final : public SmusTrackEventBase
    {
      public:
        explicit SmusTrackEventKeySignature(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
      private:
        std::string key() const noexcept;
    };

    // Volume
    class SmusTrackEventVolume final : public SmusTrackEventBase
    {
      public:
        explicit SmusTrackEventVolume(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
    };

    // Channel
    class SmusTrackEventChannel final : public SmusTrackEventBase
    {
      public:
        explicit SmusTrackEventChannel(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
    };

    // Preset
    class SmusTrackEventPreset final : public SmusTrackEventBase
    {
      public:
        explicit SmusTrackEventPreset(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
    };

    extern const midi::NumStringMap<int> clef_map;

    // Clef
    class SmusTrackEventClef final : public SmusTrackEventBase
    {
      public:
        explicit SmusTrackEventClef(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
    };

    // Tempo
    class SmusTrackEventTempo final : public SmusTrackEventBase
    {
      public:
        explicit SmusTrackEventTempo(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
    };

    // EndOfTrack
    class SmusTrackEventEnd final : public SmusTrackEventBase
    {
      public:
        explicit SmusTrackEventEnd(const SmusTrackEventFilePod evt) noexcept
          : SmusTrackEventBase{evt}
        {
        }
        std::string textmidi_tempo() noexcept override;
        std::string textmidi() noexcept override;
    };

    class SmusTrackEventFactory
    {
      public:
        std::unique_ptr<SmusTrackEventBase> operator()(const SmusTrackEventFilePod& te) noexcept;
    };

}

#endif
