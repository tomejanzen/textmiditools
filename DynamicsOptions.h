//
// TextMIDITools Version 1.0.95
//
// textmidi
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(DYNAMICS_OPTIONS_H)
#    define  DYNAMICS_OPTIONS_H

#include <cstdint>

#include <vector>
#include <string>
#include <utility>

#include "MidiMaps.h"
#include "OptionName.h"

namespace textmidi
{

const OptionName ppppp_option{"ppppp",
    "piano pianissimo pianissimo value integer 0-127"};
const OptionName pppp_option {"pppp",
    "pianissimo pianissimo value integer 0-127"};
const OptionName ppp_option  {"ppp",   "piano pianissimo value integer 0-127"};
const OptionName pp_option   {"pp",    "pianissimo value integer 0-127"};
const OptionName p_option    {"p",     "piano value integer 0-127"};
const OptionName mp_option   {"mp",    "mezzo-piano value integer 0-127"};
const OptionName mf_option   {"mf",    "mezzo-forte value integer 0-127"};
const OptionName forte_option{"forte", "forte value integer 0-127"};
const OptionName ff_option   {"ff",    "fortissimo value integer 0-127"};
const OptionName fff_option  {"fff",   "forte fortissimo value integer 0-127"};
const OptionName ffff_option {"ffff",
    "fortissimo fortissimo value integer 0-127"};
const OptionName fffff_option{"fffff",
    "forte fortissimo fortissimo value integer 0-127"};

midi::NumStringMap<std::int32_t> read_dynamics_configuration(
    const std::string& dynamics_file_name);

} // namespace textmidi

#endif

