SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .mid .txt .m4 .wav

COLLATIONS:=grossefugeperc.txt
WAVS3D:=$(COLLATIONS:%.txt=%3d.wav)
WAVSALL:=grossefugeAcousticGrandPiano.wav grossefugeXylophone.wav \
grossefugeClavi.wav grossefugeElectricGrandPiano.wav grossefugeHarpsichord.wav \
grossefugeVibraphone.wav grossefugeKalimba.wav grossefugeMarimba.wav \
grossefugeTubularBells.wav grossefugeCelesta.wav grossefugeAcousticBassFinger.wav


MIDS:=$(COLLATIONS:%.txt=%.mid)

TEXTMIDI:=/usr/bin/env textmidi
TIMIDITY:=/usr/bin/timidity 
TIMIDITY_OPTS:=-Ow

TIMIDITY_OPT_AcousticGrandPiano:=-Q2,3,4,5,6,7,8,9,11,12
TIMIDITY_OPT_Celesta:=-Q1,3,4,5,6,7,8,9,11,12
TIMIDITY_OPT_Clavi:=-Q1,2,4,5,6,7,8,9,11,12
TIMIDITY_OPT_ElectricGrandPiano:=-Q1,2,3,5,6,7,8,9,11,12
TIMIDITY_OPT_Harpsichord:=-Q1,2,3,4,6,7,8,9,11,12
TIMIDITY_OPT_Kalimba:=-Q1,2,3,4,5,7,8,9,11,12
TIMIDITY_OPT_Marimba:=-Q1,2,3,4,5,6,8,9,11,12
TIMIDITY_OPT_TubularBells:=-Q1,2,3,4,5,6,7,9,11,12
TIMIDITY_OPT_Vibraphone:=-Q1,2,3,4,5,6,7,8,11,12
TIMIDITY_OPT_Xylophone:=-Q1,2,3,4,5,6,7,8,9,12
TIMIDITY_OPT_AcousticBassFinger:=-Q1,2,3,4,5,6,7,8,9,11

MIXWAVE3D:=~/me/janzent/miscprog/mixwave3d
M4:=/usr/bin/m4
M4_OPTS=-Dpolypressure=1
M4_OPTS2=-Dclicktrack=1
MV:=/bin/mv


.PHONY: all
all:grossefugeperc3d.wav

midi:$(MIDS)

grossefugeperc3d.wav : grossefugeAcousticGrandPiano.wav grossefugeXylophone.wav grossefugeClavi.wav \
grossefugeElectricGrandPiano.wav grossefugeHarpsichord.wav grossefugeVibraphone.wav \
grossefugeKalimba.wav grossefugeMarimba.wav grossefugeTubularBells.wav grossefugeCelesta.wav \
    grossefugeAcousticBassFinger.wav
	$(MIXWAVE3D) \
	-l -x -1.5 -y 5.0 -z 10.0 -c 0 -a 1.0 \
	-r -x  1.5 -y 5.0 -z 10.0 -c 0 -a 1.0 \
	-i -x  0.0 -y 16.0 -z 3.0 -c 0 -a 0.9 -n grossefugeAcousticGrandPiano.wav -p \
	-i -x -6.0 -y 13.0 -z 3.0 -c 0 -a 0.5 -n grossefugeXylophone.wav -p \
	-i -x -3.0 -y 13.0 -z 3.0 -c 0 -a 0.9 -n grossefugeClavi.wav -p \
	-i -x  3.0 -y 13.0 -z 3.0 -c 0 -a 0.9 -n grossefugeElectricGrandPiano.wav -p \
	-i -x  6.0 -y 13.0 -z 3.0 -c 0 -a 0.9 -n grossefugeHarpsichord.wav -p \
	-i -x -6.0 -y 10.0 -z 3.0 -c 0 -a 0.5 -n grossefugeVibraphone.wav -p \
	-i -x -3.0 -y 10.0 -z 3.0 -c 0 -a 0.9 -n grossefugeKalimba.wav -p \
	-i -x  0.0 -y 10.0 -z 3.0 -c 0 -a 0.9 -n grossefugeMarimba.wav -p \
	-i -x  3.0 -y 10.0 -z 3.0 -c 0 -a 0.5 -n grossefugeTubularBells.wav -p \
	-i -x  6.0 -y 10.0 -z 3.0 -c 0 -a 0.9 -n grossefugeCelesta.wav -p \
	-i -x  6.0 -y 15.0 -z 4.0 -c 0 -a 0.9 -n grossefugeAcousticBassFinger.wav -p \
	-o $@

grossefugeAcousticGrandPiano.wav \
grossefugeXylophone.wav \
grossefugeClavi.wav \
grossefugeElectricGrandPiano.wav \
grossefugeHarpsichord.wav \
grossefugeVibraphone.wav \
grossefugeKalimba.wav \
grossefugeMarimba.wav \
grossefugeTubularBells.wav \
grossefugeCelesta.wav \
grossefugeAcousticBassFinger.wav : grossefugeperc.mid 
	$(TIMIDITY) $(TIMIDITY_OPTS) -o $@ $($(patsubst $(opus)%,TIMIDITY_OPT_%,$(basename $@))) $< 
#	$(MV) $(subst mid,wav,$<) $@

%.mid:%.txt
	$(TEXTMIDI) --textmidi $< --midi $@	

grossefugeperc.mid : grossefugeperc.txt
grossefugeperc.txt : grossefuge.m4 grossefuge.tempo.m4 grossefuge.primorh.m4 grossefuge.primolh.m4 grossefuge.secondorh.m4 grossefuge.secondorhextra.m4 grossefuge.secondolh.m4 grossefuge.trills.m4 grossefuge.click.m4 instruments.m4 percussion.m4
	/usr/bin/m4 grossefuge.m4 > $@
.PHONY:clean
clean:
	/bin/rm grossefugeperc.txt $(WAVSALL) grossefugeperc3d.wav
.PHONY:cleanmidi
cleanmidi:
	/bin/rm grossefugeperc.mid
cleanwav3d:
	$(RM) -f $(WAVS3D)
cleanwavsall:
	$(RM) -f $(WAVSALL) grossefugeperc3d.wav
