.SUFFIXES:
.SUFFIXES: .mid .txt .m4

grossefugeperc.mid : grossefugeperc.txt
	/usr/bin/env textmidi --textmidi $< --midi $@
grossefugeperc.txt : grossefuge.m4 grossefuge.tempo.m4 grossefuge.primorh.m4 grossefuge.primolh.m4 grossefuge.secondorh.m4 grossefuge.secondorhextra.m4 grossefuge.secondolh.m4 grossefuge.trills.m4 grossefuge.click.m4 instruments.m4 percussion.m4
	/usr/bin/m4 grossefuge.m4 > $@
.PHONY:clean
clean:
	/bin/rm grossefuge.txt
.PHONY:cleanmidi
cleanmidi:
	/bin/rm grossefugeperc.mid

