//
// TextMIDITools Version 1.0.12
//
// MIDIKeyString.cc
//
// textmiditools 1.0
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// 2020-08-06
#if !defined(MIDIKEYSTRING)
#    define  MIDIKEYSTRING

#include <utility>
#include <string>

namespace textmidi
{
    constexpr int halfsteps_per_octave{12};
    extern std::pair<int, bool> key_sig_name_to_accidentals(std::string &key_sig_name);
    extern std::pair<int, bool> pitchname_to_keynumber(const std::string& pitchname);
    extern std::string num_to_note(int num);

    struct CompareLowerNoteName
    {
        bool operator()(const std::string& left, const std::string& right) const;
    };
}
#endif

