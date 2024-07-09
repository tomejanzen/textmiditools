; TextMIDITools Version 1.0.81
; Copyright © 2024 Thomas E. Janzen
; License GPLv3+: GNU GPL version 3 or later <https:; gnu.org/licenses/gpl.html>
; This is free software: you are free to change and redistribute it.
; There is NO WARRANTY, to the extent permitted by law.
;
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


