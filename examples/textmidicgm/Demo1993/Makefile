%.txt:%.xml
	textmidicgm --xmlform $< --textmidi $@

midis = \
20fastwalk.mid \
Animations.mid \
concerto.mid \
drums.mid \
fasthira.mid \
fastpure.mid \
fastwalk.mid \
gamelan.mid \
harmonics.mid \
heavenhigh.mid \
heavenlow.mid \
koto.mid \
lucy.mid \
opentheflute.mid \
philfisa.mid \
test.mid \
wholetruth.mid \
windoctet.mid

txts := $(midis:%.mid=%.txt)

all : $(midis)

$(midis): %.mid: %.txt
	textmidi --textmidi $< --midi $@

clean:
	/usr/bin/env rm $(midis) $(txts)
