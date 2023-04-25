//
// TextMIDITools Version 1.0.55
//
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cstdio>

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <filesystem>

#include <unistd.h>

#include <boost/program_options.hpp>

#include "textmidi.h"
#include "Options.h"

using namespace std;
using namespace boost;

namespace
{
    const string DetacheOpt{"detache"};
    constexpr char DetacheTxt[]{"number of ticks to cheat notes to separate consecutive notes"};
    const string LazyNoteOffOpt{"lazynoteoff"};
    constexpr char LazyNoteOffTxt[]{"To end notes, write MIDI note-offs with current dynamic rather than note-ons with velocity 0"};
    const string NoRunningStatusOpt{"norunningstatus"};
    constexpr char NoRunningStatusTxt[]{"to always have a status byte; otherwise follows MIDI Spec"};
}

namespace textmidi
{
    std::shared_ptr<textmidi::TextMidiFeatures> textmidi_features{};
}

extern FILE* yyin;
extern int yylex(void);

//
// The main function for the textmidi program.
int main(int argc, char *argv[])
{
    program_options::options_description desc("Allowed options");
    desc.add_options()
        ((HelpOpt        + ",h").c_str(),                                        HelpTxt)
        ((VerboseOpt     + ",v").c_str(),                                     VerboseTxt)
        ((VersionOpt     + ",V").c_str(),                                     VersionTxt)
        ((TextmidiOpt    + ",i").c_str(), program_options::value<string>(),  TextmidiTxt)
        ((MidiOpt        + ",o").c_str(), program_options::value<string>(),      MidiTxt)
        ((AnswerOpt      + ",a").c_str(),                                      AnswerTxt)
        ((DetacheOpt     + ",d").c_str(), program_options::value<uint32_t>(), DetacheTxt)
        ((LazyNoteOffOpt + ",l").c_str(),                                 LazyNoteOffTxt)
        ((NoRunningStatusOpt + ",n").c_str(),                         NoRunningStatusTxt)
    ;
    program_options::positional_options_description pos_opts_desc;
    program_options::variables_map var_map;

    ofstream midi_filestr{}; // set in main.cc
    bool verbose{};          // set in main.cc as option

    try
    {
        pos_opts_desc.add(TextmidiOpt.c_str(), -1);
        program_options::store(
            program_options::command_line_parser(argc, argv)
            .options(desc).positional(pos_opts_desc).run(), var_map);
        program_options::notify(var_map);
    }
    catch (std::logic_error& err)
    {
        const string errstr{(string("Program options error: " ) += err.what()) += '\n'};
        cerr << errstr;
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(HelpOpt)) [[unlikely]]
    {
        const string logstr{((string{"Usage: textmidi [OPTION]... [TEXTMIDIFILE]\ntextmidi 1.0.35\n"}
            += lexical_cast<string>(desc)) += '\n')
            += "Report bugs to: janzentome@gmail.com\ntextmidi home page: <https://www\n"};
        cout << logstr;
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(VersionOpt)) [[unlikely]]
    {
        cout << "textmidi\nTextMIDITools 1.0.55\nCopyright © 2023 Thomas E. Janzen\n"
            "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n"
            "This is free software: you are free to change and redistribute it.\n"
            "There is NO WARRANTY, to the extent permitted by law.\n";
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(VerboseOpt)) [[unlikely]]
    {
       verbose = true;
    }

    bool answer{};
    if (var_map.count(AnswerOpt)) [[unlikely]]
    {
        answer = true;
    }

    string text_filename{}; // set in main.cc as option
    if (var_map.count(TextmidiOpt))
    {
        text_filename = var_map[TextmidiOpt].as<string>();
        if (!filesystem::exists(text_filename))
        {
            const string errstr{((string(TextmidiOpt) + ' ') += text_filename) += " File does not exist.\n"};
            cerr << errstr;
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        cerr << "Need a textmidi text file name!\n";
        cerr << desc << '\n';
        exit(EXIT_SUCCESS);
    }

    string midi_filename{};
    if (var_map.count(MidiOpt))
    {
        midi_filename = var_map[MidiOpt].as<string>();
        if (answer && filesystem::exists(midi_filename))
        {
            const string outstr{(string{"Overwrite "} += midi_filename) += "?\n"};
            cout << outstr;
            string answer{};
            cin >> answer;
            if (!((answer[0] == 'y') || (answer[0] == 'Y')))
            {
                exit(0);
            }
        }
        if (midi_filename == text_filename)
        {
            cerr << "The text and MIDI filenames are the same!  You would have overwritten the text file!; Must exit!\n";
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        cerr << "Need a binary MIDI file name!\n";
        cerr << desc << '\n';
        exit(EXIT_SUCCESS);
    }

    uint32_t detache{};   // set from cmd line; separation between notes
    if (var_map.count(DetacheOpt)) [[unlikely]]
    {
        detache = var_map[DetacheOpt].as<uint32_t>();
    }

    bool note_off_select{};
    if (var_map.count(LazyNoteOffOpt)) [[unlikely]]
    {
        note_off_select = true;
    }

    if (var_map.count(NoRunningStatusOpt)) [[unlikely]]
    {
        running_status.policy(midi::RunningStatus::RunningStatusPolicy::Never);
    }

    if (midi_filename.empty()) [[unlikely]]
    {
        midi_filename = text_filename + ".mid";
    }

    yyin = fopen(text_filename.c_str(), "r");
    if (0 == yyin)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    midi_filestr.open(midi_filename.c_str(), ios_base::binary);
    if (!midi_filestr)
    {
        const string errstr{(string("Can't open ") += midi_filename) += '\n'};
        cerr << errstr;
    }
    textmidi::textmidi_features = make_shared<textmidi::TextMidiFeatures>(text_filename, midi_filestr,
        detache, note_off_select, verbose);
    while (yylex());
    fclose (yyin);
    if (verbose)
    {
        const string verbose_str{(string{"Lines processed: "} +=
           lexical_cast<string>(textmidi::textmidi_features->line_ctr_ - 1)) += '\n'};
        cout << verbose_str;
    }

    return EXIT_SUCCESS;
}
