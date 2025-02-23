#!/usr/bin/env python3
"""TextMIDITools: TextMidiFormEdit.py Voice Window for editing a voice's attributes."""
# TextMIDITools Version 1.0.90
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
from tkinter import ttk
from tkinter.ttk import *

from GeneralMidi import *

class VoiceWindow(tkinter.Toplevel):
    general_midi_list = list(GENERAL_MIDI_INSTRUMENT_DICT)
    def __init__(self, XmlForm):
        """Init the voice window."""
        super().__init__()
        self.frame = tkinter.ttk.Frame(self, padding='1 1 1 1')
        self.frame.grid()
        self.xml_form = XmlForm
        self.number_of_voices = tkinter.IntVar()
        self.voice_number = tkinter.IntVar()
        self.low_pitch = tkinter.StringVar()
        self.high_pitch = tkinter.StringVar()
        self.channel = tkinter.IntVar()
        self.program = tkinter.StringVar()
        #self.walking = tkinter.IntVar()
        self.walking = tkinter.DoubleVar(value=0.0)
        # no variable for pan
        self.follow = tkinter.StringVar()
        self.leader = tkinter.StringVar()
        self.interval_type = tkinter.StringVar()
        self.interval_type.set(0)
        self.follow_interval = tkinter.StringVar()
        self.follow_delay = tkinter.StringVar()
        self.follow_duration_factor = tkinter.StringVar()
        self.inversion = tkinter.IntVar()
        self.retrograde = tkinter.IntVar()
        self.voice_number.set(0)

        self.loading_high_pitch = False
        self.loading_low_pitch = False
        self.create_widgets()
        self.title('Instrumental Voices')
        self.geometry('450x500-50+50')

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
              textvariable=self.number_of_voices, state='readonly', wrap=True)
        self.number_of_voices_spinbox['increment'] = 1
        self.number_of_voices_spinbox['from']      = 1
        self.number_of_voices_spinbox['state'] = '!readonly'
        self.number_of_voices_spinbox.set(len(self.xml_form['voices']))
        self.number_of_voices_spinbox['to'] = len(self.xml_form['voices']) - 1
        self.number_of_voices_spinbox['state'] = 'readonly'

        self.voice_number_label = tkinter.ttk.Label(self.frame, text='Voice')
        self.voice_number_spinbox = tkinter.ttk.Spinbox(self.frame, wrap=True,
              textvariable=self.voice_number)
        self.voice_number_spinbox['increment'] = 1.0
        self.voice_number_spinbox['from'] = 0.0
        self.voice_number_spinbox['to'] = len(self.xml_form['voices']) - 1
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
        self.low_pitch.set(self.xml_form['voices'][vox]['low_pitch'])
        self.low_pitch_from_program_button = tkinter.ttk.Button(self.frame, text='From Program',
            command=self.low_pitch_from_program_callback)

        self.high_pitch_label = tkinter.ttk.Label(self.frame, text='High Pitch')
        self.high_pitch_entry = tkinter.ttk.Entry(self.frame, textvariable=self.high_pitch,
              validatecommand=validate_command, validate='focusout')
        self.high_pitch.set(self.xml_form['voices'][vox]['high_pitch'])
        self.high_pitch_from_program_button = tkinter.ttk.Button(self.frame, text='From Program',
            command=self.high_pitch_from_program_callback)

        self.channel_label = tkinter.ttk.Label(self.frame, text='Channel')
        self.channel_spinbox = tkinter.ttk.Spinbox(self.frame, textvariable=self.channel,
              wrap=True)
        self.channel_spinbox['increment'] = 1.0
        self.channel_spinbox['from'] = 1.0
        self.channel_spinbox['to'] = 16.0
        self.channel_spinbox.set(self.xml_form['voices'][vox]['channel'])
        self.channel_spinbox['state'] = 'readonly'

        self.program_label = tkinter.ttk.Label(self.frame, text='Program')
        self.program_spinbox = tkinter.ttk.Spinbox(self.frame,
            textvariable=self.program, values=self.general_midi_list,
            state='readonly', wrap=True)
        self.program_spinbox['increment'] = 1.0
        self.program_spinbox['from'] = 1.0
        self.program_spinbox['to'] = 128.0
        self.program_spinbox.set(
            self.general_midi_list[self.xml_form['voices'][vox]['program'] - 1])
        self.program_spinbox['state'] = 'readonly'

        self.walking_scroll_label = tkinter.ttk.Label(self.frame,
            text='Random.............................Walking')
        self.walking_label = tkinter.ttk.Label(self.frame, text='Walking')
        self.walking_scrollbar = tkinter.ttk.Scrollbar(self.frame,
            command=self.walking_callback)
        self.walking_scrollbar['orient'] = 'horizontal'
        self.walking_scrollbar.bind('<B1-Motion>', self.walking_callback)
        tempwalking = self.xml_form['voices'][vox]['walking']
        self.walking_scrollbar.set(tempwalking, tempwalking)

        self.pan_scroll_label = tkinter.ttk.Label(self.frame,
            text='-pi..................0..................pi')
        self.pan_label = tkinter.ttk.Label(self.frame, text='Pan')
        self.pan_scrollbar = tkinter.ttk.Scrollbar(self.frame,
            command=self.pan_callback)
        self.pan_scrollbar['orient'] = 'horizontal'
        temppan = self.xml_form['voices'][vox]['pan'] / 128 + 0.5
        self.pan_scrollbar.set(temppan, temppan)

        self.follow_label = tkinter.ttk.Label(self.frame, text='Follow')
        self.follow_checkbutton = tkinter.ttk.Checkbutton(self.frame,
            text='follow', variable=self.follow)
        self.follow.set(self.xml_form['voices'][vox]['follower']['follow'])

        self.leader_label = tkinter.ttk.Label(self.frame, text='Leader')
        self.leader_spinbox = tkinter.ttk.Spinbox(self.frame,
            textvariable=self.leader, wrap=True)
        self.leader_spinbox['increment'] = 1.0
        self.leader_spinbox['from'] = 0.0
        self.leader_spinbox['to'] = self.number_of_voices_spinbox.get()
        if self.xml_form['voices'][vox]['follower']['follow'] == 1:
            self.leader_spinbox['state'] = 'normal'
        else:
            self.leader_spinbox['state'] = 'disabled'
        self.leader_spinbox.set(
            self.xml_form['voices'][vox]['follower']['leader'])

        self.scaler_interval_type_label = tkinter.ttk.Label(self.frame, text='Interval Type')
        self.interval_type.set(
            self.xml_form['voices'][vox]['follower']['interval_type'])
        self.scaler_interval_type_radiobutton = tkinter.ttk.Radiobutton(self.frame,
            text='Scaler', variable=self.interval_type, value='1')

        self.chromatic_interval_type_radiobutton = tkinter.ttk.Radiobutton(self.frame,
            text='Chromatic', variable=self.interval_type, value='2')
        if self.xml_form['voices'][vox]['follower']['follow'] == '1':
            self.scaler_interval_type_radiobutton['state'] = 'normal'
            self.chromatic_interval_type_radiobutton['state'] = 'normal'
        else:
            self.scaler_interval_type_radiobutton['state'] = 'disabled'
            self.chromatic_interval_type_radiobutton['state'] = 'disabled'
        self.chromatic_interval_type_radiobutton['state'] = 'disabled'

        self.follow_interval_label = tkinter.ttk.Label(self.frame, text='Interval')
        self.follow_interval_spinbox = tkinter.ttk.Spinbox(self.frame,
              textvariable=self.follow_interval, wrap=True)
        self.follow_interval_spinbox['increment'] = 1.0
        self.follow_interval_spinbox['from'] = -24.0
        self.follow_interval_spinbox['to'] = 24.0
        self.follow_interval_spinbox['state'] = 'disabled'
        self.follow_interval_spinbox.bind('<ButtonRelease-1>',
            self.follow_interval_callback)
        self.follow_interval_spinbox.set(
            self.xml_form['voices'][vox]['follower']['interval'])

        self.follow_delay_label = tkinter.ttk.Label(self.frame, text='Delay')
        self.follow_delay_entry = tkinter.ttk.Entry(self.frame,
            textvariable=self.follow_delay)
        delay_ratio = "/"
        delay_ratio = delay_ratio.join([self
            .xml_form['voices'][vox]['follower']['delay']['numerator'],
            self.xml_form['voices'][vox]['follower']['delay']['denominator']])
        self.follow_delay.set(delay_ratio)
        self.follow_delay_entry['state'] = 'disabled'

        self.follow_duration_factor_label = tkinter.ttk.Label(self.frame, text='Duration Factor')
        self.follow_duration_factor_entry = tkinter.ttk.Entry(self.frame,
              textvariable=self.follow_duration_factor)
        duration_factor_ratio = "/"
        duration_factor_ratio = duration_factor_ratio.join(
            [self.xml_form['voices'][vox]['follower']['duration_factor']
            ['numerator'],
            self.xml_form['voices'][vox]['follower']['duration_factor']
            ['denominator']])
        self.follow_duration_factor.set(duration_factor_ratio)
        self.follow_duration_factor_entry['state'] = 'disabled'

        self.inversion.set(self
            .xml_form['voices'][vox]['follower']['inversion'])
        self.inversion_checkbutton = tkinter.ttk.Checkbutton(self.frame, text='inversion',
              variable=self.inversion)
        self.inversion_checkbutton['state'] = 'disabled'

        self.retrograde.set(
            self.xml_form['voices'][vox]['follower']['retrograde'])
        self.retrograde_checkbutton = tkinter.ttk.Checkbutton(self.frame, text='retrograde',
              variable=self.retrograde)
        self.retrograde_checkbutton['state'] = 'disabled'

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
        self.follow_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.follow_checkbutton.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.leader_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.leader_spinbox.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1
        self.scaler_interval_type_label.grid(
            row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.scaler_interval_type_radiobutton.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.chromatic_interval_type_radiobutton.grid(
            row=the_row, column=2, sticky=NSEW, padx=2, pady=2)
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
        the_row = the_row + 1
        self.retrograde_checkbutton.grid(
            row=the_row, column=1, sticky=NSEW, padx=2, pady=2)

        self.number_of_voices.trace('w', self.number_of_voices_callback)
        self.voice_number.trace('w', self.voice_number_callback)
        self.low_pitch.trace('w', self.low_pitch_callback)
        self.high_pitch.trace('w', self.high_pitch_callback)
        self.channel.trace('w', self.channel_callback)
        self.program.trace('w', self.program_callback)
        self.walking.trace('w', self.walking_callback)
        self.follow.trace('w', self.follow_callback)
        self.leader.trace('w', self.leader_callback)
        self.interval_type.trace('w', self.interval_type_callback)
        self.follow_interval.trace('w', self.follow_interval_callback)
        self.follow_delay.trace('w', self.follow_delay_callback)
        self.follow_duration_factor.trace('w',
            self.follow_duration_factor_callback)
        self.retrograde.trace('w', self.retrograde_callback)

    def low_pitch_callback(self, event, *args):
        """Low Pitch was set; save to the internal form."""
        vox = int(self.voice_number.get())
        self.xml_form['voices'][vox]['low_pitch'] = self.low_pitch.get()

    def low_pitch_from_program_callback(self, event=None, *args):
        """From MIDI Program number (for a typical instrument range) was pushed; 
        copy the low pitch into the internal form.
        """
        vox = int(self.voice_number.get())
        prog_index = self.program.get()
        self.xml_form['voices'][vox]['low_pitch'] = (GENERAL_MIDI_INSTRUMENT_DICT
                [prog_index][GM_LOW_PITCH_INDEX])
        self.low_pitch.set(self.xml_form['voices'][vox]['low_pitch'])

    def high_pitch_callback(self, event, *args):
        """High Pitch was set; save to the internal form."""
        vox = int(self.voice_number.get())
        self.xml_form['voices'][vox]['high_pitch'] = self.high_pitch.get()

    def high_pitch_from_program_callback(self, event=None, *args):
        """From MIDI Program number (for a typical instrument range) was pushed; 
        copy the high pitch into the internal form.
        """
        vox = int(self.voice_number.get())
        prog_index = self.program.get()
        self.xml_form['voices'][vox]['high_pitch'] = (GENERAL_MIDI_INSTRUMENT_DICT
                [prog_index][GM_HIGH_PITCH_INDEX])
        self.high_pitch.set(self.xml_form['voices'][vox]['high_pitch'])

    def interval_type_callback(self, *args):
        """Interval type for the follower (canon); set the internal form."""
        vox = int(self.voice_number.get())
        self.xml_form['voices'][vox]['follower']['interval_type'] = int(self.interval_type.get())

    def follow_interval_callback(self, event=None, *args):
        """Interval (number of steps) for the follower (canon); set the internal form."""
        vox = int(self.voice_number.get())
        self.xml_form['voices'][vox]['follower']['interval'] = int(self.follow_interval.get())

    def follow_delay_callback(self, event=None, *args):
        """Follow delay; set the internal form."""
        vox = int(self.voice_number.get())
        delay_quotient = self.follow_delay.get()
        slash_index = delay_quotient.find("/")
        if slash_index > 0:
            num_list = delay_quotient.split("/")
            if len(num_list) == 2:
                self.xml_form['voices'][vox]['follower']['delay']['numerator'] = num_list[0]
                self.xml_form['voices'][vox]['follower']['delay']['denominator'] = num_list[1]
        else:
            self.xml_form['voices'][vox]['follower']['delay']['numerator'] = delay_quotient
            self.xml_form['voices'][vox]['follower']['delay']['denominator'] = 1

    def follow_duration_factor_callback(self, event=None, *args):
        """Follower Duration multiplier was set; copy to internal form."""
        vox = int(self.voice_number.get())
        duration_factor_quotient = self.follow_duration_factor.get()
        slash_index = duration_factor_quotient.find("/")
        if slash_index > 0:
            num_list = duration_factor_quotient.split("/")
            if len(num_list) == 2:
                self.xml_form['voices'][vox]['follower']['duration_factor']['numerator'] = (
                        num_list[0])
                self.xml_form['voices'][vox]['follower']['duration_factor']['denominator'] = (
                        num_list[1])
        else:
            self.xml_form['voices'][vox]['follower']['duration_factor']['numerator'] = (
                    duration_factor_quotient)
            self.xml_form['voices'][vox]['follower']['duration_factor']['denominator'] = 1

    def inversion_callback(self, *args):
        """Invert button for follower; copy state to internal form."""
        vox = int(self.voice_number.get())
        self.xml_form['voices'][vox]['follower']['inversion'] = self.inversion

    def retrograde_callback(self, *args):
        """Retrograde for follower; copy state to internal form."""
        vox = int(self.voice_number.get())
        self.xml_form['voices'][vox]['follower']['retrograde'] = str(self.retrograde.get())

    def number_of_voices_callback(self, event=None, *args):
        """Number of voices spin button set; copy value to internal form."""
        vox = int(self.voice_number.get())
        if vox >= int(self.number_of_voices.get()):
            vox = (int(self.number_of_voices.get()) - 1)

        if len(self.xml_form['voices']) > int(self.number_of_voices.get()):
            self.xml_form['voices'] = (self.xml_form['voices']
                    [0:(int(self.number_of_voices.get()) - 1)])
        if len(self.xml_form['voices']) < int(self.number_of_voices.get()):
            # default the voices above the voices we're using
            for voice in range(len(self.xml_form['voices']),
                    int(self.number_of_voices.get())):
                vd = {}
                vd['low_pitch'] = 'A0'
                vd['high_pitch'] = 'C8'
                vd['channel'] = 1
                vd['walking'] = 0.0
                vd['program'] = 1
                vd['pan'] = 0
                fd = {}
                fd['follow'] = 0
                fd['leader'] = 2147483647
                fd['interval_type'] = 0
                fd['interval'] = 0
                dd = {}
                dd['numerator'] = 0
                dd['denominator'] = 0
                fd['delay'] = dd
                df = {}
                df['numerator'] = 1
                df['denominator'] = 1
                fd['duration_factor'] = df
                fd['inversion'] = 0
                fd['retrograde'] = 0
                vd['follower'] = fd
                self.xml_form['voices'].append(vd)
        self.voice_number_spinbox['to'] = len(self.xml_form['voices']) - 1

    def voice_number_callback(self, event=None, *args):
        """Voice number for the currently edited voice set."""
        self.update_idletasks()
        vox = int(self.voice_number.get())
        chan = self.xml_form['voices'][vox]['channel']
        self.channel.set(chan)
        instrument_name = self.general_midi_list[
            self.xml_form['voices'][vox]['program'] - 1]
        self.program_spinbox['state'] = '!readonly'
        self.program.set(instrument_name)
        self.program_spinbox['state'] = 'readonly'
        self.low_pitch.set(self.xml_form['voices'][vox]['low_pitch'])
        self.high_pitch.set(self.xml_form['voices'][vox]['high_pitch'])
        tempwalking = self.xml_form['voices'][vox]['walking']
        self.walking_scrollbar.set(tempwalking, tempwalking)

        the_pan = float(self.xml_form['voices'][vox]['pan']) / 128.0 + 0.5
        self.pan_scrollbar.set(the_pan, the_pan)
        self.pan_scrollbar.update()
        self.follow.set(self.xml_form['voices'][vox]['follower']['follow'])

        self.interval_type.set(
            self.xml_form['voices'][vox]['follower']['interval_type'])
        if int(self.follow.get()) == 1:
            self.leader_spinbox['state'] = 'normal'
            self.leader_spinbox.set(
                self.xml_form['voices'][vox]['follower']['leader'])
            self.scaler_interval_type_radiobutton['state'] = 'normal'
            self.chromatic_interval_type_radiobutton['state'] = 'normal'
            self.follow_interval_spinbox['state'] = 'normal'
            self.follow_delay_entry['state'] = 'normal'
            self.follow_duration_factor_entry['state'] = 'normal'
            self.inversion_checkbutton['state'] = 'normal'
            self.retrograde_checkbutton['state'] = 'normal'
            #self.walking_scrollbar.activate('none')
            self.follow_interval.set(
                self.xml_form['voices'][vox]['follower']['interval'])
            self.interval_type.set(
                self.xml_form['voices'][vox]['follower']['interval_type'])
            self.follow_interval.set(
                self.xml_form['voices'][vox]['follower']['interval'])
            delay_ratio = "/"
            delay_ratio = delay_ratio.join([str(
                self.xml_form['voices'][vox]['follower']['delay']['numerator']),
                str(self.xml_form['voices'][vox]['follower']
                    ['delay']['denominator'])])
            self.follow_delay.set(delay_ratio)

            duration_factor_ratio = "/"
            duration_factor_ratio = duration_factor_ratio.join([str(
                self.xml_form['voices'][vox]['follower']
                ['duration_factor']['numerator']),
                str(self.xml_form['voices'][vox]['follower']
                ['duration_factor']['denominator'])])
            self.follow_duration_factor.set(duration_factor_ratio)

            self.inversion.set(self.xml_form['voices'][vox]
                ['follower']['inversion'])
            self.retrograde.set(self.xml_form['voices'][vox]
                ['follower']['retrograde'])
            self.scaler_interval_type_radiobutton.update()
            self.chromatic_interval_type_radiobutton.update()
            self.follow_interval_spinbox.update()
            self.follow_delay_entry.update()
            self.follow_duration_factor_entry.update()
            self.inversion_checkbutton.update()
            self.retrograde_checkbutton.update()
        else:
            self.leader_spinbox['state'] = 'disabled'
            self.scaler_interval_type_radiobutton['state'] = 'disabled'
            self.chromatic_interval_type_radiobutton['state'] = 'disabled'
            self.leader_spinbox.set('2147483647')
            self.follow_delay_entry['state'] = 'disabled'
            self.follow_duration_factor_entry['state'] = 'disabled'
            self.follow_interval_spinbox['state'] = 'disabled'
            self.inversion_checkbutton['state'] = 'disabled'
            self.retrograde_checkbutton['state'] = 'disabled'
            self.scaler_interval_type_radiobutton.update()
            self.chromatic_interval_type_radiobutton.update()
            self.follow_interval_spinbox.update()
            self.follow_delay_entry.update()
            self.follow_duration_factor_entry.update()
            self.inversion_checkbutton.update()
            self.retrograde_checkbutton.update()
            #self.walking_scrollbar.activate('arrow1')
            #self.walking_scrollbar.activate('arrow2')
            #self.walking_scrollbar.activate('slider')

    def leader_callback(self, event=None, *args):
        """Leader of a follower set; set in internal form."""
        self.xml_form['voices'][self.voice_number.get()]['follower']['leader'] = self.leader.get()

    def channel_callback(self, event=None, *args):
        """MIDI channel (from 1) set; copy to internal form."""
        self.xml_form['voices'][self.voice_number.get()]['channel'] = self.channel.get()

    def program_callback(self, event, *args):
        """MIDI Program set; copy to internal form."""
        prog_index = self.program.get()
        vox = int(self.voice_number.get())
        self.xml_form['voices'][vox]['program'] = (GENERAL_MIDI_INSTRUMENT_DICT
                [prog_index][GM_PROGRAM_INDEX])

    def install_xml_form(self, XmlForm):
        """Install XML form."""
        self.xml_form = XmlForm
        self.loading_low_pitch = True
        self.loading_high_pitch = True
        vox = 0
        chan = self.xml_form['voices'][vox]['channel']
        self.number_of_voices_spinbox['state'] = '!readonly'
        self.number_of_voices.set(len(self.xml_form['voices']))
        self.number_of_voices_spinbox['state'] = 'readonly'
        self.voice_number_spinbox['state'] = '!readonly'
        self.voice_number_spinbox['to'] = len(self.xml_form['voices']) - 1
        self.voice_number_spinbox.set(vox)
        self.voice_number_spinbox['state'] = 'readonly'

        self.channel.set(chan)
        self.program.set(
            self.general_midi_list[self.xml_form['voices'][vox]['program'] - 1])
        self.low_pitch.set(self.xml_form['voices'][vox]['low_pitch'])
        self.high_pitch.set(self.xml_form['voices'][vox]['high_pitch'])

        self.walking.set(self.xml_form['voices'][vox]['walking'])

        the_pan = (float(self.xml_form['voices'][0]['pan']) / 128) + 0.5
        self.pan_scrollbar.set(the_pan, the_pan)
        self.pan_scrollbar.update()

        self.follow.set(self.xml_form['voices'][vox]['follower']['follow'])

        self.leader.set(self.xml_form['voices'][vox]['follower']['leader'])
        self.interval_type.set(
            self.xml_form['voices'][vox]['follower']['interval_type'])
        self.follow_interval.set(
            self.xml_form['voices'][vox]['follower']['interval'])

        self.inversion.set(
            self.xml_form['voices'][vox]['follower']['inversion'])

        self.retrograde.set(
            self.xml_form['voices'][vox]['follower']['retrograde'])

        self.frame.update()

    def follow_callback(self, *args):
        """Follow button pressed."""
        vox = int(self.voice_number.get())
        follow = int(self.follow.get())
        if follow == 1:
            self.xml_form['voices'][vox]['follower']['follow'] = 1
            #self.walking_scrollbar.activate('none')
            self.leader_spinbox['state'] = 'normal'
            self.scaler_interval_type_radiobutton['state'] = 'normal'
            self.chromatic_interval_type_radiobutton['state'] = 'normal'
            self.follow_interval_spinbox['state'] = 'normal'
            self.follow_delay_entry['state'] = 'normal'
            self.follow_duration_factor_entry['state'] = 'normal'
            self.inversion_checkbutton['state'] = 'normal'
            self.retrograde_checkbutton['state'] = 'normal'
        else:
            self.xml_form['voices'][vox]['follower']['follow'] = 0
            #self.walking_scrollbar.activate('arrow1')
            #self.walking_scrollbar.activate('arrow2')
            #self.walking_scrollbar.activate('slider')
            self.leader_spinbox['state'] = 'disabled'
            self.scaler_interval_type_radiobutton['state'] = 'disabled'
            self.chromatic_interval_type_radiobutton['state'] = 'disabled'
            self.follow_interval_spinbox['state'] = 'disabled'
            self.follow_delay_entry['state'] = 'disabled'
            self.follow_duration_factor_entry['state'] = 'disabled'
            self.inversion_checkbutton['state'] = 'disabled'
            self.retrograde_checkbutton['state'] = 'disabled'

    def walking_callback(self, action, sign = 0, unitspage='None'):
        """Walking button for a follower was pressed."""
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
                self.xml_form['voices'][vox]['walking'] = value
            else:
                if unitspage == 'units':
                    delta = float(sign) / 64.0
                    setting = firstlastlist[1]
                    self.coerce_delta(setting, delta)
                    setting = setting + delta
                    self.walking_scrollbar.set(setting, setting)
                    value = setting
                    self.xml_form['voices'][vox]['walking'] = value
        else:
            if action == 'moveto':
                setting = float(sign)
                setting = max(setting, 0.0)
                setting = max(setting, 1.0)
                self.walking_scrollbar.set(setting, setting)
                value = setting
                self.xml_form['voices'][vox]['walking'] = value
            else:
                if action == 'goto':
                    setting = firstlastlist[0]
                    self.walking_scrollbar.set(firstlastlist[0])
                    value = setting
                    self.xml_form['voices'][vox]['walking'] = value

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
                self.xml_form['voices'][vox]['pan'] = value
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
                    self.xml_form['voices'][vox]['pan'] = value
        else:
            if action == 'goto':
                setting = firstlastlist[0]
                self.pan_scrollbar.set(firstlastlist[0])
                value = (setting - 0.5) * 128
                if value >= 64:
                    value = 63
                self.xml_form['voices'][vox]['pan'] = value

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

