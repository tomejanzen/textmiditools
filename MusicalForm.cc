//
// textmidicgm 1.0
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// MusicalForm.cc
// The model of musical forms used by textmidicgm.
// Pitch is represented by two sinusoids, one for the mean pitch
// and one for the range of pitches from which to select a pitch.
// Rhythm (duration) is similar: one sinusoid for the mean duration
// and another for the range of durations.
// Dynamics (velocity) has a sinusoid for mean MIDI key velocity
// and another for range of velocities.
// "Texture" or complement, is one sinusoid that controls the number of
// voices playing at once.  Each voice will have its own MIDI track.
// Voices may be independent or be follows at an interval of another voice.
// There is a scale from which notes are selected.
// There are other details; cf. the documentation.
// textmidicgm runs in batch mode, reading in a form file and producing
// a textmidi text representation of a MIDI file, which can then be converted
// using the utility textmidi.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cmath>
#include <numeric>
#include <algorithm>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "MusicalForm.h"
#include "GeneralMIDI.h"
#include "MIDIKeyString.h"

using namespace std;
using namespace cgm;
using namespace boost;
using namespace textmidi;

namespace cgm
{
    constexpr double MinDynamic{30};
    constexpr double MaxDynamic{127};
}

double cgm::Sine::period() const noexcept
{
    return period_;
}

void cgm::Sine::period(double period) noexcept
{
    period_ = period;
}

double cgm::Sine::phase() const noexcept
{
    return phase_;
}

void cgm::Sine::phase(double phase) noexcept
{
    phase_ = phase;
}

double cgm::Sine::amplitude() const noexcept
{
    return amplitude_;
}
void cgm::Sine::amplitude(double amplitude) noexcept
{
    amplitude_ = amplitude;
}
double cgm::Sine::offset() const noexcept
{
    return offset_;
}
void cgm::Sine::offset(double offset) noexcept
{
    offset_ = offset;
}

double cgm::Sine::value_now(double the_time) const
{
    const auto the_sine{
        std::sin(2.0 * M_PI * the_time / period_ + phase_)};
    return amplitude_ * the_sine + offset_;
}

bool cgm::Sine::valid() const
{
    auto test = [](double period) { return 0.0 != period; };
    if (!test(period_)) [[unlikely]]
    {
        throw MusicalFormException{
        (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
            += " Invalid Sine period: ") += lexical_cast<string>(period_)};
    }
    return test(period_);
}

const Sine& cgm::MeanRangeSines::mean_sine() const noexcept
{
    return mean_sine_;
}

Sine& cgm::MeanRangeSines::mean_sine() noexcept
{
    return mean_sine_;
}

const Sine& cgm::MeanRangeSines::range_sine() const noexcept
{
    return range_sine_;
}

Sine& cgm::MeanRangeSines::range_sine() noexcept
{
    return range_sine_;
}

bool cgm::MeanRangeSines::valid() const
{
    auto test = [](Sine sine) { return sine.valid(); };
    bool rtn{};
    try
    {
        rtn = test(mean_sine_);
    }
    catch (MusicalFormException& mfe)
    {
        throw MusicalFormException{mfe.what() + " mean "};
    }
    try
    {
        rtn = test(range_sine_);
    }
    catch (MusicalFormException& mfe)
    {
        throw MusicalFormException{mfe.what() + " range "};
    }
    return rtn;
}

cgm::MelodyProbabilities::MelodyDirection
    cgm::MelodyProbabilities::operator()(double random_variable) const
{
    if (random_variable > up_)
    {
        return MelodyDirection::Up;
    }
    else
    {
        if (random_variable > same_)
        {
            return MelodyDirection::Same;
        }
        else
        {
            if (random_variable > down_)
            {
                return MelodyDirection::Down;
            }
            else
            {
                return MelodyDirection::Rest;
            }
        }
    }
}

double cgm::MusicalCharacter::duration(double rf)
{
    return (rhythm_range * rf) - (rhythm_range / 2.0) + rhythm_mean;
}

int cgm::MusicalCharacter::pitch_index(double rf)
{
    return static_cast<int>(
        round((pitch_range * rf) - (pitch_range / 2.0) + pitch_mean));
}

std::string cgm::MusicalForm::name() const noexcept
{
    return name_;
}

void cgm::MusicalForm::name(const std::string_view name) noexcept
{
    name_ = name;
}

double cgm::MusicalForm::len() const noexcept
{
    return len_;
}

void cgm::MusicalForm::len(double len) noexcept
{
    len_ = len;
}

double cgm::MusicalForm::min_note_len() const noexcept
{
    return min_note_len_;
}

void cgm::MusicalForm::min_note_len(double min_note_len) noexcept
{
    min_note_len_ = min_note_len;
}

double cgm::MusicalForm::max_note_len() const noexcept
{
    return max_note_len_;
}

void cgm::MusicalForm::max_note_len(double max_note_len) noexcept
{
    max_note_len_ = max_note_len;
}

const std::vector<std::string>& cgm::MusicalForm::scale() const noexcept
{
    return scale_;
}

std::vector<std::string>& cgm::MusicalForm::scale() noexcept
{
    return scale_;
}

double cgm::MusicalForm::pulse() const noexcept
{
    return pulse_;
}

void cgm::MusicalForm::pulse(double pulse) noexcept
{
    pulse_ = pulse;
}

const MelodyProbabilities& cgm::MusicalForm::melody_probabilities() const noexcept
{
    return melody_probabilities_;
}

void cgm::MusicalForm::melody_probabilities(
    const MelodyProbabilities& melody_probabilities) noexcept
{
    melody_probabilities_ = melody_probabilities;
}

MeanRangeSines cgm::MusicalForm::pitch_form() const noexcept
{
    return pitch_form_;
}

MeanRangeSines& cgm::MusicalForm::pitch_form() noexcept
{
    return pitch_form_;
}

MeanRangeSines cgm::MusicalForm::rhythm_form() const noexcept
{
    return rhythm_form_;
}

MeanRangeSines& cgm::MusicalForm::rhythm_form() noexcept
{
    return rhythm_form_;
}

MeanRangeSines cgm::MusicalForm::dynamic_form() const noexcept
{
    return dynamic_form_;
}

MeanRangeSines& cgm::MusicalForm::dynamic_form() noexcept
{
    return dynamic_form_;
}

Sine cgm::MusicalForm::texture_form() const noexcept
{
    return texture_form_;
}

Sine& cgm::MusicalForm::texture_form() noexcept
{
    return texture_form_;
}

void cgm::MusicalForm::texture_form(const Sine& s) noexcept
{
    texture_form_ = s;
}

std::vector<VoiceXml> cgm::MusicalForm::voices() const noexcept
{
    return voices_;
}

std::vector<VoiceXml>& cgm::MusicalForm::voices() noexcept
{
    return voices_;
}

void cgm::MusicalForm::string_scale_to_int_scale
    (std::vector<int>& key_scale) const
{
    key_scale.clear();
    for (auto str : scale_)
    {
        key_scale.push_back(textmidi::pitchname_to_keynumber(str).first);
    }
}

//
// Compute the instantaneous character, that is
// the current means and ranges of pitch, duration, dynamic
// and the number of voices playing.
void cgm::MusicalForm::character_now(TicksDuration theTime,
        MusicalCharacter& musical_character) const
{
    const double dblTime(
        static_cast<double>(theTime.count() / TicksPerQuarter)
        + (static_cast<double>(theTime.count() % TicksPerQuarter)
            / static_cast<double>(TicksPerQuarter)));
    musical_character.pitch_mean
        = pitch_form().mean_sine().value_now(dblTime)
        * double (scale().size());
    musical_character.pitch_range
        = pitch_form().range_sine().value_now(dblTime)
        * double (scale().size());

    musical_character.rhythm_mean
        = rhythm_form().mean_sine().value_now(dblTime)
        * (max_note_len() - min_note_len()) + min_note_len();
    musical_character.rhythm_range
        = rhythm_form().range_sine().value_now(dblTime)
        * (max_note_len() - min_note_len());

    musical_character.dynamic_mean
        = dynamic_form().mean_sine().value_now(dblTime)
        * (MaxDynamic - MinDynamic)
        + MinDynamic;
    musical_character.dynamic_range
        = dynamic_form().range_sine().value_now(dblTime) * MaxDynamic;

    musical_character.texture_range
        = texture_form().value_now(dblTime) * double (voices().size());
}

//
// Generate a random form.
// This is an optional command-line option in textmidicgm.
void cgm::MusicalForm::random(string formname, int32_t instrument_flags)
{
    constexpr int IdiophoneMarker{129};
    name_ = formname;
    len_  = 1800;
    min_note_len_ = 0.0;
    max_note_len_ = 2.0;
    // scale
    pulse_ = rd() * 16.0;

    double temp_tacit{rd()};
    double temp_down{rd()};
    double temp_same{rd()};
    double temp_up{temp_down};
    double temp_sum{temp_up + temp_same + temp_tacit + temp_down};
    double temp_factor{1.0 / temp_sum};
    temp_tacit *= temp_factor;
    temp_down  *= temp_factor;
    temp_same  *= temp_factor;
    temp_up    *= temp_factor;
    melody_probabilities_.down(temp_tacit);
    melody_probabilities_.same(temp_tacit + temp_down);
    melody_probabilities_.up(temp_tacit + temp_down + temp_same);
    pitch_form_.mean_sine().period(rd() * 300.0 + 60.0);
    pitch_form_.mean_sine().phase(rd() * 2.0 * M_PI - M_PI);
    pitch_form_.mean_sine().amplitude(0.5);
    pitch_form_.mean_sine().offset(0.5);

    pitch_form_.range_sine().period(rd() * 300.0 + 60.0);
    pitch_form_.range_sine().phase(rd() * 2.0 * M_PI - M_PI);
    pitch_form_.range_sine().amplitude(0.5);
    pitch_form_.range_sine().offset(0.5);

    rhythm_form_.mean_sine().period(rd() * 300.0 + 60.0);
    rhythm_form_.mean_sine().phase(rd() * 2.0 * M_PI - M_PI);
    rhythm_form_.mean_sine().amplitude(0.5);
    rhythm_form_.mean_sine().offset(0.5);

    rhythm_form_.range_sine().period(rd() * 300.0 + 60.0);
    rhythm_form_.range_sine().phase(rd() * 2.0 * M_PI - M_PI);
    rhythm_form_.range_sine().amplitude(0.5);
    rhythm_form_.range_sine().offset(0.5);

    dynamic_form_.mean_sine().period(rd() * 300.0 + 60.0);
    dynamic_form_.mean_sine().phase(rd() * 2.0 * M_PI - M_PI);
    dynamic_form_.mean_sine().amplitude(0.5);
    dynamic_form_.mean_sine().offset(0.5);

    dynamic_form_.range_sine().period(rd() * 300.0 + 60.0);
    dynamic_form_.range_sine().phase(rd() * 2.0 * M_PI - M_PI);
    dynamic_form_.range_sine().amplitude(0.5);
    dynamic_form_.range_sine().offset(0.5);

    texture_form_.period(rd() * 300.0 + 60.0);
    texture_form_.phase(rd() * 2.0 * M_PI - M_PI);
    texture_form_.amplitude(0.5);
    texture_form_.offset(0.5);

    voices_.resize(ri() % 16 + 1);

    scale_ = scale_strs[ri() % scale_strs.size()];

    vector<int> programs{};
    if (GeneralMIDIGroup::All == static_cast<GeneralMIDIGroup>(instrument_flags))
    {
        programs.resize(128);
        iota(programs.begin(), programs.end(), 1);
    }
    else
    {
        if (GeneralMIDIGroup::Melodic == static_cast<GeneralMIDIGroup>(instrument_flags))
        {
            programs.resize(96);
            iota(programs.begin(), programs.end(), 1);
        }
        else
        {
            for (int flag{0}; flag < 17; ++flag)
            {
                if (instrument_flags & (1 << flag))
                {
                    if (static_cast<GeneralMIDIGroup>(instrument_flags & (1 << 16))
                        == GeneralMIDIGroup::Idiophone)
                    {
                        for (int n{}; n < MIDIInstrumentsPerGroup; ++n)
                        {
                            programs.push_back(IdiophoneMarker);
                        }
                    }
                    else
                    {
                        for (int n{flag * MIDIInstrumentsPerGroup};
                             n < ((flag + 1) * MIDIInstrumentsPerGroup); ++n)
                        {
                            programs.push_back(n + 1);
                        }
                    }
                }
            }
        }
    }

    for (auto& v : voices_)
    {
        constexpr int NumberOfNonPercussionChannels{15};
        // MIDI Channels in (1..9, 11..16)
        v.channel((ri() % NumberOfNonPercussionChannels) + 1);
        // Coerce channels above 10 (idiophones channel)
        if (v.channel() >= IdiophoneChannel)
        {
            v.channel(v.channel() + 1);
        }
        v.walking((ri() % 2) == 1);

        const auto program_index{ri() % programs.size()};
        v.low_pitch(midi_programs[programs[program_index]].range_.first);
        v.high_pitch(midi_programs[programs[program_index]].range_.second);
        // If idiophones were selected, then coerce channel to 10.
        if (129 == programs[program_index])
        {
            v.program(lexical_cast<string>(1));
            v.channel(IdiophoneChannel);
            v.low_pitch(IdiophoneRange.first);
            v.high_pitch(IdiophoneRange.second);
        }
        else
        {
            v.program(lexical_cast<string>(programs[program_index]));
        }
        v.pan((((&v - &voices_[0]) * 128) / voices_.size()) - 64);
        v.follower(VoiceXml::Follower{});
    }
    // truncate scale to not exceed the instrument ranges.
    // The scales provided are monotonically increasing but
    // user-edited forms may have non-monotonic scales.
    // This algorithm assume a monotonically increasng scale.
    // Find the maximum instrument range note.
    CompareLowerNoteName lower_note{};
    const auto min_instrument_range = *min_element(voices_.begin(), voices_.end(),
        [lower_note](const VoiceXml& vleft, const VoiceXml& vright){ return lower_note(vleft.low_pitch(), vright.low_pitch()); }) ;
    const auto max_instrument_range = *max_element(voices_.begin(), voices_.end(),
        [lower_note](const VoiceXml& vleft, const VoiceXml& vright){ return lower_note(vleft.high_pitch(), vright.high_pitch()); }) ;
    auto erase_iter{remove_if(scale_.begin(), scale_.end(),
        [max_instrument_range, min_instrument_range, lower_note](const string& scale_pitch){
        return lower_note(scale_pitch, min_instrument_range.low_pitch())
            || lower_note(max_instrument_range.high_pitch(), scale_pitch) ; })};
    scale_.erase(erase_iter, scale_.end());
    // scale_ should now just cover the maximum extent of the instruments' ranges.
}

bool cgm::MusicalForm::valid() const
{
    bool rtn{true};
    if (0.0 == len_)
    {
        throw MusicalFormException{
            (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
            += " Invalid length: ") += lexical_cast<string>(len_)};
    }
    try
    {
        rtn = pitch_form_.valid();
    }
    catch (MusicalFormException& mfe)
    {
        throw MusicalFormException{mfe.what() + " Invalid pitch form.\n"};
    }
    try
    {
        rtn = rhythm_form_.valid();
    }
    catch (MusicalFormException& mfe)
    {
        throw MusicalFormException{mfe.what() + " Invalid rhythm form.\n"};
    }
    try
    {
        rtn = dynamic_form_.valid();
    }
    catch (MusicalFormException& mfe)
    {
        throw MusicalFormException{mfe.what() + " Invalid dynamic form.\n"};
    }
    try
    {
        rtn = texture_form_.valid();
    }
    catch (MusicalFormException& mfe)
    {
        throw MusicalFormException{mfe.what() + " Invalid texture form.\n"};
    }
    return rtn;
}

RandomDouble cgm::rd{};
RandomInt cgm::ri{};

