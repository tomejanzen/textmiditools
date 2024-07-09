#!/usr/bin/env sed
# textmidi Version 1.0.81
# Copyright © 2024 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https:# gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
s/^vel \([[:digit:]]\{1,3\}\) \([A-Ga-g][#b]\?-\?[[:digit:]]\) \([[:digit:]\/]\+\)/longtone(eval(thechannel), \2, \3, 1, \1)/g
s/^\([A-Ga-g][#b]\?-\?[[:digit:]]\) \([[:digit:]\/]\+\)/longtone(eval(thechannel), \1, \2, 1, 120)/g
s/^STARTTRACK$/STARTTRACK define(`thechannel', incr(thechannel))/g
1idefine(`thechannel', `-1')

