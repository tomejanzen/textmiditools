//
// TextMIDITools Version 1.0.82
//
// textmidicgm 1.0
// Copyright © 2024 Thomas E. Janzen
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

using namespace std;
using namespace boost;
using namespace textmidi;
using namespace textmidi::cgm;
using namespace cgmlegacy;
using namespace arrangements;

// This isn't really necessary but i wrote it in a
// quest to get glob to compile, which wouldn't because
// of a different mistake.
int glob_error(const char*, int)
{
    return 0;
}
// change random to be double
// have probability class for up/down/same/ rest
// gather rests
//
namespace {

    using GlobStatusMap = unordered_map<int, string>;

    // Derived from Stroustrup Tour of C++ 2nd Ed p 191

    const string FormOpt{"form"};
    constexpr char FormTxt[]{
        "input plain text Form files; double-quote wildcards"};
    const string XML_FormOpt{"xmlform"};
    constexpr char XML_FormTxt[]{"alternate input XML Form file"};
    const string XML_UpdateOpt{"update"};
    constexpr char XML_UpdateTxt[]{"new file name for updated XML form file"};
    const string GnuplotOpt{"gnuplot"};
    constexpr char GnuplotTxt[]{"gnuplot data output file"};
    const string RandomOpt{"random"};
    constexpr char RandomTxt[]{"{filename} - write a random form and exit"};
    const string InstrumentsOpt{"instruments"};
    constexpr char InstrumentsTxt[]{"random instrument groups: piano "
        "chromaticpercussion organ guitar bass strings ensemble brass reed pipe "
        "synthlead synthpad syntheffects ethnic percussive soundeffects all "
        "melodic idiophone"};
    const string ClampScaleOpt{"clampscale"};
    constexpr char ClampScaleTxt[]{"in each form, clamp the scale to the union of the voice ranges"};
    const string ArrangementsOpt{"arrangements"};
    constexpr char ArrangementsTxt[]{"rotateright rotateleft reverse previouspermutation "
        "nextpermutation swappairs shuffle skip heaps identity"};
    const string ArrangementsPeriodOpt{"arrangementsperiod"};
    constexpr char ArrangementsPeriodTxt[]{"floating seconds"};
    const string MaxEventsPerTrackOpt{"maxeventspertrack"};
    constexpr char MaxEventsPerTrackTxt[]{"integer"};
    const string StackTracksOpt{"stacktracks"};
    constexpr char StackTracksTxt[]{"process each form file and add its tracks to the same output textmidi score"};
}

int main(int argc, char *argv[])
{
    program_options::options_description desc("Allowed options");
    desc.add_options()
        ((HelpOpt                + ",h").c_str(),                                                                HelpTxt)
        ((VerboseOpt             + ",v").c_str(),                                                             VerboseTxt)
        ((StackTracksOpt         + ",k").c_str(),                                                         StackTracksTxt)
        ((VersionOpt             + ",V").c_str(),                                                             VersionTxt)
        ((FormOpt                + ",f").c_str(), program_options::value<string>(),                              FormTxt)
        ((XML_FormOpt            + ",x").c_str(), program_options::value<vector<string>>()->multitoken(),    XML_FormTxt)
        ((XML_UpdateOpt          + ",u").c_str(),                                                          XML_UpdateTxt)
        ((AnswerOpt              + ",a").c_str(),                                                              AnswerTxt)
        ((TextmidiOpt            + ",o").c_str(), program_options::value<string>(),                          TextmidiTxt)
        ((GnuplotOpt             + ",g").c_str(),                                                             GnuplotTxt)
        ((RandomOpt              + ",r").c_str(), program_options::value<string>(),                            RandomTxt)
        ((InstrumentsOpt         + ",i").c_str(), program_options::value<vector<string>>()->multitoken(), InstrumentsTxt)
        ((ClampScaleOpt          + ",c").c_str(),                                                          ClampScaleTxt)
        ((ArrangementsOpt        + ",z").c_str(), program_options::value<string>(),                      ArrangementsTxt)
        ((MaxEventsPerTrackOpt   + ",t").c_str(), program_options::value<int>(),                    MaxEventsPerTrackTxt)
        ((ArrangementsPeriodOpt  + ",y").c_str(), program_options::value<double>(),                ArrangementsPeriodTxt)
        ((RhythmExpressionOpt    + ",e").c_str(), program_options::value<string>(),                  RhythmExpressionTxt)
    ;
    program_options::variables_map var_map;
    try
    {
        program_options::store(program_options::parse_command_line(argc, argv, desc), var_map);
        program_options::notify(var_map);
    }
    catch (std::logic_error& err)
    {
        const string errstr{(string{"Program options error: "} += err.what()) += '\n'};
        cerr << errstr;
        exit(EXIT_SUCCESS);
    }
    if (var_map.count(HelpOpt))
    {
        const string logstr{((string{"Usage: textmidicgm [OPTION]... [XMLFORMFILE]...\ntextmidicgm Version 1.0.82\n"}
            += lexical_cast<string>(desc)) += '\n')
            += "Report bugs to: janzentome@gmail.com\ntextmidicgm home page: <https://www\n"};
        cout << logstr;
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(VersionOpt)) [[unlikely]]
    {

        cout << "textmidicgm\nTextMIDITools 1.0.82\nCopyright © 2024 Thomas E. Janzen\n"
            "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n"
            "This is free software: you are free to change and redistribute it.\n"
            "There is NO WARRANTY, to the extent permitted by law.\n";
        exit(EXIT_SUCCESS);
    }

    bool verbose{};
    if (var_map.count(VerboseOpt)) [[unlikely]]
    {
        verbose = true;
    }

    bool stacktracks{};
    if (var_map.count(StackTracksOpt)) [[unlikely]]
    {
        stacktracks = true;
    }

    vector<string> form_filename_globs;
    if (var_map.count(FormOpt))
    {
        form_filename_globs = var_map[FormOpt].as<vector<string>>();
    }
    else
    {
        if (var_map.count(XML_FormOpt))
        {
            form_filename_globs = var_map[XML_FormOpt].as<vector<string>>();
        }
        else
        {
            if (var_map.count(RandomOpt))
            {
                auto random_filename{var_map[RandomOpt].as<string>()};

                int32_t instrument_flags{0};

                if (var_map.count(InstrumentsOpt))
                {
                    vector<string> instruments{var_map[InstrumentsOpt].as<vector<string>>()};
                    for (const auto& instrument : instruments)
                    {
                        if (program_group_map.contains(instrument))
                        {
                            instrument_flags |= static_cast<int>(program_group_map.at(instrument));
                        }
                        else
                        {
                            cerr << instrument << " is not an instrument group\n";
                        }
                    }
                }
                else
                {
                    instrument_flags = static_cast<int>(GeneralMIDIGroup::Melodic);
                }

                MusicalForm xml_form{};
                xml_form.random(random_filename, instrument_flags);
                {
                    ofstream xml_form_stream{(random_filename
                            + string(".xml")).c_str()};
                    {
                        archive::xml_oarchive oarc(xml_form_stream);
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
    if (var_map.count(FormOpt) || var_map.count(XML_FormOpt))
    {
        GlobStatusMap globStatusMap
        {
            {GLOB_NOSPACE, "NOSPACE"},
            {GLOB_ABORTED, "ABORTED"},
            {GLOB_NOMATCH, "NOMATCH"}
        };
        ::glob_t glob_data;
        int globsts{};
        long unsigned int pat{};
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
        for (unsigned filectr{}; filectr < glob_data.gl_pathc; ++filectr)
        {
            form_filenames.push_back(string(glob_data.gl_pathv[filectr]));
        }
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
    if (var_map.count(FormOpt))
    {
        for (const auto& form_filename_temp : form_filenames)
        {
            TextForm form;
            form.read_from_file(form_filename_temp);
            xml_forms.push_back(MusicalForm(form_filename_temp, form));
        }
    }
    else
    {
        // The new XML form file. 2019-12-07
        if (var_map.count(XML_FormOpt))
        {
            for (const auto& form_filename : form_filenames)
            {
                ifstream xml_form_file(form_filename.c_str());
                {
                    try
                    {
                        archive::xml_iarchive iarc(xml_form_file);
                        MusicalForm xml_form;
                        iarc >> BOOST_SERIALIZATION_NVP(xml_form);
                        if (!xml_form.valid())
                        {
                            const string errstr{((string{__FILE__} += ':') += BOOST_PP_STRINGIZE(__LINE__)) += " Invalid Form.\n"};
                            cerr << errstr;
                        }
                        xml_forms.push_back(xml_form);
                        if (var_map.count(XML_UpdateOpt) && xml_form.valid())
                        {
                            string update_name{};
                            {
                                string pathstr{form_filename};
                                char tempthepath[FILENAME_MAX];
                                tempthepath[pathstr.copy(tempthepath, pathstr.size(), 0)] = '\0';

                                string thedir{::dirname(tempthepath)};
                                tempthepath[pathstr.copy(tempthepath, pathstr.size(), 0)] = '\0';
                                string thefilename{::basename(tempthepath)};
                                auto newfilename{string{"update_"} + thefilename};
                                (update_name += (thedir += '/')) += newfilename;
                            }
                            ofstream xml_form_stream{update_name.c_str()};
                            if (xml_form_stream)
                            {
                                archive::xml_oarchive oarc(xml_form_stream);
                                oarc << BOOST_SERIALIZATION_NVP(xml_form);
                            }
                        }
                    }
                    catch (MusicalFormException& mfe)
                    {
                        const string errstr{((((((string{__FILE__} += ':')
                            += BOOST_PP_STRINGIZE(__LINE__)) += ' ') += form_filename)
                            += " is an Invalid Form ") += mfe.what()) += '\n'};
                        cerr << errstr;
                        exit(EXIT_SUCCESS);
                    }
                    catch (ios_base::failure &iosfail)
                    {
                        cerr << iosfail.what() << '\n';
                        exit(EXIT_SUCCESS);
                    }
                    catch (archive::xml_archive_exception& xae)
                    {
                        cerr << xae.what() << '\n';
                    }
                    catch (archive::archive_exception& ae)
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
    if (var_map.count(AnswerOpt)) [[unlikely]]
    {
        answer = true;
    }

    PermutationEnum track_scramble_type{PermutationEnum::Undefined};
    TicksDuration track_scramble_period{60000 * TicksPerQuarter};

    if (var_map.count(ArrangementsOpt)) [[unlikely]]
    {
        const string scramble_string{var_map[ArrangementsOpt].as<string>()};
        if (arrangement_map.contains(scramble_string))
        {
            track_scramble_type = arrangement_map.at(scramble_string);
        }
        else
        {
            const string logstr{(string{"Track scrambling selections are: "} += ArrangementsTxt) += '\n'};
            cout << logstr;
            exit(EXIT_SUCCESS);
        }
        if (var_map.count(ArrangementsPeriodOpt))
        {
            track_scramble_period = TicksDuration{
                static_cast<int64_t>(floor(var_map[ArrangementsPeriodOpt].as<double>())) * TicksPerQuarter};
        }
    }
    const int max_events_per_track{var_map.count(MaxEventsPerTrackOpt) ? var_map[MaxEventsPerTrackOpt].as<int>() : 100000};

    if (var_map.count(RhythmExpressionOpt)) [[unlikely]]
    {
        string rhythm_expression_string{var_map[RhythmExpressionOpt].as<string>()};
        to_upper(rhythm_expression_string);
        if (midi::rhythm_expression_map.contains(rhythm_expression_string))
        {
            const rational::RhythmExpression rhythm_expression{midi::rhythm_expression_map[rhythm_expression_string]};
            switch (rhythm_expression)
            {
                case textmidi::rational::RhythmExpression::Rational:
                  break;
              case textmidi::rational::RhythmExpression::SimpleContinuedFraction:
                  textmidi::rational::print_rhythm.reset(new textmidi::rational::PrintRhythmSimpleContinuedFraction);
                  break;
            }
        }
    }

    string textmidi_filename;
    if (var_map.count(TextmidiOpt))
    {
        textmidi_filename = var_map[TextmidiOpt].as<string>();
    }
    if (answer && filesystem::exists(textmidi_filename))
    {
        cout << "Overwrite " << textmidi_filename << "?\n";
        string answer{};
        cin >> answer;
        if (!((answer[0] == 'y') || (answer[0] == 'Y')))
        {
            exit(0);
        }
    }
    bool gnuplot{};
    if (var_map.count(GnuplotOpt))
    {
        gnuplot = true;
    }

    // This block writes an XML form file if the input was an old form file.
    // It appends ".xml" to the name.
    if (var_map.count(FormOpt))
    {
        for (int xf{}; auto& xml_form : xml_forms)
        {
            const string form_filename_local(form_filenames[xf]);
            if (answer && filesystem::exists(form_filename_local))
            {
                cout << "Overwrite " << form_filename_local << "?\n";
                string answer{};
                cin >> answer;
                if (!((answer[0] == 'y') || (answer[0] == 'Y')))
                {
                    exit(0);
                }
            }

            ofstream xml_form_stream{(form_filename_local + ".xml").c_str()};
            {
                archive::xml_oarchive oarc(xml_form_stream);
                oarc << BOOST_SERIALIZATION_NVP(xml_form);
            }
            ++xf;
        }
    }

    if (var_map.count(ClampScaleOpt))
    {
        for (auto& xml_form : xml_forms)
        {
            xml_form.clamp_scale_to_instrument_ranges();
        }
    }

    Composer composer{gnuplot, answer, track_scramble_type, track_scramble_period, max_events_per_track};

    {
        ofstream textmidi_file;
        unsigned long int x{};
        if (stacktracks)
        {
            auto& xml_form{xml_forms[x]};

            if (var_map.count(TextmidiOpt))
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
                if (var_map.count(TextmidiOpt))
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
