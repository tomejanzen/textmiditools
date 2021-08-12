SET_MAKE = @SET_MAKE@
bin_PROGRAMS = textmidi$(EXEEXT) miditext$(EXEEXT) smustextmidi$(EXEEXT) textmidicgm$(EXEEXT) 
textmidi_SOURCES = textmidi.l Midi.cc main.cc miditext.cc
miditext_SOURCES = miditext.cc Midi.cc MidiMessages.cc MIDIKeyString.cc
smustextmidi_SOURCES = smustextmidi.cc SmusTrackEvent.cc
textmidicgm_SOURCES = textmidicgm.cc cgmlegacy.cc MIDIKeyString.cc MusicalForm.cc NoteEvent.cc Track.cc Voice.cc GnuPlot.cc Composer.cc Scales.cc GeneralMIDI.cc

textmidi_LDADD = @LEXLIB@
info_TEXINFOS = textmidi.texi miditext.texi smustextmidi.texi textmidicgm.texi

AM_LDFLAGS = -lboost_program_options -lboost_system -lboost_serialization 
CXX = /usr/bin/g++
AM_CXXFLAGS  = -std=c++20 -Wall -Wextra -Weffc++ -g
AM_CXXFLAGS += -O2
AM_LFLAGS = -i
LEXFLAGS = -i
LEX=/usr/bin/lex

all : ${bin_PROGRAMS}

textmidi$(EXEEXT) : main.o textmidi.o Midi.o MIDIKeyString.o
	$(CXX) $(AM_CXXFLAGS) -o $@ main.o textmidi.o Midi.o MIDIKeyString.o $(AM_LDFLAGS)
main.o : main.cc textmidi.h Options.h
	$(CXX) -c $(AM_CXXFLAGS) -o $@ $<
textmidi.o: textmidi.cc Midi.h MIDIKeyString.h textmidi.h
	$(CXX) -c $(AM_CXXFLAGS) -o $@ $<
textmidi.cc : textmidi.l
	$(LEX) $(LEXFLAGS) -o $@ $<
miditext$(EXEEXT) : miditext.o Midi.o MidiMessages.o MIDIKeyString.o
	$(CXX) $(AM_CXXFLAGS) -o $@ $< Midi.o MidiMessages.o MIDIKeyString.o $(AM_LDFLAGS)
miditext.o: miditext.cc rational_support.h Midi.h MidiMessages.h Options.h
	$(CXX) $(AM_CXXFLAGS) -c -g -o $@ $<
Midi.o : Midi.cc Midi.h
	$(CXX) -c $(AM_CXXFLAGS) -o $@ $<
MidiMessages.o: MidiMessages.cc MidiMessages.h Midi.h rational_support.h MIDIKeyString.h
	$(CXX) $(AM_CXXFLAGS) -c -g -o MidiMessages.o MidiMessages.cc
smustextmidi$(EXEEXT) : smustextmidi.o SmusTrackEvent.o Midi.o MidiMessages.o MIDIKeyString.o
	$(CXX) $(AM_CXXFLAGS) $< -o $@ SmusTrackEvent.o Midi.o MidiMessages.o MIDIKeyString.o $(AM_LDFLAGS)
smustextmidi.o: smustextmidi.cc MidiMessages.h Midi.h rational_support.h SmusTrackEvent.h Options.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
SmusTrackEvent.o: SmusTrackEvent.cc SmusTrackEvent.h MidiMessages.h Midi.h rational_support.h MIDIKeyString.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
textmidicgm$(EXEEXT) : textmidicgm.o cgmlegacy.o MIDIKeyString.o MusicalForm.o NoteEvent.o Track.o Voice.o GnuPlot.o Composer.o Scales.o GeneralMIDI.o
	$(CXX) $(AM_CXXFLAGS) $< -o $@ cgmlegacy.o MIDIKeyString.o MusicalForm.o NoteEvent.o Track.o Voice.o GnuPlot.o Composer.o Scales.o GeneralMIDI.o $(AM_LDFLAGS)
textmidicgm.o: textmidicgm.cc MIDIKeyString.h MusicalForm.h cgmlegacy.h Voice.h Track.h NoteEvent.h RandomDouble.h RandomInt.h Scales.h GeneralMIDI.h Options.h Composer.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
cgmlegacy.o : cgmlegacy.cc  cgmlegacy.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
MIDIKeyString.o : MIDIKeyString.cc MIDIKeyString.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
MusicalForm.o: MusicalForm.cc MusicalForm.h cgmlegacy.h Voice.h MIDIKeyString.h Track.h NoteEvent.h RandomDouble.h RandomInt.h Scales.h GeneralMIDI.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
Track.o: Track.cc Track.h NoteEvent.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
Voice.o: Voice.cc Voice.h cgmlegacy.h MIDIKeyString.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
Scales.o : Scales.cc Scales.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
NoteEvent.o : NoteEvent.cc NoteEvent.h Track.h MIDIKeyString.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
GnuPlot.o: GnuPlot.cc GnuPlot.h MusicalForm.h cgmlegacy.h Voice.h MIDIKeyString.h Track.h NoteEvent.h RandomDouble.h RandomInt.h Scales.h GeneralMIDI.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
Composer.o: Composer.cc Midi.h Composer.h MusicalForm.h cgmlegacy.h Voice.h MIDIKeyString.h Track.h NoteEvent.h RandomDouble.h RandomInt.h Scales.h GeneralMIDI.h GnuPlot.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
GeneralMIDI.o : GeneralMIDI.cc GeneralMIDI.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<

clean:
	rm -f cgmlegacy.o Composer.o GeneralMIDI.o GnuPlot.o main.o MIDIKeyString.o MidiMessages.o Midi.o miditext.o MusicalForm.o NoteEvent.o Scales.o smustextmidi.o SmusTrackEvent.o textmidicgm.o textmidi.o Track.o Voice.o textmidicgm$(EXEEXT) miditext$(EXEEXT) textmidi$(EXEEXT) smustextmidi$(EXEEXT)
install:
	$(INSTALL_PROGRAM) textmidi$(EXEEXT) $(DESTDIR)$(bindir)/textmidi$(EXEEXT)
	$(INSTALL_PROGRAM) miditext$(EXEEXT) $(DESTDIR)$(bindir)/miditext$(EXEEXT)
	$(INSTALL_PROGRAM) smustextmidi$(EXEEXT) $(DESTDIR)$(bindir)/smustextmidi$(EXEEXT)
	$(INSTALL_PROGRAM) textmidicgm$(EXEEXT) $(DESTDIR)$(bindir)/textmidicgm$(EXEEXT)
uninstall:
	/bin/rm -f $(DESTDIR)$(bindir)/textmidi$(EXEEXT)
	/bin/rm -f $(DESTDIR)$(bindir)/miditext$(EXEEXT)
	/bin/rm -f $(DESTDIR)$(bindir)/smustextmidi$(EXEEXT)
	/bin/rm -f $(DESTDIR)$(bindir)/textmidicgm$(EXEEXT)
install-strip:
	$(MAKE) $(INSTALL_PROGRAM) = '$(INSTALL_PROGRAM) -s' install
distclean : clean
mostlyclean : clean
maintainer-clean : distclean
TAGS : $(SOURCES)
	etags $(SOURCES)
	ctags $(SOURCES)
miditext.info: miditext.texi
	$(MAKEINFO) miditext.texi
textmidi.info: textmidi.texi
	$(MAKEINFO) textmidi.texi
smustextmidi.info: smustextmidi.texi
	$(MAKEINFO) smustextmidi.texi
textmidicgm.info: textmidicgm.texi
	$(MAKEINFO) textmidicgm.texi

