#!/usr/bin/env python3
# TextMIDITools Version 1.0.19
# textmidiform.py 1.0
# Copyright © 2022 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#
import math
import tkinter
import tkinter.ttk
from tkinter import *
from tkinter import ttk
from tkinter.ttk import *
import tkinter, tkinter.constants, tkinter.filedialog
import re

from FormFrame import FormFrame
from Sine import Sine
from KeyboardWindow import KeyboardWindow

class ScaleFrame(tkinter.Frame):
    scale_names = []
    scale_steps_dict = {'Unnamed'    : [0, 2, 4, 5, 7, 9, 11],
                        'Diatonic'   : [0, 2, 4, 5, 7, 9, 11],
                        'Minor'      : [0, 2, 3, 5, 7, 8, 11],
                        'WholeTone'  : [0, 2, 4, 6, 8, 10],
                        'Diminished' : [0, 2, 3, 5, 6, 8, 9, 11],
                        'Chromatic'  : [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11],
                        'Pentatonic' : [0, 2, 5, 7, 9],
                        'Tritone' : [0, 2, 4, 5, 6, 8, 10, 11]}
    full_midi_scale = ['C-1', 'C#-1', 'D-1', 'D#-1', 'E-1', 'F-1', 'F#-1', 'G-1', 'G#-1', 'A-1', 'A#-1', 'B-1',
                       'C0', 'C#0', 'D0', 'D#0', 'E0', 'F0', 'F#0', 'G0', 'G#0', 'A0', 'A#0', 'B0',
                       'C1', 'C#1', 'D1', 'D#1', 'E1', 'F1', 'F#1', 'G1', 'G#1', 'A1', 'A#1', 'B1',
                       'C2', 'C#2', 'D2', 'D#2', 'E2', 'F2', 'F#2', 'G2', 'G#2', 'A2', 'A#2', 'B2',
                       'C3', 'C#3', 'D3', 'D#3', 'E3', 'F3', 'F#3', 'G3', 'G#3', 'A3', 'A#3', 'B3',
                       'C4', 'C#4', 'D4', 'D#4', 'E4', 'F4', 'F#4', 'G4', 'G#4', 'A4', 'A#4', 'B4',
                       'C5', 'C#5', 'D5', 'D#5', 'E5', 'F5', 'F#5', 'G5', 'G#5', 'A5', 'A#5', 'B5',
                       'C6', 'C#6', 'D6', 'D#6', 'E6', 'F6', 'F#6', 'G6', 'G#6', 'A6', 'A#6', 'B6',
                       'C7', 'C#7', 'D7', 'D#7', 'E7', 'F7', 'F#7', 'G7', 'G#7', 'A7', 'A#7', 'B7',
                       'C8', 'C#8', 'D8', 'D#8', 'E8', 'F8', 'F#8', 'G8', 'G#8', 'A8', 'A#8', 'B8',
                       'C9', 'C#9', 'D9', 'D#9', 'E9', 'F9', 'F#9', 'G9']
    notenames = {'C' : 0, 'D' : 2, 'E' : 4, 'F' : 5, 'G' : 7, 'A' : 9, 'B' : 11}
    accidentals = {'b' : -1, '#' : 1, 'bb' : -2, 'x' : 2}
    half_steps_per_octave = 12
    midi_keys_qty = 128
    midi_octaves_qty = 11

    def __init__(self, topwin):
        super().__init__(topwin)
        self.grid(row=0, padx=10, pady=10)
        self.scale_names = list(self.scale_steps_dict)
        self.scales_dict = {}
        self.scale_untransposed = []
        for sn in self.scale_names:
            self.scales_dict[sn] = []
            for octave in range(0, self.midi_octaves_qty):
                for step in self.scale_steps_dict[sn]:
                    key = octave * self.half_steps_per_octave + step
                    if (key < self.midi_keys_qty):
                        self.scales_dict[sn].append(key)
        self.scale_transposed = []
        for k in self.scales_dict['Chromatic'] :
            self.scale_untransposed.append(k)
            self.scale_transposed.append(k)
        self.transpose_num = 0
        self.create_widgets()

    def set_keyboard(self):
        midi_key_selects = []
        for s in range(0, len(self.full_midi_scale)):
            midi_key_selects.append(False)
        for k in self.scale_transposed:
            midi_key_selects[k] = True
        self.keyboard_window.keyboard_frame.set_selects(midi_key_selects)

    def set_keyboard_from_xml(self, xml_form):
        scale_ints = []
        for key_string in xml_form['scale']:
            key_string[:1].upper()
            notename = key_string[0]
            if (len(key_string) == 2): # C0
                keynum = ScaleFrame.notenames[notename] + ScaleFrame.half_steps_per_octave * (int(key_string[1]) + 1)
            if (len(key_string) == 3): # C-1 Cb1 C#1 Cx2
                if (key_string[1:3] == '-1'): # bottom octave
                    keynum = ScaleFrame.notenames[notename]
                else: # C#1
                    keynum = ScaleFrame.notenames[notename] + ScaleFrame.accidentals[key_string[1]] + ScaleFrame.half_steps_per_octave * (int(key_string[2]) + 1)
            if (len(key_string) == 4): # Db-1 Cbb1 C#-1 Ebb4
                if (key_string[2:4] == '-1'):
                    keynum = ScaleFrame.notenames[notename] + ScaleFrame.accidentals[key_string[1]]
                else:
                    if (key_string[1:3] == 'bb'):
                        keynum = ScaleFrame.notenames[notename] + ScaleFrame.accidentals[key_string[1:3]] + ScaleFrame.half_steps_per_octave * (int(key_string[3]) + 1)
            if (len(key_string) == 5): # Dbb-1 ;there is no Cbb-1
                if ((key_string[3:5] == '-1') and (key_string[1:3] == 'bb')):
                    keynum = ScaleFrame.notenames[notename] + ScaleFrame.accidentals[key_string[1:3]]
            scale_ints.append(keynum)

        midi_key_selects = []

        for s in range(0, len(ScaleFrame.full_midi_scale)):
            midi_key_selects.append(False)
        for si in scale_ints:
            midi_key_selects[si] = True
        self.keyboard_window.keyboard_frame.set_selects(midi_key_selects)
        self.scale_untransposed = []
        for k in range(0, len(midi_key_selects)):
            if (midi_key_selects[k]):
                self.scale_untransposed.append(k)
        self.transpose_scale()

    def create_widgets(self):
        the_row = 0

        self.keyboard_window = KeyboardWindow()
        self.keyboard_window.geometry('1500x162+50-50')
        self.set_keyboard()

        self.rowconfigure(index=the_row, weight=1)
        self.columnconfigure(index=0, weight=1)
        self.columnconfigure(index=1, weight=1)
        self.scale_name_label = tkinter.ttk.Label(self, text='Scale')
        self.scale_name_label.grid(row=the_row, column=0, sticky=NSEW)
        self.scale_name_spinbox = tkinter.ttk.Spinbox(self, wrap=True)
        self.scale_name_spinbox['values'] = self.scale_names
        self.scale_name_spinbox.grid(row=the_row, column=1, sticky=NSEW)
        self.scale_name_spinbox.bind('<ButtonRelease-1>', self.scale_name_callback)
        self.scale_name_spinbox.set('Unnamed')
        self.scale_name_spinbox['state'] = 'readonly'
        the_row = the_row + 1

        self.rowconfigure(index=the_row, weight=1)
        self.transpose_label = tkinter.ttk.Label(self, text='Transpose')
        self.transpose_label.grid(row=the_row, column=0, sticky=NSEW)
        self.transpose_spinbox = tkinter.ttk.Spinbox(self, wrap=True)
        self.transpose_spinbox['increment'] = 1
        self.transpose_spinbox['from']      = -5
        self.transpose_spinbox['to']        = 6
        self.transpose_spinbox.grid(row=the_row, column=1, sticky=NSEW)
        self.transpose_spinbox.bind('<ButtonRelease-1>', self.transpose_callback)
        self.transpose_spinbox.set(0)
        self.transpose_spinbox['state'] = 'readonly'
        self.from_keyboard_button = tkinter.ttk.Button(self, text='From Keyboard',
            command=self.from_keyboard_callback)
        self.from_keyboard_button.grid(row=the_row, column=2, stick=NSEW)
        the_row = the_row + 1

    def from_keyboard_callback(self):
        key_ints = self.keyboard_window.keyboard_frame.get_key_ints()
        self.scale_untransposed = []
        for key in key_ints:
            self.scale_untransposed.append(key)
        self.transpose_scale()
        self.scale_name_spinbox['state'] = '!readonly'
        self.scale_name_spinbox.set(self.scale_names[0])
        self.scale_name_spinbox['state'] = 'readonly'

        self.transpose_spinbox['state'] = '!readonly'
        self.transpose_spinbox.set(0)
        self.transpose_spinbox['state'] = 'readonly'

    def transpose_callback(self, event):
        self.transpose_num = int(self.transpose_spinbox.get())
        self.transpose_scale()
        self.set_keyboard()

    def scale_name_callback(self, event):
        self.scale_name_spinbox.update()
        scale_name = self.scale_name_spinbox.get()
        self.scale_untransposed = []
        for keynum in self.scales_dict[scale_name]:
            self.scale_untransposed.append(keynum)
        self.transpose_scale()
        self.set_keyboard()

    def transpose_scale(self):
        self.scale_transposed = []
        for keynum in self.scale_untransposed:
            key_index = keynum + self.transpose_num
            self.scale_transposed.append(key_index)

        for k in range(0, len(self.scale_transposed)):
            if (self.scale_transposed[k] < 0):
                self.scale_transposed[k] = self.scale_transposed[k] + 128

        for k in range(0, len(self.scale_transposed)):
            if (self.scale_transposed[k]  >= self.midi_keys_qty):
                self.scale_transposed[k]  = self.scale_transposed[k]  - 128

        low = 0
        for k in range(1, len(self.scale_transposed)):
            if (self.scale_transposed[k - 1] > self.scale_transposed[k]):
                low = k
        right_partition = self.scale_transposed[0:low]
        left_partition  = self.scale_transposed[low:]
        left_partition.extend(right_partition)
        self.scale_transposed = []
        for key in left_partition:
            self.scale_transposed.append(key)

    def get_string_scale(self):
        scale_strings = []
        for keynum in self.scale_transposed:
            scale_strings.append(self.full_midi_scale[keynum])
        return scale_strings

class AllFormsWindow(tkinter.Toplevel):
    twopi = 2.0 * math.pi
    def __init__(self, xml_form = None):
        super().__init__()
        self.frame = ttk.Frame(self, padding='1 1 1 1')
        self.xml_form = xml_form
        self.create_widgets()
        self.frame.grid(sticky='nw', row=0, column=0)
        self.frame.rowconfigure(index=0, weight=1)
        self.frame.columnconfigure(index=0, weight=1)
        self.title('Musical Form')
        self.geometry('400x850+20+50')

    def create_widgets(self):
        the_row = 0

        self.frame.rowconfigure(index=the_row, weight=1)
        self.name_label = tkinter.ttk.Label(self.frame, text='Name')
        self.name_label.grid(row=the_row, column=0, sticky=NSEW)
        self.name_entry = tkinter.ttk.Entry(self.frame)
        self.name_entry.insert(0, self.xml_form['name'])
        self.name_entry.grid(row=the_row, column=1, sticky=NSEW)
        self.name_entry.bind('<Key-Return>', self.name_callback)
        self.name_entry.bind('<FocusOut>', self.name_callback)
        the_row = the_row + 1

        self.frame.rowconfigure(index=the_row, weight=1)
        self.frame.columnconfigure(index=0, weight=1)
        self.frame.columnconfigure(index=1, weight=1)
        self.len_label = tkinter.ttk.Label(self.frame, text='Len')
        self.len_label.grid(row=the_row, column=0, sticky=NSEW)
        self.len_entry = tkinter.ttk.Entry(self.frame)
        self.len_entry.insert(0, self.xml_form['len'])
        self.len_entry.grid(row=the_row, column=1, sticky=NSEW)
        self.len_entry.bind('<Key-Return>', self.len_callback)
        self.len_entry.bind('<FocusOut>', self.len_callback)
        the_row = the_row + 1

        self.frame.rowconfigure(index=the_row, weight=1)
        self.min_note_len_label = tkinter.ttk.Label(self.frame, text='Min Note Len')
        self.min_note_len_label.grid(row=the_row, column=0, sticky=NSEW)
        self.min_note_len_entry = tkinter.ttk.Entry(self.frame)
        self.min_note_len_entry.insert(0, self.xml_form['min_note_len'])
        self.min_note_len_entry.grid(row=the_row, column=1, sticky=NSEW)
        self.min_note_len_entry.bind('<Key-Return>', self.min_note_len_callback)
        self.min_note_len_entry.bind('<FocusOut>', self.min_note_len_callback)
        the_row = the_row + 1

        self.frame.rowconfigure(index=the_row, weight=1)
        self.max_note_len_label = tkinter.ttk.Label(self.frame, text='Max Note Len')
        self.max_note_len_label.grid(row=the_row, column=0, sticky=NSEW)
        self.max_note_len_entry = tkinter.ttk.Entry(self.frame)
        self.max_note_len_entry.insert(0, self.xml_form['max_note_len'])
        self.max_note_len_entry.grid(row=the_row, column=1, sticky=NSEW)
        self.max_note_len_entry.bind('<Key-Return>', self.max_note_len_callback)
        self.max_note_len_entry.bind('<FocusOut>', self.max_note_len_callback)
        the_row = the_row + 1

        self.frame.rowconfigure(index=the_row, weight=1)
        self.pulse_label = tkinter.ttk.Label(self.frame, text='Pulse/Sec')
        self.pulse_label.grid(row=the_row, column=0, sticky=NSEW)
        self.pulse_entry = tkinter.ttk.Entry(self.frame)
        self.pulse_entry.insert(0, self.xml_form['pulse'])
        self.pulse_entry.grid(row=the_row, column=1, sticky=NSEW)
        self.pulse_entry.bind('<Key-Return>', self.pulse_callback)
        self.pulse_entry.bind('<FocusOut>', self.pulse_callback)
        the_row = the_row + 1

        self.frame.rowconfigure(index=the_row, weight=1)
        self.melody_probabilities_label = tkinter.ttk.Label(self.frame, text='Melody Probabilities')
        self.melody_probabilities_label.grid(row=the_row, column=0, sticky=NSEW)
        self.melody_probabilities_entry = tkinter.ttk.Entry(self.frame)

        self.frame.rowconfigure(index=the_row, weight=1)
        self.down_label = tkinter.ttk.Label(self.frame, text='Down')
        self.down_label.grid(row=the_row, column=0, sticky=NSEW)
        self.down_entry = tkinter.ttk.Entry(self.frame)
        self.down_entry.insert(0, self.xml_form['melody_probabilities']['down'])
        self.down_entry.grid(row=the_row, column=1, sticky=NSEW)
        self.down_entry.bind('<Key-Return>', self.melody_down_callback)
        self.down_entry.bind('<FocusOut>', self.melody_down_callback)
        the_row = the_row + 1

        self.frame.rowconfigure(index=the_row, weight=1)
        self.same_label = tkinter.ttk.Label(self.frame, text='Same')
        self.same_label.grid(row=the_row, column=0, sticky=NSEW)
        self.same_entry = tkinter.ttk.Entry(self.frame)
        self.same_entry.insert(0, self.xml_form['melody_probabilities']['same'])
        self.same_entry.grid(row=the_row, column=1, sticky=NSEW)
        self.same_entry.bind('<Key-Return>', self.melody_same_callback)
        self.same_entry.bind('<FocusOut>', self.melody_same_callback)
        the_row = the_row + 1

        self.frame.rowconfigure(index=the_row, weight=1)
        self.up_label = tkinter.ttk.Label(self.frame, text='Up')
        self.up_label.grid(row=the_row, column=0, sticky=NSEW)
        self.up_entry = tkinter.ttk.Entry(self.frame)
        self.up_entry.insert(0, self.xml_form['melody_probabilities']['up'])
        self.up_entry.grid(row=the_row, column=1, sticky=NSEW)
        self.up_entry.bind('<Key-Return>', self.melody_up_callback)
        self.up_entry.bind('<FocusOut>', self.melody_up_callback)
        the_row = the_row + 1

        self.scale_frame = ScaleFrame(self)
        self.scale_frame.grid(sticky='w', row=the_row, column=0)
        self.scale_frame.rowconfigure(index=0, weight=1)
        self.scale_frame.columnconfigure(index=0, weight=1)

        the_row = the_row + 2

        the_row = the_row + 1
        self.pitch_form = FormFrame(parent=self, form_title='PITCH', mean_period_callback=self.pitch_mean_period_callback, mean_phase_callback=self.pitch_mean_phase_callback, range_period_callback=self.pitch_range_period_callback, range_phase_callback=self.pitch_range_phase_callback, xml_subform=self.xml_form['pitch_form'])
        self.pitch_form.grid(row=the_row, column=0, sticky=NSEW)
        the_row = the_row + 1

        the_row = the_row + 1
        self.rhythm_form = FormFrame(parent=self, form_title='RHYTHM', mean_period_callback=self.rhythm_mean_period_callback, mean_phase_callback=self.rhythm_mean_phase_callback, range_period_callback=self.rhythm_range_period_callback, range_phase_callback=self.rhythm_range_phase_callback, xml_subform=self.xml_form['rhythm_form'])
        self.rhythm_form.grid(row=the_row, column=0, sticky=NSEW)
        the_row = the_row + 1

        the_row = the_row + 1
        self.dynamic_form = FormFrame(parent=self, form_title='DYNAMIC', mean_period_callback=self.dynamic_mean_period_callback, mean_phase_callback=self.dynamic_mean_phase_callback, range_period_callback=self.dynamic_range_period_callback, range_phase_callback=self.dynamic_range_phase_callback, xml_subform=self.xml_form['dynamic_form'])
        self.dynamic_form.grid(row=the_row, column=0, sticky=NSEW)
        the_row = the_row + 1

        the_row = the_row + 1
        self.texture_form = Sine(parent=self, sine_title='TEXTURE', period_callback=self.texture_range_period_callback, phase_callback=self.texture_range_phase_callback, xml_sine=self.xml_form['texture_form'])
        self.texture_form.grid(row=the_row, column=0, sticky=NSEW)

    def name_callback(self, event):
        self.xml_form['name'] = self.name_entry.get()

    def len_callback(self, event):
        self.xml_form['len'] = self.len_entry.get()

    def min_note_len_callback(self, event):
        self.xml_form['min_note_len'] = self.min_note_len_entry.get()

    def max_note_len_callback(self, event):
        self.xml_form['max_note_len'] = self.max_note_len_entry.get()

    def pulse_callback(self, event):
        self.xml_form['pulse'] = self.pulse_entry.get()

    def melody_down_callback(self, event):
        self.xml_form['melody_probabilities']['down'] = self.down_entry.get()

    def melody_same_callback(self, event):
        self.xml_form['melody_probabilities']['same'] = self.same_entry.get()

    def melody_up_callback(self, event):
        self.xml_form['melody_probabilities']['up'] = self.up_entry.get()

    def pitch_mean_period_callback(self, event):
        self.xml_form['pitch_form']['mean']['period'] = self.pitch_form.mean.period_entry.get()

    def pitch_mean_phase_callback(self, action, sign, unitspage=None):
        self.pitch_form.mean.phase_scrollbar.update()
        firstlastlist = self.pitch_form.mean.phase_scrollbar.get()
        if (action == 'scroll'):
            if (unitspage == 'pages'):
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
                if (unitspage == 'units'):
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
            if (action == 'goto'):
                setting = firstlastlist[0]
                self.pitch_form.mean.phase_scrollbar.set(setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['pitch_form']['mean']['phase'] = value

    def pitch_range_period_callback(self, event):
        self.xml_form['pitch_form']['range']['period'] = self.pitch_form.range.period_entry.get()

    def pitch_range_phase_callback(self, action, sign, unitspage=None):
        firstlastlist = self.pitch_form.range.phase_scrollbar.get()
        if (action == 'scroll'):
            if (unitspage == 'pages'):
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
                if (unitspage == 'units'):
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
            if (action == 'goto'):
                setting = firstlastlist[0]
                self.pitch_form.range.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['pitch_form']['range']['phase'] = value

    def rhythm_mean_period_callback(self, event):
        self.xml_form['rhythm_form']['mean']['period'] = self.rhythm_form.mean.period_entry.get()

    def rhythm_mean_phase_callback(self, action, sign, unitspage):
        firstlastlist = self.rhythm_form.mean.phase_scrollbar.get()
        if (action == 'scroll'):
            if (unitspage == 'pages'):
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
                if (unitspage == 'units'):
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
            if (action == 'goto'):
                setting = firstlastlist[0]
                value = (setting - 0.5) * self.twopi
                self.pitch_form.range.phase_scrollbar.set(setting, setting)
                self.xml_form['rhythm_form']['mean']['phase'] = value

    def rhythm_range_period_callback(self, event):
        self.xml_form['rhythm_form']['range']['period'] = self.rhythm_form.range.period_entry.get()

    def rhythm_range_phase_callback(self, action, sign, unitspage=None):
        firstlastlist = self.rhythm_form.range.phase_scrollbar.get()
        if (action == 'scroll'):
            if (unitspage == 'pages'):
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
                if (unitspage == 'units'):
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
            if (action == 'goto'):
                setting = firstlastlist[0]
                self.rhythms_form.range.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['rhythm_form']['range']['phase'] = value

    def dynamic_mean_period_callback(self, event):
        self.xml_form['dynamic_form']['mean']['period'] = self.dynamic_form.mean.period_entry.get()

    def dynamic_mean_phase_callback(self, action, sign, unitspage):
        firstlastlist = self.dynamic_form.mean.phase_scrollbar.get()
        if (action == 'scroll'):
            if (unitspage == 'pages'):
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
                if (unitspage == 'units'):
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
            if (action == 'goto'):
                setting = firstlastlist[0]
                self.dynamic_form.mean.phase_scrollbar.set(setting, setting)
                value = (setting - 0.5) * self.twopi
                self.xml_form['dynamic_form']['mean']['phase'] = value

    def dynamic_range_period_callback(self, event):
        self.xml_form['dynamic_form']['range']['period'] = self.dynamic_form.range.period_entry.get()

    def dynamic_range_phase_callback(self, action, sign, unitspage=None):
        firstlastlist = self.dynamic_form.range.phase_scrollbar.get()
        if (action == 'scroll'):
            if (unitspage == 'pages'):
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
                if (unitspage == 'units'):
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
            if (action == 'goto'):
                setting = firstlastlist[0]
                self.dynamic_form.range.phase_scrollbar.set(firstlastlist[0])
                value = (setting - 0.5) * self.twopi
                self.xml_form['dynamic_form']['range']['phase'] = value

    def texture_range_period_callback(self, event):
        self.xml_form['texture_form']['period'] = self.texture_form.period_entry.get()

    def texture_range_phase_callback(self, action, sign, unitspage=None):
        firstlastlist = self.texture_form.phase_scrollbar.get()
        if (action == 'scroll'):
            if (unitspage == 'pages'):
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
                if (unitspage == 'units'):
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
            if (action == 'goto'):
                setting = firstlastlist[0]
                self.texture_form.range.phase_scrollbar.set(firstlastlist[0])
                value = (setting - 0.5) * self.twopi
                self.xml_form['texture_form']['phase'] = value

    def install_xml_form(self, xml_form):
        self.xml_form = xml_form

        self.name_entry.delete(0, 1024)
        self.name_entry.insert(0, self.xml_form['name'])
        self.name_entry.update()
        
        self.len_entry.delete(0, 1024)
        self.len_entry.insert(0, self.xml_form['len'])
        self.len_entry.update()

        self.min_note_len_entry.delete(0, 1024)
        self.min_note_len_entry.insert(0, self.xml_form['min_note_len'])
        self.min_note_len_entry.update()

        self.max_note_len_entry.delete(0, 1024)
        self.max_note_len_entry.insert(0, self.xml_form['max_note_len'])
        self.max_note_len_entry.update()

        self.pulse_entry.delete(0, 1024)
        self.pulse_entry.insert(0, self.xml_form['pulse'])
        self.pulse_entry.update()

        self.down_entry.delete(0, 1024)
        self.down_entry.insert(0, self.xml_form['melody_probabilities']['down'])
        self.down_entry.update()

        self.same_entry.delete(0, 1024)
        self.same_entry.insert(0, self.xml_form['melody_probabilities']['same'])
        self.same_entry.update()

        self.up_entry.delete(0, 1024)
        self.up_entry.insert(0, self.xml_form['melody_probabilities']['up'])
        self.up_entry.update()

        self.pitch_form.install_xml_subform(self.xml_form['pitch_form'])
        self.pitch_form.update()
        self.rhythm_form.install_xml_subform(self.xml_form['rhythm_form'])
        self.rhythm_form.update()
        self.dynamic_form.install_xml_subform(self.xml_form['dynamic_form'])
        self.dynamic_form.update()
        self.texture_form.install_xml_sine(self.xml_form['texture_form'])
        self.texture_form.update()
        self.scale_frame.set_keyboard_from_xml(self.xml_form)
        self.scale_frame.update()
        self.update()

