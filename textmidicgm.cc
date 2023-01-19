//
// TextMIDITools Version 1.0.28
//
// textmidicgm 1.0
// Copyright © 2022 Thomas E. Janzen
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

#include "MIDIKeyString.h"
#include "MusicalForm.h"
#include "cgmlegacy.h"

#include "Track.h"
#include "Voice.h"
#include "Options.h"
#include "Composer.h"

using namespace std;
using namespace boost;
using namespace textmidi;
using namespace textmidi::cgm;
using namespace cgmlegacy;

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

    typedef unordered_map<int, string> GlobStatusMap;

    // Derived from Stroustrup Tour of C++ 2nd Ed p 191

    const string FormOpt{"form"};
    constexpr char FormTxt[]{
        "input plain text Form files; double-quote wildcards"};
    const string XML_FormOpt{"xmlform"};
    constexpr char XML_FormTxt[]{"alternate input XML Form file"};
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
    const string TrackScrambleOpt{"trackscramble"};
    constexpr char TrackScrambleTxt[]{"rotateright rotateleft reverse previouspermutation "
        "nextpermutation swappairs shuffle"};
    const string TrackScramblePeriodOpt{"trackscrambleperiod"};
    constexpr char TrackScramblePeriodTxt[]{"floating seconds"};
}

int main(int argc, char *argv[])
{
    program_options::options_description desc("Allowed options");
    desc.add_options()
        ((HelpOpt        + ",h").c_str(), HelpTxt)
        ((VerboseOpt     + ",v").c_str(), VerboseTxt)
        ((VersionOpt     + ",V").c_str(), VersionTxt)
        ((FormOpt        + ",f").c_str(), program_options::value<string>(), FormTxt)
        ((XML_FormOpt    + ",x").c_str(), program_options::value<string>(), XML_FormTxt)
        ((AnswerOpt      + ",a").c_str(), AnswerTxt)
        ((TextmidiOpt    + ",o").c_str(), program_options::value<string>(), TextmidiTxt)
        ((GnuplotOpt     + ",g").c_str(), GnuplotTxt)
        ((RandomOpt      + ",r").c_str(), program_options::value<string>(), RandomTxt)
        ((InstrumentsOpt + ",i").c_str(), program_options::value<vector<string>>()->multitoken(), InstrumentsTxt)
        ((ClampScaleOpt  + ",c").c_str(), ClampScaleTxt)
        ((TrackScrambleOpt + ",z").c_str(), program_options::value<string>(), TrackScrambleTxt)
        ((TrackScramblePeriodOpt + ",y").c_str(), program_options::value<double>(), TrackScramblePeriodTxt)
    ;
    program_options::variables_map var_map;
    try
    {
        program_options::store(program_options::parse_command_line(argc, argv, desc), var_map);
        program_options::notify(var_map);
    }
    catch (std::logic_error& err)
    {
        cerr << "Program options error: " << err.what() << '\n';
        exit(EXIT_SUCCESS);
    }
    if (var_map.count(HelpOpt))
    {
        cout << "Usage: textmidicgm [OPTION]... [XMLFORMFILE]...\n";
        string str{};
        str.reserve(512);
        (((str += "textmidicgm 1.0\n")
            += lexical_cast<string>(desc))
            += "\nReport bugs to: janzentome@gmail.com\n")
            += "textmidicgm home page: <https://www\n";
        cout << str;
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(VersionOpt)) [[unlikely]]
    {
        string str{};
        str.reserve(512);
        (((((((str += "textmidicgm\n")
            += "TextMIDITools 1.0.28\n")
            += "Copyright © 2022 Thomas E. Janzen\n")
            += "License GPLv3+: GNU GPL version 3 or later ")
            += "<https://gnu.org/licenses/gpl.html>\n")
            += "This is free software: ")
            += "you are free to change and redistribute it.\n")
            += "There is NO WARRANTY, to the extent permitted by law.\n";
        cout << str;
        exit(EXIT_SUCCESS);
    }

    bool verbose{};
    if (var_map.count(VerboseOpt)) [[unlikely]]
    {
        verbose = true;
    }


    string form_filename_glob;
    if (var_map.count(FormOpt))
    {
        form_filename_glob = var_map[FormOpt].as<string>();
    }
    else
    {
        if (var_map.count(XML_FormOpt))
        {
            form_filename_glob = var_map[XML_FormOpt].as<string>();
        }
        else
        {
            if (var_map.count(RandomOpt))
            {
                auto random_filename{var_map[RandomOpt].as<string>()};

                int32_t instrument_flags{static_cast<int>(0)};

                if (var_map.count(InstrumentsOpt))
                {
                    vector<string> instruments{var_map[InstrumentsOpt].as<vector<string>>()};
                    for (const auto& instrument : instruments)
                    {
                        auto it{program_group_map.find(instrument)};
                        if (it != program_group_map.end())
                        {
                            instrument_flags |= static_cast<int>(it->second);
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
                            + string(".form.xml")).c_str()};
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
        int globsts{0};
        if (0 != (globsts = ::glob(form_filename_glob.c_str(),
            GLOB_TILDE_CHECK, glob_error, &glob_data)))
        {
            string str{};
            str.reserve(128);
            ((str += "glob failed: ")
                += globStatusMap[globsts]) += " Exiting\n";
            cerr << str;
            exit(0);
        }
        for (unsigned filectr{0}; filectr < glob_data.gl_pathc; ++filectr)
        {
            form_filenames.push_back(string(glob_data.gl_pathv[filectr]));
        }
        globfree(&glob_data);
    }
    else
    {
        cout << "Usage: textmidicgm [OPTION]... [XMLFORMFILE]...\n";
        string str{};
        str.reserve(128);
        ((str += "you must have either a form file or an XML form file\n")
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
                        MusicalForm temp_form;
                        iarc >> BOOST_SERIALIZATION_NVP(temp_form);
                        if (!temp_form.valid())
                        {
                            cerr << __FILE__ << ':' << BOOST_PP_STRINGIZE(__LINE__)
                                 << " Invalid Form.\n";
                        }
                        xml_forms.push_back(temp_form);
                    }
                    catch (MusicalFormException& mfe)
                    {
                        cerr << __FILE__ << ':' << BOOST_PP_STRINGIZE(__LINE__)
                             << ' ' << form_filename << " is an Invalid Form " << mfe.what() << '\n';
                        exit(EXIT_SUCCESS);
                    }
                    catch (std::ios_base::failure &iosfail)
                    {
                        cerr << iosfail.what() << '\n';
                        exit(EXIT_SUCCESS);
                    }
                    catch (archive::xml_archive_exception& xae)
                    {
                        cerr << xae.what() <<'\n';
                    }
                    catch (archive::archive_exception& ae)
                    {
                        cerr << ae.what() <<'\n';
                    }
                }
            }
        }
        else
        {
            cout << "Usage: textmidicgm [OPTION]... [XMLFORMFILE]...\n";
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

    Composer::TrackScramble track_scramble;

    if (var_map.count(TrackScrambleOpt)) [[unlikely]]
    {
        const string scramble_string{var_map[TrackScrambleOpt].as<string>()};
        TrackScrambleEnum track_scramble_type{TrackScrambleEnum::None};
        const auto it{track_scramble_map.find(scramble_string)};
        if (it != track_scramble_map.end())
        {
            track_scramble_type = it->second;
        }
        TicksDuration track_scramble_period{120 * TicksPerQuarter};
        if (var_map.count(TrackScramblePeriodOpt))
        {
            track_scramble_period = TicksDuration{static_cast<int64_t>(floor(var_map[TrackScramblePeriodOpt].as<double>())) * TicksPerQuarter};
        }
        track_scramble = Composer::TrackScramble(track_scramble_type, track_scramble_period);
    }

    string textmidi_filename;
    if (var_map.count(TextmidiOpt))
    {
        textmidi_filename = var_map[TextmidiOpt].as<string>();
    }
    if (answer && std::filesystem::exists(textmidi_filename))
    {
        cout << "Overwrite " << textmidi_filename << "?" << '\n';
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
            if (answer && std::filesystem::exists(form_filename_local))
            {
                cout << "Overwrite " << form_filename_local << "?" << '\n';
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

    Composer composer{gnuplot, answer, track_scramble};
    for (auto& xml_form : xml_forms)
    {
        if (var_map.count(ClampScaleOpt))
        {
            xml_form.clamp_scale_to_instrument_ranges();
        }
        //
        // write the textmidi file, which can be translated by textmidi.
        //
        ofstream textmidi_file;

        if (var_map.count(TextmidiOpt))
        {
            textmidi_file.open(textmidi_filename.c_str());
        }
        else
        {
            textmidi_file.open((xml_form.name() + ".txt").c_str());
        }

        composer(textmidi_file, xml_form);

        textmidi_file.close();
    }
}
