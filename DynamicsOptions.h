//
// TextMIDITools Version 1.0.71
//
// textmidi
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(DYNAMICS_OPTIONS_H)
#    define  DYNAMICS_OPTIONS_H

#include <vector>
#include <string>
#include <utility>

#include "MidiMaps.h"

namespace textmidi
{

const std::string pppppOpt{"ppppp"};
inline constexpr char pppppTxt[]{"piano pianissimo pianissimo value integer 0-127"};

const std::string ppppOpt{"pppp"};
inline constexpr char ppppTxt[]{"pianissimo pianissimo value integer 0-127"};

const std::string pppOpt{"ppp"};
inline constexpr char pppTxt[]{"piano pianissimo value integer 0-127"};

const std::string ppOpt{"pp"};
inline constexpr char ppTxt[]{"pianissimo value integer 0-127"};

const std::string pOpt{"p"};
inline constexpr char pTxt[]{"piano value integer 0-127"};

const std::string mpOpt{"mp"};
inline constexpr char mpTxt[]{"mezzo-piano value integer 0-127"};

const std::string mfOpt{"mf"};
inline constexpr char mfTxt[]{"mezzo-forte value integer 0-127"};

const std::string forteOpt{"forte"};
inline constexpr char forteTxt[]{"forte value integer 0-127"};

const std::string ffOpt{"ff"};
inline constexpr char ffTxt[]{"fortissimo value integer 0-127"};

const std::string fffOpt{"fff"};
inline constexpr char fffTxt[]{"forte fortissimo value integer 0-127"};

const std::string ffffOpt{"ffff"};
inline constexpr char ffffTxt[]{"fortissimo fortissimo value integer 0-127"};

const std::string fffffOpt{"fffff"};
inline constexpr char fffffTxt[]{"forte fortissimo fortissimo value integer 0-127"};

midi::NumStringMap<int> read_dynamics_configuration(const std::string& dynamics_file_name);

}

#endif

