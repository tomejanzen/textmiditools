#!/usr/bin/bash
/usr/bin/env textmidicgm --random random
/usr/bin/env xmllint random.xml
../textmidiformedit.py random.xml
/usr/bin/env xmllint random.xml
/usr/bin/env textmidicgm --xmlform random.xml --textmidi random.txt --instruments piano strings reed
/usr/bin/env textmidi --textmidi random.txt --midi random.mid
/usr/bin/env musescore random.mid
/usr/bin/env timidity -invvvt -Os random.mid
