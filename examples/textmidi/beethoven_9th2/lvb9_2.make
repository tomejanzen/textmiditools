.SUFFIXES:
.SUFFIXES: .mid .txt .m4 .wav

lvb9_2.mid : lvb9_2.txt
	/home/janzent/me/janzent/miscprog/textmidi/textmidi --textmidi $< --midi $@
lvb9_2.txt :  \
                lvb9_2.m4 \
                lvb9_2bridge2_piano1.m4 \
                lvb9_2bridge2_piano2.m4 \
                lvb9_2bridge2_tempo.m4 \
                lvb9_2bridge_piano1.m4 \
                lvb9_2bridge_piano2.m4 \
                lvb9_2bridge_tempo.m4 \
                lvb9_2coda_piano1.m4 \
                lvb9_2coda_piano2.m4 \
                lvb9_2coda_tempo.m4 \
                lvb9_2ending_piano1.m4 \
                lvb9_2ending_piano2.m4 \
                lvb9_2ending_tempo.m4 \
                lvb9_2fanfare_piano1.m4 \
                lvb9_2fanfare_piano2.m4 \
                lvb9_2fanfare_tempo.m4 \
                lvb9_2repeat1_piano1.m4 \
                lvb9_2repeat1_piano2.m4 \
                lvb9_2repeat1_tempo.m4 \
                lvb9_2repeat2_piano1.m4 \
                lvb9_2repeat2_piano2.m4 \
                lvb9_2repeat2_tempo.m4 \
                lvb9_2repeat3_piano1.m4 \
                lvb9_2repeat3_piano2.m4 \
                lvb9_2repeat3_tempo.m4 \
                lvb9_2repeat4_piano1.m4 \
                lvb9_2repeat4_piano2.m4 \
                lvb9_2repeat4_tempo.m4
	/usr/bin/m4 lvb9_2.m4 > $@
lvb9_2.wav : lvb9_2.mid
	wildmidi -o $@ $<
clean:
	/bin/rm lvb9_2.mid lvb9_2.txt


