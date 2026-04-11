//
// TextMIDITools Version 1.1.2
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#include <tuple>
#include <utility>
#include "GeneralMIDIRandom.h"

using std::tuple;
using midi::MidiPitchQty, midi::MaxKeyboardKey;

void textmidi::cgm::GeneralMIDIRandom::build_scale_coverage()
{
    scale_coverage_.clear();
    scale_coverage_.resize(MidiPitchQty);
    for (int pitch{1}; pitch <= MidiPitchQty; ++pitch)
    {
        for (auto prog : voice_.random_program().ensemble())
        {
            // why is midi_programs a C array?
            if (midi_programs[prog].range_nums_.first <= pitch
                && pitch <= midi_programs[prog].range_nums_.second)
            {
                scale_coverage_[pitch].push_back(prog);
            }
        }
    }
}

tuple<bool, int> textmidi::cgm::GeneralMIDIRandom::random_program(int key)
{
    const bool pick_random_program{rd() < voice_.random_program().probability()};
    // Return (false, 0) : no change, no PROGRAM statuement
    // Return (true,  n) : change program to program n
    // Return (true,  0) : out of range, make a rest
    const auto program_base(0);
    const auto ensemble{(voice_.random_program().ensemble().size()) - program_base};
    bool is_new_program{false};
    int program{0};
    if (pick_random_program)
    {
        if (ignore_range_)
        {
            if (0 == ensemble)
            {
                program = program_base;
                is_new_program = (previous_program_ != program);
                previous_program_ = program;
            }
            else
            {
                const auto program_offset{ri() % ensemble};
                program = program_base + program_offset;
                is_new_program = (previous_program_ != program);
                previous_program_ = program;
            }
        }
        else
        {
            if ((key <= MaxKeyboardKey) && scale_coverage_[key].size())
            {
                auto program_qty{scale_coverage_[key].size()};
                if (program_qty == 0)
                {
                    program = 0;
                    is_new_program = true;
                }
                else
                {
                    program = scale_coverage_[key][ri() % program_qty];
                    is_new_program = (previous_program_ != program);
                    previous_program_ = program;
                }
            }
        }
    }
    return tuple<bool, int>(is_new_program, program);
}

