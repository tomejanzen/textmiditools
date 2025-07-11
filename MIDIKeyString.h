//
// TextMIDITools Version 1.1.0
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// 2020-08-06
#if !defined(MIDIKEYSTRING_H)
#    define  MIDIKEYSTRING_H

#include <memory>
#include <string>
#include <utility>

#include "Midi.h"

namespace textmidi
{
    inline constexpr std::int32_t halfsteps_per_octave{12};
    extern std::pair<std::int32_t, bool>
        key_sig_name_to_accidentals(const std::string& key_sig_name);
    extern std::pair<std::int32_t, bool>
        pitchname_to_keynumber(const std::string& pitchname);
    extern std::string
        num_to_note(std::int32_t keynum, std::shared_ptr<bool> prefer_sharp) noexcept;

    struct CompareLowerNoteName
    {
        bool operator()
            (const std::string& left, const std::string& right) const noexcept;
    };

} // namespace textmidi
#endif

