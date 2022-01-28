//
// TextMIDITools Version 1.0.11
//
// textmidicgm 1.0
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(COMPOSER)
#    define  COMPOSER

#include <vector>
#include <fstream>

#include "MusicalForm.h"

namespace cgm
{

    void compose(const MusicalForm& xml_form, std::ofstream& textmidi_file, 
        bool gnuplot, bool answer);

}

#endif
