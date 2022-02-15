#!/usr/bin/env python3
# TextMIDITools Version 1.0.19
# Copyright © 2022 Thomas E. Janzen
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
from tkinter import ttk
from tkinter.ttk import *
import tkinter.constants, tkinter.filedialog

class KeyboardButtons(tkinter.Frame):
    def __init__(self, parent, keyboard):
        super().__init__(parent)
        self.frame = ttk.Frame(self, padding='1 1 1 1')
        self.frame.grid(sticky=NSEW, row=1, column=0)
        self.draw_widgets()
        self.keyboard = keyboard

    def draw_widgets(self):
        self.clear_button = tkinter.ttk.Button(self.frame, text='Clear',
            command=self.clear_keyboard_callback)
        self.clear_button.grid(sticky=NSEW, row=1, column=0)

        self.all_button = tkinter.ttk.Button(self.frame, text='All', command=self.all_keyboard_callback)
        self.all_button.grid(sticky=NSEW, row=1, column=1)

        self.complement_button = tkinter.ttk.Button(self.frame, text='Complement',
            command=self.complement_keyboard_callback)
        self.complement_button.grid(sticky=NSEW, row=1, column=2)

        self.octave_repeat_button = tkinter.ttk.Button(self.frame, text='Octave Repeat',
            command=self.octave_repeat_keyboard_callback)
        self.octave_repeat_button.grid(sticky=NSEW, row=1, column=3)

    def clear_keyboard_callback(self):
        self.keyboard.draw_keyboard()
        for k in range(0, len(self.keyboard.midi_key_selects)):
            self.keyboard.midi_key_selects[k] = False
        self.keyboard.set_keys_from_selects()

    def all_keyboard_callback(self):
        for k in range(0, len(self.keyboard.midi_key_selects)):
            self.keyboard.midi_key_selects[k] = True
        self.keyboard.set_keys_from_selects()

    def complement_keyboard_callback(self):
        for k in range(0, len(self.keyboard.midi_key_selects)):
            self.keyboard.midi_key_selects[k] = not self.keyboard.midi_key_selects[k]
        self.keyboard.set_keys_from_selects()

    def octave_repeat_keyboard_callback(self):
        for k in range(0, len(self.keyboard.midi_key_selects)):
            if (self.keyboard.midi_key_selects[k]):
                for s in range(k % 12, 128, 12):
                    self.keyboard.midi_key_selects[s] = True
        self.keyboard.set_keys_from_selects()

class OneOctave():
    full_octaves = 10
    remainder_keys = 8
    left = 0
    top = 0
    white_key_width = 100
    middle = 403
    bottom = 613
    b_right = white_key_width * 7
    black_key_width = 60
    c_sharp_left = 60
    c_sharp_right = c_sharp_left + black_key_width
    d_left = white_key_width
    d_sharp_left = 180
    d_sharp_right = d_sharp_left + black_key_width
    e_left = 2 * white_key_width
    f_left = 3 * white_key_width
    f_sharp_left  = 356
    f_sharp_right = f_sharp_left + black_key_width
    g_left = 4 * white_key_width
    g_sharp_left = 470
    g_sharp_right = g_sharp_left + black_key_width
    a_left = 5 * white_key_width
    a_sharp_left = 583
    a_sharp_right = a_sharp_left + black_key_width
    b_left = 6 * white_key_width
    ref_keys = [
            ['C', 'white', [[left, top], [left, bottom],
                            [d_left, bottom], [white_key_width, middle],
                            [c_sharp_left, middle], [c_sharp_left, top]]],
            ['C#', 'black', [[c_sharp_left, top], [c_sharp_left, middle],
                             [c_sharp_right, middle], [c_sharp_right, top]]],
            ['D', 'white', [[c_sharp_right, top],[c_sharp_right, middle],
                            [d_left, middle], [d_left, bottom],
                            [e_left, bottom], [e_left, middle],
                            [d_sharp_left, middle], [d_sharp_left, top]]],
            ['D#', 'black', [[d_sharp_left, top], [d_sharp_left, middle],
                             [d_sharp_right, middle], [d_sharp_right, top]]],
            ['E', 'white', [[d_sharp_right, top], [d_sharp_right, middle],
                            [e_left, middle], [e_left, bottom],
                            [f_left, bottom], [f_left, top]]],
            ['F', 'white', [[f_left, top], [f_left, bottom],
                            [g_left, bottom], [g_left, middle],
                            [f_sharp_left, middle], [f_sharp_left, top]]],
            ['F#', 'black', [[f_sharp_left, top], [f_sharp_left, middle],
                             [f_sharp_right, middle], [f_sharp_right, top]]],
            ['G', 'white', [[f_sharp_right, top],[f_sharp_right, middle],
                            [g_left, middle], [g_left, bottom],
                            [a_left, bottom], [a_left, middle],
                            [g_sharp_left, middle], [g_sharp_left, top]]],
            ['G#', 'black', [[g_sharp_left, top], [g_sharp_left, middle],
                             [g_sharp_right, middle], [g_sharp_right, top]]],
            ['A', 'white', [[g_sharp_right, top], [g_sharp_right, middle],
                            [a_left, middle], [a_left, bottom],
                            [b_left, bottom], [b_left, middle],
                            [a_sharp_left, middle], [a_sharp_left, top]]],
            ['A#', 'black', [[a_sharp_left, top], [a_sharp_left, middle],
                             [a_sharp_right, middle], [a_sharp_right, top]]],
            ['B', 'white', [[a_sharp_right, top], [a_sharp_right, middle],
                            [b_left, middle], [b_left, bottom],
                            [b_right, bottom], [b_right, top]]]
        ]

    def __init__(self):
        self.keys = []
        for k in range(0, len(self.ref_keys)):
            temp_key = []
            temp_key.append(self.ref_keys[k][0])
            temp_key.append(self.ref_keys[k][1])
            temp_poly = []
            for p in range(0, len(self.ref_keys[k][2])):
                temp_point = []
                for c in range(0, len(self.ref_keys[k][2][p])):
                    temp_point.append(self.ref_keys[k][2][p][c])
                temp_poly.append(temp_point)
            temp_key.append(temp_poly)
            self.keys.append(temp_key)

    def scale(self, scaler):
        for k in range(0, len(self.ref_keys)):
            for p in range(0, len(self.ref_keys[k][2])):
                for c in range(0, len(self.ref_keys[k][2][p])):
                    self.keys[k][2][p][c] = int(scaler * self.ref_keys[k][2][p][c])

    def translate_one_octave(self):
        self.translate_x(self.keys[11][2][5][0] - self.keys[0][2][0][0])

    def translate_x(self, delta_x):
        for k in range(0, len(self.keys)):
            for p in range(0, len(self.keys[k][2])):
                self.keys[k][2][p][0] = int(delta_x + self.keys[k][2][p][0])

class KeyboardFrame(tkinter.Frame):
    number_of_white_keys = 75
    full_keyboard = []
    midi_key_selects = []

    def __init__(self, parent):
        super().__init__(parent)
        self.frame = ttk.Frame(self, padding='1 1 1 1')
        for k in range(0, 128):
            self.midi_key_selects.append(False)

        self.frame.grid(sticky=NSEW, row=0, column=0)
        self.one_octave = OneOctave()
        self.one_octave.scale(0.20)
        self.canvas_width = self.one_octave.keys[0][2][3][0] * self.number_of_white_keys
        self.canvas_height = self.one_octave.keys[0][2][1][1]
        self.canvas = Canvas(self.frame, bg='white', height=self.canvas_height, width=self.canvas_width, background='#88AAFF')
        self.canvas.grid(row=0, column=0)
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.canvas.bind('<Button-1>', self.on_click)
        self.canvas.bind('<Configure>', self.configure_callback)

    def on_click(self, event):
        the_id = self.canvas.find('closest',  event.x, event.y)
        midi_key = (the_id[0] - 1) % 128
        self.midi_key_selects[midi_key] = not self.midi_key_selects[midi_key]
        self.set_keys_from_selects()

    def configure_callback(self, event=None):
        self.win_width = event.width
        self.canvas_width = event.width
        self.win_height = event.height
        self.canvas_height = event.height + 100
        self.draw_keyboard()

    def draw_keyboard(self):
        self.full_keyboard = []

        for octave in range(0, self.one_octave.full_octaves):
            for k in range(0, len(self.one_octave.keys)):
                temp_key = []
                temp_key.append(self.one_octave.keys[k][0] + str(octave - 1))
                temp_key.append(self.one_octave.keys[k][1])
                temp_poly = []
                for p in range(0, len(self.one_octave.keys[k][2])):
                    temp_point = []
                    for c in range(0, len(self.one_octave.keys[k][2][p])):
                        temp_point.append(self.one_octave.keys[k][2][p][c])
                    temp_poly.append(temp_point)
                temp_key.append(temp_poly)
                self.full_keyboard.append(temp_key)
            self.one_octave.translate_one_octave()

        # put in the C to G in the top octave
        octave = 10
        for k in range(0, 8):
            temp_key = []
            temp_key.append(self.one_octave.keys[k][0] + str(octave - 1))
            temp_key.append(self.one_octave.keys[k][1])
            temp_poly = []
            for p in range(0, len(self.one_octave.keys[k][2])):
                temp_point = []
                for c in range(0, len(self.one_octave.keys[k][2][p])):
                    temp_point.append(self.one_octave.keys[k][2][p][c])
                temp_poly.append(temp_point)
            temp_key.append(temp_poly)
            self.full_keyboard.append(temp_key)

        self.canvas.delete(all)
        key_ids = []
        for k in range(0, len(self.full_keyboard)):
            the_width = '1.0'
            if (60 == k):
                the_width = '4.0'
            key_ids.append(self.canvas.create_polygon(self.full_keyboard[k][2], fill=self.full_keyboard[k][1],
                activefill='yellow', disabledfill='gray', width=the_width, outline='black', tags=(self.full_keyboard[k][0])))
        self.set_keys_from_selects()

    def set_keys_from_selects(self):
        if (len(self.full_keyboard) > 0):
            for k in range(0, len(self.midi_key_selects)):
                if (self.midi_key_selects[k]):
                    if (self.full_keyboard[k][1] == 'white'):
                        self.canvas.itemconfigure(tagOrId=self.full_keyboard[k][0], fill='blue', stipple='gray50')
                    else:
                        self.canvas.itemconfigure(tagOrId=self.full_keyboard[k][0], fill='red', stipple='gray50')
                else:
                    self.canvas.itemconfigure(tagOrId=self.full_keyboard[k][0], fill=self.full_keyboard[k][1], stipple='')

    def set_selects(self, selects):
        if ((len(selects) > 0) and (len(selects) == len(self.midi_key_selects))):
            for k in range(0, len(self.midi_key_selects)):
                self.midi_key_selects[k] = selects[k]
            self.set_keys_from_selects()

    def get_key_ints(self):
        key_ints = []
        for flag_index in range(0, len(self.midi_key_selects)):
            if (self.midi_key_selects[flag_index]):
                key_ints.append(flag_index)
        return key_ints

class KeyboardWindow(tkinter.Toplevel):
    def __init__(self):
        super().__init__()
        self.frame = ttk.Frame(self, padding='1 1 1 1')
        self.frame.grid(sticky=NSEW, row=0, column=0)
        self.create_widgets()
        self.title('Keyboard')

    def create_widgets(self):
        self.keyboard_frame = KeyboardFrame(self.frame)
        self.keyboard_frame.grid(sticky=NSEW, row=0, column=0)
        self.keyboard_frame.rowconfigure(index=0, weight=1)
        self.keyboard_frame.columnconfigure(index=0, weight=1)

        self.keyboard_buttons_frame = KeyboardButtons(self.frame, self.keyboard_frame)
        self.keyboard_buttons_frame.grid(sticky=NSEW, row=1, column=0)
        self.keyboard_buttons_frame.rowconfigure(index=1, weight=1)
        self.keyboard_buttons_frame.columnconfigure(index=0, weight=1)

if __name__ == '__main__':
    root = tkinter.Tk()
    keyboard_window = KeyboardWindow()

    root.title('keyboard')

    keyboard_window.mainloop()
