#!/usr/bin/make

SHELL=/usr/bin/ksh

.SUFFIXES: .txt .mid .xml

M4=/usr/bin/m4
CP=/bin/cp
RM=/bin/rm
TEXTMIDI:=textmidi
TEXTMIDICGM:=textmidicgm
opus=Demo93

names:=\
20fastwalk \
Animations_v0 \
Animations_v2_v2 \
Animations_v2 \
Animations_v3_v2 \
Animations_v3_v3 \
Animations \
concerto \
drums \
fasthira \
fastpure \
fastwalk \
gamelan \
harmonics \
heavenhigh \
heavenlow \
koto \
lucy \
opentheflute \
philfisa \
test \
wholetruth \
windoctet \


forms:=$(names:%=%.xml)
txts:=$(names:%=%.txt)
mids:=$(names:%=%.mid)

.PHONY:all
all:$(mids)

%.mid:%.txt
	$(TEXTMIDI) --textmidi $< --midi $@
%.txt:%.xml
	$(TEXTMIDICGM) --xmlform $< --textmidi $@

.PHONY: clean
clean:
	$(RM) $(txts) $(mids)
