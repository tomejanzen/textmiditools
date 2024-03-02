//
// TextMIDITools Version 1.0.70
//
// textmidi 1.0.6
// Copyright © 2024 Thomas E. Janzen
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
using namespace textmidi;
using namespace textmidi::cgm;

string VoiceXml::low_pitch() const noexcept
{
    return low_pitch_;
}

void VoiceXml::low_pitch(string_view low_pitch) noexcept
{
    low_pitch_ = low_pitch;
}

string VoiceXml::high_pitch() const noexcept
{
    return high_pitch_;
}

void VoiceXml::high_pitch(string_view high_pitch) noexcept
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

void VoiceXml::program(string program) noexcept
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

void VoiceXml::follower(const Follower& follower) noexcept
{
    follower_ = follower;
}

