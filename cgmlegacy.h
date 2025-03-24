//
// TextMIDITools Version 1.0.94
//
// textmidicgm 1.0
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
// textmidicgm Copyright © 2025 Thomas E. Janzen
//
#if !defined(CGMLEGACY_H)
#    define  CGMLEGACY_H

#include <cstdlib>

#include <vector>
#include <string>
#include <iostream>

namespace cgmlegacy
{
    inline constexpr auto DEFAULT_PULSE{100.0};

#pragma pack(4) // crashes without alignment after the bool.
    struct VoiceOld
    {
        VoiceOld() = default;
        VoiceOld(std::uint32_t low_pitch,
                 std::uint32_t high_pitch,
                 std::uint32_t channel,
                 bool walking) noexcept
          : low_pitch_(low_pitch),
            high_pitch_(high_pitch),
            channel_(channel),
            walking_(walking)
        {
        }

        std::uint32_t low_pitch_{};
        std::uint32_t high_pitch_{};
        std::uint32_t channel_{}; // zero-based in VoiceOld
        bool walking_{};
        friend std::istream& operator>>(std::istream& is, VoiceOld& v)
        {
            is >> v.low_pitch_ >> v.high_pitch_ >> v.channel_ >> v.walking_;
            return is;
        }
    };
#pragma pack()

    class OldFormElement
    {
      public:
        OldFormElement() = default;
        OldFormElement(const double mean_period,
            const double mean_phase, const double range_period,
            const double range_phase) noexcept
          : mean_period_{mean_period},
            mean_phase_{mean_phase},
            range_period_{range_period},
            range_phase_{range_phase}
        {
        }
        double mean_period() const noexcept;
        double mean_phase() const noexcept;
        double range_period() const noexcept;
        double range_phase() const noexcept;
      private:
        double mean_period_  = 60.0;
        double mean_phase_   = 0.0;
        double range_period_ = 60.0;
        double range_phase_  = 0.0;
    };

    // obsolescent but needed for conversions of old files (use --form *.frm).
    struct TextForm
    {
        TextForm() = default;

        void read_from_file(const std::string& form_filename);
        double len{60};
        double min_note_len{1.0};
        double max_note_len{1.0};
        std::size_t scale_qty{15};
        std::vector<std::uint32_t>
            scale{48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72};
        std::size_t voice_qty{};
        double pulse{1.0};
        OldFormElement pitch_form{};
        OldFormElement rhythm_form{};
        OldFormElement dynamic_form{};
        OldFormElement texture_form{};
        std::vector<VoiceOld> voices{};
    };

} // namespace cgmlegacy

#endif
