%.txt:%.xml
	textmidicgm --xmlform $< --textmidi $@

xmls = \
arctan.xml \
cosine.xml \
mean.xml \
normdist.xml \
range.xml \
sine.xml \
stddev.xml \
tan.xml \
testssig.xml \
unitcircle.xml \
variance.xml

midis = $(xmls:%.xml=%.mid)
txts = $(xmls:%.xml=%.txt)

.PHONY:all
all : $(midis)

$(midis): %.mid: %.txt
	textmidi --textmidi $< --midi $@

.PHONY: clean
clean:
	/usr/bin/env rm $(midis) $(txts)
