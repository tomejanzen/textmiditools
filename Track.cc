//
// TextMIDITools Version 1.0.97
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Track represents a single MIDI Track.
//
#include "Track.h"

textmidi::cgm::TicksDuration
    textmidi::cgm::Track::the_next_time() const noexcept
{
    return the_next_time_;
}
int32_t textmidi::cgm::Track::last_pitch_index() const noexcept
{
    return last_pitch_index_;
}

void textmidi::cgm::Track::
    the_last_time(const textmidi::cgm::TicksDuration the_last_time) noexcept
{
    the_last_time_ = the_last_time;
}
void textmidi::cgm::Track::
    the_next_time(const textmidi::cgm::TicksDuration the_next_time) noexcept
{
    the_next_time_ = the_next_time;
}

void textmidi::cgm::Track::
    last_pitch_index(const int32_t last_pitch_index) noexcept
{
    last_pitch_index_ = last_pitch_index;
}






















