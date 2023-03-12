//
// TextMIDITools Version 1.0.49
//
// textmidicgm 1.0
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "Scales.h"

using namespace std;
using namespace textmidi;
using namespace textmidi::cgm;

vector<vector<string>> cgm::scale_strs{
    // 0: DiatonicOnC
    {"C-1", "D-1", "E-1", "F-1", "G-1", "A-1", "B-1",
     "C0", "D0", "E0", "F0", "G0", "A0", "B0",
     "C1", "D1", "E1", "F1", "G1", "A1", "B1",
     "C2", "D2", "E2", "F2", "G2", "A2", "B2",
     "C3", "D3", "E3", "F3", "G3", "A3", "B3",
     "C4", "D4", "E4", "F4", "G4", "A4", "B4",
     "C5", "D5", "E5", "F5", "G5", "A5", "B5",
     "C6", "D6", "E6", "F6", "G6", "A6", "B6",
     "C7", "D7", "E7", "F7", "G7", "A7", "B7",
     "C8", "D8", "E8", "F8", "G8", "A8", "B8",
     "C9", "D9", "E9", "F9", "G9"},
    // 1: DiatonicOnCSharp
    {"C-1",
     "C#-1", "D#-1", "E#-1", "F#-1", "G#-1", "A#-1", "B#-1",
     "C#0", "D#0", "E#0", "F#0", "G#0", "A#0", "B#0",
     "C#1", "D#1", "E#1", "F#1", "G#1", "A#1", "B#1",
     "C#2", "D#2", "E#2", "F#2", "G#2", "A#2", "B#2",
     "C#3", "D#3", "E#3", "F#3", "G#3", "A#3", "B#3",
     "C#4", "D#4", "E#4", "F#4", "G#4", "A#4", "B#4",
     "C#5", "D#5", "E#5", "F#5", "G#5", "A#5", "B#5",
     "C#6", "D#6", "E#6", "F#6", "G#6", "A#6", "B#6",
     "C#7", "D#7", "E#7", "F#7", "G#7", "A#7", "B#7",
     "C#8", "D#8", "E#8", "F#8", "G#8", "A#8", "B#8",
     "C#9", "D#9", "E#9", "F#9"},
    // 2: DiatonicOnD
    {"C#-1", "D-1", "E-1", "F#-1", "G-1", "A-1", "B-1", "C#0",
     "D0", "E0", "F#0", "G0", "A0", "B0", "C#1",
     "D1", "E1", "F#1", "G1", "A1", "B1", "C#2",
     "D2", "E2", "F#2", "G2", "A2", "B2", "C#3",
     "D3", "E3", "F#3", "G3", "A3", "B3", "C#4",
     "D4", "E4", "F#4", "G4", "A4", "B4", "C#5",
     "D5", "E5", "F#5", "G5", "A5", "B5", "C#6",
     "D6", "E6", "F#6", "G6", "A6", "B6", "C#7",
     "D7", "E7", "F#7", "G7", "A7", "B7", "C#8",
     "D8", "E8", "F#8", "G8", "A8", "B8", "C#9",
     "D9", "E9", "F#9", "G9"},
    // 3: DiatonicOnEflat
    {"C-1", "D-1",
     "Eb-1", "F-1", "G-1", "Ab-1", "Bb-1", "C0", "D0",
     "Eb0", "F0", "G0", "Ab0", "Bb0", "C1", "D1",
     "Eb1", "F1", "G1", "Ab1", "Bb1", "C2", "D2",
     "Eb2", "F2", "G2", "Ab2", "Bb2", "C3", "D3",
     "Eb3", "F3", "G3", "Ab3", "Bb3", "C4", "D4",
     "Eb4", "F4", "G4", "Ab4", "Bb4", "C5", "D5",
     "Eb5", "F5", "G5", "Ab5", "Bb5", "C6", "D6",
     "Eb6", "F6", "G6", "Ab6", "Bb6", "C7", "D7",
     "Eb7", "F7", "G7", "Ab7", "Bb7", "C8", "D8",
     "Eb8", "F8", "G8", "Ab8", "Bb8", "C9", "D9",
     "Eb9", "F9", "G9"},
    // 4: DiatonicOnE
    {"C#-1", "D#-1",
     "E-1", "F#-1", "G#-1", "A-1", "B-1", "C#0", "D#0",
     "E0", "F#0", "G#0", "A0", "B0", "C#1", "D#1",
     "E1", "F#1", "G#1", "A1", "B1", "C#2", "D#2",
     "E2", "F#2", "G#2", "A2", "B2", "C#3", "D#3",
     "E3", "F#3", "G#3", "A3", "B3", "C#4", "D#4",
     "E4", "F#4", "G#4", "A4", "B4", "C#5", "D#5",
     "E5", "F#5", "G#5", "A5", "B5", "C#6", "D#6",
     "E6", "F#6", "G#6", "A6", "B6", "C#7", "D#7",
     "E7", "F#7", "G#7", "A7", "B7", "C#8", "D#8",
     "E8", "F#8", "G#8", "A8", "B8", "C#9", "D#9",
     "E9", "F#9"},
    // 5: DiatonicOnF
    {"C-1", "D-1", "E-1",
     "F-1", "G-1", "A-1", "Bb-1", "C0", "D0", "E0",
     "F0", "G0", "A0", "Bb0", "C1", "D1", "E1",
     "F1", "G1", "A1", "Bb1", "C2", "D2", "E2",
     "F2", "G2", "A2", "Bb2", "C3", "D3", "E3",
     "F3", "G3", "A3", "Bb3", "C4", "D4", "E4",
     "F4", "G4", "A4", "Bb4", "C5", "D5", "E5",
     "F5", "G5", "A5", "Bb5", "C6", "D6", "E6",
     "F6", "G6", "A6", "Bb6", "C7", "D7", "E7",
     "F7", "G7", "A7", "Bb7", "C8", "D8", "E8",
     "F8", "G8", "A8", "Bb8", "C9", "D9", "E9",
     "F9", "G9"},
    // 6: DiatonicOnFSharp
    {"C#-1", "D#-1", "E#-1",
     "F#-1", "G#-1", "A#-1", "B-1", "C#0", "D#0", "E#0",
     "F#0", "G#0", "A#0", "B0", "C#1", "D#1", "E#1",
     "F#1", "G#1", "A#1", "B1", "C#2", "D#2", "E#2",
     "F#2", "G#2", "A#2", "B2", "C#3", "D#3", "E#3",
     "F#3", "G#3", "A#3", "B3", "C#4", "D#4", "E#4",
     "F#4", "G#4", "A#4", "B4", "C#5", "D#5", "E#5",
     "F#5", "G#5", "A#5", "B5", "C#6", "D#6", "E#6",
     "F#6", "G#6", "A#6", "B6", "C#7", "D#7", "E#7",
     "F#7", "G#7", "A#7", "B7", "C#8", "D#8", "E#8",
     "F#8", "G#8", "A#8", "B8", "C#9", "D#9", "E#9",
     "F#9"},
    // 7: DiatonicOnG
    {"C-1", "D-1", "E-1", "F#-1",
     "G-1", "A-1", "B-1", "C0", "D0", "E0", "F#0",
     "G0", "A0", "B0", "C1", "D1", "E1", "F#1",
     "G1", "A1", "B1", "C2", "D2", "E2", "F#2",
     "G2", "A2", "B2", "C3", "D3", "E3", "F#3",
     "G3", "A3", "B3", "C4", "D4", "E4", "F#4",
     "G4", "A4", "B4", "C5", "D5", "E5", "F#5",
     "G5", "A5", "B5", "C6", "D6", "E6", "F#6",
     "G6", "A6", "B6", "C7", "D7", "E7", "F#7",
     "G7", "A7", "B7", "C8", "D8", "E8", "F#8",
     "G8", "A8", "B8", "C9", "D9", "E9", "F#9",
     "G9"},
    // 8: DiatonicOnAFlat
    {"C-1", "Db-1", "Eb-1", "F-1", "G-1",
     "Ab-1", "Bb-1", "C0", "Db0", "Eb0", "F0", "G0",
     "Ab0", "Bb0", "C1", "Db1", "Eb1", "F1", "G1",
     "Ab1", "Bb1", "C2", "Db2", "Eb2", "F2", "G2",
     "Ab2", "Bb2", "C3", "Db3", "Eb3", "F3", "G3",
     "Ab3", "Bb3", "C4", "Db4", "Eb4", "F4", "G4",
     "Ab4", "Bb4", "C5", "Db5", "Eb5", "F5", "G5",
     "Ab5", "Bb5", "C6", "Db6", "Eb6", "F6", "G6",
     "Ab6", "Bb6", "C7", "Db7", "Eb7", "F7", "G7",
     "Ab7", "Bb7", "C8", "Db8", "Eb8", "F8", "G8",
     "Ab8", "Bb8", "C9", "Db9", "Eb9", "F9", "G9"},
    // 9: DiatonicOnA
    {"C#-1", "D-1", "E-1", "F#-1", "G#-1",
     "A-1", "B-1", "C#0", "D0", "E0", "F#0", "G#0",
     "A0", "B0", "C#1", "D1", "E1", "F#1", "G#1",
     "A1", "B1", "C#2", "D2", "E2", "F#2", "G#2",
     "A2", "B2", "C#3", "D3", "E3", "F#3", "G#3",
     "A3", "B3", "C#4", "D4", "E4", "F#4", "G#4",
     "A4", "B4", "C#5", "D5", "E5", "F#5", "G#5",
     "A5", "B5", "C#6", "D6", "E6", "F#6", "G#6",
     "A6", "B6", "C#7", "D7", "E7", "F#7", "G#7",
     "A7", "B7", "C#8", "D8", "E8", "F#8", "G#8",
     "A8", "B8", "C#9", "D9", "E9", "F#9"},
    // 10: DiatonicOnBFlat
    {"C-1", "D-1", "Eb-1", "F-1", "G-1", "A-1",
     "Bb-1", "C0", "D0", "Eb0", "F0", "G0", "A0",
     "Bb0", "C1", "D1", "Eb1", "F1", "G1", "A1",
     "Bb1", "C2", "D2", "Eb2", "F2", "G2", "A2",
     "Bb2", "C3", "D3", "Eb3", "F3", "G3", "A3",
     "Bb3", "C4", "D4", "Eb4", "F4", "G4", "A4",
     "Bb4", "C5", "D5", "Eb5", "F5", "G5", "A5",
     "Bb5", "C6", "D6", "Eb6", "F6", "G6", "A6",
     "Bb6", "C7", "D7", "Eb7", "F7", "G7", "A7",
     "Bb7", "C8", "D8", "Eb8", "F8", "G8", "A8",
     "Bb8", "C9", "D9", "Eb9", "F9", "G9"},
    // 11: DiatonicOnB
    {"C#-1", "D#-1", "E-1", "F#-1", "G#-1", "A#-1",
     "B-1", "C#0", "D#0", "E0", "F#0", "G#0", "A#0",
     "B0", "C#1", "D#1", "E1", "F#1", "G#1", "A#1",
     "B1", "C#2", "D#2", "E2", "F#2", "G#2", "A#2",
     "B2", "C#3", "D#3", "E3", "F#3", "G#3", "A#3",
     "B3", "C#4", "D#4", "E4", "F#4", "G#4", "A#4",
     "B4", "C#5", "D#5", "E5", "F#5", "G#5", "A#5",
     "B5", "C#6", "D#6", "E6", "F#6", "G#6", "A#6",
     "B6", "C#7", "D#7", "E7", "F#7", "G#7", "A#7",
     "B7", "C#8", "D#8", "E8", "F#8", "G#8", "A#8",
     "B8", "C#9", "D#9", "E9", "F#9"},
    // 12: MinorOnc
    {"C-1", "D-1", "Eb-1", "F-1", "G-1", "Ab-1", "B-1",
     "C0", "D0", "Eb0", "F0", "G0", "Ab0", "B0",
     "C1", "D1", "Eb1", "F1", "G1", "Ab1", "B1",
     "C2", "D2", "Eb2", "F2", "G2", "Ab2", "B2",
     "C3", "D3", "Eb3", "F3", "G3", "Ab3", "B3",
     "C4", "D4", "Eb4", "F4", "G4", "Ab4", "B4",
     "C5", "D5", "Eb5", "F5", "G5", "Ab5", "B5",
     "C6", "D6", "Eb6", "F6", "G6", "Ab6", "B6",
     "C7", "D7", "Eb7", "F7", "G7", "Ab7", "B7",
     "C8", "D8", "Eb8", "F8", "G8", "Ab8", "B8",
     "C9", "D9", "Eb9", "F9", "G9"},
    // 13: MinorOnCSharp
    {"C-1",
     "C#-1", "D#-1", "E-1", "F#-1", "G#-1", "A-1", "B#-1",
     "C#0", "D#0", "E0", "F#0", "G#0", "A0", "B#0",
     "C#1", "D#1", "E1", "F#1", "G#1", "A1", "B#1",
     "C#2", "D#2", "E2", "F#2", "G#2", "A2", "B#2",
     "C#3", "D#3", "E3", "F#3", "G#3", "A3", "B#3",
     "C#4", "D#4", "E4", "F#4", "G#4", "A4", "B#4",
     "C#5", "D#5", "E5", "F#5", "G#5", "A5", "B#5",
     "C#6", "D#6", "E6", "F#6", "G#6", "A6", "B#6",
     "C#7", "D#7", "E7", "F#7", "G#7", "A7", "B#7",
     "C#8", "D#8", "E8", "F#8", "G#8", "A8", "B#8",
     "C#9", "D#9", "E9", "F#9"},
    // 14: MinorOnD
    {"C#-1",
     "D-1", "E-1", "F-1", "G-1", "A-1", "Bb-1", "C#0",
     "D0", "E0", "F0", "G0", "A0", "Bb0", "C#1",
     "D1", "E1", "F1", "G1", "A1", "Bb1", "C#2",
     "D2", "E2", "F2", "G2", "A2", "Bb2", "C#3",
     "D3", "E3", "F3", "G3", "A3", "Bb3", "C#4",
     "D4", "E4", "F4", "G4", "A4", "Bb4", "C#5",
     "D5", "E5", "F5", "G5", "A5", "Bb5", "C#6",
     "D6", "E6", "F6", "G6", "A6", "Bb6", "C#7",
     "D7", "E7", "F7", "G7", "A7", "Bb7", "C#8",
     "D8", "E8", "F8", "G8", "A8", "Bb8", "C#9",
     "D9", "E9", "F9", "G9"},
    // 15: MinorOnEflat
    {"D-1",
     "Eb-1", "F-1", "Gb-1", "Ab-1", "Bb-1", "Cb0", "D0",
     "Eb0", "F0", "Gb0", "Ab0", "Bb0", "Cb1", "D1",
     "Eb1", "F1", "Gb1", "Ab1", "Bb1", "Cb2", "D2",
     "Eb2", "F2", "Gb2", "Ab2", "Bb2", "Cb3", "D3",
     "Eb3", "F3", "Gb3", "Ab3", "Bb3", "Cb4", "D4",
     "Eb4", "F4", "Gb4", "Ab4", "Bb4", "Cb5", "D5",
     "Eb5", "F5", "Gb5", "Ab5", "Bb5", "Cb6", "D6",
     "Eb6", "F6", "Gb6", "Ab6", "Bb6", "Cb7", "D7",
     "Eb7", "F7", "Gb7", "Ab7", "Bb7", "Cb8", "D8",
     "Eb8", "F8", "Gb8", "Ab8", "Bb8", "Cb9", "D9",
     "Eb9", "F9", "Gb9"},
    // 16: MinorOnE
    {"C-1", "D#-1",
     "E-1", "F#-1", "G-1", "A-1", "B-1", "C0", "D#0",
     "E0", "F#0", "G0", "A0", "B0", "C1", "D#1",
     "E1", "F#1", "G1", "A1", "B1", "C2", "D#2",
     "E2", "F#2", "G2", "A2", "B2", "C3", "D#3",
     "E3", "F#3", "G3", "A3", "B3", "C4", "D#4",
     "E4", "F#4", "G4", "A4", "B4", "C5", "D#5",
     "E5", "F#5", "G5", "A5", "B5", "C6", "D#6",
     "E6", "F#6", "G6", "A6", "B6", "C7", "D#7",
     "E7", "F#7", "G7", "A7", "B7", "C8", "D#8",
     "E8", "F#8", "G8", "A8", "B8", "C9", "D#9",
     "E9", "F#9"},
    // 17: MinorOnF
    {"C-1", "Db-1", "E-1",
     "F-1", "G-1", "Ab-1", "Bb-1", "C0", "Db0", "E0",
     "F0", "G0", "Ab0", "Bb0", "C1", "Db1", "E1",
     "F1", "G1", "Ab1", "Bb1", "C2", "Db2", "E2",
     "F2", "G2", "Ab2", "Bb2", "C3", "Db3", "E3",
     "F3", "G3", "Ab3", "Bb3", "C4", "Db4", "E4",
     "F4", "G4", "Ab4", "Bb4", "C5", "Db5", "E5",
     "F5", "G5", "Ab5", "Bb5", "C6", "Db6", "E6",
     "F6", "G6", "Ab6", "Bb6", "C7", "Db7", "E7",
     "F7", "G7", "Ab7", "Bb7", "C8", "Db8", "E8",
     "F8", "G8", "Ab8", "Bb8", "C9", "Db9", "E9",
    "F9", "G9"},
    // 18: MinorOnFSharp
    { "C#-1", "D-1", "E#-1",
     "F#-1", "G#-1", "A-1", "B-1", "C#0", "D0", "E#0",
     "F#0", "G#0", "A0", "B0", "C#1", "D1", "E#1",
     "F#1", "G#1", "A1", "B1", "C#2", "D2", "E#2",
     "F#2", "G#2", "A2", "B2", "C#3", "D3", "E#3",
     "F#3", "G#3", "A3", "B3", "C#4", "D4", "E#4",
     "F#4", "G#4", "A4", "B4", "C#5", "D5", "E#5",
     "F#5", "G#5", "A5", "B5", "C#6", "D6", "E#6",
     "F#6", "G#6", "A6", "B6", "C#7", "D7", "E#7",
     "F#7", "G#7", "A7", "B7", "C#8", "D8", "E#8",
     "F#8", "G#8", "A8", "B8", "C#9", "D9", "E#9",
     "F#9"},
    // 19: MinorOnG
    {"C-1", "D-1", "Eb-1", "F#-1",
     "G-1", "A-1", "Bb-1", "C0", "D0", "Eb0", "F#0",
     "G0", "A0", "Bb0", "C1", "D1", "Eb1", "F#1",
     "G1", "A1", "Bb1", "C2", "D2", "Eb2", "F#2",
     "G2", "A2", "Bb2", "C3", "D3", "Eb3", "F#3",
     "G3", "A3", "Bb3", "C4", "D4", "Eb4", "F#4",
     "G4", "A4", "Bb4", "C5", "D5", "Eb5", "F#5",
     "G5", "A5", "Bb5", "C6", "D6", "Eb6", "F#6",
     "G6", "A6", "Bb6", "C7", "D7", "Eb7", "F#7",
     "G7", "A7", "Bb7", "C8", "D8", "Eb8", "F#8",
     "G8", "A8", "Bb8", "C9", "D9", "Eb9", "F#9",
     "G9"},
    // 20: MinorOnAFlat
    {"Db-1", "Eb-1", "Fb-1", "G-1",
     "Ab-1", "Bb-1", "Cb0", "Db0", "Eb0", "Fb0", "G0",
     "Ab0", "Bb0", "Cb1", "Db1", "Eb1", "Fb1", "G1",
     "Ab1", "Bb1", "Cb2", "Db2", "Eb2", "Fb2", "G2",
     "Ab2", "Bb2", "Cb3", "Db3", "Eb3", "Fb3", "G3",
     "Ab3", "Bb3", "Cb4", "Db4", "Eb4", "Fb4", "G4",
     "Ab4", "Bb4", "Cb5", "Db5", "Eb5", "Fb5", "G5",
     "Ab5", "Bb5", "Cb6", "Db6", "Eb6", "Fb6", "G6",
     "Ab6", "Bb6", "Cb7", "Db7", "Eb7", "Fb7", "G7",
     "Ab7", "Bb7", "Cb8", "Db8", "Eb8", "Fb8", "G8",
     "Ab8", "Bb8", "Cb9", "Db9", "Eb9", "Fb9", "G9"},
    // 21: MinorOnA
    {"C-1", "D-1", "E-1", "F-1", "G#-1",
     "A-1", "B-1", "C0", "D0", "E0", "F0", "G#0",
     "A0", "B0", "C1", "D1", "E1", "F1", "G#1",
     "A1", "B1", "C2", "D2", "E2", "F2", "G#2",
     "A2", "B2", "C3", "D3", "E3", "F3", "G#3",
     "A3", "B3", "C4", "D4", "E4", "F4", "G#4",
     "A4", "B4", "C5", "D5", "E5", "F5", "G#5",
     "A5", "B5", "C6", "D6", "E6", "F6", "G#6",
     "A6", "B6", "C7", "D7", "E7", "F7", "G#7",
     "A7", "B7", "C8", "D8", "E8", "F8", "G#8",
     "A8", "B8", "C9", "D9", "E9", "F9"},
    // 22: MinorOnBFlat
    {"C-1", "Db-1", "Eb-1", "F-1", "Gb-1", "A-1",
     "Bb-1", "C0", "Db0", "Eb0", "F0", "Gb0", "A0",
     "Bb0", "C1", "Db1", "Eb1", "F1", "Gb1", "A1",
     "Bb1", "C2", "Db2", "Eb2", "F2", "Gb2", "A2",
     "Bb2", "C3", "Db3", "Eb3", "F3", "Gb3", "A3",
     "Bb3", "C4", "Db4", "Eb4", "F4", "Gb4", "A4",
     "Bb4", "C5", "Db5", "Eb5", "F5", "Gb5", "A5",
     "Bb5", "C6", "Db6", "Eb6", "F6", "Gb6", "A6",
     "Bb6", "C7", "Db7", "Eb7", "F7", "Gb7", "A7",
     "Bb7", "C8", "Db8", "Eb8", "F8", "Gb8", "A8",
     "Bb8", "C9", "Db9", "Eb9", "F9", "Gb9"},
    // 23: MinorOnB
    {"C#-1", "D-1", "E-1", "F#-1", "G-1", "A#-1",
     "B-1", "C#0", "D0", "E0", "F#0", "G0", "A#0",
     "B0", "C#1", "D1", "E1", "F#1", "G1", "A#1",
     "B1", "C#2", "D2", "E2", "F#2", "G2", "A#2",
     "B2", "C#3", "D3", "E3", "F#3", "G3", "A#3",
     "B3", "C#4", "D4", "E4", "F#4", "G4", "A#4",
     "B4", "C#5", "D5", "E5", "F#5", "G5", "A#5",
     "B5", "C#6", "D6", "E6", "F#6", "G6", "A#6",
     "B6", "C#7", "D7", "E7", "F#7", "G7", "A#7",
     "B7", "C#8", "D8", "E8", "F#8", "G8", "A#8",
     "B8", "C#9", "D9", "E9", "F#9"},
    // 24: WholeToneOnC
    {"C-1", "D-1", "E-1", "F#-1", "G#-1", "A#-1",
     "C0", "D0", "E0", "F#0", "G#0", "A#0",
     "C1", "D1", "E1", "F#1", "G#1", "A#1",
     "C2", "D2", "E2", "F#2", "G#2", "A#2",
     "C3", "D3", "E3", "F#3", "G#3", "A#3",
     "C4", "D4", "E4", "F#4", "G#4", "A#4",
     "C5", "D5", "E5", "F#5", "G#5", "A#5",
     "C6", "D6", "E6", "F#6", "G#6", "A#6",
     "C7", "D7", "E7", "F#7", "G#7", "A#7",
     "C8", "D8", "E8", "F#8", "G#8", "A#8",
     "C9", "D9", "E9",
     "F#9"},
    // 25: WholeToneOnCSharp
    {"C#-1", "D#-1", "F-1", "G-1", "A-1", "B-1",
     "C#0", "D#0", "F0", "G0", "A0", "B0",
     "C#1", "D#1", "F1", "G1", "A1", "B1",
     "C#2", "D#2", "F2", "G2", "A2", "B2",
     "C#3", "D#3", "F3", "G3", "A3", "B3",
     "C#4", "D#4", "F4", "G4", "A4", "B4",
     "C#5", "D#5", "F5", "G5", "A5", "B5",
     "C#6", "D#6", "F6", "G6", "A6", "B6",
     "C#7", "D#7", "F7", "G7", "A7", "B7",
     "C#8", "D#8", "F8", "G8", "A8", "B8",
     "C#9", "D#9", "F9", "G9"},
    // 26: DiminishedOnC
    {"C-1", "D-1", "D#-1", "F-1", "F#-1", "G#-1", "A-1", "B-1",
     "C0", "D0", "D#0", "F0", "F#0", "G#0", "A0", "B0",
     "C1", "D1", "D#1", "F1", "F#1", "G#1", "A1", "B1",
     "C2", "D2", "D#2", "F2", "F#2", "G#2", "A2", "B2",
     "C3", "D3", "D#3", "F3", "F#3", "G#3", "A3", "B3",
     "C4", "D4", "D#4", "F4", "F#4", "G#4", "A4", "B4",
     "C5", "D5", "D#5", "F5", "F#5", "G#5", "A5", "B5",
     "C6", "D6", "D#6", "F6", "F#6", "G#6", "A6", "B6",
     "C7", "D7", "D#7", "F7", "F#7", "G#7", "A7", "B7",
     "C8", "D8", "D#8", "F8", "F#8", "G#8", "A8", "B8",
     "C9", "D9", "D#9", "F9", "F#9"},
    // 27: DiminishedOnCSharp
    {"C-1",
     "C#-1", "D#-1", "E-1", "F#-1", "G-1", "A-1", "A#-1", "C0",
     "C#0", "D#0", "E0", "F#0", "G0", "A0", "A#0", "C1",
     "C#1", "D#1", "E1", "F#1", "G1", "A1", "A#1", "C2",
     "C#2", "D#2", "E2", "F#2", "G2", "A2", "A#2", "C3",
     "C#3", "D#3", "E3", "F#3", "G3", "A3", "A#3", "C4",
     "C#4", "D#4", "E4", "F#4", "G4", "A4", "A#4", "C5",
     "C#5", "D#5", "E5", "F#5", "G5", "A5", "A#5", "C6",
     "C#6", "D#6", "E6", "F#6", "G6", "A6", "A#6", "C7",
     "C#7", "D#7", "E7", "F#7", "G7", "A7", "A#7", "C8",
     "C#8", "D#8", "E8", "F#8", "G8", "A8", "A#8", "C9",
     "C#9", "D#9", "E9", "F#9", "G9"},
    // 28: DiminishedOnD
    {"C#-1",
     "D-1", "E-1", "F-1", "G-1", "G#-1", "A#-1", "B-1", "C#0",
     "D0", "E0", "F0", "G0", "G#0", "A#0", "B0", "C#1",
     "D1", "E1", "F1", "G1", "G#1", "A#1", "B1", "C#2",
     "D2", "E2", "F2", "G2", "G#2", "A#2", "B2", "C#3",
     "D3", "E3", "F3", "G3", "G#3", "A#3", "B3", "C#4",
     "D4", "E4", "F4", "G4", "G#4", "A#4", "B4", "C#5",
     "D5", "E5", "F5", "G5", "G#5", "A#5", "B5", "C#6",
     "D6", "E6", "F6", "G6", "G#6", "A#6", "B6", "C#7",
     "D7", "E7", "F7", "G7", "G#7", "A#7", "B7", "C#8",
     "D8", "E8", "F8", "G8", "G#8", "A#8", "B8", "C#9",
     "D9", "E9", "F9", "G9"},
    // 29: Chromatic
    {"C-1", "C#-1", "D-1", "D#-1", "E-1", "F-1", "F#-1", "G-1", "G#-1", "A-1", "A#-1", "B-1",
     "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
     "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
     "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
     "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
     "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
     "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
     "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
     "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
     "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
     "C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9"},
     // 30: Pentatonic on C
     {"C-1", "D-1", "F-1", "G-1", "Bb-1",
      "C0", "D0", "F0", "G0", "Bb0",
      "C1", "D1", "F1", "G1", "Bb1",
      "C2", "D2", "F2", "G2", "Bb2",
      "C3", "D3", "F3", "G3", "Bb3",
      "C4", "D4", "F4", "G4", "Bb4",
      "C5", "D5", "F5", "G5", "Bb5",
      "C6", "D6", "F6", "G6", "Bb6",
      "C7", "D7", "F7", "G7", "Bb7",
      "C8", "D8", "F8", "G8", "Bb8",
      "C9", "D9", "F9", "G9"},
     // 31: Pentatonic on C#
     {"C#-1", "D#-1", "F#-1", "G#-1", "B-1",
      "C#0", "D#0", "F#0", "G#0", "B0",
      "C#1", "D#1", "F#1", "G#1", "B1",
      "C#2", "D#2", "F#2", "G#2", "B2",
      "C#3", "D#3", "F#3", "G#3", "B3",
      "C#4", "D#4", "F#4", "G#4", "B4",
      "C#5", "D#5", "F#5", "G#5", "B5",
      "C#6", "D#6", "F#6", "G#6", "B6",
      "C#7", "D#7", "F#7", "G#7", "B7",
      "C#8", "D#8", "F#8", "G#8", "B8",
      "C#9", "D#9", "F#9"},
     // 32: Pentatonic on D
     {"C-1",
      "D-1", "E-1", "G-1", "A-1", "C0"
      "D0", "E0", "G0", "A0", "C1",
      "D1", "E1", "G1", "A1", "C2",
      "D2", "E2", "G2", "A2", "C3",
      "D3", "E3", "G3", "A3", "C4",
      "D4", "E4", "G4", "A4", "C5",
      "D5", "E5", "G5", "A5", "C6",
      "D6", "E6", "G6", "A6", "C7",
      "D7", "E7", "G7", "A7", "C8",
      "D8", "E8", "G8", "A8", "C9",
      "D9", "E9", "G9"},
     // 33: Pentatonic on D#
     {"C#-1",
      "D#-1", "F-1", "G#-1", "A#-1", "C#0",
      "D#0", "F0", "G#0", "A#0", "C#1",
      "D#1", "F1", "G#1", "A#1", "C#2",
      "D#2", "F2", "G#2", "A#2", "C#3",
      "D#3", "F3", "G#3", "A#3", "C#4",
      "D#4", "F4", "G#4", "A#4", "C#5",
      "D#5", "F5", "G#5", "A#5", "C#5",
      "D#6", "F6", "G#6", "A#6", "C#7",
      "D#7", "F7", "G#7", "A#7", "C#8",
      "D#8", "F8", "G#8", "A#8", "C#9",
      "D#9", "F9"},
     // 34: Pentatonic on E
     {"D-1",
      "E-1", "F#-1", "A-1", "B-1", "D0",
      "E0", "F#0", "A0", "B0", "D1",
      "E1", "F#1", "A1", "B1", "D2",
      "E2", "F#2", "A2", "B2", "D3",
      "E3", "F#3", "A3", "B3", "D4",
      "E4", "F#4", "A4", "B4", "D5",
      "E5", "F#5", "A5", "B5", "D6",
      "E6", "F#6", "A6", "B6", "D7",
      "E7", "F#7", "A7", "B7", "D8",
      "E8", "F#8", "A8", "B8", "D9",
      "E9", "F#9"},
     // 35: Tritone on C
     {"C-1", "D-1", "E-1", "F-1", "F#-1", "G#-1", "A#-1", "B-1",
      "C0", "D0", "E0", "F0", "F#0", "G#0", "A#0", "B0",
      "C1", "D1", "E1", "F1", "F#1", "G#1", "A#1", "B1",
      "C2", "D2", "E2", "F2", "F#2", "G#2", "A#2", "B2",
      "C3", "D3", "E3", "F3", "F#3", "G#3", "A#3", "B3",
      "C4", "D4", "E4", "F4", "F#4", "G#4", "A#4", "B4",
      "C5", "D5", "E5", "F5", "F#5", "G#5", "A#5", "B5",
      "C6", "D6", "E6", "F6", "F#6", "G#6", "A#6", "B6",
      "C7", "D7", "E7", "F7", "F#7", "G#7", "A#7", "B7",
      "C8", "D8", "E8", "F8", "F#8", "G#8", "A#8", "B8",
      "C9", "D9", "E9", "F9", "F#9"},
     // 36: Tritone on C#
     {"C-1",
      "C#-1", "D#-1", "E#-1", "F#-1", "G-1", "A-1", "B#-1",
      "C#0", "D#0", "E#0", "F#0", "G0", "A0", "B#0", "B#0",
      "C#1", "D#1", "E#1", "F#1", "G1", "A1", "B#1", "B#1",
      "C#2", "D#2", "E#2", "F#2", "G2", "A2", "B#2", "B#2",
      "C#3", "D#3", "E#3", "F#3", "G3", "A3", "B#3", "B#3",
      "C#4", "D#4", "E#4", "F#4", "G4", "A4", "B#4", "B#4",
      "C#5", "D#5", "E#5", "F#5", "G5", "A5", "B#5", "B#5",
      "C#6", "D#6", "E#6", "F#6", "G6", "A6", "B#6", "B#6",
      "C#7", "D#7", "E#7", "F#7", "G7", "A7", "B#7", "B#7",
      "C#8", "D#8", "E#8", "F#8", "G8", "A8", "B#8", "B#8",
      "C#9", "D#9", "E#9", "F#9", "G9"},
     // 37: Tritone on D
     {"C-1", "C#-1",
      "D-1", "E-1", "F#-1", "G-1", "G#-1", "A#-1", "B#-1", "C#0",
      "D0", "E0", "F#0", "G0", "G#0", "A#0", "B#0", "C#1",
      "D1", "E1", "F#1", "G1", "G#1", "A#1", "B#1", "C#2",
      "D2", "E2", "F#2", "G2", "G#2", "A#2", "B#2", "C#3",
      "D3", "E3", "F#3", "G3", "G#3", "A#3", "B#3", "C#4",
      "D4", "E4", "F#4", "G4", "G#4", "A#4", "B#4", "C#5",
      "D5", "E5", "F#5", "G5", "G#5", "A#5", "B#5", "C#6",
      "D6", "E6", "F#6", "G6", "G#6", "A#6", "B#6", "C#7",
      "D7", "E7", "F#7", "G7", "G#7", "A#7", "B#7", "C#8",
      "D8", "E8", "F#8", "G8", "G#8", "A#8", "B#8", "C#9",
      "D9", "E9", "F#9", "G9"},
     // 38: Tritone on Eb
     {"C#-1", "D-1",
      "Eb-1", "F-1", "G-1", "Ab-1", "A-1", "B-1", "C#1", "D1",
      "Eb0", "F0", "G0", "Ab0", "A0", "B0", "C#1", "D1",
      "Eb1", "F1", "G1", "Ab1", "A1", "B1", "C#2", "D2",
      "Eb2", "F2", "G2", "Ab2", "A2", "B2", "C#3", "D3",
      "Eb3", "F3", "G3", "Ab3", "A3", "B3", "C#4", "D4",
      "Eb4", "F4", "G4", "Ab4", "A4", "B4", "C#5", "D5",
      "Eb5", "F5", "G5", "Ab5", "A5", "B5", "C#6", "D6",
      "Eb6", "F6", "G6", "Ab6", "A6", "B6", "C#7", "D7",
      "Eb7", "F7", "G7", "Ab7", "A7", "B7", "C#8", "D8",
      "Eb8", "F8", "G8", "Ab8", "A8", "B8", "C#9", "D9",
      "Eb9", "F9", "G9"},
     // Tritone on E
     {"C-1", "D-1", "Eb-1",
      "E-1", "F#-1", "G#-1", "A-1", "Bb-1", "C0", "D0", "Eb0",
      "E0", "F#0", "G#0", "A0", "Bb0", "C1", "D1", "Eb1",
      "E1", "F#1", "G#1", "A1", "Bb1", "C2", "D2", "Eb2",
      "E2", "F#2", "G#2", "A2", "Bb2", "C3", "D3", "Eb3",
      "E3", "F#3", "G#3", "A3", "Bb3", "C4", "D4", "Eb4",
      "E4", "F#4", "G#4", "A4", "Bb4", "C5", "D5", "Eb5",
      "E5", "F#5", "G#5", "A5", "Bb5", "C5", "D6", "Eb6",
      "E6", "F#6", "G#6", "A6", "Bb6", "C7", "D7", "Eb7",
      "E7", "F#7", "G#7", "A7", "Bb7", "C8", "D8", "Eb8",
      "E8", "F#8", "G#8", "A8", "Bb8", "C9", "D9", "Eb9",
      "E9", "F#9"}

     };

