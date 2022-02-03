//
// TextMIDITools Version 1.0.12
//
// textmidi 1.0.6
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(TEXTMIDI_H)
#    define  TEXTMIDI_H

#include <cstdint>
#include <string>
#include <iostream>

//
// Globals for textmidi.
//
// line_ctr counts the lines in the input text file so that error messages can
// refer to the line on which the error was encountered.
extern std::uint32_t line_ctr;
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

#endif
