changecom(`;')
;Beethoven: Grosse fuge, trans. by the composer, in textmidi language.  © 2024 by Thomas E. Janzen is licensed under CC BY 4.0 🅯🅭
define(`gallopfullhalf',  `vel +15 $1 8 vel -15 R 16 $2 16 $3 8 R 16 $4 16 ')
define(`galloppickup', `gallopfullhalf($1,$2,$1,$1)')
define(`gallopfull',`gallopfullhalf($1,$2,$3,$4)gallopfullhalf($5,$6,$7,$8)')
define(`gallopskip',`gallopfull($1,$2,$1,$1,$1,$2,$3,$3)')
define(`gallopskip2',`gallopfull($1,$2,$1,$1,$1,$3,$4,$5)')
define(`gallopeighthsfull',`R 8. $1 16 $2 8 R 16 $3 16 vel +15 $4 8 vel -15 R 8 $5 8 R 8 ')
define(`gallopeighths',`gallopeighthsfull($1,$1,$2,$3,$4) ')
define(`gallopqtrs',`R 8. $1 16 $1 8 R 16 $2 16 vel +15 $3 4 vel -15 R 4 ')
define(`gallopdownhalf',`gallopfullhalf($1,$2,$2,$3) ')
define(`gallopdown',`gallopdownhalf($1,$2,$3)gallopdownhalf($3,$4,$5)')
define(`repquaversqtr',`$1 8 $1 8 ')
define(`repquavershalf',`repquaversqtr($1)repquaversqtr($2)')
define(`repquavers',`repquavershalf($1,$2)repquavershalf($3,$4)')
define(`quavershalffull',`$1 8 $2 8 $3 8 $4 8 ')
define(`quaversfull',`quavershalffull($1,$2,$3,$4)quavershalffull($5,$6,$7,$8)')
define(`quavers2',`vel +10 $1 8 vel -10 $1 8 vel +10 $2 8 vel -10 $2 8 vel +10 $3 8 vel -10 $3 8 vel +10 $4 8 vel -10 $5 8 ')
define(`restquaversfullhalf',`R 4 vel +10 $1 8 vel -10 $2 8 ')
define(`quaversrestfullhalf',`$1 8 $2 8 R 4')
define(`quaversresthalf',`quaversrestfullhalf($1,$1)')
define(`restquavershalf',`restquaversfullhalf($1,$1)')
define(`quaversrest',`quaversresthalf($1)quaversresthalf($2)')
define(`restquavers',`restquavershalf($1)restquavershalf($2)')
define(`offrestquavershalffull',`$1 8 vel +10 $2 8 vel -10 $3 8 vel +10 $4 8 vel -10 ')
define(`offrestquavershalf',`offrestquavershalffull(R,$1,$1,R) ')
define(`offrestquavers',`offrestquavershalf($1)offrestquavershalf($2)')
define(`restquavers2',`R 4 vel +10 $1 8 vel -10 $1 8 R 4 vel +10 $2 8 vel -10 $3 8 ')
define(`tripletsqtr',`$1 12 $2 12 $3 12 ')
define(`tripletshalf',`tripletsqtr($1,$2,$3)tripletsqtr($4,$5,$6) ')
define(`tripletshalfosc',`tripletsqtr($1,$2,$1)tripletsqtr($2,$1,$2) ')
define(`triplets',`tripletshalf($1,$2,$3,$4,$5,$6)tripletshalf($7,$8,$9,$10,$11,$12)')
define(`sixqtr',`$1 8 $2 8 $3 8 ')
define(`sixhalf',`sixqtr($1,$2,$3)sixqtr($4,$5,$6) ')
define(`sixhalfosc',`sixhalf($1,$2,$1,$2,$1,$2)')
define(`six',`sixhalf($1,$2,$3,$4,$5,$6)sixhalf($7,$8,$9,$10,$11,$12)')
define(`dahditdithalf',`$1 16 R 16 $2 16 $3 16 $4 16 R 16 $5 16 $6 16 ')
define(`dahditditfull',`dahditdithalf($1,$2,$3,$4,$5,$6)dahditdithalf($7,$8,$9,$10,$11,$12)')
define(`sixteenthsqtr',`$1 16 $2 16 $3 16 $4 16 ')

define(`sixteenthsqtrdyn1',`vel +6 $1 16 vel -6 $2 16 $3 16 $4 16 ')
define(`sixteenthsqtrdyn13',`vel +6 $1 16 vel -6 $2 16 vel +6 $3 16 vel -6 $4 16 ')
define(`sixteenthsqtrdyn2',`$1 16 vel +6 $2 16 vel -6 $3 16 $4 16 ')
define(`sixteenthsqtrdyn3',`$1 16 $2 16 vel +6 $3 16 vel -6 $4 16 ')
define(`sixteenthsqtrdyn4',`$1 16 $2 16 $3 16 vel +6 $4 16 vel -6 ')

define(`long16th',`98/1536')
define(`real16th',`96/1536')
define(`short16th',`94/1536')
define(`sixteenthsqtr1',`vel +4 $1 long16th vel -4 $2 8/128 $3 16 $4 short16th ')
define(`sixteenthsqtr13',`vel +4 $1 long16th vel -4 $2 short16th vel +4 $3 long16th vel -4 $4 short16th ')
define(`sixteenthsqtr2',`$1 16 vel +4 $2 long16th vel -4 $3 16 $4 short16th ')
define(`sixteenthsqtr3',`$1 short16th $2 16 vel +4 $3 long16th vel -4 $4 8/128 ')
define(`sixteenthsqtr4',`$1 short16th $2 16 $3 8/128 vel +4 $4 long16th vel -4 ')
define(`sixteenthshalf',`sixteenthsqtr($1,$2,$3,$4)sixteenthsqtr($5,$6,$7,$8)')

define(`sixteenthshalfdyn3_1',`sixteenthsqtrdyn3($1,$2,$3,$4) sixteenthsqtrdyn1($5,$6,$7,$8)')
define(`sixteenthshalfdyn1_3',`sixteenthsqtrdyn1($1,$2,$3,$4) sixteenthsqtrdyn3($5,$6,$7,$8)')
define(`sixteenthshalfdyn1',`sixteenthsqtrdyn1($1,$2,$3,$4) sixteenthsqtrdyn1($5,$6,$7,$8)')
define(`sixteenthshalfdyn2',`sixteenthsqtrdyn2($1,$2,$3,$4) sixteenthsqtrdyn2($5,$6,$7,$8)')
define(`sixteenthshalfdyn3',`sixteenthsqtrdyn3($1,$2,$3,$4) sixteenthsqtrdyn3($5,$6,$7,$8)')


define(`sixteenthshalf1_3',`sixteenthsqtr1($1,$2,$3,$4) sixteenthsqtr3($5,$6,$7,$8)')
define(`sixteenthshalf1',`sixteenthsqtr1($1,$2,$3,$4) sixteenthsqtr1($5,$6,$7,$8)')
define(`sixteenthshalf2',`sixteenthsqtr2($1,$2,$3,$4) sixteenthsqtr2($5,$6,$7,$8)')
define(`sixteenthshalf3',`sixteenthsqtr3($1,$2,$3,$4) sixteenthsqtr3($5,$6,$7,$8)')
define(`sixteenthsqtrpickup1',`R 5/64 R long16th vel +10 $1 long16th vel -10 $2 3/64 ')
define(`sixteenthsqtrpickup2',`$1 long16th vel -10 $2 short16th ')
define(`sixteenthsqtrrestpickup2',`R short16th R 3/64 vel +10 $1 11/128 vel -10 $2 16 ')
define(`sixteenthsqtrpickdown',`$1 11/128 $2 short16th R 3/64 R 16 ')
define(`sixteenthsqtrosc',`vel -5 sixteenthsqtrdyn3($1,$2,$1,$2) vel +5 ')
define(`sixteenthshalfosc',`sixteenthsqtrosc($1,$2)sixteenthsqtrosc($1,$2) ')
define(`sixteenthshalflate',`sixteenthshalf($1,$2,$3,$3,$3,$4,$5,$5)')
define(`sixteenthshalf7',`sixteenthshalfdyn2(R,$1,$2,$2,$2,$1,$2,$2)')

define(`swingstacc',`vel +10 $1 8 vel -10 R 8 $2 8 $3 4 $4 8 ')
define(`swinggrace',`vel +10 $1 4 vel -10 $2 8 $3 4 $4 12 $5 24 ')
define(`swing',`vel +10 $1 4 vel -10 $2 8 $3 4 $4 8 ')
define(`swingcoda1',`vel +10 $1 4 vel -10 $2 8 $3 4 $4 4. ')
define(`swingcoda2',`$2 4. $4 4. ')
define(`swingcoda3',`$2 4. $4 8 ')
define(`swingcoda4',`$2 8 $3 4 $4 4. ')
define(`swingcoda5',`$2 8 $3 4 $4 8 ')
define(`swingcoda6',`$1 4 $2 4. $4 4. ')
define(`swingreststacc',`swing($1,R,$2,R )')
define(`swingrest',`swing($1,R,$2,R )')
define(`swingrep',`vel +10 $1 4 vel -10 $2 8 $2 4 $3 8 ')
define(`liltcontd',`swingstacc($1,$2,$3,$4) ')
define(`lilt',`swing(R,$1,$2,$3) ')
define(`liltpickup',`$1 5/8 $2 8 ')
define(`liltpickupsynco',`$1 8 $2 5/8 ')
define(`click68', `crashcymbal2 8 acoustic_snare 8 acoustic_snare 8 AcousticBassDrum 8 acoustic_snare 8 acoustic_snare 8 ')
define(`click24', `crashcymbal2 8 acoustic_snare 8 closedhihat 8 acoustic_snare 8 ')
define(`click44', `crashcymbal2 8 acoustic_snare 8 AcousticBassDrum 8 acoustic_snare 8 closedhihat 8 acoustic_snare 8 AcousticBassDrum 8 acoustic_snare 8 ')
define(`offbeats68',`R 4. $1 4 R 8')
define(`eighths',`$1 8 R 8 $2 8 R 8')

define(`primorhchan',`1')
define(`primolhchan',`2')
define(`secondorhchan',`3')
define(`secondolhchan',`4')

define(`pppp',`vel 10')
define(`ppp',`vel 25')
define(`pp',`vel 43')
define(`p',`vel 52')
define(`mp',`vel 62')
define(`mf',`vel 75')
define(`forte',`vel 90')
define(`ff',`vel 110')
define(`fff',`vel 120')
define(`ffff',`vel 127')

;define(`pppp',`vel 20')
;define(`ppp',`vel 31')
;define(`pp',`vel 42')
;define(`p',`vel 53')
;define(`mp',`vel 64')
;define(`mf',`vel 76')
;define(`forte',`vel 87')
;define(`ff',`vel 98')
;define(`fff',`vel 109')
;define(`ffff',`vel 120')

include(`grossefuge.trills.m4')
include(`percussion.m4')
include(`instruments.m4')

FILEHEADER 6 384 
TEXT Grosse Fuge by Ludwig van Beethoven
COPYRIGHT Thomas E. Janzen 2020

include(`grossefuge.tempo.m4')
;include(`grossefuge.click.m4')
include(`grossefuge.primorh.m4')
include(`grossefuge.primolh.m4')
include(`grossefuge.secondorh.m4')
include(`grossefuge.secondorhextra.m4')
include(`grossefuge.secondolh.m4')

