//
// TextMIDITools Version 1.0.98
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// 2020-08-06
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cstdlib>

#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "Midi.h"
#include "MIDIKeyString.h"

using std::unordered_map, std::regex, std::smatch, std::string, std::int32_t, std::cerr,
    std::pair, std::make_pair, std::size_t, std::hash;
using midi::KeyNumBool, midi::KeyNumBoolEqual;
using boost::lexical_cast;

namespace
{
    // This map is actually from the letter of a note in ASCII - 'A'
    // to a musical note in which C is 0.
    const unordered_map<char, int> noteletter_to_halfstep
        {{0, 9}, {1, 11}, {2, 0}, {3, 2}, {4, 4}, {5, 5}, {6, 7} };
    // This map adjusts a pitch number by the effect of an accidental
    // b is flat
    // # is sharp
    // x is double sharp
    // bb as double flat is handled separately.
    const unordered_map<char, int> accidental_to_deltahalfstep{{'b', -1},
                                                          {'#', 1},
                                                          {'x', 2}};
    // Note names that begin with plus or minus (+|-)
    // are deltas.  They can only be "K" key numbers, and step
    // in the direction from the previous note in the current track
    // as specified.
    const unordered_map<char, int> sign_to_deltahalfstep{{'-', -1}, {'+', 1}};

    //
    // notename_to_halfstep converts a note name string into
    // an integer MIDI key number in the bottom octave, which starts with 0.
    int32_t notename_to_halfstep(const string& notename)
    {
        int32_t halfstep{};
        smatch matches{};
        // To find the index into matches[], count the left parantheses
        // in the regular expression, start with 0.
        const regex notename_re{R"(([A-Ga-g])(([b#x]?)|(bb)))"};
        const auto mat{regex_match(notename, matches, notename_re)};
        if (!mat)
        {
            cerr << "incorrect note name: " << notename << '\n';
        }
        else
        {
            constexpr int32_t match_note_letter{1};
            constexpr int32_t match_accidental_or_double_flat{2};
            constexpr int32_t match_accidental{3};
            constexpr int32_t match_double_flat{4};
            if (!matches[match_note_letter].str().empty()) [[likely]]
            {
                auto iter{noteletter_to_halfstep.find
                    (toupper(matches[match_note_letter].str()[0]) - 'A')};
                if (iter !=noteletter_to_halfstep.end())
                {
                    halfstep = iter->second;
                }
            }
            if (!matches[match_accidental].str().empty())
            {
                auto iter{accidental_to_deltahalfstep.
                    find(matches[match_accidental_or_double_flat].str()[0])};
                if (iter !=accidental_to_deltahalfstep.end())
                {
                    halfstep += iter->second;
                }
            }
            else
            {
                // double-flat
                if (!matches[match_double_flat].str().empty()) [[unlikely]]
                {
                    halfstep -= 2;
                }
            }
        }
        return halfstep;
    }

    // note_name_to_scale_step converts a note_name string
    // to a scale step number for key signatures in textmidi.
    // Double flats and double sharps are not supported in key signatures.
    int32_t note_name_to_scale_step(const string& note_name, int32_t* accidental)
        noexcept
    {
        // The diatonic scale.
        constexpr std::array<uint32_t, 7> step_array{0, 2, 4, 5, 7, 9, 11};
        int32_t step{};
        // The musical scale starts with C but the alphabet starts with A.
        if (!note_name.empty()) [[likely]]
        {
            int32_t stepindex{toupper(note_name[0]) - 'A'};
            stepindex += 5;
            stepindex %= 7;
            step = step_array[stepindex];
        }
        *accidental = 0;
        if (note_name.size() == 2)
        {
            if (accidental_to_deltahalfstep.contains(note_name[1]))
            {
                *accidental = accidental_to_deltahalfstep.at(note_name[1]);
                step += *accidental;
            }
        }
        return step;
    }
    using NoteStringBool = pair<string, bool>;
} // namespace

namespace
{
    // key_number+prefer_sharp => PitchString+prefer_sharp
    std::unordered_map<KeyNumBool, NoteStringBool> PitchnumToPitchName
        = {
        {{ 0, true }, {"C",  false}},
        {{ 1, true }, {"C#", true }},
        {{ 2, true }, {"D",  true }},
        {{ 3, true }, {"D#", true }},
        {{ 4, true }, {"E",  true }},
        {{ 5, true }, {"F",  false}},
        {{ 6, true }, {"F#", true }},
        {{ 7, true }, {"G",  true }},
        {{ 8, true }, {"G#", true }},
        {{ 9, true }, {"A",  true }},
        {{10, true }, {"A#", true }},
        {{11, true }, {"B",  true }},
        {{ 0, false}, {"C",  false}},
        {{ 1, false}, {"Db", false}},
        {{ 2, false}, {"D",  false}},
        {{ 3, false}, {"Eb", false}},
        {{ 4, false}, {"E",  true }},
        {{ 5, false}, {"F",  false}},
        {{ 6, false}, {"Gb", false}},
        {{ 7, false}, {"G",  false}},
        {{ 8, false}, {"Ab", false}},
        {{ 9, false}, {"A",  true }},
        {{10, false}, {"Bb", false}},
        {{11, false}, {"B",  true }}
    };
} // namespace

//
// Public function.
// Convert a MIDI key number to a string.
//
string textmidi::num_to_note(int32_t keynum, std::shared_ptr<bool> prefer_sharp) noexcept
{
    const int32_t octave{keynum / halfsteps_per_octave - 1};
    //
    // prefer_sharp if true will tend to encourage the use of sharps
    // for accidentals.
    // prefer_sharp if false will tend to encourage the use of flats
    // for accidentals.
    // This makes is more likely that a piece that is basically in Bb will be
    // written out by miditext as all in flatted notes,
    // and a piece in A major should come out in sharped notes.
    //
    // {Pitchnum within an octave, prefersharp} TO {Pitch string, prefershapr}

    const auto note_pair{make_pair(keynum % halfsteps_per_octave, *prefer_sharp)};
    if (PitchnumToPitchName.contains(note_pair))
    {
        const auto nn{PitchnumToPitchName.at(note_pair)};
        *prefer_sharp = nn.second;
        return nn.first + lexical_cast<string>(octave);
    }
    return string{};
}

// key_sig_name_to_accidentals
// returns an integer per MIDI spec 1.1
//
pair<int32_t, bool> textmidi::
    key_sig_name_to_accidentals(const string& key_sig_name)
{
    uint32_t step{};
    int32_t accidental{};
    step = note_name_to_scale_step(key_sig_name, &accidental);
    step += halfsteps_per_octave; // it can go negative for C-flat, so force +
    step %= halfsteps_per_octave;

    // is_minor_mode is false for major and true for minor.
    auto is_minor_mode{(std::isupper(key_sig_name[0]) != 0) ? false : true};

    // force it to a major key for the algorithm below
    // MIDI SFF requires a count of sharps or -flats in the key signature.
    // Also a flag of 0 for Major or 1 for Minor keys.
    if (is_minor_mode)
    {
        step += 3;
        step %= halfsteps_per_octave;
    }
    int32_t fifths{};
    uint32_t arpeggio{};
    constexpr std::array<int32_t, halfsteps_per_octave>
        accidentals{0, 1, 2, 3, 4, 5, 6, 7, -4, -3, -2, -1};
    // find the least common multiple of step and 7, mod 12.
    constexpr int32_t fifth{7};
    while (arpeggio != step)
    {
        arpeggio += fifth;
        arpeggio %= halfsteps_per_octave;
        ++fifths;
    }
    // Adjust for the two enharmonic keys, GbMaj and C#Maj
    int32_t tempkey{accidentals[fifths]};

    if (-1 == accidental)
    {
        switch (tempkey)
        {
          case 5:
            // is B  Major; change to Cb Major.
          case 6:
            // is F# Major; change to Gb Major.
          case 7:
            // is C# Major, change to Db Major.
            tempkey = -(halfsteps_per_octave - tempkey);
            break;
        }
    }
    return make_pair(tempkey, is_minor_mode);
}

//
// pitchname_to_keynumber
// converts a string such as "C4" to 60, or "K60" to 60,
// or "K-1" to -1 with is_delta set true.
// MIDI supports values from "C-1" to "G9" or K0 to K127.
//
pair<int32_t, bool> textmidi::pitchname_to_keynumber(const string& pitchname)
{
    int32_t keynumber{midi::MiddleC};
    smatch matches{};
    const regex pitchname_re
        {R"(((([A-Ga-g])(([b#x]?)|(bb)))(-?)([0-9]))|(([Kk])([+-]?)([[:digit:]]+)))"};
    const auto mat{regex_match(pitchname, matches, pitchname_re)};
    bool is_delta{};
    if (!mat)
    {
        cerr << "Improper pitch name: " << pitchname << '\n';
    }
    else
    {
        constexpr int32_t match_note{2};
        constexpr int32_t match_neg_octave{7};
        constexpr int32_t match_octave{8};
        constexpr int32_t match_k{10};
        constexpr int32_t match_key_step{11};
        constexpr int32_t match_key_number{12};
        if (matches[match_k].str().empty()) [[likely]] // is this NOT a K event
        {
            if (!matches[match_note].str().empty()) [[unlikely]]
            {
                keynumber = notename_to_halfstep(matches[match_note].str());
                int32_t octave{lexical_cast<int32_t>(
                    matches[match_octave].str())};
                // if negative octave (there is only octave -1)
                if (!matches[match_neg_octave].str().empty() && (1 == octave))
                {
                    octave = -octave;
                }
                const auto temp_octave = octave + 1;
                keynumber += (halfsteps_per_octave * temp_octave);
            }
        }
        // This is a K for key-number pitchname and might be a delta.
        else [[unlikely]]
        {
            keynumber = lexical_cast<int32_t>(matches[match_key_number]);
            const auto the_sign{matches[match_key_step].str()[0]};
            if (sign_to_deltahalfstep.contains(the_sign))
            {
                keynumber *= sign_to_deltahalfstep.at(the_sign);
                is_delta = true;
            }
        }
    }
    return make_pair(keynumber, is_delta);
}

bool textmidi::CompareLowerNoteName::operator()(
    const string& left, const string& right) const noexcept
{
    return pitchname_to_keynumber(left) < pitchname_to_keynumber(right);
}

