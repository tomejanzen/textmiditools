;Schumann Quintet in textmidi language © 2003 by Thomas E. Janzen is licensed under CC BY 4.0 🅯🅭
include(`instruments.m4')
include(`percussion.m4')
define(`intmidi',`internal')

define(`ticks',`240')
define(`midi_channel_piano',5)
define(`midi_channel_violin1',1)
define(`midi_channel_violin2',2)
define(`midi_channel_viola',3)
define(`midi_channel_cello',4)
define(`midi_channel_click',16)
define(`midi_channel_tempo',percussionchannel)

define(`damperdown', `END_LAZY DAMPER 5 127 LAZY')
define(`damperup',   `END_LAZY DAMPER 5   0 LAZY')
define(`damper', `damperup damperdown')
define(`damperbar',`r 16 damperdown r 14/16 damperup r 16')
define(`damperhalfbar',`r 16 damperdown r 6/16 damperup r 16 r 16 damperdown r 6/16 damperup r 16')

define(`oct', `format(`%s%d %s%d', $1, $2, $1, eval($2 + 1))')
define(`oct3', `format(`%s%d %s%d %s%d', $1, $2, $1, eval($2 + 1), $1, eval($2 + 2))')
define(`oct8', `oct($1, $2) 8')
define(`brokenoct8', `format(`%s%d 8 %s%d 8', $1,$2,$1,eval($2 + 1))')
define(`oct16', `oct($1, $2) 16')
define(`oct24', `oct($1, $2) 24')
define(`oct4', `oct($1, $2) 4')
define(`stacoct4', `oct($1, $2) 6 r 12  ')
define(`oct34', `oct3($1, $2) 4')
define(`oct32', `oct3($1, $2) 2')
define(`oct2', `oct($1, $2) 2')
define(`oct1', `oct($1, $2) 1')
define(`octs8', `format(`%s%d', $1, $2) oct($1, eval($2+2)) 8')
define(`stacoct34', `oct3($1, $2) 6 r 12')
define(`octstac8', `oct16($1,$2) r 16')
define(`octstac12', `oct24($1,$2) r 24')
define(`stacoct8', `oct($1, $2) 16 r 16')
define(`turnoct',`oct16($1,$2) oct16($3,$4) oct16($1,$2) oct16($5,$6)')

define(`triplet',`$1 12 $2 12 $3 12')
define(`tripletstac',`$1 12 $2 12 $3 24 R 24')
define(`chordtripa',`$1 6 $2 $3 $4 6 $1 6') 
define(`chordtripb',`$2 $3 $4 6 $1 6 $2 $3 $4 6') 
define(`triplet4a',`$1 6 $2 6 $1 6') 
define(`triplet4b',`$2 6 $1 6 $2 6') 

define(`slowtrillr',`R 8 $2 8 $1 8 $2 8')
define(`slowtrill', `$1 8 $2 8 $1 8 $2 8')

define(`stac8',`$1 16 R 16')
define(`stac8r8',`$1 16. R 32 r 8')
define(`eightr8',`$1 8 r 8')
define(`stac4',`$1 8 R 8')
define(`stac12',`$1 24 R 24')

define(`barsrest',`DELAY eval(4*ticks*$1)')
define(`barsrest68',`DELAY eval((4*ticks*$1*6)/8)')

define(`bowtrem4_68',`$1 16 $1 16 $1 16 $1 16')

ifdef(`polypressure',`define(`bowtrem4a',`END_LAZY 
NOTE_ON 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
NOTE_OFF 2 $1 40
LAZY')',
'define(`bowtrem4a',`$1 24 $1 24 $1 24 $1 24 $1 24 $1 24')')

define(`trem82', `$1 8 $1 8 $1 8 $1 8')
define(`trem82decres', `ff $1 8 mf $1 8 mp $1 8 p $1 8')
define(`trem84', `$1 8 $1 8')

define(`gracer', `r 8 $1 16 $2 16')
define(`graces', `$1 16 $2 16')

define(`caesura',`r 16')
define(`caesura8',`r 8')

ifdef(`intmidi',`define(`pizz',    `END_LAZY PROGRAM $1 46 LAZY')',`define(`pizz', `chan 6')')
ifdef(`intmidi',`define(`endpizz', `END_LAZY PROGRAM $1 $2 LAZY')',`define(`endpizz',`chan $1')')
ifdef(`intmidi',`define(`program', `PROGRAM $1 $2')',`define(`program',`')')

; pan is -64 to 63; 0 is center; LEFT CENTER RIGHT should work too.
ifdef(`threedee',`define(`pan',`')',`define(`pan',`PAN $1 $2')')

define(`vela', `vel 60')
define(`velb', `vel 62')
define(`velc', `vel 64')
define(`veld', `vel 66')
define(`vele', `vel 68')
define(`velf', `vel 70')
define(`velg', `vel 72')
define(`velh', `vel 74')
define(`veli', `vel 76')
define(`velj', `vel 78')
define(`velk', `vel 80')
define(`vell', `vel 82')
define(`velm', `vel 84')
define(`veln', `vel 86')
define(`velo', `vel 88')
define(`velp', `vel 90')
define(`velq', `vel 92')
define(`velr', `vel 94')
define(`vels', `vel 96')
define(`velt', `vel 98')
define(`velu', `vel 100')
define(`velv', `vel 102')
define(`velw', `vel 104')
define(`velx', `vel 106')
define(`vely', `vel 108')
define(`velz', `vel 110')
define(`velaa', `vel 112')
define(`velab', `vel 114')
define(`velac', `vel 116')
define(`velad', `vel 118')
define(`velae', `vel 120')

define(`pocostac4',`$1 8. R 16')
define(`pocostac8',`$1 16. R 32')

define(`stac4b',`$1 1/6 R 1/12')

; pass in channel, bend amount 0 to 8k-1
define(`bend', `PITCH_WHEEL $1 $2');

; mordent(note1, note2, rhythm1, rhythm2)
define(`mordent',`$1 $3 $2 $3 $1 $4')
define(`mordent4',`mordent($1, $2, 16, 8)')

define(`stac8b',`$1 12 R 24')

define(`turn',` $1 16 $2 16 $1 16 $3 16 ')
define(`turnacc',` forte $1 16 mf $2 16 $1 16 $3 16 ')


define(`trem',`$1 $2 $3 16 $4 16 $1 $2 $3 16 $4 16 $1 $2 $3 16 R 16 R 8 ')

define(`bassline', `forte $1 8 $2 $3 $4 8 ff $2 $3 $4 4')
define(`basslinep', `p $1 8 $2 $3 $4 8 mp $2 $3 $4 4')
define(`basslinedot', `forte $1 8. $2 $3 $4 16 ff $2 $3 $4 4')
define(`basstrem',`$1 16 $2 16 $3 $4 16 $2 16 $3 $4 16 r 16 r 8')

; pass in channel, pitchname, velocity
define(`porttrailingdown38', `END_LAZY NOTE_ON $1 $2 $3 DELAY 270 
bend($1, 8021) DELAY 10
bend($1, 7850) DELAY 10
bend($1, 7680) DELAY 10 
bend($1, 7509) DELAY 10 
bend($1, 7338) DELAY 10 
bend($1, 7168) DELAY 10 
bend($1, 6997) DELAY 10 
bend($1, 6826) DELAY 10 
NOTE_ON $1 $2 0 bend($1, 8192) DELAY 10 LAZY')

 ; pass in channel, pitchname, velocity
define(`porttrailingup4', `END_LAZY NOTE_ON $1 $2 $3 DELAY 120
bend($1, 8362) DELAY 10
bend($1, 8532) DELAY 10
bend($1, 8702) DELAY 10
bend($1, 8872) DELAY 10 
bend($1, 9042) DELAY 10 
bend($1, 9212) DELAY 10 
bend($1, 9382) DELAY 10 
bend($1, 9552) DELAY 10 
bend($1, 9722) DELAY 10 
bend($1, 9892) DELAY 10 
bend($1,10962) DELAY 10 
NOTE_ON $1 $2 0 bend($1, 8192) DELAY 10 LAZY')



;decresctone(channel,pitch,len_in_whole_notes,startvel,endvel)
ifdef(`polypressure',`define(`decresctone',`
END_LAZY
NOTE_ON $1 $2 $4
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((39*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((38*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((37*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((36*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((35*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((34*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((33*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((32*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((31*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((30*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((29*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((28*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((27*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((26*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((25*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((24*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((23*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((22*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((21*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((20*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((19*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((18*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((17*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((16*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((15*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((14*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((13*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((12*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((11*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((10*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((9*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((8*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((7*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((6*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((5*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((4*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((3*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((2*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((1*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((0*($4-$5))/39)+$5)
NOTE_OFF $1 $2 0
LAZY')',`define(`decresctone',`vel $4 $2 $3')')

;decresctonestop(channel,pitch,len_in_whole_notes,startvel,endvel,pitch2)
ifdef(`polypressure',`define(`decresctonestop',`
END_LAZY
NOTE_ON $1 $2 $4
NOTE_ON $1 $6 $4
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((39*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((39*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((38*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((38*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((37*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((37*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((36*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((36*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((35*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((35*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((34*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((34*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((33*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((33*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((32*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((32*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((31*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((31*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((30*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((30*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((29*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((29*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((28*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((28*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((27*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((27*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((26*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((26*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((25*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((25*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((24*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((24*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((23*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((23*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((22*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((22*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((21*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((21*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((20*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((20*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((19*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((19*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((18*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((18*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((17*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((17*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((16*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((16*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((15*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((15*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((14*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((14*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((13*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((13*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((12*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((12*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((11*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((11*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((10*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((10*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((9*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((9*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((8*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((8*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((7*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((7*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((6*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((6*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((5*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((5*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((4*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((4*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((3*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((3*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((2*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((2*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((1*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((1*($4-$5))/39)+$5)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((0*($4-$5))/39)+$5)
                        POLY_KEY_PRESSURE $1 $6 eval(((0*($4-$5))/39)+$5)
NOTE_OFF $1 $2 0
NOTE_OFF $1 $6 0
LAZY')',`define(`decresctonestop',`vel $4 $6 $2 $3')')

;longtone(channel,pitch,len_in_whole_notes,startvel,midvel)
ifdef(`polypressure',`define(`longtone',`
END_LAZY
NOTE_ON $1 $2 $4
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((0*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((2*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((4*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((6*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((8*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((10*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((12*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((14*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((16*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((18*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((20*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((22*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((24*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((26*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((28*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((30*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((32*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((34*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((36*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((38*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((39*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((37*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((35*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((33*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((31*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((29*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((27*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((25*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((23*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((21*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((19*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((17*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((15*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((13*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((11*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((9*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((7*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((5*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((3*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((1*($5-$4))/39)+$4)
NOTE_OFF $1 $2 0
LAZY')',`define(`longtone',`vel $5 $2 $3')')

;longtonestop(channel,pitch,len_in_whole_notes,startvel,midvel,pitch2)
ifdef(`polypressure',`define(`longtonestop',`
END_LAZY
NOTE_ON $1 $2 $4
NOTE_ON $1 $6 $4
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((0*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((0*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((2*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((2*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((4*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((4*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((6*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((6*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((8*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((8*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((10*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((10*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((12*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((12*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((14*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((14*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((16*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((16*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((18*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((18*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((20*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((20*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((22*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((22*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((24*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((24*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((26*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((26*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((28*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((28*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((30*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((30*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((32*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((32*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((34*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((34*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((36*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((36*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((38*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((38*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((39*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((39*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((37*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((37*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((35*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((35*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((33*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((33*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((31*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((31*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((29*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((29*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((27*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((27*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((25*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((25*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((23*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((23*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((21*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((21*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((19*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((19*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((17*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((17*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((15*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((15*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((13*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((13*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((11*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((11*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((9*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((9*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((7*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((7*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((5*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((5*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((3*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((3*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((1*($5-$4))/39)+$4)
POLY_KEY_PRESSURE $1 $6 eval(((1*($5-$4))/39)+$4)
NOTE_OFF $1 $2 0
NOTE_OFF $1 $6 0
LAZY')',`define(`longtonestop',`vel $5 $2 $6 $3')')

;longtonefront(channel,pitch,len_in_whole_notes,startvel,midvel)
ifdef(`polypressure',`define(`longtonefront',`
END_LAZY
NOTE_ON $1 $2 $4
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((0*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((2*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((4*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((6*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((8*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((10*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((12*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((14*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((16*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((18*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((20*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((22*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((24*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((26*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((28*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((30*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((32*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((34*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((36*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((38*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((39*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((37*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((35*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((33*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((31*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((29*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((27*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((25*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((23*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((21*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((19*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((17*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((15*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((13*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((11*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((9*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((7*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((5*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((3*($5-$4))/39)+$4)
DELAY eval((960*$3)/80) CHANNEL_PRESSURE $1 eval(((1*($5-$4))/39)+$4)
DELAY eval((960*$3)/2)
NOTE_OFF $1 $2 0
LAZY')',`define(`longtonefront',`vel $5 $2 $3')')

;longtone23 lopsided 2/3 cresc 1/3 decresc
;longtone23(channel,pitch,len_in_whole_notes,startvel,midvel)
ifdef(`polypressure',`define(`longtone23',`
END_LAZY
NOTE_ON $1 $2 $4
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((0*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((1*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((3*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((4*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((6*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((7*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((9*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((10*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((12*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((13*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((15*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((16*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((18*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((19*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((21*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((22*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((24*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((25*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((27*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((28*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((29*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((31*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((33*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((35*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((37*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((39*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((36*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((33*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((30*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((27*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((24*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((21*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((18*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((15*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((12*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((9*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((6*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((3*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((2*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((1*($5-$4))/39)+$4)
NOTE_OFF $1 $2 0
LAZY')',`define(`longtone23',`vel $5 $2 $3')')

;sound card ppp is almost too quiet to hear so promote it to pp
ifdef(`polypressure',`define(`ppp',`vel 25')',`define(`ppp',`pp')')

define(`figure1',
`vel eval($5+20) $1 16. R 32 vel eval($5-20) $2 16. R 32 vel eval($5-15) $3 16. R 32 vel $5 $4 16. R 32')

ifdef(`polypressure',`define(`cresctone',`
END_LAZY
NOTE_ON $1 $2 $4
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((0*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((1*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((2*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((3*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((4*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((5*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((6*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((7*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((8*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((9*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((10*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((11*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((12*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((13*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((14*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((15*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((16*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((17*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((18*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((19*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((20*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((21*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((22*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((23*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((24*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((25*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((26*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((27*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((28*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((29*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((30*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((31*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((32*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((33*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((34*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((35*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((36*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((37*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((38*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((39*($5-$4))/39)+$4)
NOTE_OFF $1 $2 0
LAZY')',`define(`cresctone',`vel $5 $2 $3')')


ifdef(`polypressure',`define(`bowtrem4',`END_LAZY 
NOTE_ON 2 $1 40 
NOTE_ON 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
NOTE_OFF 2 $1 40 NOTE_OFF 2 $2 40
LAZY')',
'define(`bowtrem4',`$1 24 $1 24 $1 24 $1 24 $1 24 $1 24')')

ifdef(`polypressure',`define(`bowtrem2',`END_LAZY 
NOTE_ON 2 $1 40 
NOTE_ON 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40

NOTE_OFF 2 $1 40 
NOTE_OFF 2 $2 40
LAZY')',
'define(`bowtrem2',`$1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24')')

ifdef(`polypressure',`define(`bowtrem2decr',`END_LAZY
NOTE_ON 2 $1 $3
NOTE_ON 2 $2 $3
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(11*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(11*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(10*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(10*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(9*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(9*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(8*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(8*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(7*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(7*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(6*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(6*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(5*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(5*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(4*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(4*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(3*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(3*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(2*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(2*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(1*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(1*(($3-$4)/12)) 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 eval(1*(($3-$4)/12)) POLY_KEY_PRESSURE 2 $2 eval(1*(($3-$4)/12)) 
NOTE_OFF 2 $1 0
NOTE_OFF 2 $2 0
LAZY')',
'define(`bowtrem2decr',`$1 24 vel eval(12*(($3-$4)/12)) $1 24 vel eval(11*(($3-$4)/12)) $1 24 vel eval(10*(($3-$4)/12)) $1 24 vel eval(9*(($3-$4)/12)) $1 24 vel eval(8*(($3-$4)/12)) $1 24 vel eval(7*(($3-$4)/12)) $1 24 vel eval(6*(($3-$4)/12)) $1 24 vel eval(5*(($3-$4)/12)) $1 24 vel eval(4*(($3-$4)/12)) $1 24 vel eval(3*(($3-$4)/12)) $1 24 vel eval(2*(($3-$4)/12)) $1 24')')

ifdef(`polypressure',`define(`bowtrem1',`END_LAZY 
NOTE_ON 2 $1 40 
NOTE_ON 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 POLY_KEY_PRESSURE 2 $2  0
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 POLY_KEY_PRESSURE 2 $2 40

NOTE_OFF 2 $1 40 NOTE_OFF 2 $2 40
LAZY')',
'define(`bowtrem1',`$1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24')')

ifdef(`polypressure',`define(`bowtrem2a',`END_LAZY 
NOTE_ON 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
NOTE_OFF 2 $1 40 
LAZY')',
'define(`bowtrem2a',`$1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24')')

ifdef(`polypressure',`define(`bowtrem1a',`END_LAZY 
NOTE_ON 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
DELAY 30 POLY_KEY_PRESSURE 2 $1  0 
DELAY 10 POLY_KEY_PRESSURE 2 $1 40 
NOTE_OFF 2 $1 40
LAZY')',
'define(`bowtrem1a',`$1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24 $1 24')')


define(`longtone64',`END_LAZY NOTE_ON $1 $2 40 
DELAY 120 CHANNEL_PRESSURE $1 40
DELAY 120 CHANNEL_PRESSURE $1 45
DELAY 120 CHANNEL_PRESSURE $1 50
DELAY 120 CHANNEL_PRESSURE $1 55
DELAY 120 CHANNEL_PRESSURE $1 60
DELAY 120 CHANNEL_PRESSURE $1 65
DELAY 120 CHANNEL_PRESSURE $1 70
DELAY 120 CHANNEL_PRESSURE $1 75
DELAY 120 CHANNEL_PRESSURE $1 80
DELAY 120 CHANNEL_PRESSURE $1 85
DELAY 120 CHANNEL_PRESSURE $1 90
DELAY 120 NOTE_OFF $1 $2 0 
LAZY')

define(`longbreath64',`END_LAZY NOTE_ON $1 $2 40 
DELAY 120 BREATH $1 40
DELAY 120 BREATH $1 45
DELAY 120 BREATH $1 50
DELAY 120 BREATH $1 55
DELAY 120 BREATH $1 60
DELAY 120 BREATH $1 65
DELAY 120 BREATH $1 70
DELAY 120 BREATH $1 75
DELAY 120 BREATH $1 80
DELAY 120 BREATH $1 85
DELAY 120 BREATH $1 90
DELAY 120 NOTE_OFF $1 $2 0 
LAZY')

;cresctone(3,D4,1,1,127)
;cresctone(channel,pitch,len_in_whole_notes,startvel,endvel)
ifdef(`polypressure',`define(`cresctone',`
END_LAZY
NOTE_ON $1 $2 $4
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((0*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((1*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((2*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((3*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((4*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((5*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((6*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((7*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((8*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((9*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((10*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((11*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((12*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((13*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((14*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((15*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((16*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((17*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((18*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((19*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((20*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((21*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((22*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((23*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((24*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((25*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((26*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((27*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((28*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((29*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((30*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((31*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((32*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((33*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((34*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((35*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((36*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((37*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((38*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((39*($5-$4))/39)+$4)
NOTE_OFF $1 $2 0
LAZY')',`define(`cresctone',`vel $5 $2 $3')')

define(`articulate',`$1 8. R 16 $1 8. R 16') 

;longtone27 lopsided 2/3 cresc 1/3 decresc
;longtone27(channel,pitch,len_in_whole_notes,startvel,midvel)
ifdef(`polypressure',`define(`longtone27',`
END_LAZY
NOTE_ON $1 $2 $4
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((0*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((4*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((8*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((12*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((16*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((20*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((24*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((28*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((32*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((36*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((40*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((38*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((37*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((36*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((35*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((34*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((32*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((31*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((29*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((28*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((26*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((25*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((24*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((22*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((20*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((18*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((16*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((14*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((13*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((12*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1 eval(((10*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1  eval(((9*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1  eval(((8*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1  eval(((7*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1  eval(((6*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1  eval(((5*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1  eval(((4*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1  eval(((3*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1  eval(((2*($5-$4))/39)+$4)
DELAY eval((960*$3)/40) CHANNEL_PRESSURE $1  eval(((1*($5-$4))/39)+$4)
NOTE_OFF $1 $2 0
LAZY')',`define(`longtone27',`vel $5 $2 $3')')


