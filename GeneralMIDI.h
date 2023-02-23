//
// TextMIDITools Version 1.0.41
//
// textmidicgm 1.0
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(GENERALMIDI_H)
#    define  GENERALMIDI_H

#include <string>
#include <utility>
#include <map>

namespace textmidi
{
    namespace cgm
    {
        const std::pair IdiophoneRange{std::string{"Eb1"}, std::string{"Eb6"}};
        constexpr int MIDIInstrumentsPerGroup{8};
        constexpr int IdiophoneChannel{10};
        enum class GeneralMIDIGroup
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
            Melodic             = 4095,
            SynthEffects        = (1 << 12),
            Ethnic              = (1 << 13),
            Percussive          = (1 << 14),
            SoundEffects        = (1 << 15),
            All                 = 65535,
            Idiophone           = (1 << 16)
        };

        extern std::map<std::string_view, GeneralMIDIGroup> program_group_map;

        constexpr auto MaxNoteStrLen{6}; // "Db-1" + null is 5
        constexpr auto MaxMIDIProgramNameLen{48};

        struct MIDI_Program
        {
            int Number_;
            const std::string_view Name_;
            std::pair<std::string_view, std::string_view> range_;
        };

        const extern MIDI_Program midi_programs[];

    }
}

#endif

