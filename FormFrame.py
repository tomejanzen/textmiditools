#!/usr/bin/env python3
# TextMIDITools Version 1.0.69
# textmidiform.py 1.0
# Copyright © 2024 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#

from Sine import Sine

import tkinter
import tkinter.ttk
from tkinter import *
from tkinter.ttk import *

class FormFrame(tkinter.Frame):
    def __init__(self, parent, form_title, mean_period_callback, mean_phase_callback, range_period_callback, range_phase_callback, mean_amplitude_callback, mean_offset_callback, range_amplitude_callback, range_offset_callback, xml_subform, mean_on = True, range_on = True):
        self.mean_period_callback     = mean_period_callback
        self.mean_phase_callback      = mean_phase_callback
        self.range_period_callback    = range_period_callback
        self.range_phase_callback     = range_phase_callback
        self.mean_amplitude_callback  = mean_amplitude_callback
        self.mean_offset_callback     = mean_offset_callback
        self.range_amplitude_callback = range_amplitude_callback
        self.range_offset_callback    = range_offset_callback
        self.xml_subform = xml_subform
        self.mean_on = mean_on
        self.range_on = range_on
        super().__init__(parent)
        self.grid(sticky=NSEW, padx=10, pady=10)
        self.form_title = form_title
        self.create_widgets()

    def create_widgets(self):
        the_row = 0
        self.form_title_label = tkinter.ttk.Label(self, text=self.form_title)
        self.form_title_label.grid(row=the_row, column=0)

        if (self.mean_on):
            self.mean = Sine(parent=self, sine_title='Mean', period_callback=self.mean_period_callback, phase_callback=self.mean_phase_callback, amplitude_callback=self.mean_amplitude_callback, offset_callback=self.mean_offset_callback, xml_sine=self.xml_subform['mean'])
            self.mean.grid(row=the_row, column=2, sticky=NSEW, padx=5, pady=5)
            the_row = the_row + 1
        if (self.range_on):
            self.range = Sine(parent=self, sine_title='Range', period_callback=self.range_period_callback, phase_callback=self.range_phase_callback, amplitude_callback=self.range_amplitude_callback, offset_callback=self.range_offset_callback, xml_sine=self.xml_subform['range'])
            self.range.grid(row=the_row, column=2, sticky=NSEW, padx=5, pady=5)
            the_row = the_row + 1

    def install_callbacks(self):
        self.mean.install_callbacks()
        self.range.install_callbacks()

    def install_xml_subform(self, xml_subform):
        self.xml_subform = xml_subform
        self.mean.install_xml_sine(xml_subform['mean'])
        self.range.install_xml_sine(xml_subform['range'])
        self.update()
