//
// TextMIDITools Version 1.0.12
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
#include "Midi.h"
#include "GeneralMIDI.h"
#include "MIDIKeyString.h"

using namespace std;
using namespace cgm;
using namespace boost;
using namespace textmidi;

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
    // The probabilities are cumulative probabilities
    // and like thresholds.
    // down = probability(resting)
    // same = probability(resting) + probability(walking down)
    // up   = probability(resting) + probability(walking down) +
    //        probability(repeating the last pitch)
    //
    // Because the probabilities are cumulative, the following holds:
    //   0.0 >= down
    //   down >= same
    //   up >= 1.0
    // To prevent voices from walking off to one end of their ranges, keep
    //   up == down

    if (random_variable >= up_)
    {
        return MelodyDirection::Up;
    }
    else
    {
        if (random_variable >= same_)
        {
            return MelodyDirection::Same;
        }
        else
        {
            if (random_variable >= down_)
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

    // We are finding the cumulative probabilities if walking has
    // been randomly selected.  If tacit is allowed to be random to 1.0, you can
    // get leading voice pauses that are quite long and disconcerting,
    // so we set it to 1/8.
    const double temp_tacit{0.125};
    double temp_down{rd()};
    double temp_same{rd()};
    // temp_up == temp_down so that there is an equal probability of
    // going either up or down, and the voice won't drift off to one
    // end of its range.
    double temp_up{temp_down};
    // Scale the sum P(up)+P(same)+P(down) to 1-P(tacit)
    const double temp_sum{temp_up + temp_same + temp_down};
    const double temp_factor{(1.0 - temp_tacit) / temp_sum};
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

    const int MaxRandomVoiceQty{24};
    voices_.resize(ri() % MaxRandomVoiceQty + 1);

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

    // We will count through the melodic channel numbers.
    // (General MIDI puts idiophones on channel 10.)
    vector<int> melodic_channels(textmidi::MidiChannelQty);
    iota(melodic_channels.begin(), melodic_channels.end(), 1);
    auto mc_it{find(melodic_channels.begin(), melodic_channels.end(),
        MidiIdiophoneChannel)};
    if (mc_it != melodic_channels.end())
    {
        melodic_channels.erase(mc_it);
    }

    for (auto& v : voices_)
    {
        v.channel(melodic_channels[ri() % melodic_channels.size()]);
        v.walking((ri() % 2) == 1);

        // No Followers in a random form.
        v.follower(VoiceXml::Follower{});
    }
    vector<int> channels(voices_.size());
    transform(voices_.begin(), voices_.end(), channels.begin(),
           [](const VoiceXml& v) { return v.channel(); });
    sort(channels.begin(), channels.end());
    auto uniq_it{unique(channels.begin(), channels.end())};
    channels.erase(uniq_it, channels.end());
    //
    // We don't want voices way over at one side.
    // So we will add a stereo zone to the number of channels.
    // For example:
    // 1 voice has two zones, one on the left and one on the right:
    // L---------C---------R
    //           ^
    // 2 voices have 3 zones:
    // L---------C---------R
    //        ^      ^
    //
    // 3 voices have 4 zones:
    // L---------C---------R
    //     ^     ^     ^
    //
    const int stereo_zones{static_cast<int>(channels.size() + 1)};
    const auto pan_step{(textmidi::PanExcess64 * 2) / stereo_zones};
    const auto first_pan{pan_step - textmidi::PanExcess64};
    map<int, int> channel_to_pan;
    int pan(first_pan);
    for (auto ch : channels)
    {
        channel_to_pan[ch] = pan;
        pan += pan_step;

    }
    for (auto& v : voices_)
    {
        v.pan(channel_to_pan[v.channel()]);
    }

    map<int, int> channel_to_program;
    for (auto ch : channels)
    {
        channel_to_program[ch] = programs[ri() % programs.size()];
    }
    for (auto& v : voices_)
    {
        const auto program{channel_to_program[v.channel()]};
        v.low_pitch(midi_programs[program].range_.first);
        v.high_pitch(midi_programs[program].range_.second);
        // If idiophones were selected, then coerce channel to 10.
        if (IdiophoneMarker == program)
        {
            v.program(lexical_cast<string>(1));
            v.channel(IdiophoneChannel);
            v.low_pitch(IdiophoneRange.first);
            v.high_pitch(IdiophoneRange.second);
        }
        else
        {
            v.program(lexical_cast<string>(program));
        }
    }
    // Coerce idiophones to be pan-centered.
    // It doesn't matter whether it's already in the map.
    channel_to_pan[10] = 0;
    // truncate scale to not exceed the instrument ranges.
    // The scales provided are monotonically increasing but
    // user-edited forms may have non-monotonic scales.
    // This algorithm assume a monotonically increasng scale.
    // Find the maximum instrument range note.
    CompareLowerNoteName lower_note{};
    const auto min_instrument_range = *min_element(voices_.begin(), voices_.end(),
        [lower_note](const VoiceXml& vleft,
        const VoiceXml& vright){ return lower_note(vleft.low_pitch(),
        vright.low_pitch()); });
    const auto max_instrument_range = *max_element(voices_.begin(), voices_.end(),
        [lower_note](const VoiceXml& vleft, const VoiceXml& vright){ return lower_note(vleft.high_pitch(), vright.high_pitch()); });
    auto erase_iter{remove_if(scale_.begin(), scale_.end(),
        [max_instrument_range, min_instrument_range, lower_note](const string& scale_pitch){
        return lower_note(scale_pitch, min_instrument_range.low_pitch())
            || lower_note(max_instrument_range.high_pitch(), scale_pitch); })};
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

void cgm::MusicalForm::clamp_scale_to_instrument_ranges()
{
    if (!voices_.empty())
    {
        const auto voice_min_note{min_element(voices_.begin(), voices_.end(),
            [](const cgm::VoiceXml& v1, const cgm::VoiceXml& v2)
            { return CompareLowerNoteName()(v1.low_pitch(), v2.low_pitch());})->low_pitch()};
        const auto voice_max_note{max_element(voices_.begin(), voices_.end(),
            [](const cgm::VoiceXml& v1, const cgm::VoiceXml& v2)
            { return CompareLowerNoteName()(v1.high_pitch(), v2.high_pitch());})->high_pitch()};
        scale_.erase(remove_if(scale_.begin(), scale_.end(),
            [voice_min_note, voice_max_note](const string& note_name)
            { return CompareLowerNoteName()(note_name, voice_min_note)
            || CompareLowerNoteName()(voice_max_note, note_name); }), scale_.end());
    }
}

RandomDouble cgm::rd{};
RandomInt cgm::ri{};

