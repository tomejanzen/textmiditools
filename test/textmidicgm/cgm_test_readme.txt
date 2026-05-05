cgm_test is a suite of XML form files for testing textmidicgm and TextMidiFormEdit.py.
There is at least one form file to exercise each data item in the form file.

To build the textmidi files and MIDI 1.0 Standard MIDI Files, use the make file:
    make -f cgm_test.make

The following tools are useful in auditing the results:
    1.  A text editor to inspect the textmidi text file;
    2.  A MIDI player to hear the MIDI file;
    3.  A scoring program to see the musical score.

cgm_test_arrangement_definition_algorithm_a.xml
    Use the text editor to see that in each track the textmidi plays or rests every 10 seconds.
    The MIDI player can help verify the swapping of voices.
    All algorithms are not verified, but with only 2 voices the voices merely alternate.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_arrangement_definition_algorithm_b.xml
    Use the text editor to see that in each track the textmidi plays or rests every 15 seconds.
    The MIDI player can help verify the swapping of voices.
    All algorithms are not verified, but with only 2 voices the voices merely alternate.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_arrangement_definition_period_a.xml
    Use the text editor to see that in each track the textmidi plays or rests every 20 seconds.
    The MIDI player can help verify the swapping of voices.
    All algorithms are not verified, but with only 2 voices the voices merely alternate.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_arrangement_definition_period_b.xml
    Use the text editor to see that in each track the textmidi plays or rests every 30 seconds.
    The MIDI player can help verify the swapping of voices.
    All algorithms are not verified, but with only 2 voices the voices merely alternate.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_beat_a.xml
    Use the scoring program to see a tempo of 120/quarter.
    Load the form file into TextMidiFormEdit.py and check the beat is 1/2.
    You may use miditext utility to recreate a temporary textmidi file that will show
    the tempo for a quarter note.
cgm_test_beat_b.xml
    Use the text editor to verify a tempo with a beat of 3/8.
    Use the scoring program to verify a tempo of 90/quarter.
    Load the form file into TextMidiFormEdit.py and check the data.
    You may use miditext utility to recreate a temporary textmidi file that will show
    the tempo for a quarter note.
cgm_test_beat_c.xml
    Use the text editor to verify that the TEMPO statement has a beat of 1/3.
    Use the scoring program to see a tempo of 80/quarter.
    Load the form file into TextMidiFormEdit.py and check the data.
    You may use miditext utility to recreate a temporary textmidi file that will show
    the tempo for a quarter note.
cgm_test_copyright_a.xml
    Load the form file into TextMidiFormEdit.py and check the data.
    The copyright notice does no propagate to other files as it applies to the form file.
cgm_test_dynamic_form_mean_amplitude_a.xml
    Use the MIDI player to hear the dynamics very fairly loud wherever the plot for dynamics
    crosses the upper limit line in TextMidiFormEdit.py.
    Load the form file into TextMidiFormEdit.py and check the data.
    If your MIDI player has a progress indicator, you can try to resize the windows so that
    the progress indicator in the MIDI player is close to under the correct position
    on the form plot in TextMidiFormEdit.py.
cgm_test_dynamic_form_mean_offset_a.xml
    Use the MIDI player to hear that the dynamics are fairly loud.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_dynamic_form_mean_period_a.xml
    Use the MIDI player to hear the loudness begin quietly and be loudest at the end.
    Load the form file into TextMidiFormEdit.py and check the data.
    The dynamics curve should be thin, starting at the bottom, and rise to the top line
    in 60 seconds.
cgm_test_dynamic_form_mean_period_b.xml
    Use the MIDI player to hear the loudness begin quietly and be loudest at 30 seconds,
    then fall to quiet at the end.
    Load the form file into TextMidiFormEdit.py and check that the dynamic curve starts and ends
    quietly but is loudest in the middle.
cgm_test_dynamic_form_mean_period_c.xml
    Use the MIDI player to hear the loudness start quietly and peak at 20 seconds and 60 seconds.
    Load the form file into TextMidiFormEdit.py and check the curve.
cgm_test_dynamic_form_mean_phase_a.xml
    Use the MIDI player to hear the dynamic begin and end medium loud but be loudest at 15 seconds
    and quietest at 45 seconds.
    Load the form file into TextMidiFormEdit.py and check the curve for dynamics.
cgm_test_dynamic_form_mean_phase_b.xml
    Use the MIDI player to hear the dynamic begin and end very loud but be quietest at 30 seconds.
    Load the form file into TextMidiFormEdit.py and check the curve.
cgm_test_dynamic_form_mean_phase_c.xml
    Use the MIDI player to hear the dynamics be middle loud at the beginning, end and
    at 30 seconds, lowest at 15 seconds and loudest at 45 seconds.
    Load the form file into TextMidiFormEdit.py and check the dynamic curve.
cgm_test_dynamic_form_range_amplitude_a.xml
    Use the MIDI player to hear dynamics be varied always.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_dynamic_form_range_offset_a.xml
    Use the MIDI player to hear dynamics be varied always.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_dynamic_form_range_period_a.xml
    Use the MIDI player to hear the range of dynamics be 
    consistent at the beginning and end but vary greatly in the middle.
    Load the form file into TextMidiFormEdit.py and check the dynamic range
    widen at 30 seconds but be narrow at the beginning and end.
cgm_test_dynamic_form_range_period_b.xml
    Use the MIDI player to hear the range of dynamics be 
    consistent at the beginning, end, and middle but vary greatly at 15 and 45 seconds.
    Load the form file into TextMidiFormEdit.py and check the dynamic curve to be
    like a sideways hourglass.
cgm_test_dynamic_form_range_period_c.xml
    Use the MIDI player to hear the range of dynamics be 
    consistent at the beginning and at 40 seconds, but vary greatly at 20 and 60 seconds.
    Load the form file into TextMidiFormEdit.py and check the dynamic curve.
cgm_test_dynamic_form_range_phase_a.xml
    Use the MIDI player to hear the range of dynamics start somewhat varied, becoming
    more varied until 15 seconds, becoming less varied until 45 seconds, then rising at the end.
    Load the form file into TextMidiFormEdit.py and check the dynamic curve.
cgm_test_dynamic_form_range_phase_b.xml
    Use the MIDI player to hear the dynamics be varied at the beginning and end but
    flat at the middle at 30 seconds.
    Load the form file into TextMidiFormEdit.py and check the dynamics curve.
cgm_test_dynamic_form_range_phase_c.xml
    Use the MIDI player to hear dynamics quiet and flatten until 15 seconds, becoming
    more varied until 45 seconds, then quieting somewhat at the end.
    Load the form file into TextMidiFormEdit.py and check the dynamics curve.
cgm_test_len_a.xml
    Use the MIDI player to check a length of 60 seconds.
    Load the form file into TextMidiFormEdit.py and check the length.
cgm_test_len_b.xml
    Use the MIDI player to check a length of 30 seconds.
    Load the form file into TextMidiFormEdit.py and check the length.
cgm_test_max_note_len_a.xml
    Use the text editor to verify that the longest note is a quarter note.
    Use the MIDI player to hear that the shortest note is a second long.
    Use the scoring program to see that there are no notes longer than a quarter note.
    Load the form file into TextMidiFormEdit.py and check the max_note_len value.
cgm_test_max_note_len_b.xml
    Use the text editor to verify that the longest note is 10 quarter notes.
    Use the MIDI player to hear that the shortest note is 10 seconds long.
    Use the scoring program to see that there are no notes longer than 10 quarters.
    Load the form file into TextMidiFormEdit.py and check the max_note_len value is 10.
cgm_test_melody_probabilities_a.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that the voice wanders a bit up and down but does not drift up or down very far.
    Load the form file into TextMidiFormEdit.py and check the melody probabilities values.
cgm_test_melody_probabilities_b.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that the voice wanders a bit up and down but does not drift up or down very far,
    but around every 4th note on average is a rest.
    Load the form file into TextMidiFormEdit.py and check the melody probabilities values.
cgm_test_melody_probabilities_c.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that the voice wanders a bit up and down and drifts quickly up.
    Load the form file into TextMidiFormEdit.py and check the melody probabilities values.
cgm_test_melody_probabilities_d.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that the voice wanders a bit up and down and drifts quickly down.
    Load the form file into TextMidiFormEdit.py and check the melody probabilities values.
cgm_test_meter_a.xml
    Use the text editor to verify the TIME_SIGNATURE statement gives 5/4 as the meter.
    Use the scoring program to see the meter.  This may depend on the scoring softare used.
    Load the form file into TextMidiFormEdit.py and check the meter.
cgm_test_meter_b.xml
    Use the text editor to verify the TIME_SIGNATURE statement gives 6/8 as the meter.
    Use the scoring program to see the meter.  This may depend on the scoring softare used.
    Load the form file into TextMidiFormEdit.py and check the meter.
cgm_test_meter_c.xml
    Use the text editor to verify the TIME_SIGNATURE statement gives 11/16 as the meter.
    Use the scoring program to see the meter.  This may depend on the scoring softare used.
    Load the form file into TextMidiFormEdit.py and check the meter.
cgm_test_min_note_len_a.xml
    Use the text editor to verify that no note is shorter than the minimum note length.
    You will have to convert the minimum note length, in seconds, to a musical rhythm.
    Use the scoring program to see the note lengths.
    Load the form file into TextMidiFormEdit.py and check the data for minimum note length.
cgm_test_min_note_len_b.xml
    Use the text editor to verify that no note is shorter than the minimum note length.
    You will have to convert the minimum note length, in seconds, to a musical rhythm.
    Use the scoring program to see the note lengths.
    Load the form file into TextMidiFormEdit.py and check the data for minimum note length.
cgm_test_name_a.xml
    Use the text editor to verify that the <name_> tag value in the XML form file is used after
    the TRACK TEMPO statement.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_name_b.xml
    Use the text editor to verify that the <name_> tag value in the XML form file is used after
    the TRACK TEMPO statement.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_pitch_form_mean_amplitude_a.xml
    Use the MIDI player to hear the pitch be at the lowest note when the curve is low and the highest note
    when the curve is high.
    Load the form file into TextMidiFormEdit.py and check the pitch curve to be twice as high as the dashed limit lines.
cgm_test_pitch_form_mean_offset_a.xml
    Use the text editor to verify primarily higher pitches.
    Use the MIDI player to hear primarily higher pitches.
    Use the scoring program to see primarily higher pitches.
    Load the form file into TextMidiFormEdit.py and check the pitch mean curve to be
    a line sitting above the center line. 
cgm_test_pitch_form_mean_period_a.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the pitches start and end low but are highest in the middle at 60 seconds.
    Load the form file into TextMidiFormEdit.py and check the pitch mean curve.
cgm_test_pitch_form_mean_period_b.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the pitches are low at the start, middle, and end, but peak 
    at 30 and 90 seconds.
    Load the form file into TextMidiFormEdit.py and check the pitch mean curve.
cgm_test_pitch_form_mean_period_c.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the pitches are lowest at 0, 20, 80 and 120 seconds, and peak
    at 20, 60 and 100 seconds.
    Load the form file into TextMidiFormEdit.py and check the pitch mean curve.
cgm_test_pitch_form_mean_phase_a.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the pitches at the start, middle, and end are at a middle pitch, 
    and peak at 15 seconds and are lowest at 45 seconds.
    Load the form file into TextMidiFormEdit.py and check the pitch mean curve.
cgm_test_pitch_form_mean_phase_b.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the pitches start and end at a peak but dip to lowest at 30 seconds.
    Load the form file into TextMidiFormEdit.py and check the pitch mean curve.
cgm_test_pitch_form_mean_phase_c.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the pitches at the start, middle, and end are middling loud
    and dip to lowest at 15 seconds and the highest at 45 seconds.
    Load the form file into TextMidiFormEdit.py and check the pitch mean curve.
cgm_test_pitch_form_range_amplitude_a.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the range of pitches is narrow and beginning and end and very wide
    in the middle.  The curve should probably not be so thin at the ends but this
    has not been investigated.
    Load the form file into TextMidiFormEdit.py and check the pitch range plotted
    as the width of of the pitch curve.
cgm_test_pitch_form_range_offset_a.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the range of pitches is the same width from start to end.
    Load the form file into TextMidiFormEdit.py and check the pitch range plotted
    as the width of of the pitch curve.
cgm_test_pitch_form_range_period_a.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the range of pitches at the start and end are narrow
    but wide in the middle at 60 seconds.
    Load the form file into TextMidiFormEdit.py and check the pitch range plotted
    as the width of of the pitch curve.
cgm_test_pitch_form_range_period_b.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the range of pitches at the start and at 40 seconds, middle, and end
    Load the form file into TextMidiFormEdit.py and check the pitch range plotted
    as the width of of the pitch curve.
cgm_test_pitch_form_range_period_c.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the range of pitches is mininum at 0, 40, 80 and 120 seconds,
    and is widest at 20, 60, and 100 seconds.
    Load the form file into TextMidiFormEdit.py and check the pitch range plotted
    as the width of of the pitch curve.
cgm_test_pitch_form_range_phase_a.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the range of pitches is medium at the beginning, widens until
    15 seconds, then narrows until 45 seconds, then grows until the end.
    Load the form file into TextMidiFormEdit.py and check the pitch range plotted
    as the width of of the pitch curve.
cgm_test_pitch_form_range_phase_b.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the range of pitches is widest at the start and end,
    and narrowest at 30 seconds in the middle.
    Load the form file into TextMidiFormEdit.py and check the pitch range plotted
    as the width of of the pitch curve.
cgm_test_pitch_form_range_phase_c.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the range of pitches s medium wide at the beginning, narrowest at 15 seconds,
    grows to widest at 45 seconds, then narrows a bit until the end.
    Load the form file into TextMidiFormEdit.py and check the pitch range plotted
    as the width of of the pitch curve.
cgm_test_pulse_a.xml
    Use the text editor to verify that no note is shorter then a 1/32 note.
    Use the MIDI player to hear a line that pulses 8/second.
    Use the scoring program to see 1/32 notes.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_pulse_b.xml
    Use the text editor to verify that no note is shorter then a 1/2 note.
    Use the MIDI player to hear a line that pulses 1 in 2 seconds.
    Use the scoring program to see 1/2 notes.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_rhythm_form_mean_amplitude_a.xml
    Load the form file into TextMidiFormEdit.py and check the rhythm curve
    to be thin and three times wider than the limit lines.
cgm_test_rhythm_form_mean_offset_a.xml
    Use the MIDI player to hear long notes.
    Load the form file into TextMidiFormEdit.py and check the rhythm curve,
    which should e a line above the center line but below the limit line.
cgm_test_rhythm_form_mean_period_a.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that the rhythm starts very fast and gradually slows until the end.
    Load the form file into TextMidiFormEdit.py and check the data.
    The rhythm curve is very thin and rises from a low to a high point.
cgm_test_rhythm_form_mean_period_b.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm starts and ends very fast and gradually slows until the middle.
    Load the form file into TextMidiFormEdit.py and check the data.
    The rhythm curve is very thin and rises from a low to a high point and back.
cgm_test_rhythm_form_mean_period_c.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm starts and ends very fast and gradually slows until 15 seconds,
    falling to a minimum at 45 seconds.
    Load the form file into TextMidiFormEdit.py and check the data.
    The rhythm curve is very thin and rises from a low to a high point and back, then
    approaches a mid point at the end..
cgm_test_rhythm_form_mean_phase_a.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm starts and ends very fast and gradually slows until 15 seconds,
    falling to a minimum at 45 seconds.
    Load the form file into TextMidiFormEdit.py and check the data.
    The rhythm curve is very thin and rises from a low to a high point and back, then
    approaches a mid point at the end..
cgm_test_rhythm_form_mean_phase_b.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm starts and ends very slow and gradually speeds up until 30 seconds,
    Load the form file into TextMidiFormEdit.py and check the data.
    The rhythm curve is very thin and falls from a high to a low point and back.
cgm_test_rhythm_form_mean_phase_c.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm starts and ends in the middle, falls to a minimum at 15 seconds,
    rises to a peak at 45 seconds, then falls off to the end.
    Load the form file into TextMidiFormEdit.py and check the curve.
cgm_test_rhythm_form_range_amplitude_a.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the range of durations is narrow and beginning and end and very wide
    in the middle.  The curve should probably not be so thin at the ends but this
    has not been investigated.
    Load the form file into TextMidiFormEdit.py and check the rhythm range plotted
    as the width of of the pitch curve.
cgm_test_rhythm_form_range_offset_a.xml
    Use the text editor, the MIDI player, and the scoring program 
    to verify that the range of rhythms is the same width from start to end.
    Load the form file into TextMidiFormEdit.py and check the rhythm range plotted
    as the width of of the rhythm curve.
cgm_test_rhythm_form_range_period_a.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm begins consistent in the middle of the range then broadens to
    a maximum of duration variation at the end.
    Load the form file into TextMidiFormEdit.py and check the rhythm curve 
    is narrow at the left and wide at the right.
cgm_test_rhythm_form_range_period_b.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm begins consistent in the middle of the range then broadens to
    a maximum of duration variation at 40 seconds and narrows a bit toward the end.
    Load the form file into TextMidiFormEdit.py and check the rhythm curve.
cgm_test_rhythm_form_range_period_c.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm begins consistent in the middle of the range then broadens to
    a maximum of duration variation at 20 seconds and narrows until 40 seconds, then
    broadens a bit toward the end.
    Load the form file into TextMidiFormEdit.py and check the rhythm curve.
cgm_test_rhythm_form_range_phase_a.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm begins somewhat mixed then broadens to
    a maximum of duration variation at 15 seconds and narrows until 45 seconds, then
    broadens a bit toward the end.
    Load the form file into TextMidiFormEdit.py and check the rhythm curve.
cgm_test_rhythm_form_range_phase_b.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm begins very mixed then narrows to
    a minimum of duration variation at 30 seconds and broadens until the end.
    Load the form file into TextMidiFormEdit.py and check the rhythm curve.
cgm_test_rhythm_form_range_phase_c.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the rhythm begins very mixed then narrows to
    a minimum of duration variation at 30 seconds and broadens until the end.
    Load the form file into TextMidiFormEdit.py and check the rhythm curve.
cgm_test_scale_a.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the scale used is only the C Major diatonic scale.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_scale_b.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the scale used is only the C-based whole-tone scale.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_texture_form_amplitude.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the maximum number of voices (all the voices) are used when the
    curve exceeds the limit line.
    Load the form file into TextMidiFormEdit.py and check the data.
    The texture curve exceeds the limit lines for about 20 seconds
    and for 10 seconds at the end.
cgm_test_texture_form_offset.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the maximum number of voices (all the voices) are used when the
    curve exceeds the limit line.
    Load the form file into TextMidiFormEdit.py and check the data.
    The texture curve starts already a bit wide, and exceeds the limit lines 
    for about the last 15 seconds.
cgm_test_texture_form_period_a.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the number of voices playing is half of voices at the start, 
    grows to all at 15 secondes, slowly falls to 1 at 45 seconds and
    increases a bit to the end.
    Load the form file into TextMidiFormEdit.py and check the texture curve.
cgm_test_texture_form_period_b.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the number of voices playing is half of voices at the start, 
    rises to a peak at 15 seconds, falls to 1 voice at 30 seconds, rises again
    to a all voices at 45 seconds, the falls to half at the end.
    Load the form file into TextMidiFormEdit.py and check the texture curve.
    It is like an hourglass.
cgm_test_texture_form_period_c.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the number of voices playing is half of voices at the start, 
    and has two peaks of all voices, coming back to half at the end.
    Load the form file into TextMidiFormEdit.py and check the texture curve.
cgm_test_texture_form_phase_a.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the number of voices playing is half of voices at the start, 
    grows to all at 15 secondes, slowly falls to 1 at 45 seconds and
    increases a bit to the end.
    Load the form file into TextMidiFormEdit.py and check the texture curve.
cgm_test_texture_form_phase_b.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the number of voices playing is all voices at the start, 
    falls to 1 voice at 30 seconds and increases to all at the end.
    Load the form file into TextMidiFormEdit.py and check the texture curve.
    It is like an hourglass.
cgm_test_texture_form_phase_c.xml
    Use the text editor, the MIDI player, and the scoring program to check
    that the number of voices playing is half of voices at the start, 
    falls to 1 voice at 15 seconds and increases to all at 45 seconds,
    falling to half at the end.
    Load the form file into TextMidiFormEdit.py and check the texture curve.
cgm_test_ticks_per_quarter_a.xml
    Use the text editor to verify the FILEHEADER and TIME_SIGNATURE statements
    show 1440 ticks per quarter.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_ticks_per_quarter_b.xml
    Use the text editor to verify the FILEHEADER and TIME_SIGNATURE statements
    show 720 ticks per quarter.
    Use the text editor to verify 
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_channel.xml
    Use the text editor to verify that the channel is 16 for the one voice.
    Load the form file into TextMidiFormEdit.py and check the voice channel data.
cgm_test_voices_follower_delay_a.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact canon at the unison with no delay.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_follower_delay_b.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact canon at the unison with a quarter note delay.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_follower_delay_c.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact canon at the unison with a 3-quarter note delay.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_follower_duration_factor_a.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact canon at the unison with no delay and
    no change in rhythm.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_follower_duration_factor_b.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact canon at the unison with no delay and
    halved rhythms.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_follower_duration_factor_c.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact canon at the unison with no delay and
    rhythms that are half-again as long.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_follower_follow_a.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is no follower or canon voice.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_follower_follow_b.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact follower voice.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_follower_interval_a.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is a exact follower voice in a C diatonic mode.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_follower_interval_b.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is a follower voice in a C diatonic mode one sixth interval higher.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_follower_interval_type_a.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is a follower voice in a C diatonic mode one sixth interval higher.
    The follow is also on the C scale.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_follower_interval_type_b.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is a follower voice in a C diatonic mode six semitones higher.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_follower_inversion_a.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact follower voice in a C diatonic mode.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_follower_inversion_b.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact inverted follower voice in a C diatonic mode.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_follower_leader_c.xml
    While running the make file you will see a message that voice 1 follows
    itself.
    You can see this any time by deleting cgm_test_voices_follower_leader_c.txt
    and running "make -f cgm_test.make".
    "Composer.cc:165 voice 1 is a self-follower!"
    No note events are generated for self-followers, or for members of citing circles.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_follower_retrograde_a.xml
    [retrograde is false].
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact follower voice in a C diatonic mode.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_follower_retrograde_b.xml
    Use the text editor, the MIDI player, and the scoring program to see 
    that there is an exact retrograde follower voice in a C diatonic mode.
    Load the form file into TextMidiFormEdit.py and check the follower data.
cgm_test_voices_high_pitch.xml
    Use the text editor and the scoring program to verify that there is
    no note above that given in the form file as voices_high_pitch in the voice.
    Load the form file into TextMidiFormEdit.py and check the voice data.
cgm_test_voices_low_pitch.xml
    Use the text editor and the scoring program to verify that there is
    no note below that given in the form file as voices low_pitch in the voice.
    Load the form file into TextMidiFormEdit.py and check the voice data.
cgm_test_voices_pan_a.xml
    [pan is left]
    Use the text editor to verify that the PAN command for the voice matches the XML form file pan value.
    Use the MIDI player if it helps identify pan position.
    Load the form file into TextMidiFormEdit.py and check the voice pan data.
cgm_test_voices_pan_b.xml
    [pan is center]
    Use the text editor to verify that the PAN command for the voice matches the XML form file pan value.
    Use the MIDI player if it helps identify pan position.
    Load the form file into TextMidiFormEdit.py and check the voice pan data.
cgm_test_voices_pan_c.xml
    [pan is right]
    Use the text editor to verify that the PAN command for the voice matches the XML form file pan value.
    Use the MIDI player if it helps identify pan position.
    Load the form file into TextMidiFormEdit.py and check the voice pan data.
cgm_test_voices_program_a.xml
    [program is a clarinet patch]
    Use the text editor to verify the PROGRAM value is 72. 
    Use the MIDI player to hear a clarinet.
    Load the form file into TextMidiFormEdit.py and check the voice program data.
cgm_test_voices_program_b.xml
    [program is a violin patch]
    Use the text editor to verify the PROGRAM value is 72. 
    Use the MIDI player to hear a violin.
    Load the form file into TextMidiFormEdit.py and check the voice program data.
cgm_test_voices_program_c.xml
    [program is a church organ patch]
    Use the text editor to verify the PROGRAM value is 20. 
    Use the MIDI player to hear a church organ.
    Load the form file into TextMidiFormEdit.py and check the voice program data.
cgm_test_voices_program_d.xml
    [program is a glockenspiel patch]
    Use the text editor to verify the PROGRAM value is 10. 
    Use the MIDI player to hear a glockenspiel.
    Load the form file into TextMidiFormEdit.py and check the voice program data.
cgm_test_voices_random_program_ensemble_a.xml
    [no ensemble given]
    Use the text editor, MIDI player and scoring program to show that only a piano patch plays,
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_random_program_ensemble_b.xml
    [ensemble is Acoustic grand, bright acoustic and electric grand pianos]
    Use the text editor, MIDI player and scoring program to show that 3 different
    pianos alternate randomly in the voice.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_random_program_ensemble_c.xml
    [ensemble is piano, violin, clarinet and french horn]
    Use the text editor, MIDI player and scoring program to show that 
    the instruments alternate randomly in the voice.
    Load the form file into TextMidiFormEdit.py and check voice random ensemble data.
cgm_test_voices_random_program_probability_a.xml
    [probability is 0]
    Use the text editor, MIDI player and scoring program to show that only a piano patch plays,
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_random_program_probability_b.xml
    [probability is 0.5]
    Use the text editor, MIDI player and scoring program to show that the
    instruments freely alternate but only have a 50/50 chance of changing on each note.
    Load the form file into TextMidiFormEdit.py and check the data.
cgm_test_voices_random_program_probability_c.xml
    [probability is 1]
    Use the text editor, MIDI player and scoring program to show that the
    instruments freely alternate.
    Load the form file into TextMidiFormEdit.py and check the voice random ensemble data.
cgm_test_voices_walking_a.xml
    Use the text editor, the MIDI player, and the scoring program to verify
    that the voice jumps around randomly.
    Load the form file into TextMidiFormEdit.py and check the voice data.
cgm_test_voices_walking_b.xml
    Use the text editor, the MIDI player, and the scoring program to verify
    that the voice jumps around randomly.
    Load the form file into TextMidiFormEdit.py and check the voice data.
cgm_test_voices_walking_c.xml
    [walking = 0.5]
    Use the text editor, the MIDI player, and the scoring program to verify
    that half the time the voice jumps around randomly and half the time it
    moves stepwise.
    Load the form file into TextMidiFormEdit.py and check the voice data.

