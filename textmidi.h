//
// TextMIDITools Version 1.0.62
//
// textmidi 1.0.6
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(TEXTMIDI_H)
#    define  TEXTMIDI_H

#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>

#include "Midi.h"

namespace textmidi
{
    struct TextMidiFeatures
    {
      public:
        TextMidiFeatures(std::string text_filename, std::ofstream& midi_filestr,
                std::uint32_t detache, bool note_off_select, bool verbose)
          : text_filename_{text_filename},
            midi_filestr_{midi_filestr},
            line_ctr_{1},
            detache_{detache},
            note_off_select_{note_off_select},
            verbose_{verbose}
        {}
        const std::string text_filename_;
        std::ofstream& midi_filestr_;
        int line_ctr_;
        const std::uint32_t detache_;
        const bool note_off_select_;
        const bool verbose_;
    };
    extern std::shared_ptr<TextMidiFeatures> textmidi_features;
}

//
// Globals for textmidi.
//
// line_ctr counts the lines in the input text file so that error messages can
// refer to the line on which the error was encountered.
extern std::int32_t line_ctr;
//
// detache is set by a command-line argument for the separation of consecutive
// notes.  Defaults to 10.
extern std::uint32_t detache;
//
// lazy_note_off_select is set by a command-line argument.  If true, then
// in LAZY or BRIEF mode, note_off events (with the global velocity) are
// written as the endings of notes; else note_ons (with a zero velocity) are
// used to end notes.
namespace lazy
{
    extern bool note_off_select;
}
//
// The input file name, selected by command-line options, is used in error
// messages.
extern std::string text_filename;
//
// The ofstream of the binary standard MIDI output file.
extern std::ofstream midi_filestr;

//
// Verbose flag taken from command-line options
extern bool verbose;
extern std::shared_ptr<midi::RunningStatusBase> running_status;

#endif
