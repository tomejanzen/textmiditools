//
// TextMIDITools Version 1.0.97
//
// Copyright © 2025 Thomas E. Janzen
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
#include <ctime>

#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>

#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "MusicalForm.h"
#include "Midi.h"
#include "GeneralMIDI.h"
#include "MIDIKeyString.h"

using std::int32_t, std::string, std::string_view, std::vector;
using std::ranges::for_each, std::ranges::copy, std::ranges::min_element,
      std::ranges::max_element, std::ranges::find, std::tie,
      std::ranges::unique, std::ranges::remove_if, std::cerr;
using textmidi::cgm::Sine, textmidi::cgm::MeanRangeSines,
      textmidi::cgm::MelodyProbabilities, textmidi::cgm::MusicalCharacter,
      textmidi::cgm::MusicalForm,
      textmidi::cgm::VoiceXml, textmidi::cgm::ArrangementDefinition;
using boost::lexical_cast;

double Sine::period() const noexcept
{
    return period_;
}

void Sine::period(double period) noexcept
{
    period_ = period;
}

double Sine::phase() const noexcept
{
    return phase_;
}

void Sine::phase(double phase) noexcept
{
    phase_ = phase;
}

double Sine::amplitude() const noexcept
{
    return amplitude_;
}
void Sine::amplitude(double amplitude) noexcept
{
    amplitude_ = amplitude;
}
double Sine::offset() const noexcept
{
    return offset_;
}
void Sine::offset(double offset) noexcept
{
    offset_ = offset;
}

double Sine::value_now(double the_time) const noexcept
{
    const auto the_sine{
        std::sin(2.0 * M_PI * the_time / period_ + phase_)};
    return amplitude_ * the_sine + offset_;
}

bool Sine::valid() const
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

const Sine& MeanRangeSines::mean_sine() const noexcept
{
    return mean_sine_;
}

Sine& MeanRangeSines::mean_sine() noexcept
{
    return mean_sine_;
}

const Sine& MeanRangeSines::range_sine() const noexcept
{
    return range_sine_;
}

Sine& MeanRangeSines::range_sine() noexcept
{
    return range_sine_;
}

bool MeanRangeSines::valid() const
{
    auto test = [](const Sine& sine) { return sine.valid(); };
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

MelodyProbabilities::MelodyDirection
    MelodyProbabilities::operator()(double random_variable) const noexcept
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

void MelodyProbabilities::down(double down) noexcept
{
    down_ = down;
}

void MelodyProbabilities::same(double same) noexcept
{
    same_ = same;
}

void MelodyProbabilities::up(double up) noexcept
{
    up_ = up;
}

double MusicalCharacter::duration(double rf) noexcept
{
    return (rhythm_range * rf) - (rhythm_range / 2.0) + rhythm_mean;
}

int32_t MusicalCharacter::pitch_index(double rf) noexcept
{
    auto temp{static_cast<int32_t>(
        round((pitch_range * rf) - (pitch_range / 2.0) + pitch_mean))};
    temp = (temp < 0) ? 0 : temp;
    temp = (temp >= pitch_range) ? pitch_range - 1 : temp;
    return temp;
}

string MusicalForm::name() const noexcept
{
    return name_;
}

void MusicalForm::name(const string_view name) noexcept
{
    name_ = name;
}

double MusicalForm::len() const noexcept
{
    return len_;
}

void MusicalForm::len(double len) noexcept
{
    len_ = len;
}

double MusicalForm::min_note_len() const noexcept
{
    return min_note_len_;
}

void MusicalForm::min_note_len(double min_note_len) noexcept
{
    min_note_len_ = min_note_len;
}

double MusicalForm::max_note_len() const noexcept
{
    return max_note_len_;
}

void MusicalForm::max_note_len(double max_note_len) noexcept
{
    max_note_len_ = max_note_len;
}

const vector<string>& MusicalForm::scale() const noexcept
{
    return scale_;
}

vector<string>& MusicalForm::scale() noexcept
{
    return scale_;
}

double MusicalForm::pulse() const noexcept
{
    return pulse_;
}

void MusicalForm::pulse(double pulse) noexcept
{
    pulse_ = pulse;
}

const MelodyProbabilities& MusicalForm::melody_probabilities() const noexcept
{
    return melody_probabilities_;
}

void MusicalForm::melody_probabilities(
    const MelodyProbabilities& melody_probabilities) noexcept
{
    melody_probabilities_ = melody_probabilities;
}

MeanRangeSines MusicalForm::pitch_form() const noexcept
{
    return pitch_form_;
}

MeanRangeSines& MusicalForm::pitch_form() noexcept
{
    return pitch_form_;
}

MeanRangeSines MusicalForm::rhythm_form() const noexcept
{
    return rhythm_form_;
}

MeanRangeSines& MusicalForm::rhythm_form() noexcept
{
    return rhythm_form_;
}

MeanRangeSines MusicalForm::dynamic_form() const noexcept
{
    return dynamic_form_;
}

MeanRangeSines& MusicalForm::dynamic_form() noexcept
{
    return dynamic_form_;
}

Sine MusicalForm::texture_form() const noexcept
{
    return texture_form_;
}

Sine& MusicalForm::texture_form() noexcept
{
    return texture_form_;
}

void MusicalForm::texture_form(const Sine& s) noexcept
{
    texture_form_ = s;
}

vector<VoiceXml> MusicalForm::voices() const noexcept
{
    return voices_;
}

vector<VoiceXml>& MusicalForm::voices() noexcept
{
    return voices_;
}

ArrangementDefinition& MusicalForm::arrangement_definition() noexcept
{
    return arrangement_definition_;
}

const ArrangementDefinition& MusicalForm::arrangement_definition() const
    noexcept
{
    return arrangement_definition_;
}

void MusicalForm::arrangement_definition(
    ArrangementDefinition arrangement_definition) noexcept
{
    arrangement_definition_ = arrangement_definition;
}

void MusicalForm::string_scale_to_int_scale (vector<int32_t>& key_scale) const
    noexcept
{
    key_scale.clear();
    for_each(scale_, [&](const string& str) {
        key_scale.push_back(textmidi::pitchname_to_keynumber(str).first);});
}

//
// Compute the instantaneous character, that is
// the current means and ranges of pitch, duration, dynamic
// and the number of voices playing.
void MusicalForm::character_now(TicksDuration theTime,
        MusicalCharacter& musical_character) const noexcept
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
        * (midi::MaxDynamic - midi::MinDynamic)
        + midi::MinDynamic;
    musical_character.dynamic_range
        = dynamic_form().range_sine().value_now(dblTime) * midi::MaxDynamic;

    musical_character.texture_range
        = voices().empty() ? 0.0 : texture_form().value_now(dblTime)
        * double (voices().size());
}

//
// Generate a random form.
// This is an optional command-line option in textmidicgm.
void MusicalForm::random(string formname, int32_t instrument_flags)
{
    using midi::MidiIdiophoneChannel;
    using std::ranges::sort, std::ranges::transform, std::map;
    using std::views::iota;
    constexpr int32_t IdiophoneMarker{129};
    name_ = formname;
    const auto suffixpos{name_.find(".xml")};
    if (suffixpos != name_.npos)
    {
        name_.erase(suffixpos);
    }

    copyright_ = string("This form was generated "
        "by textmidicgm using random numbers.");

    len_  = 1800;
    // 0.00128 seconds is a note-on/note-off pair with no status byters
    // at the original MIDI baud rate.
    min_note_len_ = 0.00128 + rd();
    max_note_len_ = min_note_len_ + rd() * 2.0;
    // scale
    pulse_ = rd() * 16.0;

    // We are finding the cumulative probabilities if walking has
    // been randomly selected.
    // If tacit is allowed to be random to 1.0, you can
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

    const int32_t MaxRandomVoiceQty{24};
    voices_.resize(ri() % MaxRandomVoiceQty + 1);

    scale_.clear();
    const auto scale_index{ri() % scale_strs.size()};
    transform(scale_strs[scale_index], back_inserter(scale_),
        [](const string_view& strv)
        { return string{strv.data(), strv.length()}; } );
    vector<int32_t> programs{};
    if (GeneralMIDIGroup::All
        == static_cast<GeneralMIDIGroup>(instrument_flags))
    {
        auto program_count = iota(1, midi::MidiProgramQty + 1);
        copy(program_count, back_inserter(programs));
    }
    else
    {
        if (GeneralMIDIGroup::Melodic
            == static_cast<GeneralMIDIGroup>(instrument_flags))
        {
            auto program_count = iota(1, 96 + 1);
            copy(program_count, back_inserter(programs));
        }
        else
        {
            for (int32_t flag{0}; flag < 17; ++flag)
            {
                if (instrument_flags & (1 << flag))
                {
                    if (static_cast<GeneralMIDIGroup>(instrument_flags & (1 << 16))
                        == GeneralMIDIGroup::Idiophone)
                    {
                        for (int32_t n{}; n < MIDIInstrumentsPerGroup; ++n)
                        {
                            programs.push_back(IdiophoneMarker);
                        }
                    }
                    else
                    {
                        for (int32_t n{flag * MIDIInstrumentsPerGroup};
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
    // (General MIDI puts idiophones on channel 10 in (1..16).)
    vector<int32_t> melodic_channels{};
    auto counter{iota(1, midi::MidiChannelQty + 1)};
    copy(counter, back_inserter(melodic_channels));
    auto mc_it{find(melodic_channels, MidiIdiophoneChannel)};
    if (mc_it != melodic_channels.end())
    {
        melodic_channels.erase(mc_it);
    }

    for (int32_t vctr{}; auto& v : voices_)
    {
        v.channel(melodic_channels[ri() % melodic_channels.size()]);
        v.walking(rd());
        v.follower(VoiceXml::Follower{});
        // don't have more than 1/3 of voices be followers
        auto follower{(ri() % 3) == 0};
        auto leader{ri() % voices_.size()};
        // if this is a self-follower, then don't be a follower.
        if (static_cast<size_t>(vctr) == leader)
        {
            follower = false;
        }
        if (follower && !voices_[leader].follower().follow())
        {
            v.follower().follow(true);
            v.follower().leader(leader);
            v.follower().interval_type(static_cast<VoiceXml::Follower::
                IntervalType>(ri() % 2 + 1));
            v.follower().delay(rational::RhythmRational{
                static_cast<int64_t>(ri() / 2) % (TicksPerQuarter * 4L),
                TicksPerQuarter * 4L});
            v.follower().inversion((ri() % 2) == 1);
            v.follower().retrograde((ri() % 2) == 1);
        }
        ++vctr;
    }
    vector<int32_t> channels(voices_.size());
    transform(voices_, channels.begin(),
           [](const VoiceXml& v) { return v.channel(); });
    sort(channels);
    const auto [uniq_it, the_end] = unique(channels);
    channels.erase(uniq_it, the_end);
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
    const int32_t stereo_zones{static_cast<int32_t>(channels.size() + 1)};
    const auto pan_step{(midi::PanExcess64 * 2) / stereo_zones};
    const auto first_pan{pan_step - midi::PanExcess64};
    map<int32_t, int32_t> channel_to_pan;
    int32_t pan(first_pan);
    for_each(channels, [&](int32_t ch) { channel_to_pan[ch] = pan;
        pan += pan_step; } );

    for_each(voices_, [&](VoiceXml& v) {
        v.pan(channel_to_pan[v.channel()]); });

    map<int32_t, int32_t> channel_to_program;
    for_each(channels, [&](int32_t ch) {
        channel_to_program[ch] = programs[ri() % programs.size()];} );
    for (auto& v : voices_)
    {
        const auto program_num{channel_to_program[v.channel()]};
        v.low_pitch(midi_programs[program_num].range_.first);
        v.high_pitch(midi_programs[program_num].range_.second);
        // If idiophones were selected, then coerce channel to 10.
        if (IdiophoneMarker == program_num)
        {
            v.program(lexical_cast<string>(1));
            v.channel(IdiophoneChannel);
            v.low_pitch(IdiophoneRange.first);
            v.high_pitch(IdiophoneRange.second);
        }
        else
        {
            v.program(lexical_cast<string>(program_num));
        }
    }
    // Coerce idiophones to be pan-centered.
    // It doesn't matter whether it's already in the map.
    channel_to_pan[10] = 0;
    // truncate scale to not exceed the instrument ranges.
    // The scales provided are monotonically increasing but
    // user-edited forms may have non-monotonic scales.
    // This algorithm assume a monotonically increasing scale.
    // Find the maximum instrument range note.
    CompareLowerNoteName lower_note{};
    VoiceXml min_instrument_range{},
             max_instrument_range{};
    if (!voices_.empty())
    {
        const auto [minv, maxv] = std::ranges::minmax_element(voices_,
            [lower_note](const VoiceXml& vleft, const VoiceXml& vright){
            return lower_note(vleft.high_pitch(), vright.high_pitch()); });
        min_instrument_range = *minv;
        max_instrument_range = *maxv;
    }
    else
    {
        cerr << "Empty voice list!  Exitting!";
        exit(EXIT_SUCCESS);
    }
    auto rem_if_scale_pitch_is_outside_instrument_range =
        [max_instrument_range, min_instrument_range, lower_note]
        (const string& scale_pitch){
        return lower_note(scale_pitch, min_instrument_range.low_pitch())
            || lower_note(max_instrument_range.high_pitch(), scale_pitch); };
    const auto [erase_iter, the_end2] =
        remove_if(scale_, rem_if_scale_pitch_is_outside_instrument_range);
    scale_.erase(erase_iter, the_end2);
    // scale_ should now just cover the maximum extent
    // of the instruments' ranges.
    // 10 algorithms, plus 0 undefined
    arrangement_definition().algorithm(
        static_cast<arrangements::PermutationEnum>(ri() % 10 + 1));
    arrangement_definition().period(rd() * 30.0);
}

bool MusicalForm::valid() const
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

void MusicalForm::clamp_scale_to_instrument_ranges() noexcept
{
    if (!voices_.empty())
    {
        auto min_low_pitch = [](const VoiceXml& v1, const VoiceXml& v2)
            { return CompareLowerNoteName()(v1.low_pitch(), v2.low_pitch());};
        const auto voice_min_note{
            min_element(voices_, min_low_pitch)->low_pitch()};
        auto max_high_pitch = [](const VoiceXml& v1, const VoiceXml& v2)
            { return CompareLowerNoteName()(v1.high_pitch(),
              v2.high_pitch());};
        const auto voice_max_note{max_element(voices_, max_high_pitch)->high_pitch()};
        auto out_of_range = [voice_min_note, voice_max_note] (const string& note_name)
            { return CompareLowerNoteName()(note_name, voice_min_note)
            || CompareLowerNoteName()(voice_max_note, note_name); };
        const auto [undefined, theend] = remove_if(scale_, out_of_range);
        scale_.erase(undefined, theend);
    }
}

textmidi::cgm::RandomDouble textmidi::cgm::rd{};
textmidi::cgm::RandomInt    textmidi::cgm::ri{};
