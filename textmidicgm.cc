//
// TextMIDITools Version 1.0.94
//
// textmidicgm 1.0
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// original 2004 ported from Amiga version on Fred Fish disks and
// described in my article in Readings
// in Computer-Generated Music ed. by Denis Baggi
// Thomas E. Janzen 2019-12-06
//
// Tue Dec 10 12:53:51 EST 2019 add C++ rand.
// Wed Dec 11 15:23:41 EST 2019 add amplitude to Sine
// Fri Dec 13 23:26:50 EST 2019 use maps not switch
//
// Sun Feb  9 18:52:57 EST 2020
//     fix C9 wouldn't convert in pitch to string
//     add gnuplot output optionally cf plot command below
//     use rational<> to reduce the rhythms in the LAZY mode of textmidi
//     fix amplitude and offset initializations
//       to .5 everywhere needed (it is 2 places 8^( )
//     make walking probabilities into xml file members and form struct members
//     refactor into classes more.
// Tue Feb 25 17:43:36 EST 2020 did more stuff.
//    Feb 13 add MelodyDirection; add rest ability
//    Feb 25 Fix channel.   Channel is 0-based in old forms.
//      It will be 1-based in new form.xml because
//      it is 1-based in textmidi.
//    didn't write a space before durations that were single numbers
//    implying 1/N.  oops.
//    also a bug in walking could play the lowest scale note
//        because the walk was not properly bounded.
// Wed Feb 26 21:47:57 EST 2020 implement using C++ std::chrono
//    to seed random numbers and to count time
//    in music.  Change ticks/quarter to 960.  Write the file name
//    as the TRACK name on the rhythm track.
//    Remove #if 0 code.
//    Also I re-arranged the gnuplot files to be in the same order
//        as in the original AlgoRhythms.
//
// main driver for textmidicgm, text-MIDI Computer-Generated Music.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <unistd.h>
#include <glob.h>
#include <libgen.h>

#include <cmath>
#include <cstdlib>
#include <cstdint>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <limits>
#include <unordered_map>
#include <filesystem>
#include <memory>

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/archive/basic_xml_archive.hpp>
#include <boost/archive/xml_archive_exception.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/serialization/version.hpp>
#include <boost/algorithm/string/case_conv.hpp> // to_upper

#include "MIDIKeyString.h"
#include "MusicalForm.h"
#include "cgmlegacy.h"

#include "Arrangements.h"
#include "Track.h"
#include "Voice.h"
#include "Options.h"
#include "Composer.h"
#include "RhythmRational.h"

using std::int32_t, std::string, std::vector, std::unordered_map,
      std::cerr, std::cin, std::cout;
// This isn't really necessary but i wrote it in a
// quest to get glob to compile, which wouldn't because
// of a different mistake.
int32_t glob_error(const char*, int32_t)
{
    return 0;
}
// change random to be double
// have probability class for up/down/same/ rest
// gather rests
//
namespace {
    using GlobStatusMap = unordered_map<int32_t, string>;
}

int32_t main(int argc, char *argv[])
{
    using std::make_unique, std::ranges::for_each, std::ofstream, std::ifstream;
    using boost::lexical_cast, boost::to_upper;
    using textmidi::rational::RhythmExpression, textmidi::rational::print_rhythm,
      textmidi::cgm::MusicalForm, textmidi::cgm::MusicalFormException,
      textmidi::cgm::TicksDuration, textmidi::cgm::TicksPerQuarter,
      textmidi::cgm::arrangement_map, textmidi::cgm::Composer;
    using cgmlegacy::TextForm;
    using arrangements::PermutationEnum;
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ((help_option.registered_name().c_str()), help_option.text().c_str())
        ((verbose_option.registered_name().c_str()),
            verbose_option.text().c_str())
        ((stack_tracks_option.registered_name().c_str()),
            stack_tracks_option.text().c_str())
        ((version_option.registered_name().c_str()),
            version_option.text().c_str())
        ((form_option.registered_name().c_str()),
            boost::program_options::value<string>(), form_option.text().c_str())
        ((XML_form_option.registered_name().c_str()),
            boost::program_options::value<vector<string>>()->multitoken(),
            XML_form_option.text().c_str())
        ((xml_update_option.registered_name().c_str()),
            xml_update_option.text().c_str())
        ((answer_option.registered_name().c_str()),
            answer_option.text().c_str())
        ((textmidi_out_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            textmidi_out_option.text().c_str())
        ((gnuplot_option.registered_name().c_str()),
            gnuplot_option.text().c_str())
        ((random_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            random_option.text().c_str())
        ((instruments_option.registered_name().c_str()),
            boost::program_options::value<vector<string>>()->multitoken(),
            instruments_option.text().c_str())
        ((clamp_scale_option.registered_name().c_str()),
            clamp_scale_option.text().c_str())
        ((arrangements_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            arrangements_option.text().c_str())
        ((max_events_per_track_option.registered_name().c_str()),
            boost::program_options::value<int32_t>(),
            max_events_per_track_option.text().c_str())
        ((arrangements_period_option.registered_name().c_str()),
            boost::program_options::value<double>(),
            arrangements_period_option.text().c_str())
        ((dotted_rhythms_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            dotted_rhythms_option.text().c_str())
        ((rhythm_expression_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            rhythm_expression_option.text().c_str())
    ;
    boost::program_options::variables_map var_map;
    try
    {
        boost::program_options::store(boost::program_options::
            parse_command_line(argc, argv, desc), var_map);
        boost::program_options::notify(var_map);
    }
    catch (std::logic_error& err)
    {
        const string errstr{(string{"Program options error: "} += err.what())
            += '\n'};
        cerr << errstr;
        exit(EXIT_SUCCESS);
    }
    if (var_map.count(help_option.option()))
    {
        const string logstr{((string{"Usage: textmidicgm [OPTION]... "
                    "[XMLFORMFILE]...\ntextmidicgm Version 1.0.94\n"}
            += lexical_cast<string>(desc)) += '\n')
            += "Report bugs to: janzentome@gmail.com\ntextmidicgm home page: "
            "https://github.com/tomejanzen/textmiditools\n"};
        cout << logstr;
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(version_option.option())) [[unlikely]]
    {
        cout << "textmidicgm\nTextMIDITools 1.0.94\n"
            "Copyright © 2025 Thomas E. Janzen\n"
            "License GPLv3+: GNU GPL version 3 "
            "or later <https://gnu.org/licenses/gpl.html>\n"
            "This is free software: you are free "
            "to change and redistribute it.\n"
            "There is NO WARRANTY, to the extent permitted by law.\n";
        exit(EXIT_SUCCESS);
    }

    bool verbose{};
    if (var_map.count(verbose_option.option())) [[unlikely]]
    {
        verbose = true;
    }

    bool stacktracks{};
    if (var_map.count(stack_tracks_option.option())) [[unlikely]]
    {
        stacktracks = true;
    }

    vector<string> form_filename_globs;
    if (var_map.count(form_option.option()))
    {
        form_filename_globs
            = var_map[form_option.option()].as<vector<string>>();
    }
    else
    {
        if (var_map.count(XML_form_option.option()))
        {
            form_filename_globs
                = var_map[XML_form_option.option()].as<vector<string>>();
        }
        else
        {
            if (var_map.count(random_option.option()))
            {
                auto random_filename{
                    var_map[random_option.option()].as<string>()};

                int32_t instrument_flags{0};

                if (var_map.count(instruments_option.option()))
                {
                    vector<string> instruments{
                        var_map[instruments_option.option()]
                            .as<vector<string>>()};
                    for (const auto& instrument : instruments)
                    {
                        if (textmidi::cgm::program_group_map
                            .contains(instrument))
                        {
                            instrument_flags
                                |= static_cast<int32_t>(
                                textmidi::cgm::program_group_map
                                .at(instrument));
                        }
                        else
                        {
                            cerr << instrument
                                 << " is not an instrument group\n";
                        }
                    }
                }
                else
                {
                    instrument_flags
                        = static_cast<int32_t>(
                        textmidi::cgm::GeneralMIDIGroup::Melodic);
                }

                MusicalForm xml_form{};
                xml_form.random(random_filename, instrument_flags);
                {
                    ofstream xml_form_stream{(random_filename
                            + string(".xml")).c_str()};
                    {
                        boost::archive::xml_oarchive oarc(xml_form_stream);
                        oarc << BOOST_SERIALIZATION_NVP(xml_form);
                    }
                    if (verbose)
                    {
                        cout << "Wrote " << random_filename << '\n';
                    }
                }
                exit(EXIT_SUCCESS);
            }
            else
            {
                cout << "Usage: textmidicgm [OPTION]... [XMLFORMFILE]...\n";
                string str{};
                str.reserve(256);
                ((str += "You must have either a text form file, "
                  "an XML form file or invoke --random\n")
                      += lexical_cast<string>(desc)) += '\n';
                cerr << str;
                exit(0);
            }
        }
    }
    vector<string> form_filenames;
    if (var_map.count(form_option.option())
        || var_map.count(XML_form_option.option()))
    {
        GlobStatusMap globStatusMap
        {
            {GLOB_NOSPACE, "NOSPACE"},
            {GLOB_ABORTED, "ABORTED"},
            {GLOB_NOMATCH, "NOMATCH"}
        };
        ::glob_t glob_data;
        int32_t globsts{};
        std::uint64_t pat{};
        for ( ; pat < 1; pat++)
        {
            if (0 != (globsts = ::glob(form_filename_globs[pat].c_str(),
                GLOB_TILDE_CHECK, glob_error, &glob_data)))
            {
                string str{};
                str.reserve(128);
                ((str += "glob failed: ")
                    += globStatusMap[globsts]) += " Exiting\n";
                cerr << str;
                exit(0);
            }
        }
        for ( ; pat < form_filename_globs.size(); pat++)
        {
            if (0 != (globsts = ::glob(form_filename_globs[pat].c_str(),
                GLOB_APPEND | GLOB_TILDE_CHECK, glob_error, &glob_data)))
            {
                string str{};
                str.reserve(128);
                ((str += "glob failed: ")
                    += globStatusMap[globsts]) += " Exiting\n";
                cerr << str;
                exit(0);
            }
        }
        std::for_each_n(&glob_data.gl_pathv[0], glob_data.gl_pathc,
                [&](const auto& gdgp) {
            form_filenames.emplace_back(gdgp); } );
        globfree(&glob_data);
    }
    else
    {
        cerr << "Usage: textmidicgm [OPTION]... [XMLFORMFILE]...\n";
        string str{};
        str.reserve(128);
        ((str += "you must have either old form files or XML form files\n")
              += lexical_cast<string>(desc)) += '\n';
        cerr << str;
        exit(0);
    }

    vector<MusicalForm> xml_forms;
    if (var_map.count(form_option.option()))
    {
        for (const auto& form_filename_temp : form_filenames)
        {
            TextForm form;
            form.read_from_file(form_filename_temp);
            xml_forms.emplace_back(form_filename_temp, form);
        }
    }
    else
    {
        // The new XML form file. 2019-12-07
        if (var_map.count(XML_form_option.option()))
        {
            for (const auto& form_filename : form_filenames)
            {
                ifstream xml_form_file(form_filename.c_str());
                {
                    try
                    {
                        boost::archive::xml_iarchive iarc(xml_form_file);
                        MusicalForm xml_form;
                        iarc >> BOOST_SERIALIZATION_NVP(xml_form);
                        if (!xml_form.valid())
                        {
                            const string errstr{((string{__FILE__} += ':')
                                += BOOST_PP_STRINGIZE(__LINE__))
                                += " Invalid Form.\n"};
                            cerr << errstr;
                        }
                        xml_forms.push_back(xml_form);
                        if (var_map.count(xml_update_option.option())
                            && xml_form.valid())
                        {
                            string update_name{};
                            {
                                string pathstr{form_filename};
                                char tempthepath[FILENAME_MAX];
                                tempthepath[pathstr.copy(tempthepath,
                                    pathstr.size(), 0)] = '\0';

                                string thedir{::dirname(tempthepath)};
                                tempthepath[pathstr.copy(tempthepath,
                                    pathstr.size(), 0)] = '\0';
                                string thefilename{::basename(tempthepath)};
                                auto newfilename{string{"update_"}
                                    + thefilename};
                                (update_name += (thedir += '/'))
                                    += newfilename;
                            }
                            ofstream xml_form_stream{update_name.c_str()};
                            if (xml_form_stream)
                            {
                                boost::archive::xml_oarchive oarc
                                    (xml_form_stream);
                                oarc << BOOST_SERIALIZATION_NVP(xml_form);
                            }
                        }
                    }
                    catch (MusicalFormException& mfe)
                    {
                        const string errstr{((((((string{__FILE__} += ':')
                            += BOOST_PP_STRINGIZE(__LINE__)) += ' ')
                            += form_filename)
                            += " is an Invalid Form ") += mfe.what()) += '\n'};
                        cerr << errstr;
                        exit(EXIT_SUCCESS);
                    }
                    catch (std::ios_base::failure &iosfail)
                    {
                        cerr << iosfail.what() << '\n';
                        exit(EXIT_SUCCESS);
                    }
                    catch (boost::archive::xml_archive_exception& xae)
                    {
                        cerr << xae.what() << '\n';
                    }
                    catch (boost::archive::archive_exception& ae)
                    {
                        cerr << ae.what() << '\n';
                    }
                }
            }
        }
        else
        {
            cerr << "Usage: textmidicgm [OPTION]... [XMLFORMFILE]...\n";
            string str{};
            str.reserve(128);
            ((str += "you must have either a form file or an XML form file\n")
                 += lexical_cast<string>(desc)) += '\n';
            cerr << str;
            exit(0);
        }
    }

    bool answer{};
    if (var_map.count(answer_option.option())) [[unlikely]]
    {
        answer = true;
    }

    PermutationEnum track_scramble_type{PermutationEnum::Undefined};
    TicksDuration track_scramble_period{60000 * TicksPerQuarter};

    if (var_map.count(arrangements_option.option())) [[unlikely]]
    {
        const string scramble_string{
            var_map[arrangements_option.option()].as<string>()};
        if (arrangement_map.contains(scramble_string))
        {
            track_scramble_type = arrangement_map.at(scramble_string);
        }
        else
        {
            const string logstr{(string{"Track scrambling selections are: "}
                += arrangements_option.option()) += '\n'};
            cout << logstr;
            exit(EXIT_SUCCESS);
        }
        if (var_map.count(arrangements_period_option.option()))
        {
            track_scramble_period = TicksDuration{
                static_cast<int64_t>(floor(var_map[
                    arrangements_period_option.option()].as<double>()))
                    * TicksPerQuarter};
        }
    }
    const size_t max_events_per_track{
        var_map.count(max_events_per_track_option.option())
        ? var_map[max_events_per_track_option.option()].as<size_t>() : 100000};

    bool dotted_rhythms{true};
    if (var_map.count(dotted_rhythms_option.option())) [[unlikely]]
    {
        string dotted_rhythms_string{
            var_map[dotted_rhythms_option.option()].as<string>()};
        to_upper(dotted_rhythms_string);
        dotted_rhythms = ("TRUE" == dotted_rhythms_string);
    }

    if (var_map.count(rhythm_expression_option.option())) [[unlikely]]
    {
        string rhythm_expression_string{
            var_map[rhythm_expression_option.option()].as<string>()};
        to_upper(rhythm_expression_string);
        if (midi::rhythm_expression_map.contains(rhythm_expression_string))
        {
            const RhythmExpression rhythm_expression{
                midi::rhythm_expression_map[rhythm_expression_string]};
            switch (rhythm_expression)
            {
              case RhythmExpression::Rational:
                print_rhythm = make_unique<textmidi::rational::
                    PrintRhythmRational>(dotted_rhythms);
                break;
              case RhythmExpression::SimpleContinuedFraction:
                print_rhythm = make_unique<textmidi
                    ::rational::PrintRhythmSimpleContinuedFraction>();
                break;
            }
        }
    }
    else
    {
        if (var_map.count(dotted_rhythms_option.option())) [[unlikely]]
        {
            print_rhythm = make_unique<textmidi::rational
                ::PrintRhythmRational>(dotted_rhythms);
        }
    }

    string textmidi_filename;
    if (var_map.count(textmidi_out_option.option()))
    {
        textmidi_filename = var_map[textmidi_out_option.option()].as<string>();
    }
    if (answer && std::filesystem::exists(textmidi_filename))
    {
        cout << "Overwrite " << textmidi_filename << "?\n";
        string answerstr{};
        cin >> answerstr;
        if (!((answerstr[0] == 'y') || (answerstr[0] == 'Y')))
        {
            exit(0);
        }
    }
    bool gnuplot{};
    if (var_map.count(gnuplot_option.option()))
    {
        gnuplot = true;
    }

    // This block writes an XML form file if the input was an old form file.
    // It appends ".xml" to the name.
    if (var_map.count(form_option.option()))
    {
        for (int32_t xf{}; auto& xml_form : xml_forms)
        {
            const string form_filename_local(form_filenames[xf]);
            if (answer && std::filesystem::exists(form_filename_local))
            {
                cout << "Overwrite " << form_filename_local << "?\n";
                string answerstr{};
                cin >> answerstr;
                if (!((answerstr[0] == 'y') || (answerstr[0] == 'Y')))
                {
                    exit(0);
                }
            }

            ofstream xml_form_stream{(form_filename_local + ".xml").c_str()};
            {
                boost::archive::xml_oarchive oarc(xml_form_stream);
                oarc << BOOST_SERIALIZATION_NVP(xml_form);
            }
            ++xf;
        }
    }

    if (var_map.count(clamp_scale_option.option()))
    {
        for_each(xml_forms, [](auto& xml_form)
            { xml_form.clamp_scale_to_instrument_ranges(); } );
    }

    Composer composer{gnuplot, answer, track_scramble_type,
        track_scramble_period, max_events_per_track};

    {
        ofstream textmidi_file;
        uint64_t x{};
        if (stacktracks)
        {
            auto& xml_form{xml_forms[x]};

            if (var_map.count(textmidi_out_option.option()))
            {
                textmidi_file.open(textmidi_filename.c_str());
            }
            else
            {
                textmidi_file.open((xml_form.name() + ".txt").c_str());
            }

            composer(textmidi_file, xml_form);
            ++x;
        }

        for ( ; x < xml_forms.size(); ++x)
        {
            if (stacktracks)
            {
                composer(textmidi_file, xml_forms[x], false);
            }
            else
            {
                if (var_map.count(textmidi_out_option.option()))
                {
                    textmidi_file.open(textmidi_filename.c_str());
                }
                else
                {
                    textmidi_file.open((xml_forms[x].name() + ".txt").c_str());
                }
                composer(textmidi_file, xml_forms[x]);
                textmidi_file.close();
            }
        }
    }
}
