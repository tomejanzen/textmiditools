//
// textmidi 1.0
// Copyright © 2021 Thomas E. Janzen
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

#include "Voice.h"

using namespace std;
using namespace cgm;

std::string cgm::VoiceXml::low_pitch() const noexcept
{
    return low_pitch_;
}

void cgm::VoiceXml::low_pitch(std::string_view low_pitch) noexcept
{
    low_pitch_ = low_pitch;
}

std::string cgm::VoiceXml::high_pitch() const noexcept
{
    return high_pitch_;
}

void cgm::VoiceXml::high_pitch(std::string_view high_pitch) noexcept
{
    high_pitch_ = high_pitch;
}

uint32_t cgm::VoiceXml::channel() const noexcept
{
    return channel_;
}

void cgm::VoiceXml::channel(uint32_t channel) noexcept
{
    channel_ = channel;
}

bool cgm::VoiceXml::walking() const noexcept
{
    return walking_;
}

void cgm::VoiceXml::walking(bool walking) noexcept
{
    walking_ = walking;
}

std::string cgm::VoiceXml::program() const noexcept
{
    return program_;
}

void cgm::VoiceXml::program(std::string program) noexcept
{
    program_ = program;
}

int32_t cgm::VoiceXml::pan() const noexcept
{
    return pan_;
}

void cgm::VoiceXml::pan(int32_t pan) noexcept
{
    pan_ = pan;
}

const cgm::VoiceXml::Follower& cgm::VoiceXml::follower() const noexcept
{
    return follower_;
}

void cgm::VoiceXml::follower(const Follower& follower) noexcept
{
    follower_ = follower;
}

