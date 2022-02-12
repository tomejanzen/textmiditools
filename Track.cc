//
// TextMIDITools Version 1.0.18
//
// textmidicgm 1.0.16
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Track represents a single MIDI Track.
//
#include "Track.h"

using namespace cgm;
using namespace std;

ostream& cgm::operator<<(std::ostream& os, const cgm::MusicalRhythm& mr)
{
    if (1 == mr.numerator())
    {
        os << mr.denominator();
    }
    else
    {
        os << mr.numerator() << "/"
           << mr.denominator();
    }
    return os;
}

TicksDuration cgm::Track::the_next_time() const
{
    return the_next_time_;
}
int cgm::Track::last_pitch_index() const
{
    return last_pitch_index_;
}

void cgm::Track::the_last_time(const TicksDuration the_last_time)
{
    the_last_time_ = the_last_time;
}
void cgm::Track::the_next_time(const TicksDuration the_next_time)
{
    the_next_time_ = the_next_time;
}

void cgm::Track::last_pitch_index(const int last_pitch_index)
{
    last_pitch_index_ = last_pitch_index;
}

