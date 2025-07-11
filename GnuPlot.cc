//
// TextMIDITools Version 1.1.0
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>

#include "GnuPlot.h"
#include "Track.h"

using std::string;
using textmidi::cgm::MusicalForm;

//
// Write the form in values for the sinusoidal functions
// so that gnuplot can plot them using textmidicgm.gnuplot.
//
void textmidi::cgm::write_form_as_gnuplot_data(const MusicalForm& xml_form,
        const string& gnuplot_filename)
{
    using std::ofstream;
    using boost::lexical_cast;
    using textmidi::rational::RhythmRational;
    const rational::RhythmRational time_step{1L, 1L};
    const rational::RhythmRational endTime{static_cast<rational::RhythmRational>(std::round(xml_form.len()))};

    // gnuplot : plot 'concerto.form.plot' index [0123]
    // using 1:2:3:4 with yerror
    ofstream plotfile;
    plotfile.open(gnuplot_filename);

    string str{};
    str.reserve(16384);
    (str += "#plot 'concerto.form.plot' index \"pitch\" "
            "using 1:2:3:4 with yerror lc \"red\"\n")
       += "#index may be pitch, rhythm, dynamic, texture in quotes.\n";

    str += "#pitch\n";
    for (rational::RhythmRational aTime{0L, 1L}; aTime < endTime; aTime += time_step)
    {
        MusicalCharacter mc{};
        xml_form.character_now(aTime, mc);
        ((((((str += lexical_cast<string>(static_cast<double>(aTime)) += ' ')
            += lexical_cast<string>(mc.pitch_mean)) += ' ')
            += lexical_cast<string>(mc.pitch_mean - (mc.pitch_range / 2.0))) += ' ')
            += lexical_cast<string>(mc.pitch_mean + (mc.pitch_range / 2.0))) += '\n';
    }
    (str += "\n\n") += "#rhythm\n";
    for (rational::RhythmRational aTime{}; aTime < endTime; aTime += time_step)
    {
        MusicalCharacter mc{};
        xml_form.character_now(aTime, mc);
        (((((((str += lexical_cast<string>(
               static_cast<double>(aTime)
                / static_cast<double>(xml_form.music_time().ticks_per_quarter_))) += ' ')
            += lexical_cast<string>(mc.rhythm_mean)) += ' ')
            += lexical_cast<string>(mc.rhythm_mean
                - (mc.rhythm_range / 2.0))) += ' ')
            += lexical_cast<string>(mc.rhythm_mean
                + (mc.rhythm_range / 2.0))) += '\n';
    }
    (str += "\n\n") += "#dynamic\n";
    for (rational::RhythmRational aTime{}; aTime < endTime; aTime += time_step)
    {
        MusicalCharacter mc{};
        xml_form.character_now(aTime, mc);
        (((((((str += lexical_cast<string>(
            static_cast<double>(aTime))) += ' ')
            += lexical_cast<string>(mc.dynamic_mean)) += ' ')
            += lexical_cast<string>(mc.dynamic_mean - (mc.dynamic_range / 2.0))) += ' ')
            += lexical_cast<string>(mc.dynamic_mean + (mc.dynamic_range / 2.0)))
            += '\n';
    }
    (str += "\n\n") += "#texture\n";
    for (rational::RhythmRational aTime{}; aTime < endTime; aTime += time_step)
    {
        MusicalCharacter mc{};
        xml_form.character_now(aTime, mc);
        (((((((str += lexical_cast<string>(
            static_cast<double>(aTime))) += ' ')
            += lexical_cast<string>(mc.texture_range)) += ' ') += "0.0")
            += ' ') += lexical_cast<string>(mc.texture_range)) += '\n';
    }
    plotfile << str;
    plotfile.close();
}

