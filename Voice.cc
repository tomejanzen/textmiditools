//
// TextMIDITools Version 1.1.6
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Voice represents a voice for textmidicgm.
// It holds the attributes of pitch range, channel, program,
// pan and follower flag.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <string>

#include "Voice.h"

using std::string, std::string_view;
using midi::MidiProgramQty;
using textmidi::cgm::VoiceXml;
using textmidi::rational::RhythmRational;

string VoiceXml::low_pitch() const noexcept
{
    return low_pitch_;
}

void VoiceXml::low_pitch(const string_view& low_pitch) noexcept
{
    low_pitch_ = low_pitch;
}

string VoiceXml::high_pitch() const noexcept
{
    return high_pitch_;
}

void VoiceXml::high_pitch(const string_view& high_pitch) noexcept
{
    high_pitch_ = high_pitch;
}

uint32_t VoiceXml::channel() const noexcept
{
    return channel_;
}

void VoiceXml::channel(uint32_t channel) noexcept
{
    channel_ = channel;
}

double VoiceXml::walking() const noexcept
{
    return walking_;
}

void VoiceXml::walking(double walking) noexcept
{
    walking_ = walking;
}

string VoiceXml::program() const noexcept
{
    return program_;
}

void VoiceXml::program(const string& program) noexcept
{
    program_ = program;
}

int32_t VoiceXml::pan() const noexcept
{
    return pan_;
}

void VoiceXml::pan(int32_t pan) noexcept
{
    pan_ = pan;
}

const VoiceXml::Follower& VoiceXml::follower() const noexcept
{
    return follower_;
}

VoiceXml::Follower& VoiceXml::follower() noexcept
{
    return follower_;
}

void VoiceXml::follower(const Follower& follower) noexcept
{
    follower_ = follower;
}

const VoiceXml::RandomProgram& VoiceXml::random_program() const noexcept
{
    return random_program_;
}

VoiceXml::RandomProgram& VoiceXml::random_program() noexcept
{
    return random_program_;
}

void VoiceXml::random_program(const RandomProgram& random_program) noexcept
{
    random_program_ = random_program;
}

bool VoiceXml::valid() const
{
    try
    {
        textmidi::pitchname_to_keynumber(low_pitch_);
    }
    catch (KeyStringException& kse)
    {
        throw VoiceException{kse.what() + " low_pitch "};
    }
    try
    {
        textmidi::pitchname_to_keynumber(high_pitch_);
    }
    catch (KeyStringException& kse)
    {
        throw VoiceException{kse.what() + " high_pitch "};
    }
    if (!((channel_ >= 1) && (channel_ <= midi::MidiChannelQty)))
    {
        throw VoiceException{
        (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
            += " Invalid voice channel: ") += boost::lexical_cast<string>(channel_)};
    }

    if (!((walking_ >= 0.0) && (walking_ <= 1.0)))
    {
        throw VoiceException{
        (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
            += " Invalid voice walking must be from 0.0 to 1.0: ") += boost::lexical_cast<string>(walking_)};
    }

    int prog = boost::lexical_cast<int>(program_);
    if (!((prog >= 1) && (prog <= MidiProgramQty)))
    {
        throw VoiceException{
        (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
            += " Invalid program number: ") += program_};
    }

    // 0 = center, negative is left, positive is right.
    if (!((pan_ >= midi::MinSignedPan) && (pan_ <= midi::MaxSignedPan)))
    {
        throw VoiceException{
        (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
            += " Invalid 7-bit pan: ") += pan_};
    }
    try
    {
        follower_.valid();
    }
    catch (VoiceException& mfe)
    {
        throw VoiceException{mfe.what() + " follower "};
    }
    try
    {
        random_program_.valid();
    }
    catch (VoiceException& mfe)
    {
        throw VoiceException{mfe.what() + " random_program "};
    }
    return true;
}

bool VoiceXml::Follower::follow() const
{
    return follow_;
}

int32_t VoiceXml::Follower::leader() const
{
    return leader_;
}

VoiceXml::Follower::IntervalType VoiceXml::Follower::interval_type() const
{
    return interval_type_;
}

int32_t VoiceXml::Follower::interval() const
{
    return interval_;
}

const RhythmRational& VoiceXml::Follower::delay() const
{
    return delay_;
}

const RhythmRational& VoiceXml::Follower::duration_factor() const
{
    return duration_factor_;
}

bool VoiceXml::Follower::inversion() const
{
    return inversion_;
}

bool VoiceXml::Follower::retrograde() const
{
    return retrograde_;
}

void VoiceXml::Follower::follow(bool follow)
{
    follow_ = follow;
}

void VoiceXml::Follower::leader(int32_t leader)
{
    leader_ = leader;
}

void VoiceXml::Follower::interval_type(IntervalType interval_type)
{
    interval_type_ = interval_type;
}

void VoiceXml::Follower::interval(int32_t interval)
{
    interval_ = interval;
}

void VoiceXml::Follower::delay(const RhythmRational& delay)
{
    delay_ = delay;
}

void VoiceXml::Follower::
    duration_factor(const RhythmRational& duration_factor)
{
    duration_factor_ = duration_factor;
}

void VoiceXml::Follower::inversion(bool inversion)
{
    inversion_ = inversion;
}

void VoiceXml::Follower::retrograde(bool retrograde)
{
    retrograde_ = retrograde;
}

void VoiceXml::RandomProgram::probability(double probability)
{
    probability_ = probability;
}

void VoiceXml::RandomProgram::ensemble(const RandomEnsemble& ensemble)
{
    ensemble_ = ensemble;
}

double VoiceXml::RandomProgram::probability() const
{
    return probability_;
}

VoiceXml::RandomProgram::RandomEnsemble VoiceXml::RandomProgram::ensemble() const
{
    return ensemble_;
}

bool VoiceXml::Follower::valid() const
{
    // follow_: bad bools are reported as stream errors by boost serialization XML archives.

    if (leader_ < 0)
    {
        throw VoiceException{
        (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
            += " Invalid leader index: ") += boost::lexical_cast<string>(leader_)};
    }

    // interval_type_
    switch (interval_type_)
    {
        case IntervalType::Neither:
        case IntervalType::Scalar:
        case IntervalType::Chromatic:
            break;
        default:
            throw VoiceException{
            (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
                += " Invalid follower interval_type : ")
                += boost::lexical_cast<string>(static_cast<int>(interval_type_))};
            break;
    }
    // interval_ can range freely at your own risk.  Typically an octave
    // (for chromatic interval, -12 to 12).

    if (delay_ < RhythmRational(0))
    {
        throw VoiceException{
        (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
            += " Invalid delay: ") += boost::lexical_cast<string>(delay_)};
    }

    if (duration_factor_ <= RhythmRational(0))
    {
        throw VoiceException{
        (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
            += " Invalid duration_factor: ") += boost::lexical_cast<string>(duration_factor_)};
    }
    // inversion_; bad bool is a stream error in boost serialization XML archives
    // retrograde_; bad bool is a stream error in boost serialization XML archives
    return true;
}

bool VoiceXml::RandomProgram::valid() const
{
    if (!((probability_ >= 0.0) && (probability_ <= 1.0)))
    {
        throw VoiceException{
        (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
            += " Invalid random_program_probability, must be 0.0 to 1.0: ") += boost::lexical_cast<string>(probability_)};
    }
    for (const auto patch : ensemble_)
    {
        if (!((patch >= 0) && (patch < midi::MidiProgramQty)))
        {
            throw VoiceException{
            (((string(__FILE__) += ':') += BOOST_PP_STRINGIZE(__LINE__))
                += " Invalid random_program ensemble program: ") += boost::lexical_cast<string>(patch)};
        }
    }
    return true;
}
