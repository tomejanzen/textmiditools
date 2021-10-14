#!/usr/bin/env python3
# TextMIDITools Version 1.0.6
# textmidiform.py 1.0
# Copyright © 2021 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#
import math
import tkinter
import tkinter.ttk
from tkinter import *
from tkinter.ttk import *
import tkinter, tkinter.constants, tkinter.filedialog
import re

from form import FormWindow
from sine import Sine

class ScaleWindow(tkinter.Frame):
    scale_names = ['Unnamed', 'Diatonic', 'Minor', 'WholeTone', 'Diminished', 'Chromatic', 'Pentatonic', 'Tritone']
    scale_steps_dict = {'Unnamed'    : [0],
                        'Diatonic'   : [0, 2, 4, 5, 7, 9, 11],
                        'Minor'      : [0, 2, 3, 5, 7, 8, 11],
                        'WholeTone'  : [0, 2, 4, 6, 8, 10],
                        'Diminished' : [0, 2, 3, 5, 6, 8, 9, 11],
                        'Chromatic'  : [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11],
                        'Pentatonic' : [0, 2, 5, 7, 9],
                        'Tritone' : [0, 2, 4, 5, 6, 8, 10, 11]}
    full_midi_scale = ["C-1", "C#-1", "D-1", "D#-1", "E-1", "F-1", "F#-1", "G-1", "G#-1", "A-1", "A#-1", "B-1",
                       "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
                       "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
                       "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
                       "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
                       "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
                       "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
                       "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
                       "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
                       "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
                       "C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9"]

    def __init__(self, master=None, xml_form=None, the_row = 0):
        super().__init__(master)
        self.grid()
        self.scales_dict = {}
        for sn in self.scale_names:
            self.scales_dict[sn] = []
            for octave in range(0, 10):
                for step in self.scale_steps_dict[sn]:
                    key = octave * 12 + step
                    if (key < 128):
                        self.scales_dict[sn].append(key)
        self.the_row = the_row
        self.xml_form = xml_form
        self.transpose_num = 0
        self.create_widgets()

    def create_widgets(self):
        self.the_row = 0

        self.rowconfigure(index=self.the_row, weight=1)
        self.columnconfigure(index=0, weight=1)
        self.columnconfigure(index=1, weight=1)
        self.scale_name_label = tkinter.ttk.Label(self, text="Scale")
        self.scale_name_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.scale_name_spinbox = tkinter.ttk.Spinbox(self)
        self.scale_name_spinbox["values"] = self.scale_names
        self.scale_name_spinbox.grid(row=self.the_row, column=1, sticky=NSEW)
        self.scale_name_spinbox.bind('<Key-Return>', self.scale_name_callback)
        self.scale_name_spinbox.bind('<FocusOut>', self.scale_name_callback)
        self.scale_name = tkinter.StringVar()
        self.scale_name.set("Unnamed")
        self.scale_name_spinbox["textvariable"] = self.scale_name
        self.the_row = self.the_row + 1

        self.rowconfigure(index=self.the_row, weight=1)
        self.transpose_label = tkinter.ttk.Label(self, text="Tranpose")
        self.transpose_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.transpose_spinbox = tkinter.ttk.Spinbox(self)
        self.transpose_spinbox["increment"] = 1
        self.transpose_spinbox["from"]      = 0
        self.transpose_spinbox["to"]        = 11
        self.transpose_spinbox.grid(row=self.the_row, column=1, sticky=NSEW)
        self.transpose_spinbox.bind('<Key-Return>', self.transpose_callback)
        self.transpose_spinbox.bind('<FocusOut>', self.transpose_callback)
        self.transpose = tkinter.StringVar()
        self.transpose.set("0")
        self.transpose_spinbox["textvariable"] = self.transpose
        self.the_row = self.the_row + 1

    def transpose_callback(self, event):
        self.tranpose_num = self.transpose.get()

    def scale_name_callback(self, event):
        scale_name = self.scale_name.get()
        self.xml_form['scale'] = []
        for keynum in self.scales_dict[scale_name]:
            self.xml_form['scale'].append(self.full_midi_scale[keynum + int(self.transpose.get())])

    def install_xml_form(self, xml_form):
        self.scale = (self.xml_form['scale'])
        self.update()


class AllFormsWindow(tkinter.Frame):
    twopi = 2.0 * math.pi
    def __init__(self, master, xml_form):
        self.the_row = 0
        super().__init__(master)
        #self.grid(sticky=NSEW, padx=100)
        self.xml_form = xml_form
        self.create_widgets()
        self.grid(sticky="nw", row=0, column=0)
        self.rowconfigure(index=0, weight=1)
        self.columnconfigure(index=0, weight=1)

    def create_widgets(self):
        self.the_row = 0

        self.rowconfigure(index=self.the_row, weight=1)
        self.name_label = tkinter.ttk.Label(self, text="Name")
        self.name_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.name_entry = tkinter.ttk.Entry(self)
        self.name = tkinter.StringVar()
        self.name.set(self.xml_form['name'])
        self.name_entry["textvariable"] = self.name
        self.name_entry.grid(row=self.the_row, column=1, sticky=NSEW)
        self.name_entry.bind('<Key-Return>', self.name_callback)
        self.name_entry.bind('<FocusOut>', self.name_callback)
        self.the_row = self.the_row + 1

        self.rowconfigure(index=self.the_row, weight=1)
        self.columnconfigure(index=0, weight=1)
        self.columnconfigure(index=1, weight=1)
        self.len_label = tkinter.ttk.Label(self, text="Len")
        self.len_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.len_entry = tkinter.ttk.Entry(self)
        self.len = tkinter.StringVar()
        self.len.set(self.xml_form['len'])
        self.len_entry["textvariable"] = self.len
        self.len_entry.grid(row=self.the_row, column=1, sticky=NSEW)
        self.len_entry.bind('<Key-Return>', self.len_callback)
        self.len_entry.bind('<FocusOut>', self.len_callback)
        self.the_row = self.the_row + 1

        self.rowconfigure(index=self.the_row, weight=1)
        self.min_note_len_label = tkinter.ttk.Label(self, text="Min Note Len")
        self.min_note_len_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.min_note_len_entry = tkinter.ttk.Entry(self)
        self.min_note_len = tkinter.StringVar()
        self.min_note_len.set(self.xml_form['min_note_len'])
        self.min_note_len_entry["textvariable"] = self.min_note_len
        self.min_note_len_entry.grid(row=self.the_row, column=1, sticky=NSEW)
        self.min_note_len_entry.bind('<Key-Return>', self.min_note_len_callback)
        self.min_note_len_entry.bind('<FocusOut>', self.min_note_len_callback)
        self.the_row = self.the_row + 1

        self.rowconfigure(index=self.the_row, weight=1)
        self.max_note_len_label = tkinter.ttk.Label(self, text="Max Note Len")
        self.max_note_len_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.max_note_len_entry = tkinter.ttk.Entry(self)
        self.max_note_len = tkinter.StringVar()
        self.max_note_len.set(self.xml_form['max_note_len'])
        self.max_note_len_entry["textvariable"] = self.max_note_len
        self.max_note_len_entry.grid(row=self.the_row, column=1, sticky=NSEW)
        self.max_note_len_entry.bind('<Key-Return>', self.max_note_len_callback)
        self.max_note_len_entry.bind('<FocusOut>', self.max_note_len_callback)
        self.the_row = self.the_row + 1

        self.rowconfigure(index=self.the_row, weight=1)
        self.pulse_label = tkinter.ttk.Label(self, text="Pulse/Sec")
        self.pulse_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.pulse_entry = tkinter.ttk.Entry(self)
        self.pulse = tkinter.StringVar()
        self.pulse.set(self.xml_form['pulse'])
        self.pulse_entry["textvariable"] = self.pulse
        self.pulse_entry.grid(row=self.the_row, column=1, sticky=NSEW)
        self.pulse_entry.bind('<Key-Return>', self.pulse_callback)
        self.pulse_entry.bind('<FocusOut>', self.pulse_callback)
        self.the_row = self.the_row + 1

        self.rowconfigure(index=self.the_row, weight=1)
        self.melody_probabilities_label = tkinter.ttk.Label(self, text="Melody Probabilities")
        self.melody_probabilities_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.melody_probabilities_entry = tkinter.ttk.Entry(self)

        self.rowconfigure(index=self.the_row, weight=1)
        self.down_label = tkinter.ttk.Label(self, text="Down")
        self.down_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.down_entry = tkinter.ttk.Entry(self)
        self.down = tkinter.StringVar()
        self.down.set(self.xml_form['melody_probabilities']['down'])
        self.down_entry["textvariable"] = self.down
        self.down_entry.grid(row=self.the_row, column=1, sticky=NSEW)
        self.down_entry.bind('<Key-Return>', self.melody_down_callback)
        self.down_entry.bind('<FocusOut>', self.melody_down_callback)
        self.the_row = self.the_row + 1

        self.rowconfigure(index=self.the_row, weight=1)
        self.same_label = tkinter.ttk.Label(self, text="Same")
        self.same_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.same_entry = tkinter.ttk.Entry(self)
        self.same = tkinter.StringVar()
        self.same.set(self.xml_form['melody_probabilities']['same'])
        self.same_entry["textvariable"] = self.same
        self.same_entry.grid(row=self.the_row, column=1, sticky=NSEW)
        self.same_entry.bind('<Key-Return>', self.melody_same_callback)
        self.same_entry.bind('<FocusOut>', self.melody_same_callback)
        self.the_row = self.the_row + 1

        self.rowconfigure(index=self.the_row, weight=1)
        self.up_label = tkinter.ttk.Label(self, text="Up")
        self.up_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.up_entry = tkinter.ttk.Entry(self)
        self.up = tkinter.StringVar()
        self.up.set(self.xml_form['melody_probabilities']['up'])
        self.up_entry["textvariable"] = self.up
        self.up_entry.grid(row=self.the_row, column=1, sticky=NSEW)
        self.up_entry.bind('<Key-Return>', self.melody_up_callback)
        self.up_entry.bind('<FocusOut>', self.melody_up_callback)
        self.the_row = self.the_row + 1

        self.scale_window = ScaleWindow(self, self.xml_form, self.the_row)
        self.scale_window.grid(sticky="w", row=self.the_row, column=1)
        self.scale_window.rowconfigure(index=2, weight=1)
        self.scale_window.columnconfigure(index=1, weight=1)

        self.the_row = self.the_row + 2

        self.form_curves_label = tkinter.ttk.Label(self, text="Form Curves")
        self.form_curves_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.the_row = self.the_row + 1
        self.pitch_label = tkinter.ttk.Label(self, text="Pitch")
        self.pitch_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.pitch_form = FormWindow(master=self, arow=self.the_row, mean_period_callback=self.pitch_mean_period_callback, mean_phase_callback=self.pitch_mean_phase_callback, range_period_callback=self.pitch_range_period_callback, range_phase_callback=self.pitch_range_phase_callback, xml_subform=self.xml_form['pitch_form'])
        self.the_row = self.the_row + 4

        self.rhythm_label = tkinter.ttk.Label(self, text="Rhythm")
        self.rhythm_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.the_row = self.the_row + 1
        self.rhythm_form = FormWindow(master=self, arow=self.the_row, mean_period_callback=self.rhythm_mean_period_callback, mean_phase_callback=self.rhythm_mean_phase_callback, range_period_callback=self.rhythm_range_period_callback, range_phase_callback=self.rhythm_range_phase_callback, xml_subform=self.xml_form['rhythm_form'])
        self.the_row = self.the_row + 4

        self.dynamic_label = tkinter.ttk.Label(self, text="Dynamic")
        self.dynamic_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.the_row = self.the_row + 1
        self.dynamic_form = FormWindow(master=self, arow=self.the_row, mean_period_callback=self.dynamic_mean_period_callback, mean_phase_callback=self.dynamic_mean_phase_callback, range_period_callback=self.dynamic_range_period_callback, range_phase_callback=self.dynamic_range_phase_callback, xml_subform=self.xml_form['dynamic_form'])

        self.the_row = self.the_row + 4

        self.texture_label = tkinter.ttk.Label(self, text="Texture")
        self.texture_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.the_row = self.the_row + 1
        self.texture_form = Sine(master=self, arow=self.the_row, period_callback=self.texture_range_period_callback, phase_callback=self.texture_range_phase_callback, xml_sine=self.xml_form['texture_form'])

        #self.the_row = self.the_row + 2

    def name_callback(self, event):
        self.xml_form['name'] = self.name.get()

    def len_callback(self, event):
        self.xml_form['len'] = self.len.get()

    def min_note_len_callback(self, event):
        self.xml_form['min_note_len'] = self.min_note_len.get()

    def max_note_len_callback(self, event):
        self.xml_form['max_note_len'] = self.max_note_len.get()

    def pulse_callback(self, event):
        self.xml_form['pulse'] = self.pulse.get()

    def melody_down_callback(self, event):
        self.xml_form['melody_probabilities']['down'] = self.down.get()

    def melody_same_callback(self, event):
        self.xml_form['melody_probabilities']['same'] = self.same.get()

    def melody_up_callback(self, event):
        self.xml_form['melody_probabilities']['up'] = self.up.get()

    def pitch_mean_period_callback(self, event):
        self.xml_form['pitch_form']['mean']['period'] = self.pitch_form.mean.period_entry.get()

    def pitch_mean_phase_callback(self, action, sign, unitspage=None):
        self.pitch_form.mean.phase_scrollbar.update()
        firstlastlist = self.pitch_form.mean.phase_scrollbar.get()
        if (action == "scroll"):
            if (unitspage == "pages"):
                delta = float(sign) * 1.0 / 4
                setting = firstlastlist[0]
                if ((setting <= 0.0) and (delta < 0.0)):
                    delta = 0.0
                if ((setting >= 1.0) and (delta > 0.0)):
                    delta = 0.0
                setting = setting + delta
                self.pitch_form.mean.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['pitch_form']['mean']['phase'] = value
            else:
                if (unitspage == "units"):
                    delta = float(sign) * 1.0 / 32
                    setting = firstlastlist[0]
                    if ((setting <= 0.0) and (delta < 0.0)):
                        delta = 0.0
                    if ((setting >= 1.0) and (delta > 0.0)):
                        delta = 0.0
                    setting = setting + delta
                    self.pitch_form.mean.phase_scrollbar.set(setting, setting)
                    value = (setting - 0.5) * self.twopi
                    self.xml_form['pitch_form']['mean']['phase'] = value
        else:
            if (action == "goto"):
                setting = firstlastlist[0]
                self.pitch_form.mean.phase_scrollbar.set(setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['pitch_form']['mean']['phase'] = value

    def pitch_range_period_callback(self, event):
        self.xml_form['pitch_form']['range']['period'] = self.pitch_form.range.period_entry.get()

    def pitch_range_phase_callback(self, action, sign, unitspage=None):
        firstlastlist = self.pitch_form.range.phase_scrollbar.get()
        if (action == "scroll"):
            if (unitspage == "pages"):
                delta = float(sign) * 1.0 / 4
                setting = firstlastlist[0]
                if ((setting <= 0.0) and (delta < 0.0)):
                    delta = 0.0
                if ((setting >= 1.0) and (delta > 0.0)):
                    delta = 0.0
                setting = setting + delta
                self.pitch_form.range.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['pitch_form']['range']['phase'] = value
            else:
                if (unitspage == "units"):
                    delta = float(sign) * 1.0 / 32
                    setting = firstlastlist[0]
                    if ((setting <= 0.0) and (delta < 0.0)):
                        delta = 0.0
                    if ((setting >= 1.0) and (delta > 0.0)):
                        delta = 0.0
                    setting = setting + delta
                    self.pitch_form.range.phase_scrollbar.set(setting, setting)
                    value = (setting - 0.5) * self.twopi
                    self.xml_form['pitch_form']['range']['phase'] = value
        else:
            if (action == "goto"):
                setting = firstlastlist[0]
                self.pitch_form.range.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['pitch_form']['range']['phase'] = value

    def rhythm_mean_period_callback(self, event):
        self.xml_form['rhythm_form']['mean']['period'] = self.rhythm_form.mean.period_entry.get()

    def rhythm_mean_phase_callback(self, action, sign, unitspage):
        firstlastlist = self.rhythm_form.mean.phase_scrollbar.get()
        if (action == "scroll"):
            if (unitspage == "pages"):
                delta = float(sign) * 1.0 / 4
                setting = firstlastlist[0]
                if ((setting <= 0.0) and (delta < 0.0)):
                    delta = 0.0
                if ((setting >= 1.0) and (delta > 0.0)):
                    delta = 0.0
                setting = setting + delta
                self.rhythm_form.mean.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['rhythm_form']['mean']['phase'] = value
            else:
                if (unitspage == "units"):
                    delta = float(sign) * 1.0 / 32
                    setting = firstlastlist[0]
                    if ((setting <= 0.0) and (delta < 0.0)):
                        delta = 0.0
                    if ((setting >= 1.0) and (delta > 0.0)):
                        delta = 0.0
                    setting = setting + delta
                    self.rhythm_form.mean.phase_scrollbar.set(setting, setting)
                    value = (setting - 0.5) * self.twopi
                    self.xml_form['rhythm_form']['mean']['phase'] = value
        else:
            if (action == "goto"):
                setting = firstlastlist[0]
                value = (setting - 0.5) * self.twopi
                self.pitch_form.range.phase_scrollbar.set(setting, setting)
                self.xml_form['rhythm_form']['mean']['phase'] = value

    def rhythm_range_period_callback(self, event):
        self.xml_form['rhythm_form']['range']['period'] = self.rhythm_form.range.period_entry.get()

    def rhythm_range_phase_callback(self, action, sign, unitspage=None):
        firstlastlist = self.rhythm_form.range.phase_scrollbar.get()
        if (action == "scroll"):
            if (unitspage == "pages"):
                delta = float(sign) * 1.0 / 4
                setting = firstlastlist[0]
                if ((setting <= 0.0) and (delta < 0.0)):
                    delta = 0.0
                if ((setting >= 1.0) and (delta > 0.0)):
                    delta = 0.0
                setting = setting + delta
                self.rhythm_form.range.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['rhythm_form']['range']['phase'] = value
            else:
                if (unitspage == "units"):
                    delta = float(sign) * 1.0 / 32
                    setting = firstlastlist[0]
                    if ((setting <= 0.0) and (delta < 0.0)):
                        delta = 0.0
                    if ((setting >= 1.0) and (delta > 0.0)):
                        delta = 0.0
                    setting = setting + delta
                    self.rhythm_form.range.phase_scrollbar.set(setting, setting)
                    value = (setting - 0.5) * self.twopi
                    self.xml_form['rhythm_form']['range']['phase'] = value
        else:
            if (action == "goto"):
                setting = firstlastlist[0]
                self.rhythms_form.range.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['rhythm_form']['range']['phase'] = value

    def dynamic_mean_period_callback(self, event):
        self.xml_form['dynamic_form']['mean']['period'] = self.dynamic_form.mean.period_entry.get()

    def dynamic_mean_phase_callback(self, action, sign, unitspage):
        firstlastlist = self.dynamic_form.mean.phase_scrollbar.get()
        if (action == "scroll"):
            if (unitspage == "pages"):
                delta = float(sign) * 1.0 / 4
                setting = firstlastlist[0]
                if ((setting <= 0.0) and (delta < 0.0)):
                    delta = 0.0
                if ((setting >= 1.0) and (delta > 0.0)):
                    delta = 0.0
                setting = setting + delta
                self.dynamic_form.mean.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['dynamic_form']['mean']['phase'] = value
            else:
                if (unitspage == "units"):
                    delta = float(sign) * 1.0 / 32
                    setting = firstlastlist[0]
                    if ((setting <= 0.0) and (delta < 0.0)):
                        delta = 0.0
                    if ((setting >= 1.0) and (delta > 0.0)):
                        delta = 0.0
                    setting = setting + delta
                    self.dynamic_form.mean.phase_scrollbar.set(setting, setting)
                    value = (setting - 0.5) * self.twopi
                    self.xml_form['dynamic_form']['mean']['phase'] = value
        else:
            if (action == "goto"):
                setting = firstlastlist[0]
                self.dynamic_form.mean.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['dynamic_form']['mean']['phase'] = value

    def dynamic_range_period_callback(self, event):
        self.xml_form['dynamic_form']['range']['period'] = self.dynamic_form.range.period_entry.get()

    def dynamic_range_phase_callback(self, action, sign, unitspage=None):
        firstlastlist = self.dynamic_form.range.phase_scrollbar.get()
        if (action == "scroll"):
            if (unitspage == "pages"):
                delta = float(sign) * 1.0 / 4
                setting = firstlastlist[0]
                if ((setting <= 0.0) and (delta < 0.0)):
                    delta = 0.0
                if ((setting >= 1.0) and (delta > 0.0)):
                    delta = 0.0
                setting = setting + delta
                self.dynamic_form.range.phase_scrollbar.set(firstlastlist[0] + delta, firstlastlist[0] + delta)
                value = (setting - 0.5) * self.twopi
                self.xml_form['dynamic_form']['range']['phase'] = value
            else:
                if (unitspage == "units"):
                    delta = float(sign) * 1.0 / 32
                    setting = firstlastlist[0]
                    if ((setting <= 0.0) and (delta < 0.0)):
                        delta = 0.0
                    if ((setting >= 1.0) and (delta > 0.0)):
                        delta = 0.0
                    setting = setting + delta
                    self.dynamic_form.range.phase_scrollbar.set(firstlastlist[0] + delta, firstlastlist[0] + delta)
                    value = (setting - 0.5) * self.twopi
                    self.xml_form['dynamic_form']['range']['phase'] = value
        else:
            if (action == "goto"):
                setting = firstlastlist[0]
                self.dynamic_form.range.phase_scrollbar.set(firstlastlist[0])
                value = (setting - 0.5) * self.twopi
                self.xml_form['dynamic_form']['range']['phase'] = value

    def texture_range_period_callback(self, event):
        self.xml_form['texture_form']['period'] = self.texture_form.period_entry.get()

    def texture_range_phase_callback(self, action, sign, unitspage=None):
        firstlastlist = self.texture_form.phase_scrollbar.get()
        if (action == "scroll"):
            if (unitspage == "pages"):
                delta = float(sign) * 1.0 / 4
                setting = firstlastlist[1]
                if ((setting <= 0.0) and (delta < 0.0)):
                    delta = 0.0
                if ((setting >= 1.0) and (delta > 0.0)):
                    delta = 0.0
                setting = setting + delta
                self.texture_form.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['texture_form']['phase'] = value
            else:
                if (unitspage == "units"):
                    delta = float(sign) * 1.0 / 32
                    setting = firstlastlist[1]
                    if ((setting <= 0.0) and (delta < 0.0)):
                        delta = 0.0
                    if ((setting >= 1.0) and (delta > 0.0)):
                        delta = 0.0
                    setting = setting + delta
                    self.texture_form.phase_scrollbar.set(setting, setting)
                    value = (setting - 0.5) * self.twopi
                    self.xml_form['texture_form']['phase'] = value
        else:
            if (action == "goto"):
                setting = firstlastlist[0]
                self.texture_form.range.phase_scrollbar.set(firstlastlist[0])
                value = (setting - 0.5) * self.twopi
                self.xml_form['texture_form']['phase'] = value

    def install_xml_form(self, xml_form):
        self.xml_form = xml_form
        self.pitch_form.install_xml_subform(self.xml_form['pitch_form'])
        self.pitch_form.update()
        self.rhythm_form.install_xml_subform(self.xml_form['rhythm_form'])
        self.rhythm_form.update()
        self.dynamic_form.install_xml_subform(self.xml_form['dynamic_form'])
        self.dynamic_form.update()
        self.texture_form.install_xml_sine(self.xml_form['texture_form'])
        self.texture_form.update()
        self.update()


