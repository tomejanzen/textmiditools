# textmiditools
Interconvert MIDI, SMUS and text files; generate random music automatically.
These are shell utilities developed on Debian Linux using GNU autoconf tools.  A compilable release is provided.
The utilities are written in portable C++ with boost libraries; the form editor is in python.

textmidi: convert a text representation of standard MIDI 1 (Musical Instrument Digital Interface) files into the standard binary MIDI file format;

miditext: convert a standard musical MIDI 1 file into its text representation;

smustextmidi: convert a Simple Musical Score (SMUS) into a textmidi text representation of a standard MIDI 1 file

textmidicgm: computer-generated music using evenly-distributed random numbers controlled by sinusoidal waveforms that (usually slowly) change the character of the music.
textmidicgm does not make use of AI (Artificial Intelligence).  It does not train on other pre-existing music or other material.  The mathematics it
references is limited to simple population statistics (although the mean is imprecisely used) and trigonometric functions, because that's all the 
math I knew in 1976, when I conceived this approach to form (and prior to my engineering education).  The specific key (note) numbers, durations and velocities (loudnesses) are derived 
from flat, evenly-distributed random numbers, because that is what I explored in about 1980 on a Radio Shack TRS80.  Any resemblance to previously-existing
music is purely coincidental.   textmidicgm writes multiple monophonic lines (one line per track) and does not model themes, motifs, melodies, harmony or harmonic movement, nor a grammar of
note movement or harmonic movement.

