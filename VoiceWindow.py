#!/usr/bin/env python3
# TextMIDITools Version 1.0.19
# textmidiform.py 1.0
# Copyright © 2022 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#
import tkinter
import tkinter.ttk
from tkinter import *
from tkinter import ttk
from tkinter.ttk import *
from GeneralMIDI import *
import re

class VoiceWindow(tkinter.Toplevel):
    max_voice_number = 23
    general_midi_list = list(GeneralMIDIInstrumentDict)
    def __init__(self, xml_form):
        super().__init__()
        self.frame = ttk.Frame(self, padding='1 1 1 1')
        self.frame.grid()
        self.xml_form = xml_form
        self.create_widgets()
        self.title('Instrumental Voices')
        self.geometry('400x400-50+50')

    def create_widgets(self):
        the_row = 0

        self.number_of_voices_label = tkinter.ttk.Label(self.frame, text='Number of Voices')
        self.number_of_voices_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.number_of_voices_spinbox = tkinter.ttk.Spinbox(self.frame, command=self.number_of_voices_callback, state='readonly', wrap=True)
        self.number_of_voices_spinbox['increment'] = 1
        self.number_of_voices_spinbox['from']      = 1
        self.number_of_voices_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.number_of_voices_spinbox.bind('<ButtonRelease-1>', self.number_of_voices_callback)
        self.number_of_voices_spinbox['state'] = '!readonly'
        self.number_of_voices_spinbox.set(len(self.xml_form['voices']))
        self.number_of_voices_spinbox['to'] = len(self.xml_form['voices']) - 1
        self.number_of_voices_spinbox['state'] = 'readonly'
        the_row = the_row + 1

        self.voice_number_label = tkinter.ttk.Label(self.frame, text='Voice')
        self.voice_number_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.voice_number_spinbox = tkinter.ttk.Spinbox(self.frame, command=self.voice_number_callback, wrap=True)
        self.voice_number_spinbox['increment'] = 1.0
        self.voice_number_spinbox['from'] = 0.0
        self.voice_number_spinbox['to'] = len(self.xml_form['voices'])
        self.voice_number_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.voice_number_spinbox.bind('<ButtonRelease-1>', self.voice_number_callback)
        vox = 0
        self.voice_number_spinbox.set(vox)
        self.voice_number_spinbox['state'] = 'readonly'
        the_row = the_row + 1

        self.low_pitch_label = tkinter.ttk.Label(self.frame, text='Low Pitch')
        self.low_pitch_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        # TCL book p 509
        # %d type 1 insert, 0 del, -1 other
        # %i index of string to change or -1
        # %P value of entry if edit is allowed
        # %s val of entry prior to editing
        # %S the text string being inserted or deleted
        # %v type of validation currently set
        # %V type of validation that triggered us (key, focusin, focusout, forced)
        # %W tk name of widget
        validate_command = (self.register(self.validate_pitchname),
            '%d', '%i', '%P', '%s', '%S', '%v', '%V', '%W')
        self.low_pitch_entry = tkinter.ttk.Entry(self.frame, validatecommand=validate_command, validate='focusout')
        self.low_pitch_entry.delete(0, 1024)
        self.low_pitch_entry.insert(0, self.xml_form['voices'][vox]['low_pitch'])
        self.low_pitch_entry.update()
        self.low_pitch_entry.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.low_pitch_entry.bind('<Key-Return>', self.low_pitch_callback)
        self.low_pitch_entry.bind('<FocusOut>', self.low_pitch_callback)
        the_row = the_row + 1

        self.high_pitch_label = tkinter.ttk.Label(self.frame, text='High Pitch')
        self.high_pitch_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.high_pitch_entry = tkinter.ttk.Entry(self.frame, validatecommand=validate_command, validate='focusout')
        self.high_pitch_entry.delete(0, 1024)
        self.high_pitch_entry.insert(0, self.xml_form['voices'][vox]['high_pitch'])
        self.high_pitch_entry.update()
        self.high_pitch_entry.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.high_pitch_entry.bind('<Key-Return>', self.high_pitch_callback)
        the_row = the_row + 1

        self.channel_label = tkinter.ttk.Label(self.frame, text='Channel')
        self.channel_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.channel_spinbox = tkinter.ttk.Spinbox(self.frame, command=self.channel_callback, wrap=True)
        self.channel_spinbox['increment'] = 1.0
        self.channel_spinbox['from'] = 1.0
        self.channel_spinbox['to'] = 16.0
        self.channel_spinbox.set(self.xml_form['voices'][vox]['channel'])
        self.channel_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.channel_spinbox.bind('<ButtonRelease-1>', self.channel_callback)
        self.channel_spinbox['state'] = 'readonly'
        the_row = the_row + 1

        self.program_label = tkinter.ttk.Label(self.frame, text='Program')
        self.program_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.program_spinbox = tkinter.ttk.Spinbox(self.frame, values=self.general_midi_list, state='readonly', wrap=True)
        self.program_spinbox['increment'] = 1.0
        self.program_spinbox['from'] = 1.0
        self.program_spinbox['to'] = 128.0
        self.program_spinbox.set(self.general_midi_list[self.xml_form['voices'][vox]['program'] - 1])
        self.program_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.program_spinbox.bind('<ButtonRelease-1>', self.program_callback)
        self.program_spinbox['state'] = 'readonly'
        the_row = the_row + 1

        self.walking_label = tkinter.ttk.Label(self.frame, text='Walking')
        self.walking_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.walking = tkinter.IntVar()
        self.walking = self.xml_form['voices'][vox]['walking']
        self.walking_checkbutton = tkinter.ttk.Checkbutton(self.frame, text='walking',
            variable=self.walking, command=self.walking_callback)
        self.walking_checkbutton.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1

        self.pan_scroll_label = tkinter.ttk.Label(self.frame, text='-pi..................0..................pi')
        self.pan_scroll_label.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1

        self.pan_label = tkinter.ttk.Label(self.frame, text='Pan')
        self.pan_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.pan_scrollbar = tkinter.ttk.Scrollbar(self.frame, command=self.pan_callback)
        self.pan_scrollbar['orient'] = 'horizontal'
        self.pan_scrollbar.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        temppan = self.xml_form['voices'][vox]['pan'] / 128 + 0.5
        self.pan_scrollbar.set(temppan, temppan)
        the_row = the_row + 1

        self.follow_label = tkinter.ttk.Label(self.frame, text='Follow')
        self.follow_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.follow = tkinter.IntVar()
        self.follow.set(self.xml_form['voices'][vox]['follower']['follow'])
        self.follow_checkbutton = tkinter.ttk.Checkbutton(self.frame, text='follow',
            variable=self.follow, command=self.follow_callback)
        self.follow_checkbutton.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1

        self.leader_label = tkinter.ttk.Label(self.frame, text='Leader')
        self.leader_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.leader_spinbox = tkinter.ttk.Spinbox(self.frame, command=self.leader_callback, wrap=True)
        self.leader_spinbox['increment'] = 1.0
        self.leader_spinbox['from'] = 0.0
        self.leader_spinbox['to'] = self.number_of_voices_spinbox.get()
        if (self.xml_form['voices'][vox]['follower']['follow'] == 1):
            self.leader_spinbox['state'] = 'enabled'
        else:
            self.leader_spinbox['state'] = 'disabled'
        self.leader_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.leader_spinbox.bind('<Key-Return>', self.leader_callback)
        self.leader_spinbox.bind('<FocusOut>', self.leader_callback)
        self.leader_spinbox.bind('<ButtonRelease-1>', self.leader_callback)
        self.leader_spinbox.set(self.xml_form['voices'][vox]['follower']['leader'])
        the_row = the_row + 1

        self.scaler_interval_type_label = tkinter.ttk.Label(self.frame, text='Interval Type')
        self.scaler_interval_type_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.frame.interval_type = tkinter.StringVar()
        self.frame.interval_type.set(self.xml_form['voices'][vox]['follower']['interval_type'])
        self.scaler_interval_type_radiobutton = tkinter.ttk.Radiobutton(self.frame,
            text='Scaler', variable=self.frame.interval_type, value='1',
            command=self.interval_type_callback)
        self.scaler_interval_type_radiobutton.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)

        self.chromatic_interval_type_radiobutton = tkinter.ttk.Radiobutton(self.frame,
            text='Chromatic', variable=self.frame.interval_type, value='2',
            command=self.interval_type_callback)
        if (self.xml_form['voices'][vox]['follower']['follow'] == '1'):
            self.scaler_interval_type_radiobutton['state'] = 'enabled'
            self.chromatic_interval_type_radiobutton['state'] = 'enabled'
        else:
            self.scaler_interval_type_radiobutton['state'] = 'disabled'
            self.chromatic_interval_type_radiobutton['state'] = 'disabled'
        self.chromatic_interval_type_radiobutton['state'] = 'disabled'
        self.chromatic_interval_type_radiobutton.grid(row=the_row, column=2, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1

        self.follow_interval_label = tkinter.ttk.Label(self.frame, text='Interval')
        self.follow_interval_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.follow_interval_spinbox = tkinter.ttk.Spinbox(self.frame, command=self.follow_interval_callback, wrap=True)
        self.follow_interval_spinbox['increment'] = 1.0
        self.follow_interval_spinbox['from'] = -24.0
        self.follow_interval_spinbox['to'] = 24.0
        self.follow_interval_spinbox['state'] = 'disabled'
        self.follow_interval_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.follow_interval_spinbox.bind('<ButtonRelease-1>', self.follow_interval_callback)
        self.follow_interval_spinbox.set(self.xml_form['voices'][vox]['follower']['interval'])
        the_row = the_row + 1

    def low_pitch_callback(self, event):
        vox = int(self.voice_number_spinbox.get())
        self.xml_form['voices'][vox]['low_pitch'] = self.low_pitch_entry.get()

    def high_pitch_callback(self, event):
        vox = int(self.voice_number_spinbox.get())
        self.xml_form['voices'][vox]['high_pitch'] = self.high_pitch_entry.get()

    def follow_interval_callback(self, event=None):
        vox = int(self.voice_number_spinbox.get())
        self.xml_form['voices'][vox]['follower']['interval'] = self.follow_interval_spinbox.get()

    def interval_type_callback(self):
        vox = int(self.voice_number_spinbox.get())
        self.xml_form['voices'][vox]['follower']['interval_type'] = self.frame.interval_type.get()

    def number_of_voices_callback(self, event=None):
        vox = int(self.voice_number_spinbox.get())
        number_of_voices = int(self.number_of_voices_spinbox.get())
        if (vox >= number_of_voices):
            vox = (number_of_voices - 1)

        if (len(self.xml_form['voices']) > number_of_voices):
            self.xml_form['voices'] = self.xml_form['voices'][0:(number_of_voices - 1)]
        if (len(self.xml_form['voices']) < number_of_voices):
            for v in range(len(self.xml_form['voices']), number_of_voices):
                vd = {}
                vd['low_pitch'] = 'A0'
                vd['high_pitch'] = 'C8'
                vd['channel'] = 1
                vd['walking'] = 0
                vd['program'] = 1
                vd['pan'] = 0
                fd = {}
                fd['follow'] = 0
                fd['leader'] = 2147483647
                fd['interval_type'] = 0
                fd['interval'] = 0
                vd['follower'] = fd
                self.xml_form['voices'].append(vd)
        self.voice_number_spinbox['to'] = len(self.xml_form['voices']) - 1

    def voice_number_callback(self, event=None):
        self.update_idletasks()
        vox = int(self.voice_number_spinbox.get())
        ch = self.xml_form['voices'][vox]['channel']
        self.channel_spinbox.set(ch)
        self.channel_spinbox.update()
        instrument = self.general_midi_list[self.xml_form['voices'][vox]['program'] - 1]
        self.program_spinbox['state'] = '!readonly'
        self.program_spinbox.set(instrument)
        self.program_spinbox.update()
        self.program_spinbox['state'] = 'readonly'

        self.low_pitch_entry.delete(0, 1024)
        self.low_pitch_entry.insert(0, self.xml_form['voices'][vox]['low_pitch'])
        self.low_pitch_entry.update()

        self.high_pitch_entry.delete(0, 1024)
        self.high_pitch_entry.insert(0, self.xml_form['voices'][vox]['high_pitch'])
        self.high_pitch_entry.update()

        self.walking = self.xml_form['voices'][vox]['walking']
        if (self.walking_checkbutton.instate(['selected']) and not self.walking):
            self.walking_checkbutton.invoke()
        if (not self.walking_checkbutton.instate(['selected']) and self.walking):
            self.walking_checkbutton.invoke()

        the_pan = float(self.xml_form['voices'][vox]['pan']) / 128.0 + 0.5
        self.pan_scrollbar.set(the_pan, the_pan)
        self.pan_scrollbar.update()

        self.follow = self.xml_form['voices'][vox]['follower']['follow']
        if (self.follow_checkbutton.instate(['selected']) and not self.follow):
            self.follow_checkbutton.invoke()
        if (not self.follow_checkbutton.instate(['selected']) and self.follow):
            self.follow_checkbutton.invoke()

        self.leader_spinbox.set(self.xml_form['voices'][vox]['follower']['leader'])
        self.leader_spinbox.update()

        self.frame.interval_type.set(self.xml_form['voices'][vox]['follower']['interval_type'])
        if (self.xml_form['voices'][vox]['follower']['follow'] == 1):
            self.leader_spinbox['state'] = 'enabled'
            self.scaler_interval_type_radiobutton['state'] = 'enabled'
            self.chromatic_interval_type_radiobutton['state'] = 'enabled'
            self.follow_interval_spinbox.set(self.xml_form['voices'][vox]['follower']['interval'])
            self.frame.interval_type.set(self.xml_form['voices'][vox]['follower']['interval_type'])
            self.scaler_interval_type_radiobutton.update()
            self.chromatic_interval_type_radiobutton.update()
            self.follow_interval_spinbox.update()
        else:
            self.leader_spinbox['state'] = 'disabled'
            self.scaler_interval_type_radiobutton['state'] = 'disabled'
            self.chromatic_interval_type_radiobutton['state'] = 'disabled'
            self.leader_spinbox.set('2147483647')

    def leader_callback(self, event=None):
        self.xml_form['voices'][int(self.voice_number_spinbox.get())]['follower']['leader'] = self.leader_spinbox.get()

    def channel_callback(self, event=None):
        ch = self.channel_spinbox.get()
        self.xml_form['voices'][int(self.voice_number_spinbox.get())]['channel'] = ch

    def program_callback(self, event):
        prog = self.program_spinbox.get()
        vox = int(self.voice_number_spinbox.get())
        self.xml_form['voices'][int(vox)]['program'] = GeneralMIDIInstrumentDict[prog][0]
        # low pitch
        self.low_pitch_entry.delete(0, 1024)
        self.low_pitch_entry.insert(0, GeneralMIDIInstrumentDict[prog][1])
        self.low_pitch_entry.update()
        self.xml_form['voices'][vox]['low_pitch'] = self.low_pitch_entry.get()
        # high pitch

        self.high_pitch_entry.delete(0, 1024)
        self.high_pitch_entry.insert(0, GeneralMIDIInstrumentDict[prog][2])
        self.high_pitch_entry.update()
        self.xml_form['voices'][vox]['high_pitch'] = self.high_pitch_entry.get()

    def walking_callback(self):
        vox = int(self.voice_number_spinbox.get())
        if (self.walking_checkbutton.instate(['selected'])):
            self.xml_form['voices'][vox]['walking'] = 1
        else:
            self.xml_form['voices'][vox]['walking'] = 0

    def install_xml_form(self, xml_form):
        vox = 0
        ch = self.xml_form['voices'][vox]['channel']
        self.xml_form = xml_form
        self.number_of_voices_spinbox['state'] = '!readonly'
        self.number_of_voices_spinbox.set(len(self.xml_form['voices']))
        self.number_of_voices_spinbox['state'] = 'readonly'
        self.voice_number_spinbox['state'] = '!readonly'
        self.voice_number_spinbox['to'] = len(self.xml_form['voices']) - 1
        self.voice_number_spinbox.set(vox)
        self.voice_number_spinbox['state'] = 'readonly'

        self.low_pitch_entry.delete(0, 1024)
        self.low_pitch_entry.insert(0, self.xml_form['voices'][vox]['low_pitch'])
        self.low_pitch_entry.update()

        self.high_pitch_entry.delete(0, 1024)
        self.high_pitch_entry.insert(0, self.xml_form['voices'][vox]['high_pitch'])
        self.high_pitch_entry.update()

        self.channel_spinbox.set(ch)
        self.channel_spinbox.update()
        self.program_spinbox.set(
            self.general_midi_list[self.xml_form['voices'][vox]['program'] - 1])
        self.program_spinbox.update()

        self.walking = self.xml_form['voices'][vox]['walking']
        if (self.walking_checkbutton.instate(['selected']) and not self.walking):
            self.walking_checkbutton.invoke()
        if (not self.walking_checkbutton.instate(['selected']) and self.walking):
            self.walking_checkbutton.invoke()

        the_pan = (float(self.xml_form['voices'][0]['pan']) / 128) + 0.5
        self.pan_scrollbar.set(the_pan, the_pan)
        self.pan_scrollbar.update()

        self.follow = self.xml_form['voices'][vox]['follower']['follow']
        if (self.follow_checkbutton.instate(['selected']) and not self.follow):
            self.follow_checkbutton.invoke()
        if (not self.follow_checkbutton.instate(['selected']) and self.follow):
            self.follow_checkbutton.invoke()

        self.leader_spinbox.set(self.xml_form['voices'][vox]['follower']['leader'])
        self.leader_spinbox.update()
        if (self.xml_form['voices'][vox]['follower']['interval_type'] == 0):
            self.scaler_interval_type_radiobutton['state'] = 'disabled'
            self.scaler_interval_type_radiobutton['state'] = 'disabled'
        if (self.xml_form['voices'][vox]['follower']['interval_type'] == 1):
            self.scaler_interval_type_radiobutton['state'] = 'enabled'
            self.scaler_interval_type_radiobutton['state'] = 'disabled'
        if (self.xml_form['voices'][vox]['follower']['interval_type'] == 2):
            self.scaler_interval_type_radiobutton['state'] = 'disabled'
            self.scaler_interval_type_radiobutton['state'] = 'enabled'
        self.scaler_interval_type_radiobutton.update()
        self.follow_interval_spinbox.set(self.xml_form['voices'][vox]['follower']['interval'])
        self.follow_interval_spinbox.update()
        self.frame.update()

    def follow_callback(self):
        vox = int(self.voice_number_spinbox.get())
        if (self.follow_checkbutton.instate(['selected'])):
            self.xml_form['voices'][vox]['follower']['follow'] = 1
            self.walking_checkbutton['state'] = 'disabled'
            self.leader_spinbox['state'] = 'normal'
            self.scaler_interval_type_radiobutton['state'] = 'normal'
            self.chromatic_interval_type_radiobutton['state'] = 'normal'
            self.follow_interval_spinbox['state'] = 'normal'
        else:
            self.xml_form['voices'][vox]['follower']['follow'] = 0
            self.walking_checkbutton['state'] = 'normal'
            self.leader_spinbox['state'] = 'disabled'
            self.scaler_interval_type_radiobutton['state'] = 'disabled'
            self.chromatic_interval_type_radiobutton['state'] = 'disabled'
            self.follow_interval_spinbox['state'] = 'disabled'

    def pan_callback(self, action, sign, unitspage):
        vox = int(self.voice_number_spinbox.get())
        firstlastlist = self.pan_scrollbar.get()
        setting = 0
        if (action == 'scroll'):
            if (unitspage == 'pages'):
                delta = float(sign) * 1.0 / 4
                setting = firstlastlist[1]
                if ((setting <= 0.0) and (delta < 0.0)):
                    delta = 0.0
                if ((setting >= 1.0) and (delta > 0.0)):
                    delta = 0.0
                setting = setting + delta
                self.pan_scrollbar.set(setting, setting)
                value = (setting - 0.5) * 128
                if (value >= 64):
                    value = 63
                self.xml_form['voices'][vox]['pan'] = value
            else:
                if (unitspage == 'units'):
                    delta = float(sign) * 1.0 / 32
                    setting = firstlastlist[1]
                    if ((setting <= 0.0) and (delta < 0.0)):
                        delta = 0.0
                    if ((setting >= 1.0) and (delta > 0.0)):
                        delta = 0.0
                    setting = setting + delta
                    self.pan_scrollbar.set(setting, setting)
                    value = (setting - 0.5) * 128
                    if (value >= 64):
                        value = 63
                    self.xml_form['voices'][vox]['pan'] = value
        else:
            if (action == 'goto'):
                setting = firstlastlist[0]
                self.pan_scrollbar.set(firstlastlist[0])
                value = (setting - 0.5) * 128
                if (value >= 64):
                    value = 63
                self.xml_form['voices'][vox]['pan'] = value

    def validate_pitchname(self, d, i, P, s, S, v, V, W):
        # 1-insert, 0=del, -1 if forced
        pat = re.compile('[a-gA-G]([#bx]|bb)-?\d')
        ma = pat.match(str(P))
        ret = False
        if (ma):
            ret = True
        else:
            ret = False
        return ret

