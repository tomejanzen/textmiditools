" MIDI textmidi assembler language file
" Language: textmidi
" Current Maintainer: Thomas Janzen
" Previous Maintainer: Thomas Janzen
" Last Chane: 2019 May 15
"
" quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syntax keyword midiEvent FILEHEADER TEXT TEXT_08 TEXT_09 TEXT_0A TEXT_0B TEXT_0C TEXT_0D TEXT_0E TEXT_0F COPYRIGHT TRACK END_OF_TRACK INSTRUMENT LYRIC MARKER CUE_POINT TEMPO SEQUENCER_SPECIFIC STARTTRACK NOTE_ON NOTE_OFF POLY_KEY_PRESSURE CONTROL PAN BREATH DAMPER SOFTPEDAL PORTAMENTO SOSTENUTO LEGATO_FOOT PROGRAM CHANNEL_PRESSURE PITCH_WHEEL TIME_SIGNATURE KEY_SIGNATURE XMF_PATCH_TYPE DYNAMIC DELAY MIDI_PORT MIDI_CHANNEL SMPTE_OFFSET UNKNOWN_META SYSEX SYSEXRAW SEQUENCE_NUMBER PROGRAM_NAME DEVICE_NAME CHANNEL_VOLUME EXPRESSION BANK_SELECT REVERB_SEND_LEVEL CHORUS_SEND_LEVEL SMPTE_24FPS SMPTE_25FPS SMPTE_30FPSDROPFRAME SMPTE_30FPSNONDROPFRAME LEFT RIGHT CENTER PORTAMENTO_TIME REGISTERED_PARAMETER_MSB REGISTERED_PARAMETER_LSB DATA_ENTRY_MSB DATA_ENTRY_LSB 

syntax keyword Control BREATH SOFTPEDAL PORTAMENTO BANK_SELECT MODULATION FOOT_CONTROLLER PORTAMENTO_TIME DATA_ENTRY_MSB CHANNEL_VOLUME BALANCE PAN EXPRESSION EFFECT_1 EFFECT_2 GENERAL_PURPOSE_1 GENERAL_PURPOSE_2 GENERAL_PURPOSE_3 GENERAL_PURPOSE_4 DAMPER PORTAMENTO_ON_OFF SOSTENUTO LEGATO_FOOT HOLD_2 SOUND_VARIATION TIMBRE_INTENSITY RELEASE_TIME ATTACK_TIME BRIGHTNESS DECAY_TIME VIBRATO_RATE VIBRATO_DEPTH SOUND_CONTROLLER_9 SOUND_CONTROLLER_10 GENERAL_PURPOSE_5 GENERAL_PURPOSE_6 GENERAL_PURPOSE_7 GENERAL_PURPOSE_8 HIRES_VELOCITY_MSB REVERB_SEND_LEVEL TREMOLO_DEPTH CHORUS_SEND_LEVEL CELESTE_DEPTH PHASER_DEPTH DATA_INCREMENT DATA_DECREMENT NON_REGISTERED_PARAMETER_LSB NON_REGISTERED_PARAMETER_MSB REGISTERED_PARAMETER_LSB REGISTERED_PARAMETER_MSB ALL_SOUND_OFF RESET_ALL_CONTROLLERS LOCAL_CONTROL ALL_NOTES_OFF OMNI_OFF OMNI_ON MONO_ON POLY_ON LSB_00 LSB_01 LSB_02 LSB_03 LSB_04 LSB_05 LSB_06 LSB_07 LSB_08 LSB_09 LSB_10 LSB_11 LSB_12 LSB_13 LSB_14 LSB_15 LSB_16 LSB_17 LSB_18 LSB_19 LSB_20 LSB_21 LSB_22 LSB_23 LSB_24 LSB_25 LSB_26 LSB_27 LSB_28 LSB_29 LSB_30 LSB_31 AZIMUTH ELEVATION GAIN DISTANCE MAXIMUM_DISTANCE GAIN_AT_MAX_DISTANCE REFERENCE_DISTANCE_RATIO PAN_SPREAD_ANGLE ROLL_ANGLE
syntax keyword CommandValue ON OFF SMPTE_24FPS SMPTE_25FPS SMPTE_30FPSDROPFRAME SMPTE_30FPSNONDROPFRAME LEFT RIGHT CENTER GM1 GM2 DLS
syntax keyword SysexDeviceid ALL_CALL
syntax keyword SysexSubid NON_COMMERCIAL NON_REALTIME REALTIME
syntax keyword SysexNonrtSubid1 NONRT_SAMPLE_DUMP_HEADER NONRT_SAMPLE_DATA_PACKET NONRT_SAMPLE_DUMP_REQUEST NONRT_TIMECODE NONRT_SAMPLE_DUMP_EXTENSIONS NONRT_GENERAL_INFO NONRT_FILE_DUMP NONRT_TUNING_STD NONRT_GM NONRT_END_OF_FILE NONRT_WAIT NONRT_CANCEL NONRT_NAK NONRT_ACK
syntax keyword SysexRtSubid1 RT_TIMECODE RT_SHOW_CONTROL RT_NOTATION_INFORMATION RT_DEVICE_CONTROL RT_MTC_CUEING RT_MACHINE_CONTROL_COMMANDS RT_MACHINE_CONTROL_RESPONSES RT_TUNING_STD

syntax keyword lazyEvent vel chan
syntax keyword lazyRest r R
syntax keyword lazycmd LAZY END_LAZY
syntax match NOTE_LETTER "[A-G]\|[a-g]"
syntax match anote "[ABCDEFGabcdefg]\([b#]\)\?[0123456789]"
syntax match DYNAMIC "\(^\|[^[:alnum:]]\)\(pppp\|ppp\|pp\|p\|mp\|mf\|forte\|ff\|fff\|ffff\|PPPP\|PPP\|PP\|P\|MP\|MF\|FORTE\|FF\|FFF\|FFFF\)\(+\?\)\([^[:alnum:]]\|$\)"

syntax match simplecontinuedfraction "[[]\([0-9]\)\+\(;[1-9]\+\(,[1-9][0-9]*\)*\)[]]"
syn match midicomment ";.*"
hi def link midiEvent Statement
hi def link CommandValue Type
hi def link lazyEvent Todo
hi def link Control Identifier
hi def link SysexDeviceid Identifier
hi def link SysexSubid Identifier
hi def link SysexNonrtSubid1 Identifier
hi def link SysexRtSubid1 Identifier
hi def link midicomment Comment
"hi def link simplecontinuedfraction Identifier
hi def link anote Identifier
hi def link lazyRest Identifier
hi def link DYNAMIC Special
hi def link lazycmd Todo
