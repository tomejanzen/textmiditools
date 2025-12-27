#!/usr/bin/m4
changecom(`;')
;Beethoven: d minor Symphony: molto vivace in textmidi language © 2024 by Thomas E. Janzen is licensed under CC BY 4.0 🅯🅭
define(`pppp',`vel 10')
define(`ppp',`vel 30')
define(`pp',`vel 40')
define(`p',`vel 50')
define(`mp',`vel 62')
define(`mf',`vel 75')
define(`forte',`vel 90')
define(`ff',`vel 110')
define(`fff',`vel 120')
define(`ffff',`vel 127')
define(`pedaldown',`END_LAZY DAMPER 1 127 LAZY ')
define(`pedalup',`END_LAZY DAMPER 1 0 LAZY ')
define(`pedalchange',`END_LAZY DAMPER 1 0 DAMPER 1 127 LAZY ')

FILEHEADER 3 240

STARTTRACK
TEMPO 348
TIME_SIGNATURE 3 4 24
KEY_SIGNATURE d
TRACK TEMPO
PROGRAM 1 1
CONTROL 1 CHANNEL_VOLUME 127
TEXT Ludwig van Beethoven Symphony in D minor: Molto vivace
TEXT Transcribed for piano by Franz Liszt.
TEXT Sequenced using the textmidi text-to-MIDI MIDI assembler.
TEXT © 2024 by Thomas E. Janzen is licensed under CC BY 4.0 🅯🅭
;© 2024 by Thomas E. Janzen is licensed under CC BY 4.0 🅯🅭

LAZY
include(`lvb9_2fanfare_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after fanfare tempo
LAZY
include(`lvb9_2repeat1_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat1 tempo
LAZY
include(`lvb9_2repeat1_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat1 tempo
LAZY
include(`lvb9_2bridge_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after bridge tempo
LAZY
include(`lvb9_2repeat2_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat2 tempo
LAZY
;m 530
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
END_LAZY
ticks lvb9_2.m4 after transition tempo
LAZY
; end of first ending new year's eve
include(`lvb9_2repeat2_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat2 tempo
LAZY
include(`lvb9_2bridge2_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat3 tempo
LAZY
include(`lvb9_2repeat3_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat3 tempo
LAZY
;m 805.1
R 1
;m 806.1
R 1
include(`lvb9_2repeat3_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat3 tempo
LAZY
; second ending
; the next measure has the repeat sign in the middle of it.
;m 813
R 1
include(`lvb9_2repeat4_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat4 tempo
LAZY
;m 882 next measure has the end repeat in mid-bar
R 1
include(`lvb9_2repeat4_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat4 tempo
LAZY
include(`lvb9_2ending_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after ending tempo
LAZY

END_LAZY
TEMPO 348
LAZY
include(`lvb9_2fanfare_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after fanfare tempo
LAZY
include(`lvb9_2repeat1_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat1 tempo
LAZY
include(`lvb9_2bridge_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after bridge tempo
LAZY
include(`lvb9_2repeat2_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after repeat2 tempo
LAZY
include(`lvb9_2coda_tempo.m4')
END_LAZY
ticks lvb9_2.m4 after coda tempo
LAZY


END_LAZY
END_OF_TRACK

STARTTRACK
TRACK Piano1
PROGRAM 1 1
LAZY 
chan 1
include(`lvb9_2fanfare_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after fanfare piano1
LAZY
include(`lvb9_2repeat1_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat1 piano1
LAZY
include(`lvb9_2repeat1_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat1 piano1
LAZY
include(`lvb9_2bridge_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after bridge piano1
LAZY
include(`lvb9_2repeat2_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat2 piano1
LAZY
; start of first ending
;m 530
A4 D5 F4 D3 D2 8 R 4 A3 D4 F3 8 A3 D4 F3 8 R 8
F5 D6 D5 Bb3 Bb2 8 R 4 Bb4 D5 F4 8 Bb4 D5 F4 8 R 8
Bb4 D5 F4 F3 F2 8 R 4 Bb3 D4 F3 8 Bb3 D4 F3 8 R 8
Bb5 D6 F5 D3 D2 8 R 4 Bb4 D5 F4 8 Bb4 D5 F4 8 R 8
Bb4 D5 F5 F4 Bb2 Bb1 8 R 4 Bb3 D4 F3 8 Bb3 D4 F3 8 R 8
Bb5 D6 D5 F3 F2 8 R 4 D5 Bb5 Bb4 8 D5 Bb5 Bb4 8 R 8
F5 Bb5 Bb4 D3 D2 8 R 4 F3 Bb3 D3 8 F3 Bb3 D3 8 R 8
G5 Bb5 Bb4 Eb3 Eb2 8 R 4 Eb2 G4 Bb4 Eb4 G3 Bb3 Eb3 8 G4 Bb4 Eb4 G3 Bb3 Eb3 8 R 8
G4 Bb4 Eb4 G3 Bb3 Eb3 8 R 8 R 2
R 2.
R 2.
R 2.
END_LAZY
ticks lvb9_2.m4 after transition piano1
LAZY
; end of first ending new year's eve
include(`lvb9_2repeat2_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat2 piano1
LAZY
include(`lvb9_2bridge2_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat3 piano1
LAZY
include(`lvb9_2repeat3_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat3 piano1
LAZY
;m 805.1 ignore Liszt's double hits on same D5
E5 Db5 A3 8 R 4. D5- D4 8 R 8 Db4 8 R 8
;m 806.1
D4 8 R 8 B3 8 R 8 E5- A3 8 R 8 G3 8 -D5 -E5 8
include(`lvb9_2repeat3_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat3 piano1
LAZY
; the next measure has the repeat sign in the middle of it.
;m 813
A3 Db5 E5 8 R 4.  D2- D3 4 E3 4
include(`lvb9_2repeat4_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat4 piano1
LAZY
;m 882 =951 next measure has the end repeat in mid-bar
Gb5 D6 D5 D3 D2 4 R 4 D2- D3 4 E3 4
include(`lvb9_2repeat4_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat4 piano1
LAZY
include(`lvb9_2ending_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after ending piano1
LAZY


include(`lvb9_2fanfare_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after fanfare piano1
LAZY
include(`lvb9_2repeat1_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat1 piano1
LAZY
include(`lvb9_2bridge_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after bridge piano1
LAZY
include(`lvb9_2repeat2_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after repeat2 piano1
LAZY
include(`lvb9_2coda_piano1.m4')
END_LAZY
ticks lvb9_2.m4 after coda piano1
LAZY


END_LAZY
END_OF_TRACK

STARTTRACK
TRACK Piano2
PROGRAM 1 1
LAZY 
chan 1
include(`lvb9_2fanfare_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after fanfare piano2
LAZY
include(`lvb9_2repeat1_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat1 piano2
LAZY
include(`lvb9_2repeat1_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat1 piano2
LAZY
include(`lvb9_2bridge_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after bridge piano2
LAZY
include(`lvb9_2repeat2_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat2 piano2
LAZY
; start of first ending
;m 530
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
R 2.
END_LAZY
ticks lvb9_2.m4 after transition piano2
LAZY
; end of first ending new year's eve
include(`lvb9_2repeat2_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat2 piano2
LAZY
include(`lvb9_2bridge2_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat3 piano2
LAZY
include(`lvb9_2repeat3_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat3 piano2
LAZY
;m 805.1
R 1
;m 806.1
R 1
include(`lvb9_2repeat3_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat3 piano2
LAZY
; second ending
; the next measure has the repeat sign in the middle of it.
;m 813
R 1
include(`lvb9_2repeat4_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat4 piano2
LAZY
;m 882 next measure has the end repeat in mid-bar
R 1
include(`lvb9_2repeat4_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat4 piano2
LAZY
include(`lvb9_2ending_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after ending piano2
LAZY

include(`lvb9_2fanfare_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after fanfare piano2
LAZY
include(`lvb9_2repeat1_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat1 piano2
LAZY
include(`lvb9_2bridge_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after bridge piano2
LAZY
include(`lvb9_2repeat2_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after repeat2 piano2
LAZY
include(`lvb9_2coda_piano2.m4')
END_LAZY
ticks lvb9_2.m4 after coda piano2
LAZY



END_LAZY
END_OF_TRACK


