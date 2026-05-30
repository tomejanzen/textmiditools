textmidi --textmidi hires.txt --midi hires.mid
miditext --textmidi hiresout.txt --midi hires.mid
../od.bash hires.mid
cat hires.txt 
echo "12341234123412341234123412341234124"
cat hiresout.txt
cat hiresod.txt
