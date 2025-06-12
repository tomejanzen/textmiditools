//
// TextMIDITools Version 1.0.98
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cstdio>
#include <cctype>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <unistd.h>

#include <boost/algorithm/string/case_conv.hpp> // to_upper
#include <boost/program_options.hpp>

#include "DynamicsOptions.h"
#include "MidiMaps.h"
#include "Options.h"
#include "textmidi.h"

extern FILE* yyin;
extern int yylex(void);

//
// The main function for the textmidi program.
int main(int argc, char *argv[])
{
    using std::string, std::cerr, std::cout, std::cin, std::toupper;
    using boost::lexical_cast, boost::to_upper;

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ((help_option.registered_name().c_str()), help_option.text().c_str())
        ((verbose_option.registered_name().c_str()),
            verbose_option.text().c_str())
        ((version_option.registered_name().c_str()),
            version_option.text().c_str())
        ((textmidi_in_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            textmidi_in_option.text().c_str())
        ((midi_out_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            midi_out_option.text().c_str())
        ((answer_option.registered_name().c_str()),
            answer_option.text().c_str())
        ((detache_option.registered_name().c_str()),
            boost::program_options::value<uint32_t>(),
            detache_option.text().c_str())
        ((lazy_note_off_option.registered_name().c_str()),
            lazy_note_off_option.text().c_str())
        ((running_status_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            running_status_option.text().c_str())
        ((dynamics_configuration_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            dynamics_configuration_option.text().c_str())
    ;
    boost::program_options::positional_options_description pos_opts_desc;
    boost::program_options::variables_map var_map;

    try
    {
        pos_opts_desc.add(textmidi_in_option.registered_name().c_str(), -1);
        boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv)
            .options(desc).positional(pos_opts_desc).run(), var_map);
        boost::program_options::notify(var_map);
    }
    catch (std::logic_error& err)
    {
        const string errstr{(string("Program options error: " )
            += err.what()) += '\n'};
        cerr << errstr;
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(help_option.option())) [[unlikely]]
    {
        const string logstr{((string{
            "Usage: textmidi [OPTION]..."
            " [TEXTMIDIFILE]\ntextmidi Version 1.0.98\n"}
            += lexical_cast<string>(desc)) += '\n')
            += "Report bugs to: janzentome@gmail.com\ntextmidi home page: "
            "https://github.com/tomejanzen/textmiditools\n"};
        cout << logstr;
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(version_option.option())) [[unlikely]]
    {
        cout << "textmidi\nTextMIDITools Version 1.0.98\n"
            "Copyright © 2025 Thomas E. Janzen\n"
            "License GPLv3+: GNU GPL version 3 or later "
            "<https://gnu.org/licenses/gpl.html>\n"
            "This is free software: you are free "
            "to change and redistribute it.\n"
            "There is NO WARRANTY, to the extent permitted by law.\n";
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(verbose_option.option())) [[unlikely]]
    {
        textmidi::TextMidiFeatures::me()->verbose(true);
    }

    bool answer{};
    if (var_map.count(answer_option.option())) [[unlikely]]
    {
        answer = true;
    }

    string text_filename{}; // set in main.cc as option
    if (var_map.count(textmidi_in_option.option()))
    {
        text_filename = var_map[textmidi_in_option.option()].as<string>();
        textmidi::TextMidiFeatures::me()->text_filename(text_filename);
        if (!std::filesystem::exists(text_filename))
        {
            const string errstr{((textmidi_in_option.option() + ' ')
                += text_filename) += " File does not exist.\n"};
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
    if (var_map.count(midi_out_option.option()))
    {
        midi_filename = var_map[midi_out_option.option()].as<string>();
        if (answer && std::filesystem::exists(midi_filename))
        {
            const string outstr{(string{"Overwrite "}
                += midi_filename) += "?\n"};
            cout << outstr;
            string answerstr{};
            cin >> answerstr;
            if (!(toupper(answerstr[0]) == 'Y'))
            {
                exit(0);
            }
        }
        if (midi_filename == text_filename)
        {
            cerr << "The text and MIDI filenames are the same!  "
                    "You would have overwritten the text file!; Must exit!\n";
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        cerr << "Need a binary MIDI file name!\n";
        cerr << desc << '\n';
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(detache_option.option())) [[unlikely]]
    {
        textmidi::TextMidiFeatures::me()->detache(
            var_map[detache_option.option()].as<uint32_t>());
    }

    if (var_map.count(lazy_note_off_option.option())) [[unlikely]]
    {
        textmidi::TextMidiFeatures::me()->note_off_select(true);
    }

    {
        string dynamics_configuration_file{};
        if (var_map.count(dynamics_configuration_option.option())) [[unlikely]]
        {
            dynamics_configuration_file
                = var_map[dynamics_configuration_option.option()].as<string>();
        }
        midi::dynamics_map = textmidi::read_dynamics_configuration(
            dynamics_configuration_file);
    }

    if (var_map.count(running_status_option.option())) [[unlikely]]
    {
        string running_status_str{var_map[running_status_option.option()]
            .as<string>()};
        to_upper(running_status_str);
        if (midi::running_status_policy_map.contains(running_status_str))
        {
            running_status = midi::RunningStatusFactory()(midi::
                running_status_policy_map[running_status_str]);
        }
        else
        {
            cerr << desc << '\n';
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        running_status = midi::RunningStatusFactory()(
            midi::running_status_policy_map[string{"STANDARD"}]);
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
    textmidi::TextMidiFeatures::me()->midi_filestr().open(
        midi_filename.c_str(), std::ios_base::binary);
    if (!textmidi::TextMidiFeatures::me()->midi_filestr())
    {
        const string errstr{(string("Can't open ") += midi_filename) += '\n'};
        cerr << errstr;
    }
    while (yylex());
    fclose (yyin);
    if (textmidi::TextMidiFeatures::me()->verbose())
    {
        const string verbose_str{(string{"Lines processed: "} +=
           lexical_cast<string>(textmidi::TextMidiFeatures::me()->line_ctr()
           - 1)) += '\n'};
        cout << verbose_str;
    }

    return EXIT_SUCCESS;
}
