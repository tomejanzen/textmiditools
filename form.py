#!/usr/bin/env python3
# TextMIDITools Version 1.0.6
# textmidiform.py 1.0
# Copyright © 2021 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#

from sine import Sine

import tkinter
import tkinter.ttk
from tkinter import *
from tkinter.ttk import *

class FormWindow(tkinter.Frame):
    def __init__(self, master, arow, mean_period_callback, mean_phase_callback, range_period_callback, range_phase_callback, xml_subform):
        self.the_row = arow;
        self.mean_period_callback  = mean_period_callback
        self.mean_phase_callback   = mean_phase_callback
        self.range_period_callback = range_period_callback
        self.range_phase_callback  = range_phase_callback
        self.xml_subform = xml_subform
        super().__init__(master)
        self.grid(sticky=NSEW, padx=100)
        self.create_widgets()

    def create_widgets(self):
        self.mean_label = tkinter.ttk.Label(self, text="Mean")
        self.mean_label.grid(row=self.the_row, column=0, sticky=NSEW)
        self.mean = Sine(master=self, arow=self.the_row, period_callback=self.mean_period_callback, phase_callback=self.mean_phase_callback, xml_sine=self.xml_subform['mean'])
        self.range_label = tkinter.ttk.Label(self, text="Range")
        self.range_label.grid(row=self.the_row + 2, column=0, sticky=NSEW)
        self.range = Sine(master=self, arow=self.the_row, period_callback=self.range_period_callback, phase_callback=self.range_phase_callback, xml_sine=self.xml_subform['range'])

    def install_xml_subform(self, xml_subform):
        self.xml_subform = xml_subform
        self.mean.install_xml_sine(xml_subform['mean'])
        self.range.install_xml_sine(xml_subform['range'])
        self.update()

