//
// textmidi 1.0
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(OPTIONS)
#    define  OPTIONS

#include <string>

const std::string HelpOpt{"help"};
constexpr char HelpTxt[]{"Write help text"};
const std::string VerboseOpt{"verbose"};
constexpr char VerboseTxt[]{"write more trace information"};
const std::string VersionOpt{"version"};
constexpr char VersionTxt[]{"Write version information"};
const std::string MidiOpt{"midi"};
constexpr char MidiTxt[]{"binary MIDI file"};
const std::string AnswerOpt{"answer"};
constexpr char AnswerTxt[]{"Ask before overwriting a file"};
const std::string TextmidiOpt{"textmidi"};
constexpr char TextmidiTxt[]{"textmidi file"};

#endif
