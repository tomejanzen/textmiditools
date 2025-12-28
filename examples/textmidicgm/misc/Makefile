TEXTMIDI:=/usr/bin/env textmidi
TEXTMIDICGM:=/usr/bin/env textmidicgm
%.txt:%.xml
	$(TEXTMIDICGM) --xmlform $< --textmidi $@

midis = \
commu9.mid \
loud.mid \
pianoduo.mid \
scales.mid \
terrharp.mid \
tritone.mid \
xmas92.mid

txts := $(midis:%.mid=%.txt)

.PHONY: all
all : $(midis)

$(midis): %.mid: %.txt
	$(TEXTMIDI) --textmidi $< --midi $@

.PHONY: clean                   
clean:
	/usr/bin/env rm $(midis) $(txts)
