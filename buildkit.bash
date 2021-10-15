#!/bin/bash
/usr/bin/env make clean
/usr/bin/env make
/usr/bin/env mkdir ~/TextMIDITools-1.0.7
/usr/bin/env cp textmidi.l ~/TextMIDITools-1.0.7/
/usr/bin/env cp textmidi.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp main.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp Midi.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp Midi.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp Options.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp MIDIKeyString.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp MIDIKeyString.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp MidiMessages.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp MidiMessages.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp NoteEvent.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp NoteEvent.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp smustextmidi.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp SmusTrackEvent.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp SmusTrackEvent.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp miditext.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp textmidicgm.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp Composer.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp Composer.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp MusicalForm.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp MusicalForm.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp GeneralMIDI.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp GeneralMIDI.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp GnuPlot.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp GnuPlot.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp RandomDouble.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp RandomInt.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp Scales.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp Scales.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp Track.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp Track.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp Voice.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp Voice.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp cgmlegacy.cc ~/TextMIDITools-1.0.7/
/usr/bin/env cp cgmlegacy.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp config.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp rational_support.h ~/TextMIDITools-1.0.7/
/usr/bin/env cp config.h.in ~/TextMIDITools-1.0.7/
/usr/bin/env cp README ~/TextMIDITools-1.0.7/
/usr/bin/env cp NEWS ~/TextMIDITools-1.0.7/
/usr/bin/env cp miditext.texi ~/TextMIDITools-1.0.7/
/usr/bin/env cp smustextmidi.texi ~/TextMIDITools-1.0.7/
/usr/bin/env cp textmidicgm.texi ~/TextMIDITools-1.0.7/
/usr/bin/env cp textmidi.texi ~/TextMIDITools-1.0.7/
/usr/bin/env cp animations1976.jpg ~/TextMIDITools-1.0.7/
/usr/bin/env cp animations.jpg ~/TextMIDITools-1.0.7/
/usr/bin/env cp aclocal.m4 ~/TextMIDITools-1.0.7/
/usr/bin/env cp instruments.m4 ~/TextMIDITools-1.0.7/
/usr/bin/env cp percussion.m4 ~/TextMIDITools-1.0.7/
/usr/bin/env cp ChangeLog ~/TextMIDITools-1.0.7/
/usr/bin/env cp configure ~/TextMIDITools-1.0.7/
/usr/bin/env cp configure.ac ~/TextMIDITools-1.0.7/
/usr/bin/env cp AUTHORS ~/TextMIDITools-1.0.7/
/usr/bin/env cp COPYING ~/TextMIDITools-1.0.7/
/usr/bin/env cp INSTALL ~/TextMIDITools-1.0.7/
/usr/bin/env cp NEWS ~/TextMIDITools-1.0.7/
/usr/bin/env cp README ~/TextMIDITools-1.0.7/
/usr/bin/env cp install-sh ~/TextMIDITools-1.0.7/
/usr/bin/env cp Makefile.am ~/TextMIDITools-1.0.7/
/usr/bin/env cp Makefile.in ~/TextMIDITools-1.0.7/
/usr/bin/env cp textmidicgm.gnuplot ~/TextMIDITools-1.0.7/
/usr/bin/env cp textmidi.vim ~/TextMIDITools-1.0.7/
/usr/bin/env cp allforms.py ~/TextMIDITools-1.0.7/
/usr/bin/env cp form.py ~/TextMIDITools-1.0.7/
/usr/bin/env cp sine.py ~/TextMIDITools-1.0.7/
/usr/bin/env cp textmidiformedit.py ~/TextMIDITools-1.0.7/
/usr/bin/env cp textmidiformedit.texi ~/TextMIDITools-1.0.7/
/usr/bin/env cp voice.py ~/TextMIDITools-1.0.7/
pushd ~/
/usr/bin/env tar cvfz TextMIDITools-1.0.7.tar.gz ./TextMIDITools-1.0.7
popd

