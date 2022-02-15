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
from tkinter.ttk import *
import re

class Sine(tkinter.Frame):
    pi = 3.1415926535
    def __init__(self, parent, sine_title, period_callback, phase_callback, xml_sine):
        # assignment of class data must be first
        self.the_row = 0
        self.period_callback = period_callback
        self.phase_callback = phase_callback
        super().__init__(parent)
        self.sine_title = sine_title
        self.xml_sine = xml_sine
        self.grid(sticky=NSEW, padx=100)
        self.create_widgets()

    def create_widgets(self):

        self.curve_label = tkinter.ttk.Label(self, text=self.sine_title)
        self.curve_label.grid(row=1, column=0)
        self.rowconfigure(index=self.the_row, weight=1)
        self.period_label = tkinter.ttk.Label(self, text='Period')
        self.period_label.grid(row=self.the_row, column=1, sticky=NSEW)
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
        self.period_entry = tkinter.ttk.Entry(self, validatecommand=validate_command, validate='focusout')
        self.period_entry.delete(0, 1024)
        self.period_entry.insert(0, self.xml_sine['period'])
        self.period_entry.update()
        self.period_entry.grid(row=self.the_row, column=2, sticky=NSEW)
        self.period_entry.bind('<Key-Return>', self.period_callback)
        self.period_entry.bind('<FocusOut>', self.period_callback)
        self.the_row = self.the_row + 1

        self.rowconfigure(index=self.the_row, weight=1)
        self.phase_scroll_label = tkinter.ttk.Label(self, text='-pi..................0..................pi')
        self.phase_scroll_label.rowconfigure(index=self.the_row, weight=1)
        self.phase_scroll_label.grid(row=self.the_row, column=2, sticky=NSEW)
        self.the_row = self.the_row + 1

        self.rowconfigure(index=self.the_row, weight=1)
        self.phase_label = tkinter.ttk.Label(self, text='Phase')
        self.phase_label.rowconfigure(index=self.the_row, weight=1)
        self.phase_label.grid(row=self.the_row, column=1, sticky=NSEW)
        self.phase_scrollbar = tkinter.ttk.Scrollbar(self, command=self.phase_callback)
        self.phase_scrollbar['orient'] = 'horizontal'
        self.phase_scrollbar.grid(row=self.the_row, column=2, sticky=NSEW)
        self.phase_scrollbar.rowconfigure(index=self.the_row, weight=1)
        phase = self.xml_sine['phase'] / (2.0 * self.pi) + 0.5
        self.phase_scrollbar.set(phase, phase)
        self.the_row = self.the_row + 1

    def install_xml_sine(self, xml_sine):
        self.xml_sine = xml_sine
        self.period_entry.delete(0, 1024)
        self.period_entry.insert(0, self.xml_sine['period'])
        self.period_entry.update()
        phase = self.xml_sine['phase'] / (2.0 * self.pi) + 0.5
        self.phase_scrollbar.set(phase, phase)
        self.phase_scrollbar.update()
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

