#!/usr/bin/make

SHELL=/usr/bin/ksh

.SUFFIXES: .txt .mid .xml

M4=/usr/bin/m4
CP=/bin/cp
RM=/bin/rm
TEXTMIDI:=/usr/bin/env textmidi
TEXTMIDICGM:=/usr/bin/env textmidicgm
opus=Demo93

names:=\
testcgmmeandynamic \
testcgmmeanpitch \
testcgmmeanrhythm \
testcgmrangedynamic \
testcgmrangepitch \
testcgmrangerhythm \
testcgmtexture \
testcgmscramble \


forms:=$(names:%=%.xml)
txts:=$(names:%=%.txt)
mids:=$(names:%=%.mid)

.PHONY:all
all:$(mids)

%.mid:%.txt
	$(TEXTMIDI) --textmidi $< --midi $@
%.txt:%.xml
	$(TEXTMIDICGM) --xmlform $< --textmidi $@
testcgmscramble.txt:testcgmscramble.xml
	$(TEXTMIDICGM) --xmlform $< --arrangements rotateright --arrangementsperiod 15.0

.PHONY: clean
clean:
	$(RM) $(txts) $(mids)
