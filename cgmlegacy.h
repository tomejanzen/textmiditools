//
// TextMIDITools Version 1.0.76
//
// textmidicgm 1.0
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
// textmidicgm Copyright © 2024 Thomas E. Janzen
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
        VoiceOld()
          : low_pitch_(),
            high_pitch_(),
            channel_(),
            walking_()
        {
        }

        VoiceOld(std::uint32_t low_pitch,
                 std::uint32_t high_pitch,
                 std::uint32_t channel,
                 bool walking)
          : low_pitch_(low_pitch),
            high_pitch_(high_pitch),
            channel_(channel),
            walking_(walking)
        {
        }

        std::uint32_t low_pitch_;
        std::uint32_t high_pitch_;
        std::uint32_t channel_; // zero-based in VoiceOld
        bool walking_;
        friend std::istream& operator>>(std::istream& is, VoiceOld& v);
    };
#pragma pack()
    std::istream& operator>>(std::istream& is, VoiceOld& v);

    class OldFormElement
    {
      public:
        OldFormElement(const double mean_period = 60,
            const double mean_phase = 0, const double range_period = 60,
            const double range_phase = 0)
          : mean_period_{mean_period},
            mean_phase_{mean_phase},
            range_period_{range_period},
            range_phase_{range_phase}
        {
        }
        double mean_period() const;
        double mean_phase() const;
        double range_period() const;
        double range_phase() const;
      private:
        double mean_period_;
        double mean_phase_;
        double range_period_;
        double range_phase_;
    };

    // obsolescent but needed for conversions of old files (use --form *.frm).
    struct TextForm
    {
        TextForm()
          : len{60},
            min_note_len{1.0},
            max_note_len{1.0},
            scale_qty{88},
            scale{},
            voice_qty{},
            pulse{1.0},
            pitch_form{},
            rhythm_form{},
            dynamic_form{},
            texture_form{},
            voices()
        {
        };

        void read_from_file(const std::string& form_filename);
        double len;
        double min_note_len;
        double max_note_len;
        std::size_t scale_qty;
        std::vector<std::uint32_t> scale;
        std::size_t voice_qty;
        double pulse;
        OldFormElement pitch_form;
        OldFormElement rhythm_form;
        OldFormElement dynamic_form;
        OldFormElement texture_form;
        std::vector<VoiceOld> voices;
    };

}

#endif
