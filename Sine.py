#!/usr/bin/env python3
# TextMIDITools Version 1.0.69
# textmidiform.py 1.0
# Copyright © 2024 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#
import tkinter
import tkinter.ttk
from tkinter import *
from tkinter.ttk import *
import re

class Sine(tkinter.Frame):
    pi = 3.14159265358979323846264338327950288419716939937508

    def __init__(self, parent, sine_title, period_callback, phase_callback, amplitude_callback, offset_callback, xml_sine):
        # assignment of class data must be first
        self.period_callback = period_callback
        self.phase_callback = phase_callback
        self.amplitude_callback = amplitude_callback
        self.offset_callback = offset_callback
        super().__init__(parent)
        self.sine_title = sine_title
        self.xml_sine = xml_sine

        self.period    = StringVar();
        self.amplitude = StringVar();

        self.grid(sticky=NSEW, padx=10)
        self.create_widgets()

    def create_widgets(self):

        # Mean or Range
        self.curve_label = tkinter.ttk.Label(self, text=self.sine_title)
        self.curve_label.grid(row=1, column=0)

        # TCL book p 509
        # %d type 1 insert, 0 del, -1 other
        # %i index of string to change or -1
        # %P value of entry if edit is allowed
        # %s val of entry prior to editing
        # %S the text string being inserted or deleted
        # %v type of validation currently set
        # %V type of validation that triggered us (key, focusin, focusout, forced)
        # %W tk name of widget
        validate_command = (self.register(self.validate_length),
            '%d', '%i', '%P', '%s', '%S', '%v', '%V', '%W')
        self.phase_scroll_label = tkinter.ttk.Label(self, text='-pi..................0..................pi')
        self.period_label = tkinter.ttk.Label(self, text='Period')
        self.period_entry = tkinter.ttk.Entry(self, validatecommand=validate_command, validate='focusout', textvariable=self.period)

        self.phase_label = tkinter.ttk.Label(self, text='Phase')
        self.phase_scrollbar = tkinter.ttk.Scrollbar(self, command=self.phase_callback)
        self.phase_scrollbar['orient'] = 'horizontal'
        phase = self.xml_sine['phase'] / (2.0 * self.pi) + 0.5
        self.phase_scrollbar.set(phase, phase)

        self.amplitude_label = tkinter.ttk.Label(self, text='Gain')
        self.amplitude_entry = tkinter.ttk.Entry(self, validatecommand=validate_command, validate='focusout', textvariable=self.amplitude)

        self.offset_label = tkinter.ttk.Label(self, text='Offset')
        self.offset_scrollbar = tkinter.ttk.Scrollbar(self, command=self.offset_callback)
        self.offset_scrollbar['orient'] = 'horizontal'
        offset = self.xml_sine['offset'] / 2.0 + 0.5
        self.offset_scrollbar.set(offset, offset)
        self.phase_scroll_label = tkinter.ttk.Label(self, text='-1...................0...................1')

        the_row = 0
        self.phase_scroll_label.rowconfigure(index=the_row, weight=1)
        self.phase_scroll_label.grid(row=the_row, column=5, sticky=NSEW)
        the_row = the_row + 1
        self.rowconfigure(index=the_row, weight=1)
        self.period_label.grid(row=the_row, column=1, sticky=NSEW)
        self.period_entry.grid(row=the_row, column=2, sticky=NSEW)
        self.phase_label.grid(row=the_row, column=4, sticky=NSEW)
        self.phase_scrollbar.grid(row=the_row, column=5, sticky=NSEW)
        self.phase_scrollbar.rowconfigure(index=the_row, weight=1)
        the_row = the_row + 1
        self.rowconfigure(index=the_row, weight=1)
        self.amplitude_label.grid(row=the_row, column=1, sticky=NSEW)
        self.amplitude_entry.grid(row=the_row, column=2, sticky=NSEW)
        self.offset_label.grid(row=the_row, column=4, sticky=NSEW)
        self.offset_scrollbar.grid(row=the_row, column=5, sticky=NSEW)
        self.offset_scrollbar.rowconfigure(index=the_row, weight=1)
        the_row = the_row + 1
        self.phase_scroll_label.rowconfigure(index=the_row, weight=1)
        self.phase_scroll_label.grid(row=the_row, column=5, sticky=NSEW)

        self.period.set(self.xml_sine['period'])
        self.amplitude.set(self.xml_sine['amplitude'])

    def install_callbacks(self):
        self.period.trace('w', self.period_callback)
        self.amplitude.trace('w', self.amplitude_callback)

    def install_xml_sine(self, xml_sine):
        self.xml_sine = xml_sine
        self.period.set(self.xml_sine['period'])
        phase = self.xml_sine['phase'] / (2.0 * self.pi) + 0.5
        self.phase_scrollbar.set(phase, phase)
        self.phase_scrollbar.update()
        self.amplitude.set(self.xml_sine['amplitude'])
        offset = self.xml_sine['offset'] / 2.0 + 0.5
        self.offset_scrollbar.set(offset, offset)
        self.offset_scrollbar.update()
        self.update()
    def validate_length(self, d, i, P, s, S, v, V, W):
        # 1-insert, 0=del, -1 if forced
        pat = re.compile('\d+([.]\d*)?')
        ma = pat.match(str(P))
        ret = False
        if (ma):
            ret = True
        else:
            ret = False
        return ret

