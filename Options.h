//
// TextMIDITools Version 1.0.91
//
// textmidi 1.0.6
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(OPTIONS_H)
#    define  OPTIONS_H

#include <string>
#include <string_view>

#include "OptionName.h"

const textmidi::OptionName help_option{"help", "help", 'h'};
const textmidi::OptionName verbose_option{"verbose", "write more trace information", 'v'};
const std::string_view VerboseTxt{"write more trace information"};
const textmidi::OptionName version_option{"version", "Write version information", 'V'};
const textmidi::OptionName midi_in_option{"midi", "binary MIDI file", 'i'};
const textmidi::OptionName midi_out_option{"midi", "binary MIDI file", 'o'};
const textmidi::OptionName answer_option{"answer", "Ask before overwriting a file", 'a'};
const textmidi::OptionName textmidi_in_option{"textmidi", "textmidi file", 'i'};
const textmidi::OptionName textmidi_out_option{"textmidi", "textmidi file", 'o'};
const textmidi::OptionName dynamics_configuration_option{"dynamics_configuration", "dynamics_configuration file", 'y'};
const textmidi::OptionName dotted_rhythms_option{"dotted_rhythms", "true or false", 'w'};
const textmidi::OptionName rhythm_expression_option{"rhythmexpression", "rational or simplecontinuedfraction", 'e'};
const textmidi::OptionName detache_option{"detache", "number of ticks to cheat notes to separate consecutive notes", 'd'};
const textmidi::OptionName lazy_note_off_option{"lazynoteoff", "To end a note in lazy mode, write a MIDI note-off instead of a note-on with zero velocity.", 'l'};
const textmidi::OptionName running_status_option{"runningstatus", "with {standard | never | persistentaftermeta | persistentaftersysex | persistentaftersysexormeta }; default is standard", 'n'};
const textmidi::OptionName quantize_option{"quantize", "quantization ratio in quotes: \"1/32\"", 'q'};
const textmidi::OptionName lazy_option{"lazy", "Attempt writing in lazy mode", 'l'};
const textmidi::OptionName smus_option{"smus", "input binary SMUS file", 'i'};
const textmidi::OptionName form_option{"form", "input plain text Form files; double-quote wildcards", 'f'};
const textmidi::OptionName XML_form_option{"xmlform", "input XML text Form files; double-quote wildcards", 'x'};
const textmidi::OptionName xml_update_option{"update", "update XML file with update_ prefix on the name given to --xmlform", 'u'};
const textmidi::OptionName gnuplot_option{"gnuplot", "gnuplot data output file", 'g'};
const textmidi::OptionName random_option{"random", "write a random form to the name given and exit", 'r'};
const textmidi::OptionName instruments_option{"instruments", "random instrument groups: piano chromaticpercussion organ guitar bass strings ensemble brass reed pipe synthlead synthpad syntheffects ethnic percussive soundeffects all melodic idiophone", 'i'};
const textmidi::OptionName clamp_scale_option{"clampscale", "In each form, clamp the scale to the union of the voice ranges", 'c'};
const textmidi::OptionName arrangements_option{"arrangements", "rotateright rotateleft reverse previouspermutation nextpermutation swappairs shuffle skip heaps identity", 'z'};
const textmidi::OptionName arrangements_period_option{"arrangementsperiod", "seconds before changing voice priority", 'y'};
const textmidi::OptionName max_events_per_track_option{"maxeventspertrack", "Maximum number of events in a track before stopping", 't'};
const textmidi::OptionName stack_tracks_option{"stacktracks", "process each form file and add its tracks to the same output textmidi score", 'k'};

#endif
