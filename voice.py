#!/usr/bin/env python3
# TextMIDITools Version 1.0.13
# textmidiform.py 1.0
# Copyright © 2021 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#
import tkinter
import tkinter.ttk
from tkinter import *
from tkinter import ttk
from tkinter.ttk import *
import re

class VoiceWindow(tkinter.Toplevel):
    max_voice_number = 23
    def __init__(self, xml_form):
        super().__init__()
        self.frame = ttk.Frame(self, padding="1 1 1 1")
        self.frame.grid()
        self.xml_form = xml_form
        self.create_widgets()
        self.title('Instrumental Voices')

    def create_widgets(self):
        the_row = 0

        self.number_of_voices_label = tkinter.ttk.Label(self.frame, text="Number of Voices")
        self.number_of_voices_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.number_of_voices_spinbox = tkinter.ttk.Spinbox(self.frame, command=self.number_of_voices_callback)
        self.number_of_voices_spinbox["increment"] = 1
        self.number_of_voices_spinbox["from"]      = 1
        self.number_of_voices_spinbox["to"]        = self.max_voice_number
        self.number_of_voices_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.number_of_voices_spinbox.bind('<Key-Return>', self.number_of_voices_callback)
        self.number_of_voices_spinbox.bind('<ButtonRelease-1>', self.number_of_voices_callback)
        self.number_of_voices = tkinter.StringVar()
        self.number_of_voices.set(len(self.xml_form['voices']))
        self.number_of_voices_spinbox["textvariable"] = self.number_of_voices
        the_row = the_row + 1

        self.voice_number_label = tkinter.ttk.Label(self.frame, text="Voice")
        self.voice_number_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.voice_number_spinbox = tkinter.ttk.Spinbox(self.frame, command=self.voice_number_callback)
        self.voice_number_spinbox["increment"] = 1.0
        self.voice_number_spinbox["from"] = 0.0
        self.voice_number_spinbox["to"] = len(self.xml_form['voices'])
        self.voice_number_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.voice_number_spinbox.bind('<Key-Return>', self.voice_number_callback)
        self.voice_number_spinbox.bind('<ButtonRelease-1>', self.voice_number_callback)
        self.voice_number = tkinter.StringVar()
        self.voice_number.set("0")
        self.voice_number_spinbox["textvariable"] = self.voice_number
        the_row = the_row + 1

        self.low_pitch_label = tkinter.ttk.Label(self.frame, text="Low Pitch")
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
        self.low_pitch_entry = tkinter.ttk.Entry(self.frame, validatecommand=validate_command, validate="focusout")
        self.low_pitch = tkinter.StringVar()
        self.low_pitch.set(self.xml_form['voices'][int(self.voice_number.get())]['low_pitch'])
        self.low_pitch_entry["textvariable"] = self.low_pitch
        self.low_pitch_entry.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.low_pitch_entry.bind('<Key-Return>', self.low_pitch_callback)
        self.low_pitch_entry.bind('<FocusOut>', self.low_pitch_callback)
        the_row = the_row + 1

        self.high_pitch_label = tkinter.ttk.Label(self.frame, text="High Pitch")
        self.high_pitch_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.high_pitch_entry = tkinter.ttk.Entry(self.frame, validatecommand=validate_command, validate="focusout")
        self.high_pitch = tkinter.StringVar()
        self.high_pitch.set(self.xml_form['voices'][int(self.voice_number.get())]['high_pitch'])
        self.high_pitch_entry["textvariable"] = self.high_pitch
        self.high_pitch_entry.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.high_pitch_entry.bind('<Key-Return>', self.high_pitch_callback)
        self.high_pitch_entry.bind('<FocusOut>', self.high_pitch_callback)
        the_row = the_row + 1

        self.channel_label = tkinter.ttk.Label(self.frame, text="Channel")
        self.channel_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.channel_spinbox = tkinter.ttk.Spinbox(self.frame, command=self.channel_callback)
        self.channel_spinbox["increment"] = 1.0
        self.channel_spinbox["from"] = 1.0
        self.channel_spinbox["to"] = 16.0
        self.channel_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.channel_spinbox.bind('<Key-Return>', self.channel_callback)
        self.channel_spinbox.bind('<FocusOut>', self.channel_callback)
        self.channel_spinbox.bind('<ButtonRelease-1>', self.channel_callback)
        self.channel = tkinter.StringVar()
        self.channel.set(self.xml_form['voices'][int(self.voice_number.get())]['channel'])
        self.channel_spinbox["textvariable"] = self.channel
        the_row = the_row + 1

        self.program_label = tkinter.ttk.Label(self.frame, text="Program")
        self.program_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.program_entry = tkinter.ttk.Entry(self.frame)
        self.program = tkinter.StringVar()
        self.program.set(self.xml_form['voices'][int(self.voice_number.get())]['program'])
        self.program_entry["textvariable"] = self.program
        self.program_entry.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.program_entry.bind('<Key-Return>', self.program_callback)
        self.program_entry.bind('<FocusOut>', self.program_callback)
        the_row = the_row + 1

        self.walking_label = tkinter.ttk.Label(self.frame, text="Walking")
        self.walking_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.walking = tkinter.IntVar()
        self.walking = self.xml_form['voices'][int(self.voice_number.get())]['walking']
        self.walking_checkbutton = tkinter.ttk.Checkbutton(self.frame, text="walking",
            variable=self.walking, command=self.walking_callback)
        self.walking_checkbutton.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1

        self.pan_scroll_label = tkinter.ttk.Label(self.frame, text="-pi..................0..................pi")
        self.pan_scroll_label.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1

        self.pan_label = tkinter.ttk.Label(self.frame, text="Pan")
        self.pan_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.pan_scrollbar = tkinter.ttk.Scrollbar(self.frame, command=self.pan_callback)
        self.pan_scrollbar["orient"] = "horizontal"
        self.pan_scrollbar.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        temppan = self.xml_form['voices'][int(self.voice_number.get())]['pan'] * 128 - 64

        self.pan_scrollbar.set(temppan, temppan)
        the_row = the_row + 1

        self.follow_label = tkinter.ttk.Label(self.frame, text="Follow")
        self.follow_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.follow = tkinter.IntVar()
        self.follow.set(self.xml_form['voices'][int(self.voice_number.get())]['follower']['follow'])
        self.follow_checkbutton = tkinter.ttk.Checkbutton(self.frame, text="follow",
            variable=self.follow, command=self.follow_callback)
        self.follow_checkbutton.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1

        self.leader_label = tkinter.ttk.Label(self.frame, text="Leader")
        self.leader_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.leader_spinbox = tkinter.ttk.Spinbox(self.frame, command=self.leader_callback)
        self.leader_spinbox["increment"] = 1.0
        self.leader_spinbox["from"] = 0.0
        self.leader_spinbox["to"] = 63.0
        if (self.xml_form['voices'][int(self.voice_number.get())]['follower']['follow'] == 1):
            self.leader_spinbox["state"] = "enabled"
        else:
            self.leader_spinbox["state"] = "disabled"
        self.leader_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.leader_spinbox.bind('<Key-Return>', self.leader_callback)
        self.leader_spinbox.bind('<FocusOut>', self.leader_callback)
        self.leader_spinbox.bind('<ButtonRelease-1>', self.leader_callback)
        self.leader = tkinter.StringVar()
        self.leader.set(self.xml_form['voices'][int(self.voice_number.get())]['follower']['leader'])
        self.leader_spinbox["textvariable"] = self.leader
        the_row = the_row + 1

        self.scaler_interval_type_label = tkinter.ttk.Label(self.frame, text="Interval Type")
        self.scaler_interval_type_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.frame.interval_type = tkinter.StringVar()
        self.frame.interval_type.set(self.xml_form['voices'][int(self.voice_number.get())]['follower']['interval_type'])
        self.scaler_interval_type_radiobutton = tkinter.ttk.Radiobutton(self.frame,
            text='Scaler', variable=self.frame.interval_type, value="1",
            command=self.interval_type_callback)
        self.scaler_interval_type_radiobutton.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)

        self.chromatic_interval_type_radiobutton = tkinter.ttk.Radiobutton(self.frame,
            text='Chromatic', variable=self.frame.interval_type, value="2",
            command=self.interval_type_callback)
        if (self.xml_form['voices'][int(self.voice_number.get())]['follower']['follow'] == '1'):
            self.scaler_interval_type_radiobutton["state"] = "enabled"
            self.chromatic_interval_type_radiobutton["state"] = "enabled"
        else:
            self.scaler_interval_type_radiobutton["state"] = "disabled"
            self.chromatic_interval_type_radiobutton["state"] = "disabled"
        self.chromatic_interval_type_radiobutton["state"] = "disabled"
        self.chromatic_interval_type_radiobutton.grid(row=the_row, column=2, sticky=NSEW, padx=2, pady=2)
        the_row = the_row + 1

        self.follow_interval_label = tkinter.ttk.Label(self.frame, text="Interval")
        self.follow_interval_label.grid(row=the_row, column=0, sticky=NSEW, padx=2, pady=2)
        self.follow_interval_spinbox = tkinter.ttk.Spinbox(self.frame, command=self.follow_interval_callback)
        self.follow_interval_spinbox["increment"] = 1.0
        self.follow_interval_spinbox["from"] = -24.0
        self.follow_interval_spinbox["to"] = 24.0
        self.follow_interval_spinbox["state"] = "disabled"
        self.follow_interval_spinbox.grid(row=the_row, column=1, sticky=NSEW, padx=2, pady=2)
        self.follow_interval_spinbox.bind('<Key-Return>', self.follow_interval_callback)
        self.follow_interval_spinbox.bind('<FocusOut>', self.follow_interval_callback)
        self.follow_interval_spinbox.bind('<ButtonRelease-1>', self.follow_interval_callback)
        self.follow_interval = tkinter.StringVar()
        self.follow_interval.set(self.xml_form['voices'][int(self.voice_number.get())]['follower']['interval'])
        self.follow_interval_spinbox["textvariable"] = self.follow_interval
        the_row = the_row + 1

    def low_pitch_callback(self, event):
        self.xml_form['voices'][int(self.voice_number.get())]['low_pitch'] = self.low_pitch.get()

    def high_pitch_callback(self, event):
        self.xml_form['voices'][int(self.voice_number.get())]['high_pitch'] = self.high_pitch.get()

    def follow_interval_callback(self, event=None):
        self.xml_form['voices'][int(self.voice_number.get())]['follower']['interval'] = self.follow_interval.get()

    def interval_type_callback(self):
        self.xml_form['voices'][int(self.voice_number.get())]['follower']['interval_type'] = self.frame.interval_type.get()

    def number_of_voices_callback(self, event=None):
        number_of_voices = int(self.number_of_voices.get())
        voice_number = int(self.voice_number.get())
        if (voice_number >= number_of_voices):
            voice_number = (number_of_voices - 1)

        if (len(self.xml_form['voices']) > number_of_voices):
            self.xml_form['voices'] = self.xml_form['voices'][0:(number_of_voices - 1)]
        if (len(self.xml_form['voices']) < number_of_voices):
            for v in range(len(self.xml_form['voices']), number_of_voices):
                vd = {}
                vd['low_pitch'] = "A0"
                vd['high_pitch'] = "C8"
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
        self.voice_number_spinbox["to"] = len(self.xml_form['voices']) - 1

    def voice_number_callback(self, event=None):
        voice_number = int(self.voice_number.get())
        self.low_pitch.set(self.xml_form['voices'][voice_number]['low_pitch'])
        self.low_pitch_entry.update()
        self.high_pitch.set(self.xml_form['voices'][voice_number]['high_pitch'])
        self.high_pitch_entry.update()
        self.channel.set(self.xml_form['voices'][voice_number]['channel'])
        self.walking = self.xml_form['voices'][voice_number]['walking']
        if (self.walking_checkbutton.instate(['selected']) and not self.walking):
            self.walking_checkbutton.invoke()
        if (not self.walking_checkbutton.instate(['selected']) and self.walking):
            self.walking_checkbutton.invoke()

        self.program_entry.delete(0, 1024)
        self.program_entry.insert(0, self.xml_form['voices'][voice_number]['program'])
        self.program_entry.update()
        the_pan = float(self.xml_form['voices'][voice_number]['pan']) / 128.0 + 0.5
        self.pan_scrollbar.set(the_pan, the_pan)
        self.pan_scrollbar.update()

        self.follow = self.xml_form['voices'][voice_number]['follower']['follow']
        if (self.follow_checkbutton.instate(['selected']) and not self.follow):
            self.follow_checkbutton.invoke()
        if (not self.follow_checkbutton.instate(['selected']) and self.follow):
            self.follow_checkbutton.invoke()

        self.leader.set(self.xml_form['voices'][voice_number]['follower']['leader'])
        self.leader_spinbox.update()

        self.frame.interval_type.set(self.xml_form['voices'][int(self.voice_number.get())]['follower']['interval_type'])
        if (self.xml_form['voices'][voice_number]['follower']['follow'] == 1):
            self.leader_spinbox["state"] = "enabled"
            self.scaler_interval_type_radiobutton["state"] = "enabled"
            self.chromatic_interval_type_radiobutton["state"] = "enabled"
            self.follow_interval.set(self.xml_form['voices'][voice_number]['follower']['interval'])
            self.frame.interval_type.set(self.xml_form['voices'][voice_number]['follower']['interval_type'])
            self.scaler_interval_type_radiobutton.update()
            self.chromatic_interval_type_radiobutton.update()
            self.follow_interval_spinbox.update()
        else:
            self.leader_spinbox["state"] = "disabled"
            self.scaler_interval_type_radiobutton["state"] = "disabled"
            self.chromatic_interval_type_radiobutton["state"] = "disabled"
            self.leader.set('2147483647')

    def leader_callback(self, event=None):
        self.xml_form['voices'][int(self.voice_number.get())]['follower']['leader'] = self.leader.get()

    def channel_callback(self, event=None):
        self.xml_form['voices'][int(self.voice_number.get())]['channel'] = self.channel.get()

    def program_callback(self, event):
        self.xml_form['voices'][int(self.voice_number.get())]['program'] = self.program.get()

    def walking_callback(self):
        if (self.walking_checkbutton.instate(['selected'])):
            self.xml_form['voices'][int(self.voice_number.get())]['walking'] = 1
        else:
            self.xml_form['voices'][int(self.voice_number.get())]['walking'] = 0

    def install_xml_form(self, xml_form):
        self.xml_form = xml_form
        self.number_of_voices.set(len(self.xml_form['voices']))
        self.voice_number_spinbox["to"] = len(self.xml_form['voices']) - 1
        self.voice_number.set("0")
        voice_number = 0
        self.low_pitch.set(self.xml_form['voices'][voice_number]['low_pitch'])
        self.low_pitch_entry.update()
        self.high_pitch.set(self.xml_form['voices'][voice_number]['high_pitch'])
        self.high_pitch_entry.update()
        self.channel.set(self.xml_form['voices'][voice_number]['channel'])
        self.channel_spinbox.update()
        self.program.set(self.xml_form['voices'][voice_number]['program'])
        self.program_entry.update()

        self.walking = self.xml_form['voices'][voice_number]['walking']
        if (self.walking_checkbutton.instate(['selected']) and not self.walking):
            self.walking_checkbutton.invoke()
        if (not self.walking_checkbutton.instate(['selected']) and self.walking):
            self.walking_checkbutton.invoke()

        the_pan = float(self.xml_form['voices'][voice_number]['pan']) / 128.0 + 0.5
        self.pan_scrollbar.set(the_pan, the_pan)
        self.pan_scrollbar.update()

        self.follow = self.xml_form['voices'][voice_number]['follower']['follow']
        if (self.follow_checkbutton.instate(['selected']) and not self.follow):
            self.follow_checkbutton.invoke()
        if (not self.follow_checkbutton.instate(['selected']) and self.follow):
            self.follow_checkbutton.invoke()

        self.leader.set(self.xml_form['voices'][voice_number]['follower']['leader'])
        self.leader_spinbox.update()
        if (self.xml_form['voices'][voice_number]['follower']['interval_type'] == 0):
            self.scaler_interval_type_radiobutton["state"] = "disabled"
            self.scaler_interval_type_radiobutton["state"] = "disabled"
        if (self.xml_form['voices'][voice_number]['follower']['interval_type'] == 1):
            self.scaler_interval_type_radiobutton["state"] = "enabled"
            self.scaler_interval_type_radiobutton["state"] = "disabled"
        if (self.xml_form['voices'][voice_number]['follower']['interval_type'] == 2):
            self.scaler_interval_type_radiobutton["state"] = "disabled"
            self.scaler_interval_type_radiobutton["state"] = "enabled"
        self.scaler_interval_type_radiobutton.update()
        self.follow_interval.set(self.xml_form['voices'][voice_number]['follower']['interval'])
        self.follow_interval_spinbox.update()
        self.frame.update()

    def follow_callback(self):
        voice_number = int(self.voice_number.get())
        if (self.follow_checkbutton.instate(['selected'])):
            self.xml_form['voices'][voice_number]['follower']['follow'] = 1
            self.walking_checkbutton["state"] = "disabled"
            self.leader_spinbox["state"] = "normal"
            self.scaler_interval_type_radiobutton["state"] = "normal"
            self.chromatic_interval_type_radiobutton["state"] = "normal"
            self.follow_interval_spinbox["state"] = "normal"
        else:
            self.xml_form['voices'][voice_number]['follower']['follow'] = 0
            self.walking_checkbutton["state"] = "normal"
            self.leader_spinbox["state"] = "disabled"
            self.scaler_interval_type_radiobutton["state"] = "disabled"
            self.chromatic_interval_type_radiobutton["state"] = "disabled"
            self.follow_interval_spinbox["state"] = "disabled"

    def pan_callback(self, action, sign, unitspage):
        firstlastlist = self.pan_scrollbar.get()
        setting = 0
        if (action == "scroll"):
            if (unitspage == "pages"):
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
                self.xml_form['voices'][int(self.voice_number.get())]['pan'] = value
            else:
                if (unitspage == "units"):
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
                    self.xml_form['voices'][int(self.voice_number.get())]['pan'] = value
        else:
            if (action == "goto"):
                setting = firstlastlist[0]
                self.pan_scrollbar.set(firstlastlist[0])
                value = (setting - 0.5) * 128
                if (value >= 64):
                    value = 63
                self.xml_form['voices'][int(self.voice_number.get())]['pan'] = value

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

