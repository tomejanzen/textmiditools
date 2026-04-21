#!/usr/bin/env python3
"""TextMIDITools: TextMidiFormEdit.py Voice Window for editing a voice's attributes."""
# TextMIDITools Version 1.1.2
# TextMidiFormEdit.py 1.0
# Copyright © 2025 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#

import re
import tkinter
import tkinter.ttk

from tkinter import *
from tkinter import messagebox
from tkinter.ttk import *
from fractions import Fraction

from GeneralMidi import *

class VoiceWindow(tkinter.Toplevel):
    general_midi_list = list(GENERAL_MIDI_INSTRUMENT_DICT)
    def __init__(self, xmlvoices):
        """Init the voice window."""
        super().__init__()
        self.frame = tkinter.ttk.Frame(self, padding='1 1 1 1', borderwidth=4, relief='sunken')
        self.resizable(False, False)
        self.frame.grid()
        self.xml_voices = xmlvoices
        self.number_of_voices = tkinter.IntVar()
        self.voice_number = tkinter.IntVar()
        self.low_pitch = tkinter.StringVar()
        self.high_pitch = tkinter.StringVar()
        self.channel = tkinter.IntVar()
        self.program = tkinter.StringVar()
        #self.walking = tkinter.IntVar()
        self.walking = tkinter.DoubleVar(value=0.0)
        # no variable for pan
        self.follow = tkinter.BooleanVar()
        self.follow.set(False)
        self.leader = tkinter.StringVar()
        self.leader.set(0)
        self.interval_type = tkinter.StringVar()
        self.interval_type.set(1)
        self.follow_interval = tkinter.StringVar()
        self.follow_delay = tkinter.StringVar()
        self.follow_duration_factor = tkinter.StringVar()
        self.inversion = tkinter.BooleanVar()
        self.retrograde = tkinter.BooleanVar()
        self.random_ensemble_list = []
        self.random_ensemble = tkinter.StringVar()

        self.voice_number.set(1)
        self.create_widgets()
        self.title('Instrumental Voices')

    def get_xml_voices(self):
        return self.xml_voices

    def coerce_delta(self, setting, delta):
        """Coerce delta to be positive."""
        if setting <= 0.0 and delta < 0.0:
            delta = 0.0
        if setting >= 1.0 and delta > 0.0:
            delta = 0.0
        return delta

    def create_widgets(self):
        """Create the widgets for the voice window."""

        self.number_of_voices_label = tkinter.ttk.Label(self.frame, text='Number of Voices')
        self.number_of_voices_spinbox = tkinter.ttk.Spinbox(self.frame,
              textvariable=self.number_of_voices, state='readonly', wrap=False)
        self.number_of_voices_spinbox['increment'] = 1
        self.number_of_voices_spinbox['from']      = 1
        self.number_of_voices_spinbox.set(len(self.xml_voices))
        self.number_of_voices_spinbox['to'] = 128

        self.voice_number_label = tkinter.ttk.Label(self.frame, text='Voice')
        self.voice_number_spinbox = tkinter.ttk.Spinbox(self.frame, wrap=False,
              textvariable=self.voice_number)
        self.voice_number_spinbox['increment'] = 1.0
        self.voice_number_spinbox['from'] = 0.0
        self.voice_number_spinbox['to'] = len(self.xml_voices) - 1
        vox = 0
        self.voice_number_spinbox.set(vox)
        self.voice_number_spinbox['state'] = 'readonly'

        self.low_pitch_label = tkinter.ttk.Label(self.frame, text='Low Pitch')
        # TCL book p 509
        # %d type 1 insert, 0 del, -1 other
        # %i index of string to change or -1
        # %P value of entry if edit is allowed
        # %s val of entry prior to editing
        # %S the text string being inserted or deleted
        # %v type of validation currently set
        # %V type of validation that triggered (key, focusin, focusout, forced)
        # %W tk name of widget
        validate_command = (self.register(self.validate_pitchname),
            '%d', '%i', '%P', '%s', '%S', '%v', '%V', '%W')
        self.low_pitch_entry = tkinter.ttk.Entry(self.frame,
            textvariable=self.low_pitch, validatecommand=validate_command,
            validate='focusout')
        self.low_pitch.set(self.xml_voices[vox]['low_pitch'])
        self.low_pitch_from_program_button = tkinter.ttk.Button(self.frame, text='From Program',
            command=self.low_pitch_from_program_callback)

        self.high_pitch_label = tkinter.ttk.Label(self.frame, text='High Pitch')
        self.high_pitch_entry = tkinter.ttk.Entry(self.frame, textvariable=self.high_pitch,
              validatecommand=validate_command, validate='focusout')
        self.high_pitch.set(self.xml_voices[vox]['high_pitch'])
        self.high_pitch_from_program_button = tkinter.ttk.Button(self.frame, text='From Program',
            command=self.high_pitch_from_program_callback)

        self.channel_label = tkinter.ttk.Label(self.frame, text='Channel')
        self.channel_spinbox = tkinter.ttk.Spinbox(self.frame, textvariable=self.channel,
              wrap=False)
        self.channel_spinbox['increment'] = 1.0
        self.channel_spinbox['from'] = 1.0
        self.channel_spinbox['to'] = 16.0
        self.channel_spinbox.set(self.xml_voices[vox]['channel'])
        self.channel_spinbox['state'] = 'readonly'

        self.program_label = tkinter.ttk.Label(self.frame, text='Program')
        self.program_spinbox = tkinter.ttk.Spinbox(self.frame,
            textvariable=self.program, values=self.general_midi_list,
            state='readonly', wrap=True)
        self.program_spinbox.set(self.general_midi_list[self.xml_voices[vox]['program']])
        self.program_spinbox['state'] = 'readonly'

        self.walking_scroll_label = tkinter.ttk.Label(self.frame,
            text='Random.............................Walking')
        self.walking_label = tkinter.ttk.Label(self.frame, text='Walking')
        self.walking_scrollbar = tkinter.ttk.Scrollbar(self.frame,
            command=self.walking_callback)
        self.walking_scrollbar['orient'] = 'horizontal'
        self.walking_scrollbar.bind('<B1-Motion>', self.walking_callback)
        tempwalking = self.xml_voices[vox]['walking']
        self.walking_scrollbar.set(tempwalking, tempwalking)

        self.pan_scroll_label = tkinter.ttk.Label(self.frame,
            text='    L..................C..................R')
        self.pan_label = tkinter.ttk.Label(self.frame, text='Pan')
        self.pan_scrollbar = tkinter.ttk.Scrollbar(self.frame, command=self.pan_callback)
        self.pan_scrollbar['orient'] = 'horizontal'
        temppan = self.xml_voices[vox]['pan'] / 128 + 0.5
        self.pan_scrollbar.set(temppan, temppan)

        self.follow_frame = tkinter.ttk.Frame(self.frame, padding='1 1 1 1',
                borderwidth=4, relief='sunken')
        self.follow_checkbutton = tkinter.ttk.Checkbutton(self.follow_frame,
            text='follow', variable=self.follow, command=self.follow_callback)
        self.follow.set(self.xml_voices[vox]['follower']['follow'])

        self.leader_label = tkinter.ttk.Label(self.follow_frame, text='Leader')
        self.leader_spinbox = tkinter.ttk.Spinbox(self.follow_frame,
            textvariable=self.leader, wrap=True)
        self.leader_spinbox['increment'] = 1.0
        self.leader_spinbox['from'] = 0.0
        self.leader_spinbox['to'] = self.number_of_voices_spinbox.get()
        if self.xml_voices[vox]['follower']['follow'] is True:
            self.leader_spinbox['state'] = 'normal'
        else:
            self.leader_spinbox['state'] = 'disabled'
        self.leader_spinbox.set(
            self.xml_voices[vox]['follower']['leader'])

        # relief can be flat (nothing), raised, sunken, ridge, groove, solid (line) p. 615
        self.interval_frame = tkinter.ttk.Frame(self.follow_frame, padding='1 1 1 1',
                borderwidth=4, relief='sunken')

        self.scalar_interval_type_label = tkinter.ttk.Label(self.interval_frame,
                text='Interval Type')
        self.interval_type.set(
            self.xml_voices[vox]['follower']['interval_type'])
        self.scalar_interval_type_radiobutton = tkinter.ttk.Radiobutton(self.interval_frame,
            text='Scaler', variable=self.interval_type, value='1')

        self.chromatic_interval_type_radiobutton = tkinter.ttk.Radiobutton(self.interval_frame,
            text='Chromatic', variable=self.interval_type, value='2')
        if self.xml_voices[vox]['follower']['follow'] is True:
            self.scalar_interval_type_radiobutton['state'] = 'normal'
            self.chromatic_interval_type_radiobutton['state'] = 'normal'
        else:
            self.scalar_interval_type_radiobutton['state'] = 'disabled'
            self.chromatic_interval_type_radiobutton['state'] = 'disabled'
        self.chromatic_interval_type_radiobutton['state'] = 'disabled'
        self.scalar_interval_type_radiobutton.update()

        self.follow_interval_label = tkinter.ttk.Label(self.interval_frame, text='Interval')
        self.follow_interval_spinbox = tkinter.ttk.Spinbox(self.interval_frame,
              textvariable=self.follow_interval, wrap=True)
        self.follow_interval_spinbox['increment'] = 1.0
        self.follow_interval_spinbox['from'] = -24.0
        self.follow_interval_spinbox['to'] = 24.0
        self.follow_interval_spinbox['state'] = 'disabled'
        self.follow_interval_spinbox.bind('<ButtonRelease-1>',
            self.follow_interval_callback)
        self.follow_interval_spinbox.set(
            self.xml_voices[vox]['follower']['interval'])

        self.follow_delay_label = tkinter.ttk.Label(self.follow_frame, text='Delay')
        self.follow_delay_entry = tkinter.ttk.Entry(self.follow_frame,
            textvariable=self.follow_delay)
        self.follow_delay.set(str(Fraction(int(self.xml_voices[vox]['follower']['delay']['numerator']),
                                           int(self.xml_voices[vox]['follower']['delay']['denominator']))))
        self.follow_delay_entry['state'] = 'disabled'

        self.follow_duration_factor_label = tkinter.ttk.Label(self.follow_frame,
                text='Duration Factor')
        self.follow_duration_factor_entry = tkinter.ttk.Entry(self.follow_frame,
              textvariable=self.follow_duration_factor)
        duration_factor_ratio = Fraction(int(self.xml_voices[vox]['follower']['duration_factor']['numerator']),
                                         int(self.xml_voices[vox]['follower']['duration_factor']['denominator']))
        self.follow_duration_factor.set(str(duration_factor_ratio))
        self.follow_duration_factor_entry['state'] = 'disabled'

        self.inversion.set(
            self.xml_voices[vox]['follower']['inversion'])
        self.inversion_checkbutton = tkinter.ttk.Checkbutton(self.follow_frame, text='inversion',
              variable=self.inversion)
        self.inversion_checkbutton['state'] = 'disabled'

        self.retrograde.set(
            self.xml_voices[vox]['follower']['retrograde'])
        self.retrograde_checkbutton = tkinter.ttk.Checkbutton(self.follow_frame, text='retrograde',
              variable=self.retrograde)
        self.retrograde_checkbutton['state'] = 'disabled'

        self.random_program_frame = tkinter.ttk.Frame(self.frame, padding='1 1 1 1',
                borderwidth=4, relief='sunken')
        self.random_probability_scroll_label = tkinter.ttk.Label(self.random_program_frame,
            text='Static.............................Random')
        self.random_probability_label = tkinter.ttk.Label(self.random_program_frame, text='Random Prog Prob')
        self.random_probability_scrollbar = tkinter.ttk.Scrollbar(self.random_program_frame,
            command=self.random_probability_callback)
        self.random_probability_scrollbar['orient'] = 'horizontal'
#        self.random_probability_scrollbar.bind('<B1-Motion>', self.random_probability_callback)
        temprandom_probability = self.xml_voices[vox]['random_program']['probability']
        self.random_probability_scrollbar.set(temprandom_probability, temprandom_probability)

        self.random_ensemble_list.clear()
        if self.xml_voices[vox]['random_program']['ensemble'] == []:
           self.random_ensemble_list.append('empty')
           self.random_ensemble.set("empty") 
        else:
            for prog in self.xml_voices[vox]['random_program']['ensemble']:
                self.random_ensemble_list.append(self.general_midi_list[int(prog)])
        self.random_ensemble_label = tkinter.ttk.Label(self.random_program_frame, text='Random Ensemble')
        self.random_ensemble_spinbox = tkinter.ttk.Spinbox(self.random_program_frame,
            textvariable=self.random_ensemble, values=self.random_ensemble_list, state='readonly', wrap=False)
        self.random_ensemble_spinbox['state'] = 'readonly'

        self.random_program_add_button = tkinter.ttk.Button(self.random_program_frame, text='Add',
            command=self.random_program_add_callback)

        self.random_program_delete_button = tkinter.ttk.Button(self.random_program_frame, text='Delete',
            command=self.random_program_delete_callback)

        the_row = 0
        self.number_of_voices_label.grid(row=the_row, column=0, sticky=NSEW,
            padx=2, pady=2)
        self.number_of_voices_spinbox.grid(row=the_row, column=1, sticky=NSEW,
            padx=2, pady=2)
        the_row = the_row + 1
        self.voice_number_label.grid(row=the_row, column=0, sticky=NSEW,
            padx=2, pady=2)
        self.voice_number_spinbox.grid(row=the_row, column=1, sticky=NSEW,
            padx=2, pady=2)
        the_row = the_row + 1

        self.channel_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.channel_spinbox.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.program_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.program_spinbox.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1

        self.columnconfigure(index=the_row, weight=1)
        self.rowconfigure(index=the_row, weight=1)
        self.low_pitch_label.grid(row=the_row, column=0, sticky=NSEW)
        self.low_pitch_entry.grid(row=the_row, column=1, sticky=NSEW)
        self.low_pitch_from_program_button.grid(
            row=the_row, column=2, stick=NSEW)
        the_row = the_row + 1
        self.high_pitch_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.high_pitch_entry.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.high_pitch_from_program_button.grid(
            row=the_row, column=2, stick=NSEW)
        the_row = the_row + 1

        the_row = the_row + 1
        self.walking_scroll_label.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.walking_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.walking_scrollbar.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)

        the_row = the_row + 1
        self.pan_scroll_label.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.pan_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.pan_scrollbar.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.follow_frame.grid(row=the_row, column=0, sticky=NSEW, padx = 0, pady=0, columnspan=3)
        follow_row = 0
        self.follow_checkbutton.grid(
            row=follow_row, column=0, sticky=NSEW, padx=2, pady=2)
        follow_row = follow_row + 1
        self.leader_label.grid(
            row=follow_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.leader_spinbox.grid(
            row=follow_row, column=1, sticky=NSEW, padx=2, pady=2)
        follow_row = follow_row + 1
        self.interval_frame.grid(row=follow_row, column=0, sticky=NSEW, padx=2, pady=2,
                columnspan=3)
        self.scalar_interval_type_label.grid(
            row=0, column=0, sticky=NSEW, padx=2, pady=2)
        self.scalar_interval_type_radiobutton.grid(
            row=0, column=1, sticky=NSEW, padx=2, pady=2)
        self.chromatic_interval_type_radiobutton.grid(
            row=0, column=2, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.follow_interval_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.follow_interval_spinbox.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.follow_delay_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.follow_delay_entry.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.follow_duration_factor_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.follow_duration_factor_entry.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.inversion_checkbutton.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.retrograde_checkbutton.grid(
            row=the_row, column=2, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        the_row = the_row + 1
        self.random_program_frame.grid(row=the_row, column=0, sticky=NSEW, padx = 0, pady=0, columnspan=3)
        self.random_probability_scroll_label.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.random_probability_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)

        self.random_probability_scrollbar.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.random_ensemble_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.random_ensemble_spinbox.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.random_program_add_button.grid(sticky=NSEW, row=the_row, column=2)
        self.random_program_delete_button.grid(sticky=NSEW, row=the_row, column=3)

        self.number_of_voices.trace_add("write", self.number_of_voices_callback)
        self.voice_number.trace_add("write", self.voice_number_callback)
        self.low_pitch.trace_add("write", self.low_pitch_callback)
        self.high_pitch.trace_add("write", self.high_pitch_callback)
        self.channel.trace_add("write", self.channel_callback)
        self.program.trace_add("write", self.program_callback)
        self.walking.trace_add("write", self.walking_callback)
        self.follow.trace_add("write", self.follow_callback)
        self.leader.trace_add("write", self.leader_callback)
        self.interval_type.trace_add("write", self.interval_type_callback)
        self.follow_interval.trace_add("write", self.follow_interval_callback)
        self.follow_delay.trace_add("write", self.follow_delay_callback)
        self.follow_duration_factor.trace_add("write",
            self.follow_duration_factor_callback)
        self.inversion.trace_add("write", self.inversion_callback)
        self.retrograde.trace_add("write", self.retrograde_callback)
        self.random_ensemble.trace_add("write", self.random_probability_callback)

    def low_pitch_callback(self, event, *args):
        """Low Pitch was set; save to the internal form."""
        vox = int(self.voice_number.get())
        self.xml_voices[vox]['low_pitch'] = self.low_pitch.get()

    def low_pitch_from_program_callback(self, event=None, *args):
        """From MIDI Program number (for a typical instrument range) was pushed;
        copy the low pitch into the internal form.
        """
        vox = int(self.voice_number.get())
        prog_index = self.program.get()
        self.xml_voices[vox]['low_pitch'] = (GENERAL_MIDI_INSTRUMENT_DICT
                [prog_index][GM_LOW_PITCH_INDEX])
        self.low_pitch.set(self.xml_voices[vox]['low_pitch'])

    def high_pitch_callback(self, event, *args):
        """High Pitch was set; save to the internal form."""
        vox = int(self.voice_number.get())
        self.xml_voices[vox]['high_pitch'] = self.high_pitch.get()

    def high_pitch_from_program_callback(self, event=None, *args):
        """From MIDI Program number (for a typical instrument range) was pushed;
        copy the high pitch into the internal form.
        """
        vox = int(self.voice_number.get())
        prog_index = self.program.get()
        self.xml_voices[vox]['high_pitch'] = (GENERAL_MIDI_INSTRUMENT_DICT
                [prog_index][GM_HIGH_PITCH_INDEX])
        self.high_pitch.set(self.xml_voices[vox]['high_pitch'])

    def interval_type_callback(self, *args):
        """Interval type for the follower (canon); set the internal form."""
        vox = int(self.voice_number.get())
        self.xml_voices[vox]['follower']['interval_type'] = int(self.interval_type.get())

    def follow_interval_callback(self, event=None, *args):
        """Interval (number of steps) for the follower (canon); set the internal form."""
        vox = int(self.voice_number.get())
        self.xml_voices[vox]['follower']['interval'] = int(self.follow_interval.get())

    def follow_delay_callback(self, event=None, *args):
        """Follow delay; set the internal form."""
        vox = int(self.voice_number.get())
        try:
            delay_quotient = Fraction(self.follow_delay.get())
        except ValueError:
            return
        self.xml_voices[vox]['follower']['delay']['numerator']   = delay_quotient.numerator
        self.xml_voices[vox]['follower']['delay']['denominator'] = delay_quotient.denominator

    def follow_duration_factor_callback(self, event=None, *args):
        """Follower Duration multiplier was set; copy to internal form."""
        vox = int(self.voice_number.get())
        try:
            duration_factor_quotient = Fraction(self.follow_duration_factor.get())
        except ValueError:
            return
        self.xml_voices[vox]['follower']['duration_factor']['numerator']   = duration_factor_quotient.numerator
        self.xml_voices[vox]['follower']['duration_factor']['denominator'] = duration_factor_quotient.denominator

    def inversion_callback(self, *args):
        """Invert button for follower; copy state to internal form."""
        vox = int(self.voice_number.get())
        self.xml_voices[vox]['follower']['inversion'] = self.inversion.get()

    def retrograde_callback(self, *args):
        """Retrograde for follower; copy state to internal form."""
        vox = int(self.voice_number.get())
        self.xml_voices[vox]['follower']['retrograde'] = self.retrograde.get()

    def number_of_voices_callback(self, event=None, *args):
        """Number of voices spin button set; use value to set voice container."""
        vox = int(self.voice_number.get())
        if vox >= int(self.number_of_voices.get()):
            vox = int(self.number_of_voices.get()) - 1

        if len(self.xml_voices) > int(self.number_of_voices.get()):
            self.xml_voices = (self.xml_voices
                    [0:(int(self.number_of_voices.get()))])
        if len(self.xml_voices) < int(self.number_of_voices.get()):
            # default the voices above the voices we're using
            for voice in range(len(self.xml_voices), int(self.number_of_voices.get())):
                voice_dict = {}
                voice_dict['low_pitch'] = 'A0'
                voice_dict['high_pitch'] = 'C8'
                voice_dict['channel'] = 1
                voice_dict['walking'] = 0.0
                voice_dict['program'] = 1
                voice_dict['pan'] = 0
                follow_dict = {}
                follow_dict['follow'] = False
                follow_dict['leader'] = 0
                follow_dict['interval_type'] = 1
                follow_dict['interval'] = 0
                delay_dict = {}
                delay_dict['numerator'] = 0
                delay_dict['denominator'] = 0
                follow_dict['delay'] = delay_dict
                duration_dict = {}
                duration_dict['numerator'] = 1
                duration_dict['denominator'] = 1
                follow_dict['duration_factor'] = duration_dict
                follow_dict['inversion'] = False
                follow_dict['retrograde'] = False
                voice_dict['follower'] = follow_dict
                random_program_dict = {}
                random_program_dict['probability'] = 0.0
                random_program_dict['ensemble'] = []
                voice_dict['random_program'] = random_program_dict
                self.xml_voices.append(voice_dict)
        self.voice_number_spinbox['to'] = len(self.xml_voices) - 1

    def voice_number_callback(self, event=None, *args):
        """Voice number for the currently edited voice set."""
        self.update_idletasks()
        vox = int(self.voice_number.get())
        instrument_name = self.general_midi_list[self.xml_voices[vox]['program']]
        self.low_pitch.set(self.xml_voices[vox]['low_pitch'])
        self.high_pitch.set(self.xml_voices[vox]['high_pitch'])
        chan = self.xml_voices[vox]['channel']
        self.channel.set(chan)
        tempwalking = self.xml_voices[vox]['walking']
        self.walking_scrollbar.set(tempwalking, tempwalking)
        self.program_spinbox['state'] = '!readonly'
        self.program.set(instrument_name)
        self.program_spinbox['state'] = 'readonly'

        the_pan = float(self.xml_voices[vox]['pan']) / 128.0 + 0.5
        self.pan_scrollbar.set(the_pan, the_pan)
        self.pan_scrollbar.update()
        self.follow.set(self.xml_voices[vox]['follower']['follow'])

        self.interval_type.set(
            self.xml_voices[vox]['follower']['interval_type'])
        self.leader_spinbox.set(
        self.xml_voices[vox]['follower']['leader'])
        self.follow_interval.set(
            self.xml_voices[vox]['follower']['interval'])
        self.interval_type.set(
            self.xml_voices[vox]['follower']['interval_type'])
        self.follow_interval.set(
            self.xml_voices[vox]['follower']['interval'])
        delay_ratio = Fraction(int(self.xml_voices[vox]['follower']['delay']['numerator']),
            int(self.xml_voices[vox]['follower']['delay']['denominator']))
        self.follow_delay.set(str(delay_ratio))

        duration_factor_ratio = Fraction(int(self.xml_voices[vox]['follower']['duration_factor']['numerator']),
                                         int(self.xml_voices[vox]['follower']['duration_factor']['denominator']))
        self.follow_duration_factor.set(str(duration_factor_ratio))

        self.inversion.set(self.xml_voices[vox]['follower']['inversion'])
        self.retrograde.set(self.xml_voices[vox]['follower']['retrograde'])
        if int(self.follow.get()) == 1:
            self.leader_spinbox['state'] = 'normal'
            self.scalar_interval_type_radiobutton['state'] = 'normal'
            self.chromatic_interval_type_radiobutton['state'] = 'normal'
            self.follow_interval_spinbox['state'] = 'normal'
            self.follow_delay_entry['state'] = 'normal'
            self.follow_duration_factor_entry['state'] = 'normal'
            self.inversion_checkbutton['state'] = 'normal'
            self.retrograde_checkbutton['state'] = 'normal'
            self.scalar_interval_type_radiobutton.update()
            self.chromatic_interval_type_radiobutton.update()
            self.follow_interval_spinbox.update()
            self.follow_delay_entry.update()
            self.follow_duration_factor_entry.update()
            self.inversion_checkbutton.update()
            self.retrograde_checkbutton.update()
        else:
            self.leader_spinbox['state'] = 'disabled'
            self.scalar_interval_type_radiobutton['state'] = 'disabled'
            self.chromatic_interval_type_radiobutton['state'] = 'disabled'
            self.follow_delay_entry['state'] = 'disabled'
            self.follow_duration_factor_entry['state'] = 'disabled'
            self.follow_interval_spinbox['state'] = 'disabled'
            self.inversion_checkbutton['state'] = 'disabled'
            self.retrograde_checkbutton['state'] = 'disabled'

            self.scalar_interval_type_radiobutton.update()
            self.chromatic_interval_type_radiobutton.update()
            self.follow_interval_spinbox.update()
            self.follow_delay_entry.update()
            self.follow_duration_factor_entry.update()
            self.inversion_checkbutton.update()
            self.retrograde_checkbutton.update()

        self.random_probability_scrollbar.set(self.xml_voices[vox]['random_program']['probability'], 
            self.xml_voices[vox]['random_program']['probability'])
        self.random_probability_scrollbar.update()
        self.random_ensemble_list.clear()
        if self.xml_voices[vox]['random_program']['ensemble'] == []:
            self.random_ensemble_list.append('empty')
            self.random_ensemble.set("empty") 
        else:
            for prog in self.xml_voices[vox]['random_program']['ensemble']:
                self.random_ensemble_list.append(self.general_midi_list[int(prog)])
            self.random_ensemble.set(self.random_ensemble_list[0])
        self.random_ensemble_spinbox.update()

    def leader_callback(self, event=None, *args):
        """Leader of a follower set; set in internal form."""
        self.xml_voices[self.voice_number.get()]['follower']['leader'] = self.leader.get()

    def channel_callback(self, event=None, *args):
        """MIDI channel (from 1) set; copy to internal form."""
        self.xml_voices[self.voice_number.get()]['channel'] = self.channel.get()

    def program_callback(self, event, *args):
        """MIDI Program set; copy to internal form."""
        prog_index = self.program.get()
        vox = int(self.voice_number.get())
        self.xml_voices[vox]['program'] = (GENERAL_MIDI_INSTRUMENT_DICT
                [prog_index][GM_PROGRAM_INDEX])

    def random_program_add_callback(self):
        """User pressed Add.  Copy the program from the voice program.  Update spinbox->to. """
        vox = int(self.voice_number.get()) # the index into general_midi_list
        add_program_name = self.program_spinbox.get()
        if self.random_ensemble_list[0] == 'empty':
            self.random_ensemble_list.clear()
        self.random_ensemble_list.append(add_program_name)
        self.random_ensemble_spinbox['values'] = self.random_ensemble_list
        self.random_ensemble_spinbox.set(self.random_ensemble_list[-1])
        self.random_ensemble_spinbox.update()
        self.xml_voices[vox]['random_program']['ensemble'].append(GENERAL_MIDI_INSTRUMENT_DICT[add_program_name][0])

    def random_program_delete_callback(self):
        """User pressed Delete.  Delete the displayed program from the program list.  Update spinbox->to. """
        program_string = self.random_ensemble_spinbox.get()
        if self.random_ensemble_list != []:
            self.random_ensemble_list.remove(program_string)
            if self.random_ensemble_list == []:
                self.random_ensemble_list.append('empty')
                self.random_ensemble.set('empty')
            self.random_ensemble_spinbox['values'] = self.random_ensemble_list
            self.random_ensemble_spinbox.set(self.random_ensemble_list[-1])
            self.random_ensemble_spinbox.update()
            vox = int(self.voice_number.get())
            self.xml_voices[vox]['random_program']['ensemble'].clear()
            if self.random_ensemble_list != []:
                for progstr in self.random_ensemble_list:
                    self.xml_voices[vox]['random_program']['ensemble'].append(GENERAL_MIDI_INSTRUMENT_DICT[progstr][0])
        self.random_ensemble_spinbox['values'] = self.random_ensemble_list 

    def install_xml_voices(self, xml_voices):
        """Install XML form."""
        self.xml_voices = xml_voices
        vox = 0
        chan = self.xml_voices[vox]['channel']
        self.number_of_voices.set(len(self.xml_voices))
        self.number_of_voices_spinbox['state'] = 'readonly'
        self.voice_number_spinbox['from'] = 0
        self.voice_number_spinbox['to'] = len(self.xml_voices) - 1
        self.voice_number_spinbox.set(vox)
        self.voice_number_spinbox['state'] = 'readonly'

        self.channel.set(chan)
        self.program.set(self.general_midi_list[self.xml_voices[vox]['program']])
        self.low_pitch.set(self.xml_voices[vox]['low_pitch'])
        self.high_pitch.set(self.xml_voices[vox]['high_pitch'])

        self.walking.set(self.xml_voices[vox]['walking'])

        the_pan = (float(self.xml_voices[0]['pan']) / 128) + 0.5
        self.pan_scrollbar.set(the_pan, the_pan)
        self.pan_scrollbar.update()

        self.follow.set(self.xml_voices[vox]['follower']['follow'])

        self.leader.set(self.xml_voices[vox]['follower']['leader'])
        self.interval_type.set(
            self.xml_voices[vox]['follower']['interval_type'])

        self.follow_interval.set(
            self.xml_voices[vox]['follower']['interval'])

        self.inversion.set(
            self.xml_voices[vox]['follower']['inversion'])

        self.retrograde.set(
            self.xml_voices[vox]['follower']['retrograde'])

        self.random_probability_scrollbar.set(self.xml_voices[vox]['random_program']['probability'], 
            self.xml_voices[vox]['random_program']['probability'])
        self.random_probability_scrollbar.update()

        self.random_ensemble_list.clear()
        if self.xml_voices[vox]['random_program']['ensemble'] == []:
            self.random_ensemble_list.append('empty')
            self.random_ensemble.set('empty')
        else:
            for prog in self.xml_voices[vox]['random_program']['ensemble']:
                self.random_ensemble_list.append(self.general_midi_list[int(prog)])
                self.random_ensemble.set(self.random_ensemble_list[0])
        self.random_ensemble_spinbox['values'] = self.random_ensemble_list
        self.random_ensemble_spinbox.set(self.random_ensemble_list[0])
        self.random_ensemble_spinbox.update()
        self.random_program_frame.update()
        self.frame.update()

    def follow_callback(self, *args):
        """Follow checkbutton pressed."""
        vox = int(self.voice_number.get())
        follow = self.follow.get()
        self.xml_voices[vox]['follower']['follow'] = follow
        if follow is True:
            self.leader_spinbox['state'] = 'normal'
            self.scalar_interval_type_radiobutton['state'] = 'normal'
            self.chromatic_interval_type_radiobutton['state'] = 'normal'
            self.follow_interval_spinbox['state'] = 'normal'
            self.follow_delay_entry['state'] = 'normal'
            self.follow_duration_factor_entry['state'] = 'normal'
            self.inversion_checkbutton['state'] = 'normal'
            self.retrograde_checkbutton['state'] = 'normal'
        else:
            self.leader_spinbox['state'] = 'disabled'
            self.scalar_interval_type_radiobutton['state'] = 'disabled'
            self.chromatic_interval_type_radiobutton['state'] = 'disabled'
            self.follow_interval_spinbox['state'] = 'disabled'
            self.follow_delay_entry['state'] = 'disabled'
            self.follow_duration_factor_entry['state'] = 'disabled'
            self.inversion_checkbutton['state'] = 'disabled'
            self.retrograde_checkbutton['state'] = 'disabled'

    def walking_callback(self, action, sign = 0, unitspage='None'):
        """Walking scrollbar for a follower was pressed."""
        vox = int(self.voice_number.get())
        firstlastlist = self.walking_scrollbar.get()
        setting = 0.0
        if action == 'scroll':
            if unitspage == 'pages':
                delta = float(sign) / 6.0
                setting = firstlastlist[1]
                self.coerce_delta(setting, delta)
                setting = setting + delta
                self.walking_scrollbar.set(setting, setting)
                value = setting
                self.xml_voices[vox]['walking'] = value
            else:
                if unitspage == 'units':
                    delta = float(sign) / 64.0
                    setting = firstlastlist[1]
                    self.coerce_delta(setting, delta)
                    setting = setting + delta
                    self.walking_scrollbar.set(setting, setting)
                    value = setting
                    self.xml_voices[vox]['walking'] = value
        else:
            if action == 'moveto':
                setting = float(sign)
                setting = max(setting, 0.0)
                setting = min(setting, 1.0)
                self.walking_scrollbar.set(setting, setting)
                value = setting
                self.xml_voices[vox]['walking'] = value
            else:
                if action == 'goto':
                    setting = firstlastlist[0]
                    self.walking_scrollbar.set(firstlastlist[0])
                    value = setting
                    self.xml_voices[vox]['walking'] = value

    def random_probability_callback(self, action, sign = 0, unitspage='None'):
        """Random program probability scrollbar callback."""
        vox = int(self.voice_number.get())
        firstlastlist = self.random_probability_scrollbar.get()
        setting = 0.0
        if action == 'scroll':
            if unitspage == 'pages':
                delta = float(sign) / 6.0
                setting = firstlastlist[1]
                self.coerce_delta(setting, delta)
                setting = setting + delta
                self.random_probability_scrollbar.set(setting, setting)
                self.xml_voices[vox]['random_program']['probability'] = setting
            else:
                if unitspage == 'units':
                    delta = float(sign) / 64.0
                    setting = firstlastlist[1]
                    self.coerce_delta(setting, delta)
                    setting = setting + delta
                    self.random_probability_scrollbar.set(setting, setting)
                    self.xml_voices[vox]['random_program']['probability'] = setting
        else:
            if action == 'moveto':
                setting = float(sign)
                setting = max(setting, 0.0)
                setting = min(setting, 1.0)
                self.random_probability_scrollbar.set(setting, setting)
            else:
                if action == 'goto':
                    setting = firstlastlist[0]
                    self.random_probability_scrollbar.set(firstlastlist[0])
                    setting = min(1.0, setting)
                    setting = max(0.0, setting)
                    self.xml_voices[vox]['random_program']['probability'] = setting

    def pan_callback(self, action, sign, unitspage='None'):
        """Pan scrollbar moved; save value to internal form."""
        vox = int(self.voice_number.get())
        firstlastlist = self.pan_scrollbar.get()
        setting = 0
        if action == 'scroll':
            if unitspage == 'pages':
                delta = float(sign) * 1.0 / 4
                setting = firstlastlist[1]
                delta = self.coerce_delta(setting, delta)
                setting = setting + delta
                self.pan_scrollbar.set(setting, setting)
                value = (setting - 0.5) * 128
                if value >= 64:
                    value = 63
                self.xml_voices[vox]['pan'] = value
            else:
                if unitspage == 'units':
                    delta = float(sign) * 1.0 / 32
                    setting = firstlastlist[1]
                    delta = self.coerce_delta(setting, delta)
                    setting = setting + delta
                    self.pan_scrollbar.set(setting, setting)
                    value = (setting - 0.5) * 128
                    if value >= 64:
                        value = 63
                    self.xml_voices[vox]['pan'] = value
        else:
            if action == 'goto':
                setting = firstlastlist[0]
                self.pan_scrollbar.set(firstlastlist[0])
                value = (setting - 0.5) * 128
                if value >= 64:
                    value = 63
                self.xml_voices[vox]['pan'] = value

    def validate_pitchname(self, val, add_chars, proposed, sss, sss2, vvv, vvv2, www):
        """Validate the pitch name; put up alert if it fails."""
        # 1-insert, 0=del, -1 if forced
        pat = re.compile(r"([C][#x]?-1)|([ABDEFGabdefg]([#bx]|bb)?-1)|(([A-Ga-g]([#bx]|bb)?[0-8])|([C-Fc-f]([#bx]|bb)?[9])|([Gg]([b]|bb)?[9]))")
        mat = pat.match(str(proposed))
        ret = False
        if mat:
            ret = True
        else:
            ret = False
            messagebox.showerror('message', "bad pitch name (C-1 to G9")
        return ret
