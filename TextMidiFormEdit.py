#!/usr/bin/env python3
"""TextMIDITools: TextMidiFormEdit.py top-level module."""
# TextMIDITools Version 1.1.0
# Copyright © 2025 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#
import math
import sys
import tkinter
import tkinter.constants
import tkinter.filedialog
import tkinter.ttk

from tkinter import *
from tkinter.ttk import *
from xml.dom import minidom
from xml.dom.minidom import parse, Node, getDOMImplementation

from AllFormsWindow import *
from AllFormsWindow import AllFormsWindow
from AllFormsWindow import ScaleFrame
from VoiceWindow import VoiceWindow

class XmlForm(tkinter.Tk):
    """XML Form support using C++ boost serialization XML archive style."""
    xml_form_dict = {}
    twopi = 2.0 * math.pi
    the_filename = 'Untitled'

    def __init__(self, filename):
        """Init the XML form (a C++ boost libraries serialization XML archive style DOM)."""
        super().__init__()
        self.default_xml_form()
        self.all_forms_window = AllFormsWindow(self.xml_form_dict)
        self.voice_window = VoiceWindow(self.xml_form_dict)
        self.win_height = 600
        self.win_width = 1000
        self.canvas = Canvas(self, bg='#88AAFF', height=self.win_height,
            width=self.win_width, background='#88AAFF')
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)

        self.canvas.bind('<Configure>', self.configure_callback)

        self.draw_form()
        self.title('Form Plot')

        self.frame = tkinter.ttk.Frame(self, padding='1 1 1 1')

        self.option_add('*tearOff', FALSE)
        self.frame.grid(column=0, row=0, sticky=(N, W, E, S))
        self.columnconfigure(0, weight=1)
        self.rowconfigure(0, weight=1)
        self.frame.focus()
        self.makemenu(self)

        self.frame.grid(sticky='we', row=0, column=2)
        self.frame.rowconfigure(index=0, weight=1)
        self.frame.columnconfigure(index=2, weight=1)

        if filename != '':
            self.install_file(filename)
            self.voice_window.install_xml_form(self.xml_form_dict)
            self.all_forms_window.install_xml_form(self.xml_form_dict)

    def install_file(self, afilename):
        """Install a file read from the OS."""
        self.the_filename = afilename
        # get the xml.dom.minidom.Document
        self.dom = parse(self.the_filename)
        self.traverse_xml_form()
        self.transpose = IntVar()
        self.all_forms_window.scale_frame.transpose_spinbox.set(0)
        self.draw_form()

    def configure_callback(self, event=None):
        """Configure a canvas."""
        self.win_width = event.width
        self.win_height = event.height
        self.draw_form()

    def draw_form(self):
        """Draw the form."""
        duration = float(self.xml_form_dict['len'])

        self.canvas.delete('all')
        axis_color = '#000000'
        self.canvas.grid(row=0, column=0, sticky=NSEW)
        seconds_per_pixel = duration / float(self.win_width)

        poly_lr = []
        poly_rl = []
        y_real_estate = float(self.win_height) / 16.0
        y_zero = 3 * y_real_estate
        y_midline = y_zero - 0.5 * y_real_estate
        y_label = y_midline - 60
        self.canvas.create_text(30, y_label, text='Pitch', fill=axis_color)
        self.canvas.create_line(0, y_midline, self.win_width, y_midline,
            fill=axis_color, dash='-', width=2, activewidth=2, disabledwidth=2)
        self.canvas.create_line(0, y_midline + 0.5 * y_real_estate,
            self.win_width, y_midline + 0.5 * y_real_estate, fill=axis_color,
            dash='.', width=1, activewidth=1, disabledwidth=1)
        self.canvas.create_line(0, y_midline - 0.5 * y_real_estate,
            self.win_width, y_midline - 0.5 * y_real_estate,
            fill=axis_color, dash='.', width=1, activewidth=1, disabledwidth=1)
        for x_pixel in range(1, self.win_width, 1):
            mean_freq  = (1.0 / float(self.xml_form_dict
                        ['pitch_form']['mean']['period']))
            range_freq = (1.0
                / float(self.xml_form_dict['pitch_form']['range']['period']))
            the_time = float(x_pixel) * seconds_per_pixel
            # omega * t = twopi * mean_freq  * the_time
            # The phase = self.xml_form_dict['pitch_form']['mean']['phase']
            # omega * t + phase = twopi * mean_freq  * the_time
            #     + self.xml_form_dict['pitch_form']['mean']['phase']
            # Take sin(omega*t + phase) which ranges -1.0 to 1.0:
            # Scale with gain, or "amplitude"
            # sin(omega * t + phase) * amplitude
            # Add in the offset
            # sin(omega * t + phase) * amplitude + offset
            # Scale and invert for left-handed coordinates
            #     for graphical plotting on the canvas
            # (sin(omega * t + phase) * amplitude
            #     + offset) * (-self.win_height / 16)
            # position it on the canvas
            # 0.5 + sin(omega * t + phase) / 2 * self.win_height / 16
            #     + 1 * self.win_height / 16
            # oi la!
            twopi_t = self.twopi * the_time

            y_mean  = ((math.sin(twopi_t * mean_freq
                + float(self.xml_form_dict['pitch_form']['mean']['phase']))
                * float(self.xml_form_dict['pitch_form']['mean']['amplitude'])
                + float(self.xml_form_dict['pitch_form']['mean']['offset']))
                * (-y_real_estate) + y_zero)
            y_range = ((math.sin(twopi_t * range_freq
                + float(self.xml_form_dict['pitch_form']['range']['phase']))
                * float(self.xml_form_dict['pitch_form']['range']['amplitude'])
                + float(self.xml_form_dict['pitch_form']['range']['offset']))
                * y_real_estate)

            y_range = max(y_range, 1.0)
            poly_lr.append(x_pixel)
            poly_lr.append(y_mean - y_range / 2)
            poly_rl.append(y_mean + y_range / 2)
            poly_rl.append(x_pixel)
        poly_rl.reverse()
        polygon = poly_lr + poly_rl
        fill_color = '#004400'
        pitch_plot_id = self.canvas.create_polygon(polygon,
            fill=fill_color, width=1, activewidth=1, disabledwidth=1, dash='_')

        y_zero = 6.5 * y_real_estate
        y_midline = y_zero - 0.5 * y_real_estate
        y_label = y_midline - 60
        self.canvas.create_text(30, y_label, text='Rhythm', fill=axis_color)
        self.canvas.create_line(0, y_midline, self.win_width, y_midline,
            fill=axis_color, dash='-', width=2, activewidth=2, disabledwidth=2)
        self.canvas.create_line(0, y_midline + 0.5 * y_real_estate,
            self.win_width, y_midline + 0.5 * y_real_estate, fill=axis_color,
            dash='.', width=1, activewidth=1, disabledwidth=1)
        self.canvas.create_line(0, y_midline - 0.5 * y_real_estate,
            self.win_width, y_midline - 0.5 * y_real_estate,
            fill=axis_color, dash='.', width=1, activewidth=1, disabledwidth=1)
        poly_lr = []
        poly_rl = []
        for x_pixel in range(1, self.win_width):
            mean_freq  = (1.0
                / float(self.xml_form_dict['rhythm_form']['mean']['period']))
            range_freq = (1.0
                / float(self.xml_form_dict['rhythm_form']['range']['period']))
            the_time = float(x_pixel) * seconds_per_pixel
            twopi_t = self.twopi * the_time
            y_mean  = ((math.sin(twopi_t * mean_freq
                 + float(self.xml_form_dict['rhythm_form']['mean']['phase']))
                 * float(self.xml_form_dict['rhythm_form']['mean']['amplitude'])
                 + float(self.xml_form_dict['rhythm_form']['mean']['offset']))
                 * (-y_real_estate) + y_zero)
            y_range = ((math.sin(twopi_t * range_freq
                + float(self.xml_form_dict['rhythm_form']['range']['phase']))
                * float(self.xml_form_dict['rhythm_form']['range']['amplitude'])
                + float(self.xml_form_dict['rhythm_form']['range']['offset']))
                * y_real_estate)
            y_range = max(y_range, 1.0)
            poly_lr.append(x_pixel)
            poly_lr.append(y_mean - y_range / 2)
            poly_rl.append(y_mean + y_range / 2)
            poly_rl.append(x_pixel)
        poly_rl.reverse()
        polygon = poly_lr + poly_rl
        rhythm_plot_id = self.canvas.create_polygon(polygon, fill=fill_color,
            width=2, activewidth=2, disabledwidth=2, dash='_')

        y_zero = 10 * y_real_estate
        y_midline = y_zero - 0.5 * y_real_estate
        y_label = y_midline - 60
        self.canvas.create_text(30, y_label, text='Dynamic', fill=axis_color)
        self.canvas.create_line(0, y_midline, self.win_width, y_midline,
            fill=axis_color, dash='-', width=2, activewidth=2, disabledwidth=2)
        self.canvas.create_line(0, y_midline + 0.5 * y_real_estate,
            self.win_width, y_midline + 0.5 * y_real_estate, fill=axis_color,
            dash='.', width=1, activewidth=1, disabledwidth=1)
        self.canvas.create_line(0, y_midline - 0.5 * y_real_estate,
            self.win_width, y_midline - 0.5 * y_real_estate, fill=axis_color,
            dash='.', width=1, activewidth=1, disabledwidth=1)
        poly_lr = []
        poly_rl = []
        for x_pixel in range(1, self.win_width):
            mean_freq = (1.0
                / float(self.xml_form_dict['dynamic_form']['mean']['period']))
            range_freq = (1.0
                / float(self.xml_form_dict['dynamic_form']['range']['period']))
            the_time = float(x_pixel) * seconds_per_pixel
            twopi_t = self.twopi * the_time
            y_mean  = (((math.sin(twopi_t * mean_freq
                + float(self.xml_form_dict['dynamic_form']['mean']['phase']))
                * float(self.xml_form_dict['dynamic_form']['mean']['amplitude'])
                + float(self.xml_form_dict['dynamic_form']['mean']['offset']))
                * (-y_real_estate) + y_zero))
            y_range = (((math.sin(twopi_t * range_freq
               + float(self.xml_form_dict['dynamic_form']['range']['phase']))
               * float(self.xml_form_dict['dynamic_form']['range']['amplitude'])
               + float(self.xml_form_dict['dynamic_form']['range']['offset']))
               * y_real_estate))
            y_range = max(y_range, 1.0)
            poly_lr.append(x_pixel)
            poly_lr.append(y_mean - y_range / 2)
            poly_rl.append(y_mean + y_range / 2)
            poly_rl.append(x_pixel)
        poly_rl.reverse()
        polygon = poly_lr + poly_rl
        dynamic_plot_id = self.canvas.create_polygon(polygon, fill=fill_color,
            width=2, activewidth=2, disabledwidth=2, dash='_')

        # Texture (number of voices playing) was misdefined as the mean alone,
        # but really it is the range.  So there is a reversal of roles here.
        y_zero = 13.5 * y_real_estate
        y_midline = y_zero - 0.5 * y_real_estate
        y_label = y_midline - 60
        self.canvas.create_text(30, y_label, text='Texture', fill=axis_color)
        self.canvas.create_line(0, y_midline, self.win_width, y_midline,
            fill=axis_color, dash='-', width=2, activewidth=2, disabledwidth=2)
        self.canvas.create_line(0, y_midline + 0.5 * y_real_estate,
            self.win_width, y_midline + 0.5 * y_real_estate, fill=axis_color,
            dash='.', width=1, activewidth=1, disabledwidth=1)
        self.canvas.create_line(0, y_midline - 0.5 * y_real_estate,
            self.win_width, y_midline - 0.5 * y_real_estate, fill=axis_color,
            dash='.', width=1, activewidth=1, disabledwidth=1)
        poly_lr = []
        poly_rl = []
        for x_pixel in range(1, self.win_width):
            mean_freq = (1.0 / float(self.xml_form_dict['texture_form']['period']))
            the_time = float(x_pixel) * seconds_per_pixel
            twopi_t = self.twopi * the_time
            y_mean  = ((math.sin(twopi_t * mean_freq
                 + float(self.xml_form_dict['texture_form']['phase']))
                 * float(self.xml_form_dict['texture_form']['amplitude'])
                 + float(self.xml_form_dict['texture_form']['offset']))
                 * (-y_real_estate))
            y_range = y_midline
            poly_lr.append(x_pixel)
            poly_lr.append(y_range - y_mean / 2)
            poly_rl.append(y_range + y_mean / 2)
            poly_rl.append(x_pixel)
        poly_rl.reverse()
        polygon = poly_lr + poly_rl
        texture_plot_id = self.canvas.create_polygon(polygon, fill=fill_color,
            width=2, activewidth=2, disabledwidth=2, dash='_')

        y_range = max(y_range, 1.0)
        # Draw minute markers
        for time_ctr in range(0, int(duration), 60):
            tickx = time_ctr / seconds_per_pixel
            self.canvas.create_line(tickx, 0, tickx, self.win_height,
                fill=axis_color, dash='-', width=2, activewidth=2,
                disabledwidth=2)
            self.canvas.create_line(0, y_midline + 0.5 * y_real_estate,
                self.win_width, y_midline + 0.5 * y_real_estate,
                fill=axis_color, dash='.', width=2, activewidth=2,
                disabledwidth=2)
            self.canvas.create_line(0, y_midline - 0.5 * y_real_estate,
                self.win_width, y_midline - 0.5 * y_real_estate,
                fill=axis_color, dash='.', width=2, activewidth=2,
                disabledwidth=2)

        self.canvas.create_text(self.win_width / 2, 20,
            text=self.xml_form_dict['name'], fill=axis_color)
        self.canvas.create_text(self.win_width / 2, 32,
            text=self.xml_form_dict['copyright'], fill=axis_color)

    def default_melody_probabilities(self):
        """Set up default melody cumulative probabilities."""
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
        melody_probabilities_dict['down'] = 0.0
        melody_probabilities_dict['same'] = 0.333333333
        melody_probabilities_dict['up'] = 0.6666667
        return melody_probabilities_dict

    def default_form(self):
        """Install a default form."""
        parameter_dict = {}
        param_mean_dict = {}
        param_mean_dict['period'] = 180.0
        param_mean_dict['phase']  = 0.0
        param_mean_dict['amplitude'] = 0.50
        param_mean_dict['offset'] = 0.50
        parameter_dict['mean'] = param_mean_dict

        param_range_dict = {}
        param_range_dict['period'] = 180.0
        param_range_dict['phase'] = 0.0
        param_range_dict['amplitude'] = 0.50
        param_range_dict['offset'] = 0.50
        parameter_dict['range'] = param_range_dict
        return parameter_dict

    def default_texture_form(self):
        """Install default texture form."""
        texture_dict = {}
        texture_dict['period'] = 180.0
        texture_dict['phase'] = 0.0
        texture_dict['amplitude'] = 0.50
        texture_dict['offset'] = 0.50
        return texture_dict

    def default_voices(self):
        """Install default voice settings."""
        vox_list = []
        for vox in range(1, 25):
            voice_dict = {}
            voice_dict['low_pitch'] = 'A0'
            voice_dict['high_pitch'] = 'C8'
            voice_dict['channel'] = 1
            voice_dict['walking'] = 0
            voice_dict['program'] = 1
            voice_dict['pan'] = 0
            follower_dict = {}
            follower_dict['follow'] = False
            follower_dict['leader'] = 2147483647
            follower_dict['interval_type'] = 0
            follower_dict['interval'] = 0
            delay_dict = {}
            delay_dict['numerator'] = '0'
            delay_dict['denominator'] = '1'
            follower_dict['delay'] = delay_dict
            duration_factor_dict = {}
            duration_factor_dict['numerator'] = '1'
            duration_factor_dict['denominator'] = '1'
            follower_dict['duration_factor'] = duration_factor_dict
            follower_dict['inversion'] = False
            follower_dict['retrograde'] = False
            voice_dict['follower'] = follower_dict
            vox_list.append(voice_dict)

        return vox_list

    def default_arrangement_definition(self):
        """Install default arrangement."""
        arrangement_definition_dict = {}
        arrangement_definition_dict['algorithm'] = 1
        arrangement_definition_dict['period'] = 100000.0
        return arrangement_definition_dict

    def default_xml_form(self):
        """Install default form."""
        self.xml_form_dict['name'] = 'defaults'
        self.xml_form_dict['copyright'] = 'Copyright © unspecified'
        self.xml_form_dict['len'] = 600.0
        # at the old MIDI baud rate a note_on+note_off with running status
        # (i.e., no status byte) is 0.00128 seconds.  Zero can
        # make scores that don't advance in time, or make players crash.
        self.xml_form_dict['min_note_len'] = 0.00128
        self.xml_form_dict['max_note_len'] = 2.0
        self.xml_form_dict['scale'] = ScaleFrame.full_midi_scale
        self.xml_form_dict['music_time'] = {}
        self.xml_form_dict['music_time']['ticks_per_quarter'] = '1440'
        beat_dict = {}
        beat_dict['numerator']   = 1
        beat_dict['denominator'] = 4
        self.xml_form_dict['music_time']['beat'] = beat_dict
        meter_dict = {}
        meter_dict['numerator']   = 4
        meter_dict['denominator'] = 4
        self.xml_form_dict['music_time']['meter']  = meter_dict
        self.xml_form_dict['music_time']['beat_tempo'] = 60.0

        self.xml_form_dict['pulse'] = 8.0
        self.xml_form_dict['melody_probabilities'] = self.default_melody_probabilities()
        self.xml_form_dict['pitch_form'] = self.default_form()
        self.xml_form_dict['rhythm_form'] = self.default_form()
        self.xml_form_dict['dynamic_form'] = self.default_form()
        self.xml_form_dict['texture_form'] = self.default_texture_form()
        self.xml_form_dict['voices'] = self.default_voices()
        self.xml_form_dict['arrangement_definition'] = self.default_arrangement_definition()

    def traverse_scale(self, scale_dom):
        """Build a scale from the domain object model."""
        scale_array = []
        scale_list = scale_dom.getElementsByTagName('item')

        for item_node in scale_list :
            scale_array.append(item_node.firstChild.data)
        return scale_array

    def traverse_music_time(self, music_time_dom):
        """Build a music_time from the domain object model."""
        music_time_dict = {}
        ticks_per_quarter_node = music_time_dom.getElementsByTagName('ticks_per_quarter_')[0]
        music_time_dict['ticks_per_quarter'] = ticks_per_quarter_node.firstChild.data
        meter_node = music_time_dom.getElementsByTagName('meter_')[0]
        numerator_node = meter_node.getElementsByTagName('numerator_')[0]
        numerator = numerator_node.firstChild.data
        denominator_node = meter_node.getElementsByTagName('denominator_')[0]
        denominator = denominator_node.firstChild.data
        music_time_dict['meter'] = {}
        music_time_dict['meter']['numerator'] = numerator
        music_time_dict['meter']['denominator'] = denominator

        beat_node = music_time_dom.getElementsByTagName('beat_')[0]
        numerator_node = beat_node.getElementsByTagName('numerator_')[0]
        numerator = numerator_node.firstChild.data
        denominator_node = beat_node.getElementsByTagName('denominator_')[0]
        denominator = denominator_node.firstChild.data
        music_time_dict['beat'] = {}
        music_time_dict['beat']['numerator'] = numerator
        music_time_dict['beat']['denominator'] = denominator
        beat_tempo_node = music_time_dom.getElementsByTagName('beat_tempo_')[0]
        music_time_dict['beat_tempo'] = beat_tempo_node.firstChild.data
        return music_time_dict

    def traverse_melody_probabilities(self, melody_probabilities_dom):
        """Install melody probabilities from the file DOM."""
        melody_probabilities_dict = {}
        down_element_list = melody_probabilities_dom.getElementsByTagName('down_')
        melody_probabilities_dict['down'] = down_element_list[0].firstChild.data
        same_element_list = melody_probabilities_dom.getElementsByTagName('same_')
        melody_probabilities_dict['same'] = same_element_list[0].firstChild.data
        up_element_list = melody_probabilities_dom.getElementsByTagName('up_')
        melody_probabilities_dict['up'] = up_element_list[0].firstChild.data
        return melody_probabilities_dict

    def traverse_pitch_form(self, pitch_form_dom):
        """Install the pitch form settings from the DOM."""
        pitch_dict = {}
        pitch_mean_dict = {}
        mean_sine_node = pitch_form_dom.getElementsByTagName('mean_sine_')[0]
        mean_period_node    = mean_sine_node.getElementsByTagName('period_')[0]
        mean_period = mean_period_node.firstChild.data
        pitch_mean_dict['period'] = float(mean_period)
        mean_phase_node     = mean_sine_node.getElementsByTagName('phase_')[0]
        mean_phase = mean_phase_node.firstChild.data
        pitch_mean_dict['phase'] = float(mean_phase)
        mean_amplitude_node = mean_sine_node.getElementsByTagName('amplitude_')[0]
        mean_amplitude = mean_amplitude_node.firstChild.data
        pitch_mean_dict['amplitude'] = float(mean_amplitude)
        mean_offset_node    = mean_sine_node.getElementsByTagName('offset_')[0]
        mean_offset = mean_offset_node.firstChild.data
        pitch_mean_dict['offset'] = float(mean_offset)
        pitch_dict['mean'] = pitch_mean_dict

        pitch_range_dict = {}
        range_sine_node = pitch_form_dom.getElementsByTagName('range_sine_')[0]
        range_period_node = range_sine_node.getElementsByTagName('period_')[0]
        range_period = range_period_node.firstChild.data
        pitch_range_dict['period'] = float(range_period)
        range_phase_node = range_sine_node.getElementsByTagName('phase_')[0]
        range_phase = range_phase_node.firstChild.data
        pitch_range_dict['phase'] = float(range_phase)
        range_amplitude_node = range_sine_node.getElementsByTagName('amplitude_')[0]
        range_amplitude = range_amplitude_node.firstChild.data
        pitch_range_dict['amplitude'] = float(range_amplitude)
        range_offset_node = range_sine_node.getElementsByTagName('offset_')[0]
        range_offset = range_offset_node.firstChild.data
        pitch_range_dict['offset'] = float(range_offset)
        pitch_dict['range'] = pitch_range_dict
        return pitch_dict

    def traverse_rhythm_form(self, rhythm_form_dom):
        """Install the rhythm form settings from the DOM."""
        rhythm_dict = {}
        rhythm_mean_dict = {}
        mean_sine_node = rhythm_form_dom.getElementsByTagName('mean_sine_')[0]
        mean_period_node    = mean_sine_node.getElementsByTagName('period_')[0]
        mean_period = mean_period_node.firstChild.data
        rhythm_mean_dict['period'] = float(mean_period)
        mean_phase_node     = mean_sine_node.getElementsByTagName('phase_')[0]
        mean_phase = mean_phase_node.firstChild.data
        rhythm_mean_dict['phase'] = float(mean_phase)
        mean_amplitude_node = mean_sine_node.getElementsByTagName('amplitude_')[0]
        mean_amplitude = mean_amplitude_node.firstChild.data
        rhythm_mean_dict['amplitude'] = float(mean_amplitude)
        mean_offset_node    = mean_sine_node.getElementsByTagName('offset_')[0]
        mean_offset = mean_offset_node.firstChild.data
        rhythm_mean_dict['offset'] = float(mean_offset)
        rhythm_dict['mean'] = rhythm_mean_dict

        rhythm_range_dict = {}
        range_sine_node = rhythm_form_dom.getElementsByTagName('range_sine_')[0]
        range_period_node = range_sine_node.getElementsByTagName('period_')[0]
        range_period = range_period_node.firstChild.data
        rhythm_range_dict['period'] = float(range_period)
        range_phase_node = range_sine_node.getElementsByTagName('phase_')[0]
        range_phase = range_phase_node.firstChild.data
        rhythm_range_dict['phase'] = float(range_phase)
        range_amplitude_node = range_sine_node.getElementsByTagName('amplitude_')[0]
        range_amplitude = range_amplitude_node.firstChild.data
        rhythm_range_dict['amplitude'] = float(range_amplitude)
        range_offset_node = range_sine_node.getElementsByTagName('offset_')[0]
        range_offset = range_offset_node.firstChild.data
        rhythm_range_dict['offset'] = float(range_offset)
        rhythm_dict['range'] = rhythm_range_dict
        return rhythm_dict

    def traverse_dynamic_form(self, dynamic_form_dom):
        """Install the dynamic form settings from the DOM."""
        dynamic_dict = {}
        dynamic_mean_dict = {}
        mean_sine_node = dynamic_form_dom.getElementsByTagName('mean_sine_')[0]
        mean_period_node    = mean_sine_node.getElementsByTagName('period_')[0]
        mean_period = mean_period_node.firstChild.data
        dynamic_mean_dict['period'] = float(mean_period)
        mean_phase_node     = mean_sine_node.getElementsByTagName('phase_')[0]
        mean_phase = mean_phase_node.firstChild.data
        dynamic_mean_dict['phase'] = float(mean_phase)
        mean_amplitude_node = mean_sine_node.getElementsByTagName('amplitude_')[0]
        mean_amplitude = mean_amplitude_node.firstChild.data
        dynamic_mean_dict['amplitude'] = float(mean_amplitude)
        mean_offset_node    = mean_sine_node.getElementsByTagName('offset_')[0]
        mean_offset = mean_offset_node.firstChild.data
        dynamic_mean_dict['offset'] = float(mean_offset)
        dynamic_dict['mean'] = dynamic_mean_dict

        dynamic_range_dict = {}
        range_sine_node = dynamic_form_dom.getElementsByTagName('range_sine_')[0]
        range_period_node = range_sine_node.getElementsByTagName('period_')[0]
        range_period = range_period_node.firstChild.data
        dynamic_range_dict['period'] = float(range_period)
        range_phase_node = range_sine_node.getElementsByTagName('phase_')[0]
        range_phase = range_phase_node.firstChild.data
        dynamic_range_dict['phase'] = float(range_phase)
        range_amplitude_node = range_sine_node.getElementsByTagName('amplitude_')[0]
        range_amplitude = range_amplitude_node.firstChild.data
        dynamic_range_dict['amplitude'] = float(range_amplitude)
        range_offset_node = range_sine_node.getElementsByTagName('offset_')[0]
        range_offset = range_offset_node.firstChild.data
        dynamic_range_dict['offset'] = float(range_offset)
        dynamic_dict['range'] = dynamic_range_dict
        return dynamic_dict

    def traverse_texture_form(self, texture_form_dom):
        """Install the texture form settings from the DOM."""
        texture_dict = {}
        period_node    = texture_form_dom.getElementsByTagName('period_')[0]
        period = period_node.firstChild.data
        texture_dict['period'] = float(period)
        phase_node     = texture_form_dom.getElementsByTagName('phase_')[0]
        phase = phase_node.firstChild.data
        texture_dict['phase'] = float(phase)
        amplitude_node = texture_form_dom.getElementsByTagName('amplitude_')[0]
        amplitude = amplitude_node.firstChild.data
        texture_dict['amplitude'] = float(amplitude)
        offset_node    = texture_form_dom.getElementsByTagName('offset_')[0]
        offset = offset_node.firstChild.data
        texture_dict['offset'] = float(offset)
        return texture_dict

    def traverse_voices(self, voices_dom):
        """Install the voice settings from the XML file's DOM."""
        xml_voice_list = voices_dom.getElementsByTagName('item')
        voice_list = []
        for vox in xml_voice_list:
            voice_dict = {}
            low_pitch_node = vox.getElementsByTagName('low_pitch_')[0]
            low_pitch = low_pitch_node.firstChild.data
            voice_dict['low_pitch'] = low_pitch
            high_pitch_node = vox.getElementsByTagName('high_pitch_')[0]
            high_pitch = high_pitch_node.firstChild.data
            voice_dict['high_pitch'] = high_pitch
            channel_node = vox.getElementsByTagName('channel_')[0]
            channel = channel_node.firstChild.data
            voice_dict['channel'] = int(channel)
            walking_node = vox.getElementsByTagName('walking_')[0]
            walking = walking_node.firstChild.data
            voice_dict['walking'] = walking
            program_node = vox.getElementsByTagName('program_')[0]
            program = program_node.firstChild.data
            voice_dict['program'] = int(program)
            pan_node = vox.getElementsByTagName('pan_')[0]
            pan = pan_node.firstChild.data
            voice_dict['pan'] = pan
            follower_dict = {}
            follower_node = vox.getElementsByTagName('follower_')[0]
            follow_node = follower_node.getElementsByTagName('follow_')[0]
            follower_dict['follow'] = bool(int(follow_node.firstChild.data))
            leader_node = follower_node.getElementsByTagName('leader_')[0]
            follower_dict['leader'] = int(leader_node.firstChild.data)
            follower_dict['interval_type'] = (int(follower_node
                .getElementsByTagName('interval_type_')[0].firstChild.data))
            interval_node = follower_node.getElementsByTagName('interval_')[0]
            follower_dict['interval'] = int(interval_node.firstChild.data)

            delay_dict = {}
            duration_factor_dict = {}
            if int(self.dom.getElementsByTagName('follower_')[0]
                  .getAttribute('version')) >= 2:
                delay_node = follower_node.getElementsByTagName('delay_')[0]
                numerator_node = delay_node.getElementsByTagName('numerator_')[0]
                numerator = numerator_node.firstChild.data
                denominator_node = delay_node.getElementsByTagName('denominator_')[0]
                denominator = denominator_node.firstChild.data
                delay_dict['numerator'] = numerator
                delay_dict['denominator'] = denominator

                duration_factor_dict = {}
                if int(self.dom.getElementsByTagName('follower_')[0]
                      .getAttribute('version')) >= 3:
                    duration_factor_node = (follower_node
                        .getElementsByTagName('duration_factor_')[0])
                    numerator_node = (duration_factor_node
                        .getElementsByTagName('numerator_')[0])
                    numerator = numerator_node.firstChild.data
                    denominator_node = (duration_factor_node
                        .getElementsByTagName('denominator_')[0])
                    denominator = denominator_node.firstChild.data
                    duration_factor_dict['numerator'] = numerator
                    duration_factor_dict['denominator'] = denominator

                follower_dict['inversion'] = bool(int(follower_node
                    .getElementsByTagName('inversion_')[0].firstChild.data))

                follower_dict['retrograde'] = bool(int(follower_node
                    .getElementsByTagName('retrograde_')[0].firstChild.data))
            else:
                delay_dict['numerator'] = '0'
                delay_dict['denominator'] = '1'
                duration_factor_dict['numerator'] = '1'
                duration_factor_dict['denominator'] = '1'
                follower_dict['inversion'] = False
                follower_dict['retrograde'] = False
            follower_dict['delay'] = delay_dict
            follower_dict['duration_factor'] = duration_factor_dict
            voice_dict['follower'] = follower_dict
            voice_list.append(voice_dict)

        return voice_list

    def traverse_arrangement_definition(self, arrangement_definition_dom):
        """Get the file's DOM arrangement settings and install in the internal form structure."""
        arrangement_definition_dict = {}
        algorithm_list = (arrangement_definition_dom
            .getElementsByTagName('algorithm_'))
        arrangement_definition_dict['algorithm'] = algorithm_list[0].firstChild.data
        period_list = arrangement_definition_dom.getElementsByTagName('period_')
        arrangement_definition_dict['period'] = period_list[0].firstChild.data
        return arrangement_definition_dict

    def traverse_xml_form(self):
        """Top-level XML form installation;
            read the DOM from the XML file and install internally.
        """
        self.xml_form_dict['name'] = (self.dom
            .getElementsByTagName('name_')[0].firstChild.data)
        if int(self.dom.getElementsByTagName('xml_form')[0]
            .getAttribute('version')) >= 3:
            self.xml_form_dict['copyright'] = (self.dom
                .getElementsByTagName('copyright_')[0].firstChild.data)
        else:
            self.xml_form_dict['copyright']    = 'copyright unspecified'
        self.xml_form_dict['len'] = float(self.dom
            .getElementsByTagName('len_')[0].firstChild.data)
        self.xml_form_dict['min_note_len'] = (self.dom.getElementsByTagName(
                    'min_note_len_')[0].firstChild.data)
        self.xml_form_dict['max_note_len'] = (self.dom.getElementsByTagName(
                    'max_note_len_')[0].firstChild.data)
        self.xml_form_dict['scale'] = (self.traverse_scale(
                    self.dom.getElementsByTagName('scale_')[0]))
        self.xml_form_dict['music_time'] = (self.traverse_music_time(
                    self.dom.getElementsByTagName('music_time_')[0]))
        self.xml_form_dict['pulse'] = self.dom.getElementsByTagName('pulse_')[0].firstChild.data
        self.xml_form_dict['melody_probabilities'] = (self.traverse_melody_probabilities
            (self.dom.getElementsByTagName('melody_probabilities_')[0]))

        self.xml_form_dict['pitch_form'] = (self.traverse_pitch_form(
            self.dom.getElementsByTagName('pitch_form_')[0]))
        self.xml_form_dict['rhythm_form'] = self.traverse_rhythm_form(self.dom
              .getElementsByTagName('rhythm_form_')[0])
        self.xml_form_dict['dynamic_form'] = self.traverse_dynamic_form(self.dom
              .getElementsByTagName('dynamic_form_')[0])
        self.xml_form_dict['texture_form'] = self.traverse_texture_form(self.dom
              .getElementsByTagName('texture_form_')[0])
        self.xml_form_dict['voices'] = (self.traverse_voices(
                    self.dom.getElementsByTagName('voices_')[0]))
        if int(self.dom.getElementsByTagName('xml_form')[0]
              .getAttribute('version')) >= 2:
            self.xml_form_dict['arrangement_definition'] = self.traverse_arrangement_definition(
                  self.dom.getElementsByTagName('arrangement_definition_')[0])
        else:
            self.xml_form_dict['arrangement_definition'] = self.default_arrangement_definition()

    def postscript_callback(self):
        """Write a PostScript file of the form window's plot."""
        afilename=tkinter.filedialog.asksaveasfilename(defaultextension='.ps',
            initialfile = self.all_forms_window.xml_form['name'],
            initialdir = '.',title = 'Save Postscript File',
            filetypes=(('ps files','*.ps'),('all files','*.*')))
        self.canvas.postscript(file=afilename, colormode='gray',
            pageheight='8i', pagewidth='8i')

    def makemenu(self, toplevelwin):
        """Build the menu for the form plot window."""
        top_menu = Menu(toplevelwin)
        toplevelwin['menu'] = top_menu
        file_menu = Menu(top_menu)
        top_menu.add_cascade(label='File', menu=file_menu, underline=0)
        file_menu.add_command(label='Open...',   command=self.get_file_callback,
            underline=0, accelerator='O')
        file_menu.add_command(label='Save...', command=self.save_callback,
            underline=0, accelerator='S')
        file_menu.add_command(label='Defaults...',
                command=self.defaults_callback, underline=0, accelerator='D')
        file_menu.add_command(label='Redraw', command=self.redraw_callback,
            underline=0, accelerator='R')
        file_menu.add_command(label='Save Postscript...',
            command=self.postscript_callback,
            underline=0, accelerator='S')
        file_menu.add_command(label='About...', command=self.about_callback,
            underline=0, accelerator='A')
        file_menu.add_command(label='Quit', command=lambda:toplevelwin.quit(),
            underline=0, accelerator='Q')

    def get_file_callback(self):
        """Present a file selector widget and get the filename."""
        afilename=tkinter.filedialog.askopenfilename(initialdir = '.',
            title = 'Select XML Form File',
            filetypes=(('xml files','*.xml'),('all files','*.*')))
        self.install_file(afilename)
        self.voice_window.install_xml_form(self.xml_form_dict)
        self.all_forms_window.install_xml_form(self.xml_form_dict)

    def defaults_callback(self):
        """Set the default values for a form."""
        self.all_forms_window.install_xml_form(self.xml_form_dict)
        self.default_xml_form()
        self.draw_form()
        self.voice_window.install_xml_form(self.xml_form_dict)
        self.title('Form Plot')

    def save_callback(self):
        """Save the internal form to a DOM and write it."""
        afilename=tkinter.filedialog.asksaveasfilename(defaultextension='.xml',
            initialfile = self.all_forms_window.xml_form['name'],
            initialdir = '.', title = 'Select XML Form File',
            filetypes=(('xml files','*.xml'),('all files','*.*')))
        dom_impl = getDOMImplementation()
        # copy form parts of xml_form_dict from AllForms
        # copy voice parts from Voice

        class_id = 0;

        self.xml_form_dict['name']         = self.all_forms_window.xml_form['name']
        self.xml_form_dict['copyright']    = self.all_forms_window.xml_form['copyright']
        self.xml_form_dict['len']          = self.all_forms_window.xml_form['len']
        self.xml_form_dict['min_note_len'] = self.all_forms_window.xml_form['min_note_len']
        self.xml_form_dict['max_note_len'] = self.all_forms_window.xml_form['max_note_len']
        # scale
        self.xml_form_dict['scale']        = self.all_forms_window.xml_form['scale']

        self.xml_form_dict['pulse'] = self.all_forms_window.xml_form['pulse']
        self.xml_form_dict['melody_probabilities']['down'] = (
                self.all_forms_window.xml_form['melody_probabilities']['down'])
        self.xml_form_dict['melody_probabilities']['same'] = (
                self.all_forms_window.xml_form['melody_probabilities']['same'])
        self.xml_form_dict['melody_probabilities']['up'] = (
                self.all_forms_window.xml_form['melody_probabilities']['up'])
        self.xml_form_dict['pitch_form']['mean']['period'] = (
                self.all_forms_window.xml_form['pitch_form']['mean']['period'])
        self.xml_form_dict['pitch_form']['mean']['phase'] = (
                self.all_forms_window.xml_form['pitch_form']['mean']['phase'])
        self.xml_form_dict['pitch_form']['mean']['amplitude'] = (
                self.all_forms_window.xml_form['pitch_form']['mean']['amplitude'])
        self.xml_form_dict['pitch_form']['mean']['offset'] = (
                self.all_forms_window.xml_form['pitch_form']['mean']['offset'])

        self.xml_form_dict['rhythm_form']['mean']['period'] = (
                self.all_forms_window.xml_form['rhythm_form']['mean']['period'])
        self.xml_form_dict['rhythm_form']['mean']['phase'] = (
                self.all_forms_window.xml_form['rhythm_form']['mean']['phase'])
        self.xml_form_dict['rhythm_form']['mean']['amplitude'] = (
                self.all_forms_window.xml_form['rhythm_form']['mean']['amplitude'])
        self.xml_form_dict['rhythm_form']['mean']['offset'] = (
                self.all_forms_window.xml_form['rhythm_form']['mean']['offset'])

        self.xml_form_dict['dynamic_form']['mean']['period'] = (
                self.all_forms_window.xml_form['dynamic_form']['mean']['period'])
        self.xml_form_dict['dynamic_form']['mean']['phase'] = (
                self.all_forms_window.xml_form['dynamic_form']['mean']['phase'])
        self.xml_form_dict['dynamic_form']['mean']['amplitude'] = (
                self.all_forms_window.xml_form['dynamic_form']['mean']['amplitude'])
        self.xml_form_dict['dynamic_form']['mean']['offset'] = (
                self.all_forms_window.xml_form['dynamic_form']['mean']['offset'])

        self.xml_form_dict['texture_form']['period'] = (
                self.all_forms_window.xml_form['texture_form']['period'])
        self.xml_form_dict['texture_form']['phase'] = (
                self.all_forms_window.xml_form['texture_form']['phase'])
        self.xml_form_dict['texture_form']['amplitude'] = (
                self.all_forms_window.xml_form['texture_form']['amplitude'])
        self.xml_form_dict['texture_form']['offset'] = (
                self.all_forms_window.xml_form['texture_form']['offset'])

        self.xml_form_dict['pitch_form']['range']['period'] = (
                self.all_forms_window.xml_form['pitch_form']['range']['period'])
        self.xml_form_dict['pitch_form']['range']['phase'] = (
                self.all_forms_window.xml_form['pitch_form']['range']['phase'])
        self.xml_form_dict['pitch_form']['range']['amplitude'] = (
                self.all_forms_window.xml_form['pitch_form']['range']['amplitude'])
        self.xml_form_dict['pitch_form']['range']['offset'] = (
                self.all_forms_window.xml_form['pitch_form']['range']['offset'])

        self.xml_form_dict['rhythm_form']['range']['period'] = (
                self.all_forms_window.xml_form['rhythm_form']['range']['period'])
        self.xml_form_dict['rhythm_form']['range']['phase'] = (
                self.all_forms_window.xml_form['rhythm_form']['range']['phase'])
        self.xml_form_dict['rhythm_form']['range']['amplitude'] = (
                self.all_forms_window.xml_form['rhythm_form']['range']['amplitude'])
        self.xml_form_dict['rhythm_form']['range']['offset'] = (
                self.all_forms_window.xml_form['rhythm_form']['range']['offset'])

        self.xml_form_dict['dynamic_form']['range']['period'] = (
                self.all_forms_window.xml_form['dynamic_form']['range']['period'])
        self.xml_form_dict['dynamic_form']['range']['phase'] = (
                self.all_forms_window.xml_form['dynamic_form']['range']['phase'])
        self.xml_form_dict['dynamic_form']['range']['amplitude'] = (
                self.all_forms_window.xml_form['dynamic_form']['range']['amplitude'])
        self.xml_form_dict['dynamic_form']['range']['offset'] = (
                self.all_forms_window.xml_form['dynamic_form']['range']['offset'])

        form_document_type = dom_impl.createDocumentType('boost_serialization', None, None)
        form_document = dom_impl.createDocument(None, 'boost_serialization',
              form_document_type)
        top = form_document.documentElement
        top.setAttribute('signature', 'serialization::archive')
        top.setAttribute('version', '18')
        xml_form_element = form_document.createElement('xml_form')
        xml_form_element.setAttribute('class_id', str(class_id))
        class_id = class_id + 1
        xml_form_element.setAttribute('tracking_level', '0')
        xml_form_element.setAttribute('version', '4')
        top.appendChild(xml_form_element)

        self.add_text_element(form_document, xml_form_element, 'name')
        self.add_text_element(form_document, xml_form_element, 'copyright')
        self.add_text_element(form_document, xml_form_element, 'len')
        self.add_text_element(form_document, xml_form_element, 'min_note_len')
        self.add_text_element(form_document, xml_form_element, 'max_note_len')

        scale_element = form_document.createElement('scale_')
        scale_element.setAttribute('class_id', str(class_id))
        class_id = class_id + 1
        scale_element.setAttribute('tracking_level', '0')
        scale_element.setAttribute('version', '0')

        temp_scale = self.all_forms_window.scale_frame.get_string_scale()
        self.add_text_element(form_document, scale_element, 'count', 'count',
            str(len(temp_scale)))

        self.add_text_element(form_document, scale_element, 'item_version',
            'item_version', '0')

        self.xml_form_dict['scale'] = []
        self.xml_form_dict['scale'] = self.all_forms_window.scale_frame.get_string_scale()
        for key_name in self.xml_form_dict['scale']:
            self.add_text_element(form_document, scale_element, 'item',
            'item', key_name)

        xml_form_element.appendChild(scale_element)

        music_time_element = form_document.createElement('music_time_')
        music_time_element.setAttribute('class_id', str(class_id))
        class_id = class_id + 1
        music_time_element.setAttribute('tracking_level', '0')
        music_time_element.setAttribute('version', '0')
        self.add_text_element(form_document, music_time_element, 'ticks_per_quarter', 'ticks_per_quarter_', str(self.xml_form_dict['music_time']['ticks_per_quarter']))
        beat_element = form_document.createElement('beat_')
        beat_element.setAttribute('class_id', str(class_id))
        class_id = class_id + 1
        beat_element.setAttribute('tracking_level', '0')
        beat_element.setAttribute('version', '0')
        self.add_text_element(form_document, beat_element, 'numerator', 'numerator_',
                str(self.xml_form_dict['music_time']['beat']['numerator']))
        self.add_text_element(form_document, beat_element, 'denominator', 'denominator_',
                str(self.xml_form_dict['music_time']['beat']['denominator']))
        music_time_element.appendChild(beat_element)

        meter_element = form_document.createElement('meter_')
        self.add_text_element(form_document, meter_element, 'numerator', 'numerator_',
                str(self.xml_form_dict['music_time']['meter']['numerator']))
        self.add_text_element(form_document, meter_element, 'denominator', 'denominator_',
                str(self.xml_form_dict['music_time']['meter']['denominator']))
        music_time_element.appendChild(meter_element)
        self.add_text_element(form_document, music_time_element, 'beat_tempo', 'beat_tempo_', str(self.xml_form_dict['music_time']['beat_tempo']))
        xml_form_element.appendChild(music_time_element)

        self.add_text_element(form_document, xml_form_element, 'pulse')

        melody_probabilities_element = form_document.createElement('melody_probabilities_')
        melody_probabilities_element.setAttribute('class_id', str(class_id))
        class_id = class_id + 1
        melody_probabilities_element.setAttribute('tracking_level', '0')
        melody_probabilities_element.setAttribute('version', '0')

        self.add_text_element(form_document, melody_probabilities_element,
            'down', 'down_',
            str(self.xml_form_dict['melody_probabilities']['down']))
        self.add_text_element(form_document, melody_probabilities_element,
            'same', 'same_',
            str(self.xml_form_dict['melody_probabilities']['same']))
        self.add_text_element(form_document, melody_probabilities_element, 'up',
            'up_', str(self.xml_form_dict['melody_probabilities']['up']))
        xml_form_element.appendChild(melody_probabilities_element)

        self.add_form_element(form_document, xml_form_element, 'pitch_form', True, class_id)
        class_id = class_id + 2
        self.add_form_element(form_document, xml_form_element, 'rhythm_form')
        self.add_form_element(form_document, xml_form_element, 'dynamic_form')
        self.add_sine_element(form_document, xml_form_element, 'texture_form')

        voices_element = form_document.createElement('voices_')
        voices_element.setAttribute('class_id', str(class_id))
        class_id = class_id + 1
        voices_element.setAttribute('tracking_level', '0')
        voices_element.setAttribute('version', '0')
        self.add_text_element(form_document, voices_element, 'count', 'count',
            str(len(self.xml_form_dict['voices'])))
        self.add_text_element(form_document, voices_element, 'item_version',
            'item_version', '0')

        write_version = True
        for vox in self.xml_form_dict['voices']:
            self.add_voice_element(form_document, voices_element, vox, write_version, class_id)
            write_version = False
        class_id = class_id + 2
        xml_form_element.appendChild(voices_element)

        arrangement_definition_element = form_document.createElement('arrangement_definition_')
        arrangement_definition_element.setAttribute('class_id', str(class_id))
        class_id = class_id + 1
        arrangement_definition_element.setAttribute('tracking_level', '0')
        arrangement_definition_element.setAttribute('version', '0')

        self.add_text_element(form_document, arrangement_definition_element,
            'algorithm', 'algorithm_',
            str(self.xml_form_dict['arrangement_definition']['algorithm']))
        self.add_text_element(form_document, arrangement_definition_element,
            'period', 'period_',
            str(self.xml_form_dict['arrangement_definition']['period']))

        xml_form_element.appendChild(arrangement_definition_element)

        form_document.writexml(open(afilename, 'w'), indent='', addindent='\t',
            newl='\n', encoding='UTF-8', standalone=True)
        form_document.unlink()

    def add_voice_element(self, doc, parent, voice_dict, write_version = False, class_id = 0):
        """Add a voice element to the internal form structure."""
        item_element = doc.createElement('item')
        if write_version:
            item_element.setAttribute('class_id', str(class_id))
            class_id = class_id + 1
            item_element.setAttribute('tracking_level', '0')
            item_element.setAttribute('version', '0')

        self.add_text_element(doc, item_element, 'low_pitch', 'low_pitch_',
                str(voice_dict['low_pitch']))
        self.add_text_element(doc, item_element, 'high_pitch', 'high_pitch_',
                str(voice_dict['high_pitch']))
        self.add_text_element(doc, item_element, 'channel', 'channel_',
                str(int(voice_dict['channel'])))
        self.add_text_element(doc, item_element, 'walking', 'walking_',
                str(voice_dict['walking']))
        self.add_text_element(doc, item_element, 'program', 'program_',
                str(voice_dict['program']))
        self.add_text_element(doc, item_element, 'pan', 'pan_',
                str(int(voice_dict['pan'])))
        follower_element = doc.createElement('follower_')
        if write_version:
            follower_element.setAttribute('class_id', str(class_id))
            class_id = class_id + 1
            follower_element.setAttribute('tracking_level', '0')
            follower_element.setAttribute('version', '3')

        self.add_text_element(doc, follower_element, 'follow', 'follow_',
                str(int(voice_dict['follower']['follow'])))
        self.add_text_element(doc, follower_element, 'leader', 'leader_',
                str(int(voice_dict['follower']['leader'])))
        self.add_text_element(doc, follower_element, 'interval_type',
            'interval_type_', str(int(voice_dict['follower']['interval_type'])))
        self.add_text_element(doc, follower_element, 'interval', 'interval_',
                str(int(voice_dict['follower']['interval'])))

        delay_element = doc.createElement('delay_')
        self.add_text_element(doc, delay_element, 'numerator', 'numerator_',
                str(voice_dict['follower']['delay']['numerator']))
        self.add_text_element(doc, delay_element, 'denominator', 'denominator_',
                str(voice_dict['follower']['delay']['denominator']))
        follower_element.appendChild(delay_element)

        duration_factor_element = doc.createElement('duration_factor_')
        self.add_text_element(doc, duration_factor_element, 'numerator',
            'numerator_',
            str(voice_dict['follower']['duration_factor']['numerator']))
        self.add_text_element(doc, duration_factor_element, 'denominator',
            'denominator_',
            str(voice_dict['follower']['duration_factor']['denominator']))
        follower_element.appendChild(duration_factor_element)
        self.add_text_element(doc, follower_element, 'inversion', 'inversion_',
                str(int(voice_dict['follower']['inversion'])))
        self.add_text_element(doc, follower_element, 'retrograde', 'retrograde_',
                str(int(voice_dict['follower']['retrograde'])))
        item_element.appendChild(follower_element)
        parent.appendChild(item_element)

    def add_form_element(self, doc, parent, form_name, add_attributes = False, class_id = 0):
        """Add a form element to the internal form structure."""
        form_element = doc.createElement(form_name + '_')
        if add_attributes is True:
            form_element.setAttribute('class_id', str(class_id))
            class_id = class_id + 1
            form_element.setAttribute('tracking_level', '0')
            form_element.setAttribute('version', '0')

        mean_sine_element = doc.createElement('mean_sine_')
        if add_attributes is True:
            mean_sine_element.setAttribute('class_id', str(class_id))
            class_id = class_id + 1
            mean_sine_element.setAttribute('tracking_level', '0')
            mean_sine_element.setAttribute('version', '0')
        self.add_text_element(doc, mean_sine_element , 'period', 'period_',
                str(self.xml_form_dict[form_name]['mean']['period']))
        self.add_text_element(doc, mean_sine_element , 'phase', 'phase_',
                str(self.xml_form_dict[form_name]['mean']['phase']))
        self.add_text_element(doc, mean_sine_element , 'amplitude',
            'amplitude_',
            str(self.xml_form_dict[form_name]['mean']['amplitude']))
        self.add_text_element(doc, mean_sine_element , 'offset', 'offset_',
                str(self.xml_form_dict[form_name]['mean']['offset']))
        form_element.appendChild(mean_sine_element)
        range_sine_element = doc.createElement('range_sine_')
        self.add_text_element(doc, range_sine_element , 'period', 'period_',
                str(self.xml_form_dict[form_name]['range']['period']))
        self.add_text_element(doc, range_sine_element , 'phase', 'phase_',
                str(self.xml_form_dict[form_name]['range']['phase']))
        self.add_text_element(doc, range_sine_element , 'amplitude',
            'amplitude_',
            str(self.xml_form_dict[form_name]['range']['amplitude']))
        self.add_text_element(doc, range_sine_element , 'offset', 'offset_',
                str(self.xml_form_dict[form_name]['range']['offset']))
        form_element.appendChild(range_sine_element)
        parent.appendChild(form_element)

    def add_sine_element(self, doc, parent, sine_name):
        """Add a sine elemnet (there are seven) to the internal form structure."""
        sine_element = doc.createElement(sine_name + '_')
        self.add_text_element(doc, sine_element , 'period', 'period_',
                str(self.xml_form_dict[sine_name]['period']))
        self.add_text_element(doc, sine_element , 'phase', 'phase_',
                str(self.xml_form_dict[sine_name]['phase']))
        self.add_text_element(doc, sine_element , 'amplitude', 'amplitude_',
                str(self.xml_form_dict[sine_name]['amplitude']))
        self.add_text_element(doc, sine_element , 'offset', 'offset_',
                str(self.xml_form_dict[sine_name]['offset']))
        parent.appendChild(sine_element)

    def add_text_element(self, doc, parent, name, xmltag = None, value = None):
        """Add a text element to the DOM."""
        if xmltag is None:
            xmltag = name + '_'
        if value is None:
            value = str(self.xml_form_dict[name])

        any_element = doc.createElement(xmltag)
        parent.appendChild(any_element)
        any_text_node = doc.createTextNode(value)
        any_element.appendChild(any_text_node)
        parent.appendChild(any_element)

    def redraw_callback(self):
        """Redraw the form plot."""
        self.xml_form_dict['name'] = self.all_forms_window.xml_form['name']
        self.xml_form_dict['copyright'] = self.all_forms_window.xml_form['copyright']
        self.xml_form_dict['len'] = self.all_forms_window.xml_form['len']
        self.xml_form_dict['min_note_len'] = self.all_forms_window.xml_form['min_note_len']
        self.xml_form_dict['max_note_len'] = self.all_forms_window.xml_form['max_note_len']
        self.xml_form_dict['pulse'] = self.all_forms_window.xml_form['pulse']
        self.xml_form_dict['melody_probabilities']['down'] = (
                self.all_forms_window.xml_form['melody_probabilities']['down'])
        self.xml_form_dict['melody_probabilities']['same'] = (
                self.all_forms_window.xml_form['melody_probabilities']['same'])
        self.xml_form_dict['melody_probabilities']['up'] = (
                self.all_forms_window.xml_form['melody_probabilities']['up'])

        self.xml_form_dict['pitch_form']['mean']['period'] = (
                self.all_forms_window.xml_form['pitch_form']['mean']['period'])
        self.xml_form_dict['pitch_form']['mean']['phase'] = (
                self.all_forms_window.xml_form['pitch_form']['mean']['phase'])
        self.xml_form_dict['pitch_form']['mean']['amplitude'] = (
                self.all_forms_window.xml_form['pitch_form']['mean']['amplitude'])
        self.xml_form_dict['pitch_form']['mean']['offset'] = (
                self.all_forms_window.xml_form['pitch_form']['mean']['offset'])

        self.xml_form_dict['rhythm_form']['mean']['period'] = (
                self.all_forms_window.xml_form['rhythm_form']['mean']['period'])
        self.xml_form_dict['rhythm_form']['mean']['phase'] = (
                self.all_forms_window.xml_form['rhythm_form']['mean']['phase'])
        self.xml_form_dict['rhythm_form']['mean']['amplitude'] = (
                self.all_forms_window.xml_form['rhythm_form']['mean']['amplitude'])
        self.xml_form_dict['rhythm_form']['mean']['offset'] = (
                self.all_forms_window.xml_form['rhythm_form']['mean']['offset'])

        self.xml_form_dict['dynamic_form']['mean']['period'] = (
                self.all_forms_window.xml_form['dynamic_form']['mean']['period'])
        self.xml_form_dict['dynamic_form']['mean']['phase'] = (
                self.all_forms_window.xml_form['dynamic_form']['mean']['phase'])
        self.xml_form_dict['dynamic_form']['mean']['amplitude'] = (
                self.all_forms_window.xml_form['dynamic_form']['mean']['amplitude'])
        self.xml_form_dict['dynamic_form']['mean']['offset'] = (
                self.all_forms_window.xml_form['dynamic_form']['mean']['offset'])

        self.xml_form_dict['texture_form']['period'] = (
                self.all_forms_window.xml_form['texture_form']['period'])
        self.xml_form_dict['texture_form']['phase'] = (
                self.all_forms_window.xml_form['texture_form']['phase'])
        self.xml_form_dict['texture_form']['amplitude'] = (
                self.all_forms_window.xml_form['texture_form']['amplitude'])
        self.xml_form_dict['texture_form']['offset'] = (
                self.all_forms_window.xml_form['texture_form']['offset'])

        self.xml_form_dict['pitch_form']['range']['period'] = (
                self.all_forms_window.xml_form['pitch_form']['range']['period'])
        self.xml_form_dict['pitch_form']['range']['phase'] = (
                self.all_forms_window.xml_form['pitch_form']['range']['phase'])
        self.xml_form_dict['pitch_form']['range']['amplitude'] = (
                self.all_forms_window.xml_form['pitch_form']['range']['amplitude'])
        self.xml_form_dict['pitch_form']['range']['offset'] = (
                self.all_forms_window.xml_form['pitch_form']['range']['offset'])

        self.xml_form_dict['rhythm_form']['range']['period'] = (
                self.all_forms_window.xml_form['rhythm_form']['range']['period'])
        self.xml_form_dict['rhythm_form']['range']['phase'] = (
                self.all_forms_window.xml_form['rhythm_form']['range']['phase'])
        self.xml_form_dict['rhythm_form']['range']['amplitude'] = (
                self.all_forms_window.xml_form['rhythm_form']['range']['amplitude'])
        self.xml_form_dict['rhythm_form']['range']['offset'] = (
                self.all_forms_window.xml_form['rhythm_form']['range']['offset'])

        self.xml_form_dict['dynamic_form']['range']['period'] = (
                self.all_forms_window.xml_form['dynamic_form']['range']['period'])
        self.xml_form_dict['dynamic_form']['range']['phase'] = (
                self.all_forms_window.xml_form['dynamic_form']['range']['phase'])
        self.xml_form_dict['dynamic_form']['range']['amplitude'] = (self.all_forms_window
            .xml_form['dynamic_form']['range']['amplitude'])
        self.xml_form_dict['dynamic_form']['range']['offset'] = (
                self.all_forms_window.xml_form['dynamic_form']['range']['offset'])

        self.draw_form()

    def about_callback(self):
        """Present the about window."""
        about_top = tkinter.Tk()
        about_window = tkinter.Text(about_top)
        about_window.grid(sticky='we', row=0, column=0)
        about_top.title('About')
        about_window.insert('1.0',
            'TextMIDITools Version 1.1.0\nCopyright © 2025 Thomas E. Janzen\n'
            'License GPLv3+: GNU GPL version 3 \nor later <https://gnu.org/licenses/gpl.html>\n'
            'TextMidiFormEdit.py musical form editor\nUse with textmidicgm, part of '
            'TextMIDITools\nat github.com/tomejanzen/TextMIDITools')
        about_window['state'] = 'disabled'
        about_window.focus()

def TextMidiFormEdit(filename):
    """Bring up the whole interface."""
    xmlform_window = XmlForm(filename)
    xmlform_window.frame.grid(sticky='we', row=0, column=0)
    xmlform_window.frame.rowconfigure(index=0, weight=1)
    xmlform_window.frame.columnconfigure(index=0, weight=1)

    xmlform_window.mainloop()

if __name__ == '__main__':
    if len(sys.argv) > 1:
        filename_arg = sys.argv[1]
    else:
        filename_arg = ''
    TextMidiFormEdit(filename_arg)
