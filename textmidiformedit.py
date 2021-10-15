#!/usr/bin/env python3
# TextMIDITools Version 1.0.6
# textmidiform.py 1.0
# Copyright © 2021 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#

import re
import math
import sys
import tkinter
import tkinter.ttk
from tkinter import *
from tkinter.ttk import *
import tkinter.constants, tkinter.filedialog
from xml.dom import minidom
from xml.dom.minidom import parse, Node, getDOMImplementation
from voice import VoiceWindow
from allforms import AllFormsWindow

class XmlForm(tkinter.Frame):
    xml_form_dict = {}
    the_filename = "Untitled"
    def __init__(self, master):
        super().__init__(master)
        self.default_xml_form()
        self.canvas = None
        self.draw_form()

        self.grid(sticky="we", row=0, column=2)
        self.rowconfigure(index=0, weight=1)
        self.columnconfigure(index=2, weight=1)

    def install_file(self, afilename):
        self.the_filename = afilename
        # get the xml.dom.minidom.Document
        self.dom = parse(self.the_filename)
        self.traverse_xml_form()
        self.draw_form()

    def on_motion(self, event):
        a_canvas = event.widget
        self.draw_form()

    def draw_form(self):
        duration = float(self.xml_form_dict['len'])
        twopi = 2.0 * math.pi
        win_height = 600
        win_width = 1000
        if (self.canvas == None):
            self.canvas = Canvas(self, bg='white', height=win_height, width=win_width)
        else:
            self.canvas.delete()
            self.canvas = Canvas(self, bg='white', height=win_height, width=win_width)
            self.canvas.grid(row=0, column=0, sticky=NSEW)

        self.canvas.grid(sticky=NSEW)
        self.canvas.bind('<B1-Motion>', self.on_motion)

        poly_lr = []
        poly_rl = []
        for i in range(1, win_width, 1):
            mean_freq = 1.0 / float(self.xml_form_dict['pitch_form']['mean']['period'])
            range_freq = 1.0 / float(self.xml_form_dict['pitch_form']['range']['period'])
            seconds_per_pixel = duration / float(win_width)
            the_time = float(i) * seconds_per_pixel
            # omega * t = twopi * mean_freq  * the_time 
            # the phase = self.xml_form_dict['pitch_form']['mean']['phase']
            # omega * t + phase = twopi * mean_freq  * the_time + self.xml_form_dict['pitch_form']['mean']['phase']
            # take sin(omega*t + phase) which ranges -1.0 to 1.0
            # make it work with the form model by making it range from 0 to 1.
            # 0.5 + sin(omega * t + phase) / 2
            # scale it for graphical plotting on the canvas
            # 0.5 + sin(omega * t + phase) / 2 * win_height / 16
            # but window coordinates are left-handed, so invert the mean curve
            # -(0.5 + sin(omega * t + phase) / 2 * win_height / 16)
            # position it on the canvas
            # 0.5 + sin(omega * t + phase) / 2 * win_height / 16 + 1 * win_height / 16
            # oi la!
            y_mean  = (.5 - math.sin(twopi * mean_freq  * the_time + float(self.xml_form_dict['pitch_form']['mean']['phase']))  / 2.0) * (float(win_height) / 12.0) + (1.0 * float(win_height) / 16.0)
            y_range = (.5 + math.sin(twopi * range_freq * the_time + float(self.xml_form_dict['pitch_form']['range']['phase'])) / 2.0) * (float(win_height) / 12.0)
            
            poly_lr.append(i)
            poly_lr.append(y_mean - y_range / 2)
            poly_rl.append(y_mean + y_range / 2)
            poly_rl.append(i)
        poly_rl.reverse()
        polygon = poly_lr + poly_rl 
        pitch_plot_id = self.canvas.create_polygon(polygon, fill="black")

        poly_lr = []
        poly_rl = []
        polygon = []
        for i in range(1, win_width):
            mean_freq = 1.0 / float(self.xml_form_dict['rhythm_form']['mean']['period'])
            range_freq = 1.0 / float(self.xml_form_dict['rhythm_form']['range']['period'])
            seconds_per_pixel = duration / float(win_width)
            the_time = float(i) * seconds_per_pixel
            y_mean  = (.5
                       - math.sin(twopi * mean_freq  * the_time
                                  + float(self.xml_form_dict['rhythm_form']['mean']['phase']))
                       / 2.0) * float(win_height) / 12.0 + 5.0 * float(win_height / 16.0)
            y_range = (.5
                      + math.sin(twopi * range_freq * the_time
                                 + float(self.xml_form_dict['rhythm_form']['range']['phase']))
                      / 2.0) * float(win_height) / 12.0
            poly_lr.append(i)
            poly_lr.append(y_mean - y_range / 2)
            poly_rl.append(y_mean + y_range / 2)
            poly_rl.append(i)
        poly_rl.reverse()
        polygon = poly_lr + poly_rl 
        rhythm_plot_id = self.canvas.create_polygon(polygon, fill="black")


        poly_lr = []
        poly_rl = []
        polygon = []
        for i in range(1, win_width):
            mean_freq = 1.0 / float(self.xml_form_dict['dynamic_form']['mean']['period'])
            range_freq = 1.0 / float(self.xml_form_dict['dynamic_form']['range']['period'])
            seconds_per_pixel = duration / float(win_width)
            the_time = float(i) * seconds_per_pixel
            y_mean  = (.5
                       - math.sin(twopi * mean_freq  * the_time
                                  + float(self.xml_form_dict['dynamic_form']['mean']['phase']))
                       / 2.0) * float(win_height) / 12.0 + 9.0 * float(win_height / 16.0)
            y_range = (.5
                      + math.sin(twopi * range_freq * the_time
                                 + float(self.xml_form_dict['dynamic_form']['range']['phase']))
                      / 2.0) * float(win_height) / 12.0
            poly_lr.append(i)
            poly_lr.append(y_mean - y_range / 2)
            poly_rl.append(y_mean + y_range / 2)
            poly_rl.append(i)
        poly_rl.reverse()
        polygon = poly_lr + poly_rl 
        dynamic_plot_id = self.canvas.create_polygon(polygon, fill="black")

        poly_lr = []
        poly_rl = []
        polygon = []
        # Texture (number of voices playing) was misdefined as the mean alone,
        # but really it is the range.  So there is a reversal of roles here.
        for i in range(1, win_width):
            mean_freq = 1.0 / float(self.xml_form_dict['texture_form']['period'])
            seconds_per_pixel = duration / float(win_width)
            the_time = float(i) * seconds_per_pixel
            y_mean  = (.5
                      + math.sin(twopi * mean_freq  * the_time
                                 + float(self.xml_form_dict['texture_form']['phase']))
                      / 2.0) * float(win_height / 12.0)
            y_range = float(win_height) / 12.0 + (13.0 * float(win_height) / 16.0)
            poly_lr.append(i)
            poly_lr.append(y_range - y_mean / 2)
            poly_rl.append(y_range + y_mean / 2)
            poly_rl.append(i)
        poly_rl.reverse()
        polygon = poly_lr + poly_rl 
        texture_plot_id = self.canvas.create_polygon(polygon, fill="black")


        scale_height = win_height * 55 / 100
        self.canvas.create_line(0, scale_height, win_width, scale_height)
        for s in range(0, int(duration), 60):
            tickx = s / seconds_per_pixel
            self.canvas.create_line(tickx, 0, tickx, win_height)

        widget = tkinter.ttk.Label(self.canvas, text='Pitch', style="BW.TLabel")

        self.canvas.create_window(win_width, win_width, window=widget)
        self.canvas.create_text(win_width / 2, 20, text=self.xml_form_dict["name"])
        self.canvas.create_text(50, win_height * 1 / 16 - 20, text='Pitch')
        self.canvas.create_text(50, win_height * 5 / 16 - 20, text='Rhythm')
        self.canvas.create_text(50, win_height * 9 / 16  - 20, text='Dynamic')
        self.canvas.create_text(50, win_height * 13 / 16 - 20, text='Texture')

    def default_scale(self):
        scale_array = ['C4', 'D4', 'E4', 'F4', 'G4', 'A4', 'B4', 'C5']
        return scale_array

    def default_melody_probabilities(self):
        # These probabilities are cumulative.
        # 1.0 - up == same - down,  so that the walking voice
        #            goes up or down equal amounts.
        #     # up
        #     # up
        # .75 - up if above up
        #     # same
        #     # same
        # .50 - same if above same
        #     # down
        #     # down
        # .25 - down if above down
        #     # (silent)
        #     # (silent)
        melody_probabilities_dict = {}
        melody_probabilities_dict['down'] = 0.25
        melody_probabilities_dict['same'] = 0.5
        melody_probabilities_dict['up'] = 0.75
        return melody_probabilities_dict

    def default_form(self):
        pd = {}
        pmd = {}
        pmd['period'] = 180.0
        pmd['phase']  = 0.0
        pmd['amplitude'] = 0.50
        pmd['offset'] = 0.50
        pd['mean'] = pmd

        prd = {}
        prd['period'] = 180.0
        prd['phase'] = 0.0
        prd['amplitude'] = 0.50
        prd['offset'] = 0.50
        pd['range'] = prd
        return pd

    def default_texture_form(self):
        td = {}
        td['period'] = 180.0
        td['phase'] = 0.0
        td['amplitude'] = 0.50
        td['offset'] = 0.50
        return td

    def default_voices(self):
        vl = []
        for v in range(1, 24):
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
            vl.append(vd)

        return vl

    def default_xml_form(self):
        self.xml_form_dict['name'] = "Anonymous"
        self.xml_form_dict['len'] = 600.0
        self.xml_form_dict['min_note_len'] = 0.0
        self.xml_form_dict['max_note_len'] = 2.0
        self.xml_form_dict['scale'] = self.default_scale()
        self.xml_form_dict['pulse'] = 8.0
        self.xml_form_dict['melody_probabilities'] = self.default_melody_probabilities()
        self.xml_form_dict['pitch_form'] = self.default_form()
        self.xml_form_dict['rhythm_form'] = self.default_form()
        self.xml_form_dict['dynamic_form'] = self.default_form()
        self.xml_form_dict['texture_form'] = self.default_texture_form()
        self.xml_form_dict['voices'] = self.default_voices()

    def traverse_scale(self, scale):
        scale_array = []
        scale_list = scale.getElementsByTagName('item')

        for item_node in scale_list :
            scale_array.append(item_node.firstChild.data)
        return scale_array

    def traverse_melody_probabilities(self, melody_probabilities):
        melody_probabilities_dict = {}
        down_element_list = melody_probabilities.getElementsByTagName('down_')
        melody_probabilities_dict['down'] = down_element_list[0].firstChild.data
        same_element_list = melody_probabilities.getElementsByTagName('same_')
        melody_probabilities_dict['same'] = same_element_list[0].firstChild.data
        up_element_list = melody_probabilities.getElementsByTagName('up_')
        melody_probabilities_dict['up'] = up_element_list[0].firstChild.data
        return melody_probabilities_dict

    def traverse_pitch_form(self, pitch_form):
        pd = {}
        pmd = {}
        mean_sine_node = pitch_form.getElementsByTagName("mean_sine_")[0]
        mean_period_node    = mean_sine_node.getElementsByTagName("period_")[0]
        mean_period = mean_period_node.firstChild.data
        pmd['period'] = float(mean_period)
        mean_phase_node     = mean_sine_node.getElementsByTagName("phase_")[0]
        mean_phase = mean_phase_node.firstChild.data
        pmd['phase'] = float(mean_phase)
        mean_amplitude_node = mean_sine_node.getElementsByTagName("amplitude_")[0]
        mean_amplitude = mean_amplitude_node.firstChild.data
        pmd['amplitude'] = float(mean_amplitude)
        mean_offset_node    = mean_sine_node.getElementsByTagName("offset_")[0]
        mean_offset = mean_offset_node.firstChild.data
        pmd['offset'] = float(mean_offset)
        pd['mean'] = pmd

        prd = {}
        range_sine_node = pitch_form.getElementsByTagName("range_sine_")[0]
        range_period_node    = range_sine_node.getElementsByTagName("period_")[0]
        range_period = range_period_node.firstChild.data
        prd['period'] = float(range_period)
        range_phase_node     = range_sine_node.getElementsByTagName("phase_")[0]
        range_phase = range_phase_node.firstChild.data
        prd['phase'] = float(range_phase)
        range_amplitude_node = range_sine_node.getElementsByTagName("amplitude_")[0]
        range_amplitude = range_amplitude_node.firstChild.data
        prd['amplitude'] = float(range_amplitude)
        range_offset_node    = range_sine_node.getElementsByTagName("offset_")[0]
        range_offset = range_offset_node.firstChild.data
        prd['offset'] = float(range_offset)
        pd['range'] = prd
        return pd

    def traverse_rhythm_form(self, rhythm_form):
        rd = {}
        rmd = {}
        mean_sine_node = rhythm_form.getElementsByTagName("mean_sine_")[0]
        mean_period_node    = mean_sine_node.getElementsByTagName("period_")[0]
        mean_period = mean_period_node.firstChild.data
        rmd['period'] = float(mean_period)
        mean_phase_node     = mean_sine_node.getElementsByTagName("phase_")[0]
        mean_phase = mean_phase_node.firstChild.data
        rmd['phase'] = float(mean_phase)
        mean_amplitude_node = mean_sine_node.getElementsByTagName("amplitude_")[0]
        mean_amplitude = mean_amplitude_node.firstChild.data
        rmd['amplitude'] = float(mean_amplitude)
        mean_offset_node    = mean_sine_node.getElementsByTagName("offset_")[0]
        mean_offset = mean_offset_node.firstChild.data
        rmd['offset'] = float(mean_offset)
        rd['mean'] = rmd

        rrd = {}
        range_sine_node = rhythm_form.getElementsByTagName("range_sine_")[0]
        range_period_node    = range_sine_node.getElementsByTagName("period_")[0]
        range_period = range_period_node.firstChild.data
        rrd['period'] = float(range_period)
        range_phase_node     = range_sine_node.getElementsByTagName("phase_")[0]
        range_phase = range_phase_node.firstChild.data
        rrd['phase'] = float(range_phase)
        range_amplitude_node = range_sine_node.getElementsByTagName("amplitude_")[0]
        range_amplitude = range_amplitude_node.firstChild.data
        rrd['amplitude'] = float(range_amplitude)
        range_offset_node    = range_sine_node.getElementsByTagName("offset_")[0]
        range_offset = range_offset_node.firstChild.data
        rrd['offset'] = float(range_offset)
        rd['range'] = rrd
        return rd

    def traverse_dynamic_form(self, dynamic_form):
        dd = {}
        dmd = {}
        mean_sine_node = dynamic_form.getElementsByTagName("mean_sine_")[0]
        mean_period_node    = mean_sine_node.getElementsByTagName("period_")[0]
        mean_period = mean_period_node.firstChild.data
        dmd['period'] = float(mean_period)
        mean_phase_node     = mean_sine_node.getElementsByTagName("phase_")[0]
        mean_phase = mean_phase_node.firstChild.data
        dmd['phase'] = float(mean_phase)
        mean_amplitude_node = mean_sine_node.getElementsByTagName("amplitude_")[0]
        mean_amplitude = mean_amplitude_node.firstChild.data
        dmd['amplitude'] = float(mean_amplitude)
        mean_offset_node    = mean_sine_node.getElementsByTagName("offset_")[0]
        mean_offset = mean_offset_node.firstChild.data
        dmd['offset'] = float(mean_offset)
        dd['mean'] = dmd

        drd = {}
        range_sine_node = dynamic_form.getElementsByTagName("range_sine_")[0]
        range_period_node    = range_sine_node.getElementsByTagName("period_")[0]
        range_period = range_period_node.firstChild.data
        drd['period'] = float(range_period)
        range_phase_node     = range_sine_node.getElementsByTagName("phase_")[0]
        range_phase = range_phase_node.firstChild.data
        drd['phase'] = float(range_phase)
        range_amplitude_node = range_sine_node.getElementsByTagName("amplitude_")[0]
        range_amplitude = range_amplitude_node.firstChild.data
        drd['amplitude'] = float(range_amplitude)
        range_offset_node    = range_sine_node.getElementsByTagName("offset_")[0]
        range_offset = range_offset_node.firstChild.data
        drd['offset'] = float(range_offset)
        dd['range'] = drd
        return dd

    def traverse_texture_form(self, texture_form):
        td = {}
        period_node    = texture_form.getElementsByTagName("period_")[0]
        period = period_node.firstChild.data
        td['period'] = float(period)
        phase_node     = texture_form.getElementsByTagName("phase_")[0]
        phase = phase_node.firstChild.data
        td['phase'] = float(phase)
        amplitude_node = texture_form.getElementsByTagName("amplitude_")[0]
        amplitude = amplitude_node.firstChild.data
        td['amplitude'] = float(amplitude)
        offset_node    = texture_form.getElementsByTagName("offset_")[0]
        offset = offset_node.firstChild.data
        td['offset'] = float(offset)
        return td

    def traverse_voices(self, voices):
        voice_list = voices.getElementsByTagName("item")
        vl = []
        for v in voice_list:
            vd = {}
            low_pitch_node = v.getElementsByTagName("low_pitch_")[0]
            low_pitch = low_pitch_node.firstChild.data
            vd['low_pitch'] = low_pitch
            high_pitch_node = v.getElementsByTagName("high_pitch_")[0]
            high_pitch = high_pitch_node.firstChild.data
            vd['high_pitch'] = high_pitch
            channel_node = v.getElementsByTagName("channel_")[0]
            channel = channel_node.firstChild.data
            vd['channel'] = int(channel)
            walking_node = v.getElementsByTagName("walking_")[0]
            walking = walking_node.firstChild.data
            vd['walking'] = int(walking)
            program_node = v.getElementsByTagName("program_")[0]
            program = program_node.firstChild.data
            vd['program'] = int(program)
            pan_node = v.getElementsByTagName("pan_")[0]
            pan = pan_node.firstChild.data
            vd['pan'] = pan
            fd = {}
            follower_node = v.getElementsByTagName("follower_")[0]
            follow_node = follower_node.getElementsByTagName("follow_")[0]
            follow = follow_node.firstChild.data
            fd['follow'] = int(follow)
            leader_node = follower_node.getElementsByTagName("leader_")[0]
            leader = leader_node.firstChild.data
            fd['leader'] = int(leader)
            interval_type_node = follower_node.getElementsByTagName("interval_type_")[0]
            interval_type = interval_type_node.firstChild.data
            fd['interval_type'] = int(interval_type)
            interval_node = follower_node.getElementsByTagName("interval_")[0]
            interval = interval_node.firstChild.data
            fd['interval'] = int(interval)
            vd['follower'] = fd
            vl.append(vd)

        return vl

    def traverse_xml_form(self):
        self.xml_form_dict['name'] = self.dom.getElementsByTagName("name_")[0].firstChild.data
        self.xml_form_dict['len'] = float(self.dom.getElementsByTagName("len_")[0].firstChild.data)
        self.xml_form_dict['min_note_len'] = self.dom.getElementsByTagName("min_note_len_")[0].firstChild.data
        self.xml_form_dict['max_note_len'] = self.dom.getElementsByTagName("max_note_len_")[0].firstChild.data

        self.xml_form_dict['scale'] = self.traverse_scale(self.dom.getElementsByTagName("scale_")[0])
        self.xml_form_dict['pulse'] = self.dom.getElementsByTagName("pulse_")[0].firstChild.data
        self.xml_form_dict['melody_probabilities'] = self.traverse_melody_probabilities(self.dom.getElementsByTagName("melody_probabilities_")[0])

        self.xml_form_dict['pitch_form'] = self.traverse_pitch_form(self.dom.getElementsByTagName("pitch_form_")[0])
        self.xml_form_dict['rhythm_form'] = self.traverse_rhythm_form(self.dom.getElementsByTagName("rhythm_form_")[0])
        self.xml_form_dict['dynamic_form'] = self.traverse_dynamic_form(self.dom.getElementsByTagName("dynamic_form_")[0])
        self.xml_form_dict['texture_form'] = self.traverse_texture_form(self.dom.getElementsByTagName("texture_form_")[0])
        self.xml_form_dict['voices'] = self.traverse_voices(self.dom.getElementsByTagName("voices_")[0])

def postscript_callback():
    afilename=tkinter.filedialog.asksaveasfilename(initialdir = ".",title = 'Save Postscript File', filetypes=(("ps files","*.ps"),("all files","*.*")))
    xmlform_window.canvas.postscript(file=afilename)

def makemenu(win):
    top = Menu(win)
    win.config(menu=top)
    file = Menu(top)
    file.add_command(label='Open...',   command=get_file_callback, underline=0)
    file.add_command(label='Save...',   command=save_callback, underline=0)
    file.add_command(label='Redraw...', command=redraw_callback, underline=0)
    file.add_command(label='Save Postscript...', command=postscript_callback, underline=0)
    file.add_command(label='About...',   command=about_callback, underline=0)
    file.add_command(label='Quit...',   command=lambda:root.quit(), underline=0)
    top.add_cascade(label='File', menu=file, underline=0)

def get_file_callback():
    afilename=tkinter.filedialog.askopenfilename(initialdir = ".",title = 'Select XML Form File', filetypes=(("xml files","*.xml"),("all files","*.*")))
    xmlform_window.install_file(afilename)
    voice_window.install_xml_form(xmlform_window.xml_form_dict)
    all_forms_window.install_xml_form(xmlform_window.xml_form_dict)

def save_callback():
    afilename=tkinter.filedialog.asksaveasfilename(initialdir = ".",title = 'Select XML Form File', filetypes=(("xml files","*.xml"),("all files","*.*")))
    dom_impl = getDOMImplementation()
    # copy form parts of xml_form_dict from AllForms
    # copy voice parts from Voice

    xmlform_window.xml_form_dict['name'] = all_forms_window.xml_form['name']
    xmlform_window.xml_form_dict['len'] = all_forms_window.xml_form['len']
    xmlform_window.xml_form_dict['min_note_len'] = all_forms_window.xml_form['min_note_len']
    xmlform_window.xml_form_dict['max_note_len'] = all_forms_window.xml_form['max_note_len']
    # scale
    xmlform_window.xml_form_dict['scale'] = all_forms_window.xml_form['scale']
    xmlform_window.xml_form_dict['pulse'] = all_forms_window.xml_form['pulse']
    xmlform_window.xml_form_dict['melody_probabilities']['down'] = all_forms_window.xml_form['melody_probabilities']['down']
    xmlform_window.xml_form_dict['melody_probabilities']['same'] = all_forms_window.xml_form['melody_probabilities']['same']
    xmlform_window.xml_form_dict['melody_probabilities']['up'] = all_forms_window.xml_form['melody_probabilities']['up']

    xmlform_window.xml_form_dict['pitch_form']['mean']['period'] = all_forms_window.xml_form['pitch_form']['mean']['period']
    xmlform_window.xml_form_dict['pitch_form']['mean']['phase'] = all_forms_window.xml_form['pitch_form']['mean']['phase']
    xmlform_window.xml_form_dict['pitch_form']['mean']['amplitude'] = all_forms_window.xml_form['pitch_form']['mean']['amplitude']
    xmlform_window.xml_form_dict['pitch_form']['mean']['offset'] = all_forms_window.xml_form['pitch_form']['mean']['offset']

    xmlform_window.xml_form_dict['rhythm_form']['mean']['period'] = all_forms_window.xml_form['rhythm_form']['mean']['period']
    xmlform_window.xml_form_dict['rhythm_form']['mean']['phase'] = all_forms_window.xml_form['rhythm_form']['mean']['phase']
    xmlform_window.xml_form_dict['rhythm_form']['mean']['amplitude'] = all_forms_window.xml_form['rhythm_form']['mean']['amplitude']
    xmlform_window.xml_form_dict['rhythm_form']['mean']['offset'] = all_forms_window.xml_form['rhythm_form']['mean']['offset']

    xmlform_window.xml_form_dict['dynamic_form']['mean']['period'] = all_forms_window.xml_form['dynamic_form']['mean']['period']
    xmlform_window.xml_form_dict['dynamic_form']['mean']['phase'] = all_forms_window.xml_form['dynamic_form']['mean']['phase']
    xmlform_window.xml_form_dict['dynamic_form']['mean']['amplitude'] = all_forms_window.xml_form['dynamic_form']['mean']['amplitude']
    xmlform_window.xml_form_dict['dynamic_form']['mean']['offset'] = all_forms_window.xml_form['dynamic_form']['mean']['offset']

    xmlform_window.xml_form_dict['texture_form']['period'] = all_forms_window.xml_form['texture_form']['period']
    xmlform_window.xml_form_dict['texture_form']['phase'] = all_forms_window.xml_form['texture_form']['phase']
    xmlform_window.xml_form_dict['texture_form']['amplitude'] = all_forms_window.xml_form['texture_form']['amplitude']
    xmlform_window.xml_form_dict['texture_form']['offset'] = all_forms_window.xml_form['texture_form']['offset']

    xmlform_window.xml_form_dict['pitch_form']['range']['period'] = all_forms_window.xml_form['pitch_form']['range']['period']
    xmlform_window.xml_form_dict['pitch_form']['range']['phase'] = all_forms_window.xml_form['pitch_form']['range']['phase']
    xmlform_window.xml_form_dict['pitch_form']['range']['amplitude'] = all_forms_window.xml_form['pitch_form']['range']['amplitude']
    xmlform_window.xml_form_dict['pitch_form']['range']['offset'] = all_forms_window.xml_form['pitch_form']['range']['offset']

    xmlform_window.xml_form_dict['rhythm_form']['range']['period'] = all_forms_window.xml_form['rhythm_form']['range']['period']
    xmlform_window.xml_form_dict['rhythm_form']['range']['phase'] = all_forms_window.xml_form['rhythm_form']['range']['phase']
    xmlform_window.xml_form_dict['rhythm_form']['range']['amplitude'] = all_forms_window.xml_form['rhythm_form']['range']['amplitude']
    xmlform_window.xml_form_dict['rhythm_form']['range']['offset'] = all_forms_window.xml_form['rhythm_form']['range']['offset']

    xmlform_window.xml_form_dict['dynamic_form']['range']['period'] = all_forms_window.xml_form['dynamic_form']['range']['period']
    xmlform_window.xml_form_dict['dynamic_form']['range']['phase'] = all_forms_window.xml_form['dynamic_form']['range']['phase']
    xmlform_window.xml_form_dict['dynamic_form']['range']['amplitude'] = all_forms_window.xml_form['dynamic_form']['range']['amplitude']
    xmlform_window.xml_form_dict['dynamic_form']['range']['offset'] = all_forms_window.xml_form['dynamic_form']['range']['offset']

    form_document_type = dom_impl.createDocumentType("boost_serialization", None, None)
    form_document = dom_impl.createDocument(None, "boost_serialization", form_document_type)
    top = form_document.documentElement
    top.setAttribute("signature", 'serialization::archive')
    top.setAttribute("version", '14')
    xml_form_element = form_document.createElement("xml_form")
    xml_form_element.setAttribute("class_id", '0')
    xml_form_element.setAttribute("tracking_level", '0')
    xml_form_element.setAttribute("version", '0')
    top.appendChild(xml_form_element)

    add_text_element(form_document, xml_form_element, "name")
    add_text_element(form_document, xml_form_element, "len")
    add_text_element(form_document, xml_form_element, "min_note_len")
    add_text_element(form_document, xml_form_element, "max_note_len")

    scale_element = form_document.createElement("scale_")
    scale_element.setAttribute("class_id", '1')
    scale_element.setAttribute("tracking_level", '0')
    scale_element.setAttribute("version", '0')

    add_text_element(form_document, scale_element, "count", "count", str(len(xmlform_window.xml_form_dict['scale'])))

    add_text_element(form_document, scale_element, "item_version", "item_version", "0")

    for n in xmlform_window.xml_form_dict['scale']:
        add_text_element(form_document, scale_element, "item", "item", n)

    xml_form_element.appendChild(scale_element)

    add_text_element(form_document, xml_form_element, "pulse")

    melody_probabilities_element = form_document.createElement("melody_probabilities_")
    melody_probabilities_element.setAttribute("class_id", '2')
    melody_probabilities_element.setAttribute("tracking_level", '0')
    melody_probabilities_element.setAttribute("version", '0')

    add_text_element(form_document, melody_probabilities_element, "down", "down_", str(xmlform_window.xml_form_dict['melody_probabilities']['down']))
    add_text_element(form_document, melody_probabilities_element, "same", "same_", str(xmlform_window.xml_form_dict['melody_probabilities']['same']))
    add_text_element(form_document, melody_probabilities_element, "up", "up_", str(xmlform_window.xml_form_dict['melody_probabilities']['up']))
    xml_form_element.appendChild(melody_probabilities_element)

    add_form_element(form_document, xml_form_element, "pitch_form", True)
    add_form_element(form_document, xml_form_element, "rhythm_form")
    add_form_element(form_document, xml_form_element, "dynamic_form")
    add_sine_element(form_document, xml_form_element, "texture_form")

    voices_element = form_document.createElement("voices_")
    voices_element.setAttribute("class_id", '5')
    voices_element.setAttribute("tracking_level", '0')
    voices_element.setAttribute("version", '0')

    add_text_element(form_document, voices_element, "count", "count", str(len(xmlform_window.xml_form_dict['voices'])))
    add_text_element(form_document, voices_element, "item_version", "item_version", "0")

    write_version = True
    for v in xmlform_window.xml_form_dict['voices']:
        add_voice_element(form_document, voices_element, v, write_version)
        write_version = False

    xml_form_element.appendChild(voices_element)

    form_document.writexml(open(afilename, 'w'), indent="", addindent="\t", newl='\n', encoding="UTF-8", standalone=True)
    form_document.unlink()


def add_voice_element(doc, parent, voice_dict, write_version):
    item_element = doc.createElement("item")
    if (write_version):
        item_element.setAttribute("class_id", '6')
        item_element.setAttribute("tracking_level", '0')
        item_element.setAttribute("version", '0')

    add_text_element(doc, item_element, "low_pitch", "low_pitch_", str(voice_dict['low_pitch']))
    add_text_element(doc, item_element, "high_pitch", "high_pitch_", str(voice_dict['high_pitch']))
    add_text_element(doc, item_element, "channel", "channel_", str(voice_dict['channel']))
    add_text_element(doc, item_element, "walking", "walking_", str(voice_dict['walking']))
    add_text_element(doc, item_element, "program", "program_", str(voice_dict['program']))
    add_text_element(doc, item_element, "pan", "pan_", str(voice_dict['pan']))
    follower_element = doc.createElement("follower_")
    if (write_version):
        follower_element.setAttribute("class_id", '7')
        follower_element.setAttribute("tracking_level", '0')
        follower_element.setAttribute("version", '0')

    add_text_element(doc, follower_element, "follow", "follow_", str(voice_dict['follower']['follow']))
    add_text_element(doc, follower_element, "leader", "leader_", str(voice_dict['follower']['leader']))
    add_text_element(doc, follower_element, "interval_type", "interval_type_", str(voice_dict['follower']['interval_type']))
    add_text_element(doc, follower_element, "interval", "interval_", str(voice_dict['follower']['interval']))
    item_element.appendChild(follower_element)
    parent.appendChild(item_element)

def add_form_element(doc, parent, form_name, add_attributes = False):
    form_element = doc.createElement(form_name + "_")
    if (add_attributes == True):
        form_element.setAttribute("class_id", '3')
        form_element.setAttribute("tracking_level", '0')
        form_element.setAttribute("version", '0')

    mean_sine_element = doc.createElement("mean_sine_")
    if (add_attributes == True):
        mean_sine_element.setAttribute("class_id", '4')
        mean_sine_element.setAttribute("tracking_level", '0')
        mean_sine_element.setAttribute("version", '0')
    add_text_element(doc, mean_sine_element , "period", "period_", str(xmlform_window.xml_form_dict[form_name]['mean']['period']))
    add_text_element(doc, mean_sine_element , "phase", "phase_", str(xmlform_window.xml_form_dict[form_name]['mean']['phase']))
    add_text_element(doc, mean_sine_element , "amplitude", "amplitude_", str(xmlform_window.xml_form_dict[form_name]['mean']['amplitude']))
    add_text_element(doc, mean_sine_element , "offset", "offset_", str(xmlform_window.xml_form_dict[form_name]['mean']['offset']))
    form_element.appendChild(mean_sine_element)
    range_sine_element = doc.createElement("range_sine_")
    add_text_element(doc, range_sine_element , "period", "period_", str(xmlform_window.xml_form_dict[form_name]['range']['period']))
    add_text_element(doc, range_sine_element , "phase", "phase_", str(xmlform_window.xml_form_dict[form_name]['range']['phase']))
    add_text_element(doc, range_sine_element , "amplitude", "amplitude_", str(xmlform_window.xml_form_dict[form_name]['range']['amplitude']))
    add_text_element(doc, range_sine_element , "offset", "offset_", str(xmlform_window.xml_form_dict[form_name]['range']['offset']))
    form_element.appendChild(range_sine_element)
    parent.appendChild(form_element)

def add_sine_element(doc, parent, sine_name):
    sine_element = doc.createElement(sine_name + "_")
    add_text_element(doc, sine_element , "period", "period_", str(xmlform_window.xml_form_dict[sine_name]['period']))
    add_text_element(doc, sine_element , "phase", "phase_", str(xmlform_window.xml_form_dict[sine_name]['phase']))
    add_text_element(doc, sine_element , "amplitude", "amplitude_", str(xmlform_window.xml_form_dict[sine_name]['amplitude']))
    add_text_element(doc, sine_element , "offset", "offset_", str(xmlform_window.xml_form_dict[sine_name]['offset']))
    parent.appendChild(sine_element)

def add_text_element(doc, parent, name, xmltag = None, value = None):
    if (xmltag == None):
        xmltag = name + "_"
    if (value == None):
        value = str(xmlform_window.xml_form_dict[name])

    any_element = doc.createElement(xmltag)
    parent.appendChild(any_element)
    any_text_node = doc.createTextNode(value)
    any_element.appendChild(any_text_node)
    parent.appendChild(any_element)

def redraw_callback():
    xmlform_window.xml_form_dict['name'] = all_forms_window.xml_form['name']
    xmlform_window.xml_form_dict['len'] = all_forms_window.xml_form['len']
    xmlform_window.xml_form_dict['min_note_len'] = all_forms_window.xml_form['min_note_len']
    xmlform_window.xml_form_dict['max_note_len'] = all_forms_window.xml_form['max_note_len']
    xmlform_window.xml_form_dict['pulse'] = all_forms_window.xml_form['pulse']
    xmlform_window.xml_form_dict['melody_probabilities']['down'] = all_forms_window.xml_form['melody_probabilities']['down']
    xmlform_window.xml_form_dict['melody_probabilities']['same'] = all_forms_window.xml_form['melody_probabilities']['same']
    xmlform_window.xml_form_dict['melody_probabilities']['up'] = all_forms_window.xml_form['melody_probabilities']['up']

    xmlform_window.xml_form_dict['pitch_form']['mean']['period'] = all_forms_window.xml_form['pitch_form']['mean']['period']
    xmlform_window.xml_form_dict['pitch_form']['mean']['phase'] = all_forms_window.xml_form['pitch_form']['mean']['phase']
    xmlform_window.xml_form_dict['pitch_form']['mean']['amplitude'] = all_forms_window.xml_form['pitch_form']['mean']['amplitude']
    xmlform_window.xml_form_dict['pitch_form']['mean']['offset'] = all_forms_window.xml_form['pitch_form']['mean']['offset']

    xmlform_window.xml_form_dict['rhythm_form']['mean']['period'] = all_forms_window.xml_form['rhythm_form']['mean']['period']
    xmlform_window.xml_form_dict['rhythm_form']['mean']['phase'] = all_forms_window.xml_form['rhythm_form']['mean']['phase']
    xmlform_window.xml_form_dict['rhythm_form']['mean']['amplitude'] = all_forms_window.xml_form['rhythm_form']['mean']['amplitude']
    xmlform_window.xml_form_dict['rhythm_form']['mean']['offset'] = all_forms_window.xml_form['rhythm_form']['mean']['offset']

    xmlform_window.xml_form_dict['dynamic_form']['mean']['period'] = all_forms_window.xml_form['dynamic_form']['mean']['period']
    xmlform_window.xml_form_dict['dynamic_form']['mean']['phase'] = all_forms_window.xml_form['dynamic_form']['mean']['phase']
    xmlform_window.xml_form_dict['dynamic_form']['mean']['amplitude'] = all_forms_window.xml_form['dynamic_form']['mean']['amplitude']
    xmlform_window.xml_form_dict['dynamic_form']['mean']['offset'] = all_forms_window.xml_form['dynamic_form']['mean']['offset']

    xmlform_window.xml_form_dict['texture_form']['period'] = all_forms_window.xml_form['texture_form']['period']
    xmlform_window.xml_form_dict['texture_form']['phase'] = all_forms_window.xml_form['texture_form']['phase']
    xmlform_window.xml_form_dict['texture_form']['amplitude'] = all_forms_window.xml_form['texture_form']['amplitude']
    xmlform_window.xml_form_dict['texture_form']['offset'] = all_forms_window.xml_form['texture_form']['offset']

    xmlform_window.xml_form_dict['pitch_form']['range']['period'] = all_forms_window.xml_form['pitch_form']['range']['period']
    xmlform_window.xml_form_dict['pitch_form']['range']['phase'] = all_forms_window.xml_form['pitch_form']['range']['phase']
    xmlform_window.xml_form_dict['pitch_form']['range']['amplitude'] = all_forms_window.xml_form['pitch_form']['range']['amplitude']
    xmlform_window.xml_form_dict['pitch_form']['range']['offset'] = all_forms_window.xml_form['pitch_form']['range']['offset']

    xmlform_window.xml_form_dict['rhythm_form']['range']['period'] = all_forms_window.xml_form['rhythm_form']['range']['period']
    xmlform_window.xml_form_dict['rhythm_form']['range']['phase'] = all_forms_window.xml_form['rhythm_form']['range']['phase']
    xmlform_window.xml_form_dict['rhythm_form']['range']['amplitude'] = all_forms_window.xml_form['rhythm_form']['range']['amplitude']
    xmlform_window.xml_form_dict['rhythm_form']['range']['offset'] = all_forms_window.xml_form['rhythm_form']['range']['offset']

    xmlform_window.xml_form_dict['dynamic_form']['range']['period'] = all_forms_window.xml_form['dynamic_form']['range']['period']
    xmlform_window.xml_form_dict['dynamic_form']['range']['phase'] = all_forms_window.xml_form['dynamic_form']['range']['phase']
    xmlform_window.xml_form_dict['dynamic_form']['range']['amplitude'] = all_forms_window.xml_form['dynamic_form']['range']['amplitude']
    xmlform_window.xml_form_dict['dynamic_form']['range']['offset'] = all_forms_window.xml_form['dynamic_form']['range']['offset']

    xmlform_window.draw_form()

def about_callback():
    about_window = tkinter.Text(Toplevel(root))
    about_window.grid(sticky="we", row=0, column=0)
    about_window.insert('1.0', "textmidiformedit musical form editor by Thomas E. Janzen\n2021\nUse with textmidicgm, part of TextMIDITools\nat github.com/tomejanzen/TextMIDITools")
    about_window['state'] = "disabled"
    about_window.focus()

if __name__ == '__main__':
    root = tkinter.Tk()
    root.title('textmidiformedit')
    makemenu(root)
    msg = Label(root, text='Window menu')
    msg.grid(sticky="n")
    msg.config(relief=SUNKEN, width=40)

    xmlform_window = XmlForm(root)
    xmlform_window.grid(sticky="we", row=0, column=0)
    xmlform_window.rowconfigure(index=0, weight=1)
    xmlform_window.columnconfigure(index=0, weight=1)

    all_forms_window = AllFormsWindow(Toplevel(root), xml_form = xmlform_window.xml_form_dict)
    all_forms_window.grid(sticky="nw", row=2, column=0)
    all_forms_window.rowconfigure(index=2, weight=1)
    all_forms_window.columnconfigure(index=0, weight=1)

    voice_window = VoiceWindow(Toplevel(root), xml_form=xmlform_window.xml_form_dict)
    voice_window.grid(sticky="w", row=2, column=1)
    voice_window.rowconfigure(index=2, weight=1)
    voice_window.columnconfigure(index=1, weight=1)

    mainloop()
