%.txt:%.xml
	textmidicgm --xmlform $< --textmidi $@

midis = \
commu9
loud
pianoduo
scales
terrharp
tritone
xmas92

txts := $(midis:%.mid=%.txt)

all : $(midis)

$(midis): %.mid: %.txt
	textmidi --textmidi $< --midi $@

clean:
	/usr/bin/env rm $(midis) $(txts)
