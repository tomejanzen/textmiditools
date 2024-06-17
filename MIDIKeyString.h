//
// TextMIDITools Version 1.0.79
//
// MIDIKeyString.cc
//
// textmiditools 1.0
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// 2020-08-06
#if !defined(MIDIKEYSTRING_H)
#    define  MIDIKEYSTRING_H

#include <utility>
#include <string>
#include <memory>

#include "Midi.h"

namespace textmidi
{
    inline constexpr int halfsteps_per_octave{12};
    extern std::pair<int, bool> key_sig_name_to_accidentals(std::string &key_sig_name);
    extern std::pair<int, bool> pitchname_to_keynumber(const std::string& pitchname);
    extern std::string num_to_note(int num, std::shared_ptr<bool> prefer_sharp);

    struct CompareLowerNoteName
    {
        bool operator()(const std::string& left, const std::string& right) const;
    };

}
#endif

