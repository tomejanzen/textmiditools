#!/usr/bin/env python3
"""TextMIDITools: TextMidiFormEdit.py general_midi data with instrument names and codes."""
# TextMIDITools Version 1.1.0
# TextMidiFormEdit.py 1.0
# Copyright © 2025 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#
# Some instrument ranges were found on https://en.wikipedia.org or guessed.
# Corrections on unusual instruments are welcome.

GENERAL_MIDI_INSTRUMENT_DICT = {
     'Acoustic_Grand_Piano'            : (  1, 'A0',  'C8'),
     'Bright_Acoustic_Piano'           : (  2, 'A0',  'C8'),
     'Electric_Grand_Piano'            : (  3, 'E1',  'E7'),
     'Honky-tonk_Piano'                : (  4, 'A0',  'C8'),
     'Electric_Piano_1(Fender_Rhodes)' : (  5, 'A0',  'C8'),
     'Electric_Piano_2(DX-7_EP)'       : (  6, 'A0',  'C8'),
     'Harpsichord'                     : (  7, 'F1',  'F6'),
     'Clavi'                           : (  8, 'F1',  'F6'),
     # Chromatic Percussion
     'Celesta'                         : (  9, 'C4',  'C8'),
    'Glockenspiel'                     : ( 10, 'F5',  'C8'),
    'Music_Box'                        : ( 11, 'F6',  'C8'),
    'Vibraphone'                       : ( 12, 'F3',  'F6'),
    'Marimba'                          : ( 13, 'C2',  'C7'),
    'Xylophone'                        : ( 14, 'C3',  'C7'),
    'Tubular Bells'                    : ( 15, 'C4',  'G5'),
    'Dulcimer'                         : ( 16, 'C3',  'C5'),
    # Organ
    'Drawbar_Organ'                    : ( 17, 'A0',  'C8'),
    'Percussive_Organ'                 : ( 18, 'A0',  'C8'),
    'Rock_Organ'                       : ( 19, 'A0',  'C8'),
    'Church_Organ'                     : ( 20, 'A0',  'C8'),
    'Reed_Organ'                       : ( 21, 'C2',  'C6'),
    'Accordion'                        : ( 22, 'E1',  'D6'),
    'Harmonica'                        : ( 23, 'C3',  'G5'),
    'Tango Accordion'                  : ( 24, 'E1',  'D6'),
    # Guitar
    'Acoustic_Guitar_(nylon)'          : ( 25, 'E2',  'D5'),
    'Acoustic_Guitar_(steel)'          : ( 26, 'E2',  'D5'),
    'Electric_Guitar_(jazz)'           : ( 27, 'E2',  'D5'),
    'Electric_Guitar_(clean)'          : ( 28, 'E2',  'D5'),
    'Electric_Guitar_(muted)'          : ( 29, 'E2',  'D5'),
    'Overdriven_Guitar'                : ( 30, 'E2',  'D5'),
    'Distortion_Guitar'                : ( 31, 'E2',  'D5'),
    'Guitar_Harmonics'                 : ( 32, 'E3',  'D6'),
    # Bass
    'Acoustic_Bass'                    : ( 33, 'E1',  'E4'),
    'Electric_Bass_(finger)'           : ( 34, 'E1',  'E4'),
    'Electric_Bass_(pick)'             : ( 35, 'E1',  'E4'),
    'Fretless_Bass'                    : ( 36, 'E1',  'E4'),
    'Slap_Bass_1'                      : ( 37, 'E1',  'E4'),
    'Slap_Bass_2'                      : ( 38, 'E1',  'E4'),
    'Synth_Bass_1'                     : ( 39, 'E1',  'E4'),
    'Synth_Bass_2'                     : ( 40, 'E1',  'E4'),
    # Strings
    'Violin'                           : ( 41, 'G3',  'C7'),
    'Viola'                            : ( 42, 'C3',  'D7'),
    'Cello'                            : ( 43, 'C2',  'A5'),
    'Contrabass'                       : ( 44, 'E1',  'G4'),
    'Tremolo_Strings'                  : ( 45, 'E1',  'C7'),
    'Pizzicato_Strings'                : ( 46, 'E1',  'C7'),
    'Orchestral_Harp'                  : ( 47, 'B0',  'G#7'),
    'Timpani'                          : ( 48, 'C2',  'D4'),
    # Ensemble
    'String_Ensemble_1'                : ( 49, 'C0',  'C8'),
    'String_Ensemble_2'                : ( 50, 'C0',  'C8'),
    'Synth_Strings_1'                  : ( 51, 'C0',  'C8'),
    'Synth_Strings_2'                  : ( 52, 'C0',  'C8'),
    'Choir_Aahs'                       : ( 53, 'D2',  'C6'),
    'Voice_Oohs'                       : ( 54, 'D2',  'C6'),
    'Synth_Voice'                      : ( 55, 'D2',  'C6'),
    'Orchestra_Hit'                    : ( 56, 'C0',  'C8'),
    # Brass
    'Trumpet'                          : ( 57, 'F#3', 'C6'),
    'Trombone'                         : ( 58, 'C2',  'C5'),
    'Tuba'                             : ( 59, 'Eb2', 'A4'),
    'Muted_Trumpet'                    : ( 60, 'F#3', 'C6'),
    'French_Horn'                      : ( 61, 'C2',  'C6'),
    'Brass_Section'                    : ( 62, 'C2',  'C6'),
    'Synth_Brass_1'                    : ( 63, 'C2',  'C6'),
    'Synth_Brass_2'                    : ( 64, 'C2',  'C6'),
    # Reed
    'Soprano_Sax'                      : ( 65, 'Ab3', 'Eb6'),
    'Alto_Sax'                         : ( 66, 'Db3', 'Ab5'),
    'Tenor_Sax'                        : ( 67, 'Ab2', 'Eb5'),
    'Baritone_Sax'                     : ( 68, 'Db2', 'Ab4'),
    'Oboe'                             : ( 69, 'Bb3', 'G6'),
    'English_Horn'                     : ( 70, 'E3',  'A5'),
    'Bassoon'                          : ( 71, 'Bb1', 'E5'),
    'Clarinet'                         : ( 72, 'D3',  'Bb6'),
    # Pipe
    'Piccolo'                          : ( 73, 'D5',  'C8'),
    'Flute'                            : ( 74, 'C4',  'C7'),
    'Recorder'                         : ( 75, 'C-1', 'G9'),
    'Pan_Flute'                        : ( 76, 'C-1', 'G9'),
    'Blown_bottle'                     : ( 77, 'C-1', 'G9'),
    'Shakuhachi'                       : ( 78, 'C-1', 'G9'),
    'Whistle'                          : ( 79, 'C-1', 'G9'),
    'Ocarina'                          : ( 80, 'C-1', 'G9'),
    # Synth Lead
    'Lead_1_(square)'                  : ( 81, 'C0',  'C8'),
    'Lead_2_(sawtooth)'                : ( 82, 'C0',  'C8'),
    'Lead_3_(calliope)'                : ( 83, 'C0',  'C8'),
    'Lead_4_(chiff)'                   : ( 84, 'C0',  'C8'),
    'Lead_5_(charang)'                 : ( 85, 'C0',  'C8'),
    'Lead_6_(voice)'                   : ( 86, 'C0',  'C8'),
    'Lead_7_(fifths)'                  : ( 87, 'C0',  'C8'),
    'Lead_8_(bass_+_lead)'             : ( 88, 'C0',  'C8'),
    # Synth Pad
    'Pad_1_(new_age)'                  : ( 89, 'C0',  'C8'),
    'Pad_2_(warm)'                     : ( 90, 'C0',  'C8'),
    'Pad_3_(polysynth)'                : ( 91, 'C0',  'C8'),
    'Pad_4_(choir)'                    : ( 92, 'C2',  'G5'),
    'Pad_5_(bowed)'                    : ( 93, 'C0',  'C8'),
    'Pad_6_(metallic)'                 : ( 94, 'C0',  'C8'),
    'Pad_7_(halo)'                     : ( 95, 'C0',  'C8'),
    'Pad_8_(sweep)'                    : ( 96, 'C0',  'C8'),
    # Synth Effects
    'FX_1_(rain)'                      : ( 97, 'C0',  'C8'),
    'FX_2_(soundtrack)'                : ( 98, 'C0',  'C8'),
    'FX_3_(crystal)'                   : ( 99, 'C0',  'C8'),
    'FX_4_(atmosphere)'                : (100, 'C0',  'C8'),
    'FX_5_(brightness)'                : (101, 'C0',  'C8'),
    'FX_6_(goblins)'                   : (102, 'C0',  'C8'),
    'FX_7_(echoes)'                    : (103, 'C0',  'C8'),
    'FX_8_(sci-fi)'                    : (104, 'C0',  'C8'),
    # Ethnic
    'Sitar'                            : (105, 'C2',  'C6'),
    'Banjo'                            : (106, 'D3',  'C6'),
    'Shamisen'                         : (107, 'E3',  'C6'),
    'Koto'                             : (108, 'C2',  'C6'),
    'Kalimba'                          : (109, 'G3',  'G6'),
    'Bag pipe'                         : (110, 'C3',  'C6'),
    'Fiddle'                           : (111, 'G3',  'C7'),
    'Shanai'                           : (112, 'Bb3', 'G6'),
    # Percussive
    'Tinkle Bell'                      : (113, 'C-1', 'G9'),
    'Agogô'                            : (114, 'C-1', 'G9'),
    'Steel_Drums'                      : (115, 'C-1', 'G9'),
    'Woodblock'                        : (116, 'C-1', 'G9'),
    'Taiko_Drum'                       : (117, 'C-1', 'G9'),
    'Melodic_Tom'                      : (118, 'C-1', 'G9'),
    'Synth_Drum'                       : (119, 'C-1', 'G9'),
    'Reverse_Cymbal'                   : (120, 'C-1', 'G9'),
    # Sound Effects
    'Guitar_Fret_Noise'                : (121, 'C-1', 'G9'),
    'Breath_Noise'                     : (122, 'C-1', 'G9'),
    'Seashore'                         : (123, 'C-1', 'G9'),
    'Bird_Tweet'                       : (124, 'C-1', 'G9'),
    'Telephone_Ring'                   : (125, 'C-1', 'G9'),
    'Helicopter'                       : (126, 'C-1', 'G9'),
    'Applause'                         : (127, 'C-1', 'G9'),
    'Gunshot'                          : (128, 'C-1', 'G9'),
}
GM_PROGRAM_INDEX = 0
GM_LOW_PITCH_INDEX = 1
GM_HIGH_PITCH_INDEX = 2
