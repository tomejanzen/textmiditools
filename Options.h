//
// TextMIDITools Version 1.0.71
//
// textmidi 1.0.6
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(OPTIONS_H)
#    define  OPTIONS_H

#include <string>

const std::string HelpOpt{"help"};
inline constexpr char HelpTxt[]{"Write help text"};
const std::string VerboseOpt{"verbose"};
inline constexpr char VerboseTxt[]{"write more trace information"};
const std::string VersionOpt{"version"};
inline constexpr char VersionTxt[]{"Write version information"};
const std::string MidiOpt{"midi"};
inline constexpr char MidiTxt[]{"binary MIDI file"};
const std::string AnswerOpt{"answer"};
inline constexpr char AnswerTxt[]{"Ask before overwriting a file"};
const std::string TextmidiOpt{"textmidi"};
inline constexpr char TextmidiTxt[]{"textmidi file"};
const std::string DynamicsConfigurationOpt{"dynamics_configuration"};
inline constexpr char DynamicsConfigurationTxt[]{"dynamics_configuration file"};

#endif
