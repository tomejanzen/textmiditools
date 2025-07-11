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

#include <memory>

#include "MIDIKeyString.h"
#include "NoteEvent.h"

using textmidi::cgm::NoteEvent;

void NoteEvent::pitch(int32_t pitch) noexcept
{
    pitch_ = pitch;
}

int32_t NoteEvent::pitch() const noexcept
{
    return pitch_;
}

int32_t NoteEvent::vel() const noexcept
{
    return vel_;
}

textmidi::rational::RhythmRational NoteEvent::rhythm() const noexcept
{
    return rhythm_;
}

//
// Write a note event as a textmidi text event.
std::ostream& textmidi::cgm::operator<<(std::ostream& os, const NoteEvent& ne)
{
    auto flags{os.flags()};
    std::shared_ptr<bool> prefer_sharp{std::make_shared<bool>()};
    if (RestPitch == ne.pitch_)
    {
        os << "R ";
    }
    else
    {
        const auto note_name(textmidi::num_to_note(ne.pitch_, prefer_sharp));
        os << note_name << ' ';
    }
    (*textmidi::rational::print_rhythm)(os, ne.rhythm_);
    static_cast<void>(os.flags(flags));
    return os;
}
