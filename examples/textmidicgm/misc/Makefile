%.txt:%.xml
	textmidicgm --xmlform $< --textmidi $@

midis = \
commu9.mid \
loud.mid \
pianoduo.mid \
scales.mid \
terrharp.mid \
tritone.mid \
xmas92.mid

txts := $(midis:%.mid=%.txt)

all : $(midis)

$(midis): %.mid: %.txt
	textmidi --textmidi $< --midi $@

clean:
	/usr/bin/env rm $(midis) $(txts)
