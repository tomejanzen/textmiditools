#!/usr/bin/bash
/usr/bin/env echo "Examine each data item in the default set-up"
/usr/bin/env echo "Save the defaults form file without changing anything"
/usr/bin/env echo "and exit."
../TextMidiFormEdit.py
/usr/bin/env git diff defaults.xml
/usr/bin/env xmllint --schema ../TextMidiForm.xsd defaults.xml
/usr/bin/env textmidicgm --xmlform defaults.xml 
/usr/bin/env textmidi --textmidi defaults.txt --midi defaults.mid
/usr/bin/env musescore defaults.mid

/usr/bin/env echo "Check each field and save and exit."
../TextMidiFormEdit.py textmidiformedittest.xml
/usr/bin/env git diff textmidiformedittest.xml
/usr/bin/env xmllint --schema ../TextMidiForm.xsd textmidiformedittest.xml

/usr/bin/env echo "Change the numbers; add 1 to ints, double floats"
/usr/bin/env echo "Change form name to test and change the copyright"
/usr/bin/env echo "pull all phases to the left for -pi"
/usr/bin/env echo "Save the file and exit."
../TextMidiFormEdit.py textmidiformedittest.xml
/usr/bin/env xmllint --schema ../TextMidiForm.xsd test.xml
/usr/bin/env diff textmidiformedittest.xml test.xml

