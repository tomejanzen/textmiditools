//
// TextMIDITools Version 1.0.85
//
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(MIDISTRING_H)
#    define  MIDISTRING_H

#include <string>

namespace textmidi
{
    void make_raw_string(std::string& str) noexcept;
    void make_human_string(std::string& str) noexcept;
}

#endif // MIDISTRING_H

