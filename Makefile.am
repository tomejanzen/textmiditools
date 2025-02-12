SET_MAKE = @SET_MAKE@
bin_PROGRAMS = textmidi$(EXEEXT) miditext$(EXEEXT) smustextmidi$(EXEEXT) textmidicgm$(EXEEXT) 
textmidi_SOURCES = main.cc textmidi.l Midi.cc MIDIKeyString.cc RhythmRational.cc MidiMaps.cc MidiString.cc
miditext_SOURCES = miditext.cc MidiEvents.cc MIDIKeyString.cc RhythmRational.cc rational_support.cc MidiMaps.cc MidiString.cc
smustextmidi_SOURCES = smustextmidi.cc SmusTrackEvent.cc Midi.cc MidiEvents.cc MidiMaps.cc MIDIKeyString.cc RhythmRational.cc rational_support.cc MidiString.cc
textmidicgm_SOURCES = textmidicgm.cc cgmlegacy.cc MIDIKeyString.cc MusicalForm.cc NoteEvent.cc Track.cc Voice.cc GnuPlot.cc Composer.cc Scales.cc GeneralMIDI.cc RhythmRational.cc Midi.cc MidiMaps.cc Arrangements.cc

textmidi_LDADD = @LEXLIB@
info_TEXINFOS = textmidi.texi miditext.texi smustextmidi.texi textmidicgm.texi textmidiformedit.texi

AM_LDFLAGS = -pthread -lboost_program_options -lboost_system -lboost_serialization
CXX = /usr/bin/env g++
AM_CXXFLAGS  = -D_GLIBCXX_ASSERTIONS -std=c++23 -Wall -Wpedantic -Wextra -Weffc++ -Wshadow
AM_CXXFLAGS += -O2
AM_LFLAGS = -i
LEXFLAGS = -i
LEX=/usr/bin/env lex

all : ${bin_PROGRAMS}

textmidi$(EXEEXT) : main.o textmidi.o Midi.o MIDIKeyString.o RhythmRational.o MidiMaps.o MidiString.o DynamicsOptions.o
	$(CXX) $(AM_CXXFLAGS) -o $@ main.o textmidi.o Midi.o MIDIKeyString.o RhythmRational.o MidiMaps.o MidiString.o DynamicsOptions.o $(AM_LDFLAGS)
main.o: main.cc textmidi.h Midi.h Options.h OptionName.h
	$(CXX) -c $(AM_CXXFLAGS) -o $@ $<
textmidi.o: textmidi.cc Midi.h MidiMaps.h TextmidiUtils.h MIDIKeyString.h MidiString.h textmidi.h DynamicsOptions.h OptionName.h
	$(CXX) -c $(AM_CXXFLAGS) -o $@ $<
textmidi.cc : textmidi.l
	$(LEX) $(LEXFLAGS) -o $@ $<
miditext$(EXEEXT) : miditext.o Midi.o MidiEvents.o MIDIKeyString.o RhythmRational.o rational_support.o MidiMaps.o MidiString.o DynamicsOptions.o
	$(CXX) $(AM_CXXFLAGS) -o $@ $< Midi.o RhythmRational.o MidiEvents.o MIDIKeyString.o rational_support.o MidiMaps.o MidiString.o DynamicsOptions.o $(AM_LDFLAGS)
rational_support.o: rational_support.cc rational_support.h RhythmRational.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
MidiString.o: MidiString.cc MidiString.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
miditext.o: miditext.cc rational_support.h RhythmRational.h TextmidiUtils.h MidiEvents.h Midi.h MidiMaps.h Options.h DynamicsOptions.h OptionName.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
Midi.o : Midi.cc Midi.h
	$(CXX) -c $(AM_CXXFLAGS) -o $@ $<
MidiMaps.o: MidiMaps.cc MidiMaps.h Midi.h
	$(CXX) -c $(AM_CXXFLAGS) -o $@ $<
Arrangements.o: Arrangements.cc Arrangements.h HeapsAlgorithmTemplate.h
	$(CXX) -c $(AM_CXXFLAGS) -o $@ $<
MidiEvents.o: MidiEvents.cc MidiEvents.h RhythmRational.h Midi.h MidiMaps.h rational_support.h MIDIKeyString.h MidiString.h
	$(CXX) $(AM_CXXFLAGS) -c -o MidiEvents.o MidiEvents.cc
smustextmidi$(EXEEXT) : smustextmidi.o SmusTrackEvent.o Midi.o MidiMaps.o MidiEvents.o MIDIKeyString.o RhythmRational.o rational_support.o MidiString.o DynamicsOptions.o
	$(CXX) $(AM_CXXFLAGS) $< -o $@ SmusTrackEvent.o Midi.o MidiMaps.o MidiEvents.o MIDIKeyString.o RhythmRational.o rational_support.o MidiString.o DynamicsOptions.o $(AM_LDFLAGS)
smustextmidi.o: smustextmidi.cc Midi.h TextmidiUtils.h rational_support.h RhythmRational.h SmusTrackEvent.h MidiMaps.h Options.h DynamicsOptions.h OptionName.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
SmusTrackEvent.o: SmusTrackEvent.cc SmusTrackEvent.h Midi.h MidiMaps.h RhythmRational.h rational_support.h MIDIKeyString.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
RhythmRational.o: RhythmRational.cc RhythmRational.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
DynamicsOptions.o: DynamicsOptions.cc DynamicsOptions.h OptionName.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<

textmidicgm$(EXEEXT) : textmidicgm.o cgmlegacy.o MIDIKeyString.o MusicalForm.o NoteEvent.o Track.o Voice.o GnuPlot.o Composer.o Scales.o GeneralMIDI.o RhythmRational.o Midi.o MidiMaps.o Arrangements.o
	$(CXX) $(AM_CXXFLAGS) $< -o $@ cgmlegacy.o MIDIKeyString.o MusicalForm.o NoteEvent.o Track.o Voice.o GnuPlot.o Composer.o Scales.o GeneralMIDI.o RhythmRational.o Midi.o MidiMaps.o Arrangements.o $(AM_LDFLAGS) 
textmidicgm.o: textmidicgm.cc MIDIKeyString.h Midi.h MusicalForm.h cgmlegacy.h Voice.h Track.h NoteEvent.h RhythmRational.h RandomDouble.h RandomInt.h Scales.h GeneralMIDI.h MidiMaps.h Arrangements.h HeapsAlgorithmTemplate.h Options.h Composer.h RhythmRational.h OptionName.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
cgmlegacy.o : cgmlegacy.cc  cgmlegacy.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
MIDIKeyString.o: MIDIKeyString.cc Midi.h MIDIKeyString.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
MusicalForm.o: MusicalForm.cc MusicalForm.h cgmlegacy.h Voice.h MIDIKeyString.h Midi.h Track.h NoteEvent.h RhythmRational.h RandomDouble.h RandomInt.h Scales.h GeneralMIDI.h MidiMaps.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
Track.o: Track.cc Track.h NoteEvent.h RhythmRational.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
Voice.o: Voice.cc Voice.h cgmlegacy.h MIDIKeyString.h Midi.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
Scales.o : Scales.cc Scales.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
NoteEvent.o: NoteEvent.cc NoteEvent.h RhythmRational.h MIDIKeyString.h Midi.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
GnuPlot.o: GnuPlot.cc GnuPlot.h MusicalForm.h cgmlegacy.h Voice.h MIDIKeyString.h Midi.h Track.h NoteEvent.h RhythmRational.h RandomDouble.h RandomInt.h Scales.h GeneralMIDI.h MidiMaps.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
Composer.o: Composer.cc Composer.h Midi.h MidiMaps.h RhythmRational.h MusicalForm.h cgmlegacy.h Voice.h MIDIKeyString.h Track.h NoteEvent.h RandomDouble.h RandomInt.h Scales.h GeneralMIDI.h Arrangements.h HeapsAlgorithmTemplate.h GnuPlot.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<
GeneralMIDI.o: GeneralMIDI.cc GeneralMIDI.h Midi.h MidiMaps.h
	$(CXX) $(AM_CXXFLAGS) -c -o $@ $<

    clean:
	rm -f cgmlegacy.o Composer.o GeneralMIDI.o GnuPlot.o main.o MIDIKeyString.o MidiEvents.o MidiMaps.o MidiString.o Midi.o miditext.o MusicalForm.o NoteEvent.o Scales.o smustextmidi.o SmusTrackEvent.o textmidicgm.o textmidi.o textmidi.cc Track.o Voice.o RhythmRational.o rational_support.o Arrangements.o DynamicsOptions.o textmidicgm$(EXEEXT) miditext$(EXEEXT) textmidi$(EXEEXT) smustextmidi$(EXEEXT) *.cps *.toc *.cp *.aux textmidi.log textmidicgm.log smustextmidi.log miditext.log textmidiformedit.log
    install:
	$(INSTALL_PROGRAM) textmidi$(EXEEXT) $(DESTDIR)$(bindir)/textmidi$(EXEEXT)
	$(INSTALL_PROGRAM) miditext$(EXEEXT) $(DESTDIR)$(bindir)/miditext$(EXEEXT)
	$(INSTALL_PROGRAM) smustextmidi$(EXEEXT) $(DESTDIR)$(bindir)/smustextmidi$(EXEEXT)
	$(INSTALL_PROGRAM) textmidicgm$(EXEEXT) $(DESTDIR)$(bindir)/textmidicgm$(EXEEXT)
    uninstall:
	/usr/bin/env rm -f $(DESTDIR)$(bindir)/textmidi$(EXEEXT)
	/usr/bin/env rm -f $(DESTDIR)$(bindir)/miditext$(EXEEXT)
	/usr/bin/env rm -f $(DESTDIR)$(bindir)/smustextmidi$(EXEEXT)
	/usr/bin/env rm -f $(DESTDIR)$(bindir)/textmidicgm$(EXEEXT)
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
    textmidiformedit.info: textmidiformedit.texi
	$(MAKEINFO) textmidiformedit.texi

