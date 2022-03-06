//
// TextMIDITools Version 1.0.22
//
// textmidicgm 1.0
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(GNUPLOT)
#    define  GNUPLOT

#include <string>
#include <vector>

#include "MusicalForm.h"

namespace textmidi
{
    namespace cgm
    {
    
        void write_form_as_gnuplot_data(
            const MusicalForm& musical_form,
            const std::string& gnuplot_filename);
    
    }
}
#endif
