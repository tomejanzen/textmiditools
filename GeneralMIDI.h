//
// TextMIDITools Version 1.0.98
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(GENERALMIDI_H)
#    define  GENERALMIDI_H

#include <cstdint>

#include <map>
#include <string>
#include <utility>

#include "Midi.h"
#include "MidiMaps.h"

namespace textmidi
{
    namespace cgm
    {
        const std::pair IdiophoneRange{std::string{"Eb1"}, std::string{"Eb6"}};
        inline constexpr std::int32_t MIDIInstrumentsPerGroup{8};
        inline constexpr std::int32_t IdiophoneChannel{10};
        enum class GeneralMIDIGroup : std::int32_t
        {
            Piano               = (1 << 0),
            ChromaticPercussion = (1 << 1),
            Organ               = (1 << 2),
            Guitar              = (1 << 3),
            Bass                = (1 << 4),
            Strings             = (1 << 5),
            Ensemble            = (1 << 6),
            Brass               = (1 << 7),
            Reed                = (1 << 8),
            Pipe                = (1 << 9),
            SynthLead           = (1 << 10),
            SynthPad            = (1 << 11),
            Melodic             = 0b111111111111,
            SynthEffects        = (1 << 12),
            Ethnic              = (1 << 13),
            Percussive          = (1 << 14),
            SoundEffects        = (1 << 15),
            All                 = 0b1111111111111111,
            Idiophone           = (1 << 16)
        };

        extern const midi::NumStringMap<GeneralMIDIGroup> program_group_map;

        inline constexpr auto MaxNoteStrLen{6}; // "Db-1" + null is 5
        inline constexpr auto MaxMIDIProgramNameLen{48};

        struct MIDI_Program
        {
            std::int32_t Number_;
            const std::string_view Name_;
            std::pair<std::string_view, std::string_view> range_;
        };

        extern const MIDI_Program midi_programs[];
    } // namespace cgm
} // namespace textmidi

#endif

