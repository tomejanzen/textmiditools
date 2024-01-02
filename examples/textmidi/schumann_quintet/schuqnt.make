M4=/usr/bin/m4
CP=/bin/cp
RM=/bin/rm
SOX=/usr/bin/sox
TEXTMIDI:=textmidi
opus=schuqnt
mov1=$(opus)1
mov2=$(opus)2
mov3=$(opus)3
mov4=$(opus)4

all: $(mov1).mid $(mov2).mid $(mov3).mid $(mov4).mid 
%.mid: %.txt
	$(TEXTMIDI) --textmidi $< --midi $@
%.wav: %.mid
	./timbosenwav.sh $*
%.cdr: %.wav
	./blankleader.sh $*
	$(SOX) $< $@

$(mov1).wav : $(mov1).mid
$(mov2).wav : $(mov2).mid
$(mov3).wav : $(mov3).mid
$(mov4).wav : $(mov4).mid

$(mov1).cdr : $(mov1).wav
$(mov2).cdr : $(mov2).wav
$(mov3).cdr : $(mov3).wav
$(mov4).cdr : $(mov4).wav

wave : $(mov1).wav $(mov2).wav $(mov3).wav $(mov4).wav 
cdr  : $(mov1).cdr $(mov2).cdr $(mov3).cdr $(mov4).cdr

$(mov1).mid : $(mov1).txt 
$(mov2).mid : $(mov2).txt
$(mov3).mid : $(mov3).txt
$(mov4).mid : $(mov4).txt 
#	$(M4) $(mov1).m4 > $@
$(mov1).txt : $(mov1).m4 $(mov1)tempo.txt $(mov1)v1.txt $(mov1)v2.txt $(mov1)va.txt $(mov1)vc.txt $(mov1)pr.txt $(mov1)pl.txt $(mov1)pt.txt $(mov1)ped.txt $(mov2)click.txt schuqntutils.m4
	$(M4) --define=polypressure=on --define=intmidi=internal $(mov1).m4 > $@
$(mov2).txt : $(mov2).m4 $(mov2)tempo.txt $(mov2)v1.txt $(mov2)v2.txt $(mov2)va.txt $(mov2)vc.txt $(mov2)vc2.txt $(mov2)pr.txt $(mov2)pl.txt $(mov2)pt.txt $(mov2)vc2.txt $(mov2)click.txt schuqntutils.m4
	$(M4) --define=polypressure=on --define=intmidi=internal $(mov2).m4 > $@
$(mov3).txt : $(mov3).m4 $(mov3)tempo.txt $(mov3)pr.txt $(mov3)pl.txt $(mov3)pt.txt $(mov3)v1.txt $(mov3)v2.txt $(mov3)va.txt $(mov3)vc.txt $(mov3)click.txt schuqntutils.m4
	$(M4) --define=polypressure=on --define=intmidi=internal $(mov3).m4 > $@
$(mov4).txt : $(mov4).m4 $(mov4)tempo.txt $(mov4)pr.txt $(mov4)pl.txt $(mov4)pt.txt $(mov4)v1.txt $(mov4)v2.txt $(mov4)va.txt $(mov4)vc.txt $(mov4)click.txt schuqntutils.m4
	$(M4) --define=polypressure=on --define=intmidi=internal $(mov4).m4 > $@
#playmidi : $(mov3).mid
#	playmidi -ra $(mov3).mid
#timidity : $(mov3).mid
#	timidity -x 'bank 0\n40 /usr/share/timidity/instruments/viola.pat' -invvvt $(mov3).mid
#name used for Classical Archive
tjrsqit1.mid : $(mov1).mid
	$(CP) $< $@
tjrsqit2.mid : $(mov2).mid
	$(CP) $< $@
tjrsqit3.mid : $(mov3).mid
	$(CP) $< $@
tjrsqit4.mid : $(mov4).mid
	$(CP) $< $@
clean:
	$(RM) -f $(opus)*.mid $(mov1).txt $(mov2).txt $(mov3).txt $(mov4).txt $(mov1).wav $(mov2).wav $(mov3).wav $(mov4).wav $(mov1).cdr $(mov2).cdr $(mov3).cdr $(mov4).cdr
