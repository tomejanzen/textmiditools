//
// TextMIDITools Version 1.0.58
//
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "GeneralMIDI.h"

using namespace std;
using namespace textmidi;
using namespace textmidi::cgm;

//
// These values reflect the MIDI Specification 1.1,
// Chapter 5: General MIDI System Level I.
//
const midi::NumStringMap<cgm::GeneralMIDIGroup> cgm::program_group_map
{
    {"piano",        GeneralMIDIGroup::Piano},
    {"chromaticpercussion", GeneralMIDIGroup::ChromaticPercussion},
    {"organ",        GeneralMIDIGroup::Organ},
    {"guitar",       GeneralMIDIGroup::Guitar},
    {"bass",         GeneralMIDIGroup::Bass},
    {"strings",      GeneralMIDIGroup::Strings},
    {"ensemble",     GeneralMIDIGroup::Ensemble},
    {"brass",        GeneralMIDIGroup::Brass},
    {"reed",         GeneralMIDIGroup::Reed},
    {"pipe",         GeneralMIDIGroup::Pipe},
    {"synthlead",    GeneralMIDIGroup::SynthLead},
    {"synthpad",     GeneralMIDIGroup::SynthPad},
    {"syntheffects", GeneralMIDIGroup::SynthEffects},
    {"ethnic",       GeneralMIDIGroup::Ethnic},
    {"percussive",   GeneralMIDIGroup::Percussive},
    {"soundeffects", GeneralMIDIGroup::SoundEffects},
    {"all",          GeneralMIDIGroup::All},
    {"melodic",      GeneralMIDIGroup::Melodic},
    {"idiophone",    GeneralMIDIGroup::Idiophone}
};

const MIDI_Program cgm::midi_programs[] =
{
    // This is used as a 1-based sequence.
    // MIDI refers to channels and programs in a unit (1)-based way.
    // However elsewhere in this package channel and program are
    // 0-based until they are written to a binary MIDI file.
    { 0, "Dummy",                           {"A0",  "C8"}},
    // Piano
    { 1, "Acoustic Grand Piano",            {"A0",  "C8"}},
    { 2, "Bright Acoustic Piano",           {"A0",  "C8"}},
    { 3, "Electric Grand Piano",            {"E1",  "E7"}},
    { 4, "Honky-tonk Piano",                {"A0",  "C8"}},
    { 5, "Electric Piano 1(Fender Rhodes)", {"A0",  "C8"}},
    { 6, "Electric Piano 2(DX-7 EP)",       {"A0",  "C8"}},
    { 7, "Harpsichord",                     {"F1",  "F6"}},
    { 8, "Clavi",                           {"F1",  "F6"}},
    // Chromatic Percussion
    { 9, "Celesta",                         {"C4",  "C8"}},
    {10, "Glockenspiel",                    {"F5",  "C8"}},
    {11, "Music Box",                       {"F6",  "C8"}},
    {12, "Vibraphone",                      {"F3",  "F6"}},
    {13, "Marimba",                         {"C2",  "C7"}},
    {14, "Xylophone",                       {"C3",  "C7"}},
    {15, "Tubular Bells",                   {"C4",  "G5"}},
    {16, "Dulcimer",                        {"C3",  "C5"}},
    // Organ
    {17, "Drawbar Organ",                   {"A0",  "C8"}},
    {18, "Percussive Organ",                {"A0",  "C8"}},
    {19, "Rock Organ",                      {"A0",  "C8"}},
    {20, "Church Organ",                    {"A0",  "C8"}},
    {21, "Reed Organ",                      {"C2",  "C6"}},
    {22, "Accordion",                       {"E1",  "D6"}},
    {23, "Harmonica",                       {"C3",  "G5"}},
    {24, "Tango Accordion",                 {"E1",  "D6"}},
    // Guitar
    {25, "Acoustic Guitar (nylon)",         {"E3",  "D6"}},
    {26, "Acoustic Guitar (steel)",         {"E3",  "D6"}},
    {27, "Electric Guitar (jazz)",          {"E3",  "D6"}},
    {28, "Electric Guitar (clean)",         {"E3",  "D6"}},
    {29, "Electric Guitar (muted)",         {"E3",  "D6"}},
    {30, "Overdriven Guitar",               {"E3",  "D6"}},
    {31, "Distortion Guitar",               {"E3",  "D6"}},
    {32, "Guitar Harmonics",                {"E4",  "D7"}},
    // Bass
    {33, "Acoustic Bass",                   {"E1",  "E4"}},
    {34, "Electric Bass (finger)",          {"E1",  "E4"}},
    {35, "Electric Bass (pick)",            {"E1",  "E4"}},
    {36, "Fretless Bass",                   {"E1",  "E4"}},
    {37, "Slap Bass 1",                     {"E1",  "E4"}},
    {38, "Slap Bass 2",                     {"E1",  "E4"}},
    {39, "Synth Bass 1",                    {"E1",  "E4"}},
    {40, "Synth Bass 2",                    {"E1",  "E4"}},
    // Strings
    {41, "Violin",                          {"G3",  "C7"}},
    {42, "Viola",                           {"C3",  "D7"}},
    {43, "Cello",                           {"C2",  "A5"}},
    {44, "Contrabass",                      {"E1",  "G4"}},
    {45, "Tremolo Strings",                 {"E1",  "C7"}},
    {46, "Pizzicato Strings",               {"E1",  "C7"}},
    {47, "Orchestral Harp",                 {"B0",  "G#7"}},
    {48, "Timpani",                         {"C2",  "D4"}},
    // Ensemble
    {49, "String Ensemble 1",               {"C0",  "C8"}},
    {50, "String Ensemble 2",               {"C0",  "C8"}},
    {51, "Synth Strings 1",                 {"C0",  "C8"}},
    {52, "Synth Strings 2",                 {"C0",  "C8"}},
    {53, "Choir Aahs",                      {"D2",  "C6"}},
    {54, "Voice Oohs",                      {"D2",  "C6"}},
    {55, "Synth Voice",                     {"D2",  "C6"}},
    {56, "Orchestra Hit",                   {"C0",  "C8"}},
    // Brass
    {57, "Trumpet",                         {"F#3", "C6"}},
    {58, "Trombone",                        {"C2",  "C5"}},
    {59, "Tuba",                            {"Eb2", "A4"}},
    {60, "Muted Trumpet",                   {"F#3", "C6"}},
    {61, "French Horn",                     {"C2",  "C6"}},
    {62, "Brass Section",                   {"C2",  "C6"}},
    {63, "Synth Brass 1",                   {"C2",  "C6"}},
    {64, "Synth Brass 2",                   {"C2",  "C6"}},
    // Reed
    {65, "Soprano Sax",                     {"Ab3", "Eb6"}},
    {66, "Alto Sax", {"Db3", "Ab5"}},
    {67, "Tenor Sax",                       {"Ab2", "Eb5"}},
    {68, "Baritone Sax",                    {"Db2", "Ab4"}},
    {69, "Oboe",                            {"Bb3", "G6"}},
    {70, "English Horn",                    {"E3",  "A5"}},
    {71, "Bassoon",                         {"Bb1", "E5"}},
    {72, "Clarinet",                        {"D3",  "Bb6"}},
    // Pipe
    {73, "Piccolo",                         {"D5",  "C8"}},
    {74, "Flute",                           {"C4",  "C7"}},
    {75, "Recorder",                        {"C-1", "G9"}},
    {76, "Pan Flute",                       {"C-1", "G9"}},
    {77, "Blown bottle",                    {"C-1", "G9"}},
    {78, "Shakuhachi",                      {"C-1", "G9"}},
    {79, "Whistle",                         {"C-1", "G9"}},
    {80, "Ocarina",                         {"C-1", "G9"}},
    // Synth Lead
    {81, "Lead 1 (square)",                 {"C0",  "C8"}},
    {82, "Lead 2 (sawtooth)",               {"C0",  "C8"}},
    {83, "Lead 3 (calliope)",               {"C0",  "C8"}},
    {84, "Lead 4 (chiff)",                  {"C0",  "C8"}},
    {85, "Lead 5 (charang)",                {"C0",  "C8"}},
    {86, "Lead 6 (voice)",                  {"C0",  "C8"}},
    {87, "Lead 7 (fifths)",                 {"C0",  "C8"}},
    {88, "Lead 8 (bass + lead)",            {"C0",  "C8"}},
    // Synth Pad
    {89, "Pad 1 (new age)",                 {"C0",  "C8"}},
    {90, "Pad 2 (warm)",                    {"C0",  "C8"}},
    {91, "Pad 3 (polysynth)",               {"C0",  "C8"}},
    {92, "Pad 4 (choir)",                   {"C2",  "G5"}},
    {93, "Pad 5 (bowed)",                   {"C0",  "C8"}},
    {94, "Pad 6 (metallic)",                {"C0",  "C8"}},
    {95, "Pad 7 (halo)",                    {"C0",  "C8"}},
    {96, "Pad 8 (sweep)",                   {"C0",  "C8"}},
    // Synth Effects
    {97, "FX 1 (rain)",                     {"C0",  "C8"}},
    {98, "FX 2 (soundtrack)",               {"C0",  "C8"}},
    {99, "FX 3 (crystal)",                  {"C0",  "C8"}},
    {100, "FX 4 (atmosphere)",              {"C0",  "C8"}},
    {101, "FX 5 (brightness)",              {"C0",  "C8"}},
    {102, "FX 6 (goblins)",                 {"C0",  "C8"}},
    {103, "FX 7 (echoes)",                  {"C0",  "C8"}},
    {104, "FX 8 (sci-fi)",                  {"C0",  "C8"}},
    // Ethnic
    {105, "Sitar",                          {"C2",  "C6"}},
    {106, "Banjo",                          {"D3",  "C6"}},
    {107, "Shamisen",                       {"E3",  "C6"}},
    {108, "Koto",                           {"C2",  "C6"}},
    {109, "Kalimba",                        {"G3",  "G6"}},
    {110, "Bag pipe",                       {"C3",  "C6"}},
    {111, "Fiddle",                         {"G3",  "C7"}},
    {112, "Shanai",                         {"Bb3", "G6"}},
    // Percussive
    {113, "Tinkle Bell",                    {"C-1", "G9"}},
    {114, "Agogô",                          {"C-1", "G9"}},
    {115, "Steel Drums",                    {"C-1", "G9"}},
    {116, "Woodblock",                      {"C-1", "G9"}},
    {117, "Taiko Drum",                     {"C-1", "G9"}},
    {118, "Melodic Tom",                    {"C-1", "G9"}},
    {119, "Synth Drum",                     {"C-1", "G9"}},
    {120, "Reverse Cymbal",                 {"C-1", "G9"}},
    // Sound Effects
    {121, "Guitar Fret Noise",              {"C-1", "G9"}},
    {122, "Breath Noise",                   {"C-1", "G9"}},
    {123, "Seashore",                       {"C-1", "G9"}},
    {124, "Bird Tweet",                     {"C-1", "G9"}},
    {125, "Telephone Ring",                 {"C-1", "G9"}},
    {126, "Helicopter",                     {"C-1", "G9"}},
    {127, "Applause",                       {"C-1", "G9"}},
    {128, "Gunshot",                        {"C-1", "G9"}},
    // Idiophones
    {129, "Channel 10 percussion",          {"Eb1", "Eb6"}}
};

