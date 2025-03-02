//
// TextMIDITools Version 1.0.91
//
// MIDIKeyString.cc
//
// textmiditools 1.0
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// 2020-08-06
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */
#include <string>
#include <regex>
#include <map>

#include <cstdlib>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include "Midi.h"
#include "MIDIKeyString.h"

using namespace std;
using namespace boost;
using namespace textmidi;
using namespace midi;

namespace {
    // This map is actually from the letter of a note in ASCII - 'A'
    // to a musical note in which C is 0.
    const map<char, int> noteletter_to_halfstep
        {{0, 9}, {1, 11}, {2, 0}, {3, 2}, {4, 4}, {5, 5}, {6, 7} };
    // This map adjusts a pitch number by the effect of an accidental
    // b is flat
    // # is sharp
    // x is double sharp
    // bb as double flat is handled separately.
    const map<char, int> accidental_to_deltahalfstep{{'b', -1},
                                                          {'#', 1},
                                                          {'x', 2}};
    // Note names that begin with plus or minus (+|-)
    // are deltas.  They can only be "K" key numbers, and step
    // in the direction from the previous note in the current track
    // as specified.
    const map<char, int> sign_to_deltahalfstep{{'-', -1}, {'+', 1}};

    //
    // notename_to_halfstep converts a note name string into
    // an integer MIDI key number in the bottom octave, which starts with 0.
    int notename_to_halfstep(const string& notename)
    {
        int halfstep{};
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
            constexpr int match_note_letter{1};
            constexpr int match_accidental_or_double_flat{2};
            constexpr int match_accidental{3};
            constexpr int match_double_flat{4};
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
                auto iter{accidental_to_deltahalfstep.find(matches[match_accidental_or_double_flat].str()[0])};
                if (iter !=accidental_to_deltahalfstep.end())
                {
                    halfstep += iter->second;
                }
            }
            else
            {
                if (!matches[match_double_flat].str().empty()) [[unlikely]] // double-flat
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
    int note_name_to_scale_step(const string &note_name, int32_t &accident) noexcept
    {
        // The diatonic scale.
        constexpr std::array<uint32_t, 7> step_array{0, 2, 4, 5, 7, 9, 11};
        int32_t step{};
        if (note_name.size() > 0) [[likely]]
        {
            int stepindex{toupper(note_name[0]) - 'A'};
            stepindex += 5;
            stepindex %= 7;
            step = step_array[stepindex];
        }
        accident = 0;
        switch (note_name.size())
        {
          case 1:
            break;
          case 2:
            switch (note_name[1])
            {
              case 'b':
                accident = -1;
              break;
              case '#':
                accident = 1;
                break;
              case 'N':
                accident = 0;
                break;
            }
            break;
          default:
          break;
        }
        step += accident;
        return step;
    }

}

//
// Public function.
// Convert a MIDI key number to a string.
//
string textmidi::num_to_note(int num, std::shared_ptr<bool> prefer_sharp) noexcept
{
    const int octave{num / 12 - 1};
    //
    // prefer_sharp if true will tend to encourage the use of sharps for accidentals.
    // prefer_sharp if false will tend to encourage the use of flats for accidentals.
    // This makes is more likely that a piece that is basically in Bb will be
    // written out by miditext as all in flatted notes, and a piece in A major should
    // come out in sharped notes.
    //
    string notename;
    switch (num % 12)
    {
      case 0:
        notename = "C";
        *prefer_sharp = false;
        break;
      case 1:
        if (*prefer_sharp)
        {
            notename = "C#";
        }
        else
        {
            notename = "Db";
        }
        break;
      case 2:
        notename = "D";
        *prefer_sharp = true;
        break;
      case 3:
        if (*prefer_sharp)
        {
            notename = "D#";
        }
        else
        {
            notename = "Eb";
        }
        break;
      case 4:
        notename = "E";
        *prefer_sharp = true;
        break;
      case 5:
        notename = "F";
        *prefer_sharp = false;
        break;
      case 6:
        if (*prefer_sharp)
        {
            notename = "F#";
        }
        else
        {
            notename = "Gb";
        }
        break;
      case 7:
        notename = "G";
        break;
      case 8:
        if (*prefer_sharp)
        {
            notename = "G#";
        }
        else
        {
            notename = "Ab";
        }
        break;
      case 9:
        notename = "A";
        *prefer_sharp = true;
        break;
      case 10:
        if (*prefer_sharp)
        {
            notename = "A#";
        }
        else
        {
            notename = "Bb";
        }
        break;
      case 11:
        notename = "B";
        *prefer_sharp = true;
        break;
    }
    notename += lexical_cast<string>(octave);
    return notename;
}

// key_sig_name_to_accidentals
// returns an integer per MIDI spec 1.1
//
pair<int, bool> textmidi::key_sig_name_to_accidentals(const string& key_sig_name)
{
    uint32_t step{};
    int32_t accident{};
    int32_t tempkey{};
    step = note_name_to_scale_step(key_sig_name, accident);
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
    constexpr std::array<int, 12> accidentals{0, 1, 2, 3, 4, 5, 6, 7, -4, -3, -2, -1};
    // find the least common multiple of step and 7, mod 12.
    constexpr int32_t fifth{7};
    while (arpeggio != step)
    {
        arpeggio += fifth;
        arpeggio %= halfsteps_per_octave;
        ++fifths;
    }
    // Adjust for the two enharmonic keys, GbMaj and C#Maj
    tempkey = accidentals[fifths];

    switch (tempkey)
    {
      case 5:
        if (-1 == accident) // is B Maj. change to Cb Maj.
        {
            tempkey = -7;
        }
        break;
      case 6:
        if (-1 == accident) // is F# Maj. change to Gb Maj.
        {
            tempkey = -6;
        }
        break;
      case 7:
        if (-1 == accident) // is C# Maj, change to Db
        {
            tempkey = -5;
        }
        break;
      default:
        break;
    }
    return make_pair(tempkey, is_minor_mode);
}

//
// pitchname_to_keynumber
// converts a string such as "C4" to 60, or "K60" to 60,
// or "K-1" to -1 with is_delta set true.
// MIDI supports values from "C-1" to "G9" or K0 to K127.
//
pair<int, bool> textmidi::pitchname_to_keynumber(const string& pitchname)
{
    constexpr int keys_per_octave{12};
    int keynumber{MiddleC};
    smatch matches{};
    const regex pitchname_re{R"(((([A-Ga-g])(([b#x]?)|(bb)))(-?)([0-9]))|(([Kk])([+-]?)([[:digit:]]+)))"};
    const auto mat{regex_match(pitchname, matches, pitchname_re)};
    bool is_delta{};
    if (!mat)
    {
        cerr << "Improper pitch name: " << pitchname << '\n';
    }
    else
    {
        constexpr int match_note{2};
        constexpr int match_neg_octave{7};
        constexpr int match_octave{8};
        constexpr int match_k{10};
        constexpr int match_key_step{11};
        constexpr int match_key_number{12};
        if (matches[match_k].str().empty()) [[likely]] // is this NOT a K event
        {
            if (!matches[match_note].str().empty()) [[unlikely]]
            {
                keynumber = notename_to_halfstep(matches[match_note].str());
                int octave{lexical_cast<int>(matches[match_octave].str())};
                // if negative octave (there is only octave -1)
                if (!matches[match_neg_octave].str().empty() && (1 == octave))
                {
                    octave = -octave;
                }
                const auto temp_octave = octave + 1;
                keynumber += (keys_per_octave * temp_octave);
            }
        }
        else [[unlikely]] // This is a K for key-number pitchname and might be a delta.
        {
            keynumber = lexical_cast<int>(matches[match_key_number]);
            auto issigned{sign_to_deltahalfstep.find(matches[match_key_step].str()[0])};
            if(issigned != sign_to_deltahalfstep.end())
            {
                keynumber *= issigned->second;
                is_delta = true;
            }
        }
    }
    return make_pair(keynumber, is_delta);
}

bool textmidi::CompareLowerNoteName::operator()(const string& left, const string& right) const noexcept
{
    return pitchname_to_keynumber(left) < pitchname_to_keynumber(right);
}

