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

syntax keyword midiEvent FILEHEADER TEXT TEXT_08 TEXT_09 TEXT_0A TEXT_0B TEXT_0C TEXT_0D TEXT_0E TEXT_0F COPYRIGHT TRACK END_OF_TRACK INSTRUMENT LYRIC MARKER CUE_POINT TEMPO SEQUENCER_SPECIFIC STARTTRACK NOTE_ON NOTE_OFF POLY_KEY_PRESSURE CONTROL RESETALL ALLNOTESOFF PAN BREATH DAMPER SOFTPEDAL PORTAMENTO SOSTENUTO PROGRAM CHANNEL_PRESSURE PITCH_WHEEL TIME_SIGNATURE KEY_SIGNATURE DYNAMIC DELAY MIDI_PORT MIDI_CHANNEL ALLNOTES_OFF ALL_SOUND_OFF SMPTE_OFFSET UNKNOWN_META SYSEX SYSEXRAW SEQUENCE_NUMBER PROGRAM_NAME DEVICE_NAME CHANNEL_VOLUME
syntax keyword lazyEvent vel chan
syntax keyword lazyRest r R
syntax keyword lazycmd LAZY END_LAZY
syntax match NOTE_LETTER "[A-G]\|[a-g]"
"syntax match anote \([ABCDEFG]|[abcdefg]\)[b#]\?[0123456789]
syntax match anote "[ABCDEFGabcdefg]\([b#]\)\?[0123456789]"
syntax match DYNAMIC "\(^\|[^[:alnum:]]\)\(pppp\|ppp\|pp\|p\|mp\|mf\|forte\|ff\|fff\|ffff\|PPPP\|PPP\|PP\|P\|MP\|MF\|FORTE\|FF\|FFF\|FFFF\)\(+\?\)\([^[:alnum:]]\|$\)"

syn match midicomment ";.*"
hi def link midiEvent Statement
hi def link lazyEvent Todo
hi def link midicomment Comment
hi def link anote Identifier
hi def link lazyRest Identifier
hi def link DYNAMIC Special
hi def link lazycmd Todo
