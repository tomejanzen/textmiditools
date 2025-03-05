textmidi --textmidi chorded.txt  --midi chorded.mid
miditext --lazy --midi chorded.mid --textmidi chordedlazy.txt
textmidi --textmidi chordedlazy.txt  --midi chordedlazy.mid
diff -s chorded.mid chordedlazy.mid
textmidi --textmidi chorded2.txt --midi chorded2.mid
miditext --lazy --midi chorded2.mid --textmidi chorded2lazy.txt
textmidi --textmidi chorded2lazy.txt  --midi chorded2lazy.mid
diff -s chorded2.mid chorded2lazy.mid
textmidi --textmidi chorded3.txt --midi chorded3.mid
miditext --lazy --midi chorded3.mid --textmidi chorded3lazy.txt
textmidi --textmidi chorded3lazy.txt  --midi chorded3lazy.mid
diff -s chorded3.mid chorded3lazy.mid

