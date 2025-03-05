#!/usr/bin/bash
textmidicgm --random random
xmllint random.xml
../textmidiformedit.py random.xml
xmllint random.xml
textmidicgm --xmlform random.xml --textmidi random.txt --instruments piano strings reed
textmidi --textmidi random.txt --midi random.mid
musescore random.mid
timidity -invvvt -Os random.mid
