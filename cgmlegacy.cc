//
// TextMIDITools Version 1.0.25
//
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <fstream>
#include <sstream>

#include "cgmlegacy.h"

using namespace std;
using namespace cgmlegacy;

/*
** Read in the legacy form text file from AlgoRhythms 3.0 for the Amiga.
*/
istream& cgmlegacy::operator>>(istream& is, cgmlegacy::VoiceOld& v)
{
    is >> v.low_pitch_ >> v.high_pitch_ >> v.channel_ >> v.walking_;
    return is;
}

double cgmlegacy::OldFormElement::mean_period() const
{
    return mean_period_;
};

double cgmlegacy::OldFormElement::mean_phase() const
{
    return mean_phase_;
};

double cgmlegacy::OldFormElement::range_period() const
{
    return range_period_;
};

double cgmlegacy::OldFormElement::range_phase() const
{
    return range_phase_;
};

void cgmlegacy::TextForm::read_from_file(const string& form_filename)
{
    ifstream form_stream(form_filename.c_str());
    string form_string;

    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> len;            // length in seconds.
    }

    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> min_note_len;            // min note length in seconds
    }

    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> max_note_len;            // max note length in seconds
    }

    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> scale_qty;            // number of scale tones
    }

    scale.clear();
    for (size_t scale_ctr(0); scale_ctr < scale_qty; ++scale_ctr)
    {
        if (getline(form_stream, form_string)) [[likely]]
        {
            istringstream iss(form_string);
            uint32_t key{};
            iss >> key;        // a MIDI key number in the scale
            scale.push_back(key);
        }
    }

    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> voice_qty;            // number of voices
    }

    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        double local_pulse{};
        iss >> local_pulse;            // pulse per second
        if (0 == local_pulse) [[unlikely]]
        {
            pulse = DEFAULT_PULSE;
        }
        else [[likely]]
        {
            pulse = local_pulse;
        }
    }

    double tempMeanPeriod{0.0};
    double tempMeanPhase{0.0};
    double tempRangePeriod{0.0};
    double tempRangePhase{0.0};

    // pitch
    // mean_period

    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempMeanPeriod;
    }
    // mean_phase
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempMeanPhase;
    }
    // range_period
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempRangePeriod;
    }
    // range_phase
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempRangePhase;
    }
    pitch_form = OldFormElement(tempMeanPeriod, tempMeanPhase,
                    tempRangePeriod, tempRangePhase);
    // rhythm
    // mean_period
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempMeanPeriod;
    }
    // mean_phase
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempMeanPhase;
    }
    // range_period
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempRangePeriod;
    }
    // range_phase
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempRangePhase;
    }

    rhythm_form
      = OldFormElement(tempMeanPeriod, tempMeanPhase,
                    tempRangePeriod, tempRangePhase);

    // dynamic
    // mean_period
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempMeanPeriod;
    }
    // mean_phase
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempMeanPhase;
    }
    // range_period
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempRangePeriod;
    }
    // range_phase
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempRangePhase;
    }

    dynamic_form
      = OldFormElement(tempMeanPeriod, tempMeanPhase,
                    tempRangePeriod, tempRangePhase);

    // texture
    // range_period
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempMeanPeriod;
    }
    // range_phase
    if (getline(form_stream, form_string)) [[likely]]
    {
        istringstream iss(form_string);
        iss >> tempMeanPhase;
    }

    texture_form = OldFormElement(0.0, 0.0,
        tempMeanPeriod, tempMeanPhase);

    // per voice
    // lowpitch, highpitch, channel, walking, audio
    // (audio may be absent)
    voices.clear();
    while (getline(form_stream, form_string)) {
        istringstream iss(form_string);

        VoiceOld vo;
        iss >> vo;
        voices.push_back(vo);
    }
    // Ignore channel, string of audio instrument until end of file
    form_stream.close();
}

