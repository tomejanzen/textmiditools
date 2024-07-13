#!/usr/bin/env bash
# TextMIDITools Version 1.0.81
# Copyright © 2024 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https:# gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
/usr/bin/env textmidicgm --xmlform pure.xml
/usr/bin/env sed -f addincludes.sed pure.txt > pure.m4
/usr/bin/env sed -f longtone_and_channel.sed -i pure.m4
/usr/bin/env m4 pure.m4 > purem4.txt
/usr/bin/env textmidi --textmidi purem4.txt --midi pure.mid
