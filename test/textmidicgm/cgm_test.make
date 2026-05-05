#!/usr/bin/make

SHELL=/usr/bin/ksh

.SUFFIXES: .txt .mid .xml

M4=/usr/bin/m4
CP=/bin/cp
RM=/bin/rm
TEXTMIDI:=/usr/bin/env textmidi
TEXTMIDICGM:=/usr/bin/env textmidicgm
opus=testtextmidicgm

names:=\
cgm_test_name_a \
cgm_test_name_b \
cgm_test_copyright_a \
cgm_test_len_a \
cgm_test_len_b \
cgm_test_min_note_len_a \
cgm_test_min_note_len_b \
cgm_test_max_note_len_a \
cgm_test_max_note_len_b \
cgm_test_scale_a \
cgm_test_scale_b \
cgm_test_ticks_per_quarter_a \
cgm_test_ticks_per_quarter_b \
cgm_test_beat_a \
cgm_test_beat_b \
cgm_test_beat_c \
cgm_test_meter_a \
cgm_test_meter_b \
cgm_test_meter_c \
cgm_test_pulse_a \
cgm_test_pulse_b \
cgm_test_melody_probabilities_a \
cgm_test_melody_probabilities_b \
cgm_test_melody_probabilities_c \
cgm_test_melody_probabilities_d \
cgm_test_pitch_form_mean_period_a \
cgm_test_pitch_form_mean_period_b \
cgm_test_pitch_form_mean_period_c \
cgm_test_pitch_form_mean_phase_a \
cgm_test_pitch_form_mean_phase_b \
cgm_test_pitch_form_mean_phase_c \
cgm_test_pitch_form_mean_amplitude_a \
cgm_test_pitch_form_mean_offset_a \
cgm_test_pitch_form_range_period_a \
cgm_test_pitch_form_range_period_b \
cgm_test_pitch_form_range_period_c \
cgm_test_pitch_form_range_phase_a \
cgm_test_pitch_form_range_phase_b \
cgm_test_pitch_form_range_phase_c \
cgm_test_pitch_form_range_amplitude_a \
cgm_test_pitch_form_range_offset_a \
cgm_test_rhythm_form_mean_period_a \
cgm_test_rhythm_form_mean_period_b \
cgm_test_rhythm_form_mean_period_c \
cgm_test_rhythm_form_mean_phase_a \
cgm_test_rhythm_form_mean_phase_b \
cgm_test_rhythm_form_mean_phase_c \
cgm_test_rhythm_form_mean_amplitude_a \
cgm_test_rhythm_form_mean_offset_a \
cgm_test_rhythm_form_range_period_a \
cgm_test_rhythm_form_range_period_b \
cgm_test_rhythm_form_range_period_c \
cgm_test_rhythm_form_range_phase_a \
cgm_test_rhythm_form_range_phase_b \
cgm_test_rhythm_form_range_phase_c \
cgm_test_rhythm_form_range_amplitude_a \
cgm_test_rhythm_form_range_offset_a \
cgm_test_dynamic_form_mean_period_a \
cgm_test_dynamic_form_mean_period_b \
cgm_test_dynamic_form_mean_period_c \
cgm_test_dynamic_form_mean_phase_a \
cgm_test_dynamic_form_mean_phase_b \
cgm_test_dynamic_form_mean_phase_c \
cgm_test_dynamic_form_mean_amplitude_a \
cgm_test_dynamic_form_mean_offset_a \
cgm_test_dynamic_form_range_period_a \
cgm_test_dynamic_form_range_period_b \
cgm_test_dynamic_form_range_period_c \
cgm_test_dynamic_form_range_phase_a \
cgm_test_dynamic_form_range_phase_b \
cgm_test_dynamic_form_range_phase_c \
cgm_test_dynamic_form_range_amplitude_a \
cgm_test_dynamic_form_range_offset_a \
cgm_test_texture_form_period_a \
cgm_test_texture_form_period_b \
cgm_test_texture_form_period_c \
cgm_test_texture_form_phase_a \
cgm_test_texture_form_phase_b \
cgm_test_texture_form_phase_c \
cgm_test_texture_form_amplitude \
cgm_test_texture_form_offset \
cgm_test_voices_low_pitch \
cgm_test_voices_high_pitch \
cgm_test_voices_channel \
cgm_test_voices_walking_a \
cgm_test_voices_walking_b \
cgm_test_voices_walking_c \
cgm_test_voices_program_a \
cgm_test_voices_program_b \
cgm_test_voices_program_c \
cgm_test_voices_program_d \
cgm_test_voices_pan_a \
cgm_test_voices_pan_b \
cgm_test_voices_pan_c \
cgm_test_voices_follower_follow_a \
cgm_test_voices_follower_follow_b \
cgm_test_voices_follower_leader_c \
cgm_test_voices_follower_interval_type_a \
cgm_test_voices_follower_interval_type_b \
cgm_test_voices_follower_interval_a \
cgm_test_voices_follower_interval_b \
cgm_test_voices_follower_delay_a \
cgm_test_voices_follower_delay_b \
cgm_test_voices_follower_delay_c \
cgm_test_voices_follower_duration_factor_a \
cgm_test_voices_follower_duration_factor_b \
cgm_test_voices_follower_duration_factor_c \
cgm_test_voices_follower_inversion_a \
cgm_test_voices_follower_inversion_b \
cgm_test_voices_follower_retrograde_a \
cgm_test_voices_follower_retrograde_b \
cgm_test_voices_random_program_probability_a \
cgm_test_voices_random_program_probability_b \
cgm_test_voices_random_program_probability_c \
cgm_test_voices_random_program_ensemble_a \
cgm_test_voices_random_program_ensemble_b \
cgm_test_voices_random_program_ensemble_c \
cgm_test_arrangement_definition_algorithm_a \
cgm_test_arrangement_definition_algorithm_b \
cgm_test_arrangement_definition_period_a \
cgm_test_arrangement_definition_period_b \



forms:=$(names:%=%.xml)
txts:=$(names:%=%.txt)
mids:=$(names:%=%.mid)

.PHONY:all
all:$(mids) $(txts)

%.mid:%.txt
	$(TEXTMIDI) --textmidi $< --midi $@
%.txt:%.xml
	$(TEXTMIDICGM) --xmlform $< --textmidi $@

.PHONY: clean
clean:
	$(RM) $(txts) $(mids)
