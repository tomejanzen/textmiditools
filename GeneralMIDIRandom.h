//
// TextMIDITools Version 1.1.2
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(GENERALMIDIRANDOM_H)
#    define  GENERALMIDIRANDOM_H

#include <vector>
#include <tuple>

#include "Midi.h"
#include "GeneralMIDI.h"
#include "MIDIKeyString.h"
#include "RandomInt.h"
#include "MusicalForm.h"

namespace textmidi
{
    namespace cgm
    {
        class GeneralMIDIRandom
        {
          public:
            explicit GeneralMIDIRandom(const VoiceXml& voice, bool ignore_range = false)
              : voice_(voice),
                scale_coverage_(),
                previous_program_{},
                ignore_range_{ignore_range}
            {
                if (!ignore_range)
                {
                    build_scale_coverage();
                }
            }

            std::tuple<bool, int> random_program(int key);

            ~GeneralMIDIRandom()
            {}
          private:
            // this probably belongs in a closer association with GeneralMIDI.h
            void build_scale_coverage();
            const textmidi::cgm::VoiceXml voice_;
            std::vector<std::vector<int>> scale_coverage_;
            int previous_program_;
            bool ignore_range_;
        };

    } // namespace cgm
} // namespace textmidi

#endif

