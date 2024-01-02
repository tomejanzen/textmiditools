SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .mid .txt .m4 .wav

opus=schuqnt
mov1=$(opus)1
mov2=$(opus)2
mov3=$(opus)3
mov4=$(opus)4

COLLATIONS:=$(mov1).txt $(mov2).txt $(mov3).txt $(mov4).txt 
WAVS3D:=$(COLLATIONS:%.txt=%3d.wav)
WAVSALL:=*.wav
MIDS:=$(COLLATIONS:%.txt=%.mid)

TEXTMIDI:=~/me/janzent/miscprog/textmidi/textmidi
TIMIDITY:=/usr/bin/timidity 
TIMIDITY_OPTS:=-invvvt -Ow 
TIMIDITY_OPT_1p:=-Q1,2,3,4,16 
TIMIDITY_OPT_1v1:=-Q2,3,4,5,16
TIMIDITY_OPT_1v2:=-Q1,3,4,5,16
TIMIDITY_OPT_1va:=-Q1,2,4,5,16
TIMIDITY_OPT_1vc:=-Q1,2,3,5,16 

TIMIDITY_OPT_2p:=-Q1,2,3,4,16 
TIMIDITY_OPT_2v1:=-Q2,3,4,5,16
TIMIDITY_OPT_2v2:=-Q1,3,4,5,16
TIMIDITY_OPT_2va:=-Q1,2,4,5,16
TIMIDITY_OPT_2vc:=-Q1,2,3,5,16 

TIMIDITY_OPT_3p:=-Q1,2,3,4,16 
TIMIDITY_OPT_3v1:=-Q2,3,4,5,16
TIMIDITY_OPT_3v2:=-Q1,3,4,5,16
TIMIDITY_OPT_3va:=-Q1,2,4,5,16
TIMIDITY_OPT_3vc:=-Q1,2,3,5,16 

TIMIDITY_OPT_4p:=-Q1,2,3,4,16 
TIMIDITY_OPT_4v1:=-Q2,3,4,5,16
TIMIDITY_OPT_4v2:=-Q1,3,4,5,16
TIMIDITY_OPT_4va:=-Q1,2,4,5,16
TIMIDITY_OPT_4vc:=-Q1,2,3,5,16 

MIXWAVE3D:=~/me/janzent/miscprog/mixwave3d
M4:=/usr/bin/m4
M4_OPTS=-Dpolypressure=1
M4_OPTS2=-Dclicktrack=1
MV:=/bin/mv


.PHONY: all
all:$(mov1)3d.wav $(mov2)3d.wav $(mov3)3d.wav $(mov4)3d.wav 

midi:$(MIDS)

%3d.wav : %p.wav %v1.wav %v2.wav %va.wav %vc.wav
	$(MIXWAVE3D) \
	-l -x  -1.5 -y -10.0 -z 10.0 -c 1 -a 1.0 \
	-r -x   1.5 -y -10.0 -z 10.0 -c 1 -a 1.0 \
	-i -x -1.5 -y 10.0 -z 2.5 -c 1 -a 0.9 -n $(subst 3d.wav,p.wav,$@) -p \
	-i -x  1.5 -y 10.0 -z 2.5 -c 1 -a 0.9 -n $(subst 3d.wav,p.wav,$@) -p \
	-i -x -6.0 -y  4.0 -z 3.0 -c 1 -a 0.5 -n $(subst 3d.wav,v1.wav,$@) -p \
	-i -x -3.0 -y  6.0 -z 3.0 -c 1 -a 0.5 -n $(subst 3d.wav,v2.wav,$@) -p \
	-i -x  6.0 -y  4.0 -z 3.0 -c 1 -a 0.5 -n $(subst 3d.wav,va.wav,$@) -p \
	-i -x  3.0 -y  6.0 -z 2.0 -c 1 -a 0.5 -n $(subst 3d.wav,vc.wav,$@) -p \
	-o $@

$(mov1)p.wav $(mov1)v1.wav $(mov1)v2.wav $(mov1)va.wav $(mov1)vc.wav : $(mov1).mid 
	$(TIMIDITY) $(TIMIDITY_OPTS) $($(patsubst $(opus)%,TIMIDITY_OPT_%,$(basename $@))) $< 
	$(MV) $(subst mid,wav,$<) $@

$(mov2)p.wav $(mov2)v1.wav $(mov2)v2.wav $(mov2)va.wav $(mov2)vc.wav : $(mov2).mid 
	$(TIMIDITY) $(TIMIDITY_OPTS) $($(patsubst $(opus)%,TIMIDITY_OPT_%,$(basename $@))) $< 
	$(MV) $(subst mid,wav,$<) $@

$(mov3)p.wav $(mov3)v1.wav $(mov3)v2.wav $(mov3)va.wav $(mov3)vc.wav : $(mov3).mid 
	$(TIMIDITY) $(TIMIDITY_OPTS) $($(patsubst $(opus)%,TIMIDITY_OPT_%,$(basename $@))) $< 
	$(MV) $(subst mid,wav,$<) $@

$(mov4)p.wav $(mov4)v1.wav $(mov4)v2.wav $(mov4)va.wav $(mov4)vc.wav : $(mov4).mid 
	$(TIMIDITY) $(TIMIDITY_OPTS) $($(patsubst $(opus)%,TIMIDITY_OPT_%,$(basename $@))) $< 
	$(MV) $(subst mid,wav,$<) $@

%.mid:%.txt
	$(TEXTMIDI) --textmidi $< --midi $@	

$(mov1).txt : $(mov1)ped.txt $(mov1)pl.txt $(mov1)pr.txt $(mov1)pt.txt $(mov1)tempo.txt $(mov1)v1.txt $(mov1)v2.txt $(mov1)va.txt $(mov1)vc.txt schuqntutils.m4 $(mov1).m4
	$(M4) $(M4_OPTS) $(M4_OPTS2) $(mov1).m4 > $@
$(mov2).txt : $(mov2)pl.txt $(mov2)pr.txt $(mov2)pt.txt $(mov2)tempo.txt $(mov2)v1.txt $(mov2)v2.txt $(mov2)va.txt $(mov2)vc.txt schuqntutils.m4 $(mov2).m4
	$(M4) $(M4_OPTS) $(M4_OPTS2) $(mov2).m4 > $@
$(mov3).txt : $(mov3)pl.txt $(mov3)pr.txt $(mov3)pt.txt $(mov3)tempo.txt $(mov3)v1.txt $(mov3)v2.txt $(mov3)va.txt $(mov3)vc.txt schuqntutils.m4 $(mov3).m4
	$(M4) $(M4_OPTS) $(M4_OPTS2) $(mov3).m4 > $@
$(mov4).txt : $(mov4)pl.txt $(mov4)pr.txt $(mov4)pt.txt $(mov4)tempo.txt $(mov4)v1.txt $(mov4)v2.txt $(mov4)va.txt $(mov4)vc.txt schuqntutils.m4 $(mov4).m4
	$(M4) $(M4_OPTS) $(M4_OPTS2) $(mov4).m4 > $@

clean:
	$(RM) -f $(MIDS) $(COLLATIONS) $(WAVSALL)
cleanmid:
	$(RM) -f $(MIDS)
cleanwav3d:
	$(RM) -f $(WAVS3D)
cleanwavsall:
	$(RM) -f $(WAVSALL)

