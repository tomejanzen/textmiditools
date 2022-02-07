#!/usr/bin/env python3
# TextMIDITools Version 1.0.15
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
from tkinter import ttk
from tkinter.ttk import *
import tkinter.constants, tkinter.filedialog

class KeyboardButtons(tkinter.Frame):
    def __init__(self, parent, keyboard):
        super().__init__(parent)
        self.grid(sticky="we", row=0, column=0)
        self.rowconfigure(index=0, weight=1)
        self.columnconfigure(index=2, weight=1)
        self.draw_widgets()
        self.keyboard = keyboard

    def draw_widgets(self):
        self.clear_button = tkinter.ttk.Button(self, text='Clear',
            command=self.clear_keyboard_callback)
        self.clear_button.grid(sticky="we", row=0, column=0)
        self.all_button = tkinter.ttk.Button(self, text='All', command=self.all_keyboard_callback)
        self.all_button.grid(sticky="we", row=0, column=1)
        self.complement_button = tkinter.ttk.Button(self, text='Complement',
            command=self.complement_keyboard_callback)
        self.complement_button.grid(sticky="we", row=0, column=2)
        self.octave_repeat_button = tkinter.ttk.Button(self, text='Octave Repeat',
            command=self.octave_repeat_keyboard_callback)
        self.octave_repeat_button.grid(sticky="we", row=0, column=3)

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

class KeyboardFrame(tkinter.Frame):
    xml_form_dict = {}
    full_octaves = 10
    remainder_keys = 8
    divisor = 4
    left = 0
    top = 0
    middle = 403 / divisor
    bottom = 613 / divisor
    right  = 700 / divisor
    white_key_width = 100 / divisor
    black_key_width = 60 / divisor
    c_sharp_left = 60 / divisor
    c_sharp_right = c_sharp_left + black_key_width
    d_left = white_key_width
    d_sharp_left = 180 / divisor
    d_sharp_right = d_sharp_left + black_key_width
    e_left = 2 * white_key_width
    f_left = 3 * white_key_width
    f_sharp_left  = 356 / divisor
    f_sharp_right = f_sharp_left + black_key_width
    g_left = 4 * white_key_width
    g_sharp_left = 470 / divisor
    g_sharp_right = g_sharp_left + black_key_width
    a_left = 5 * white_key_width
    a_sharp_left = 583 / divisor
    a_sharp_right = a_sharp_left + black_key_width
    b_left = 6 * white_key_width
    keys = [
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
                            [right, bottom], [right, top]]]
        ]
    full_keyboard = []
    midi_key_selects = []

    def __init__(self, parent):
        super().__init__(parent)
        self.canvas = None
        self.draw_keyboard()

        self.grid(sticky="we", row=0, column=2)
        self.rowconfigure(index=0, weight=1)
        self.columnconfigure(index=2, weight=1)
        for k in range(0, 128):
            self.midi_key_selects.append(False)

    def on_click(self, event):
        self.click_x = event.x
        self.click_y = event.y
        the_id = self.canvas.find('closest',  event.x, event.y)
        midi_key = the_id[0] - 1
        self.midi_key_selects[midi_key] = not self.midi_key_selects[midi_key]
        self.set_keys_from_selects()

    def on_motion(self, event):
        a_canvas = event.widget

    def draw_keyboard(self):
        win_height = self.bottom
        win_width = self.right * (self.full_octaves + 1) - self.white_key_width * 2
        if (self.canvas != None):
            self.canvas.delete()

        self.canvas = Canvas(self, bg='white', scrollregion=(0, 0, win_width, win_height), height=win_height, width=win_width)
        self.canvas.grid(row=0, column=0, sticky=(N,S,E,W))
        self.canvas.bind('<B1-Motion>', self.on_motion)
        self.canvas.bind('<Button-1>', self.on_click)
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)

        keys_copy = self.keys
        for octave in range(0, self.full_octaves):
            for k in range(0, len(keys_copy)):
                newkey = []
                newkey.append(keys_copy[k][0] + str(octave - 1))
                newkey.append(keys_copy[k][1])
                newpoints = []
                for point in keys_copy[k][2]:
                    newpoint = []
                    for coord in point:
                        newpoint.append(coord)
                    newpoints.append(newpoint)
                newkey.append(newpoints)
                self.full_keyboard.append(newkey)
                for point in range(0, len(keys_copy[k][2])):
                    keys_copy[k][2][point][0] = keys_copy[k][2][point][0] + self.right

        # put in the C to G in the top octave
        octave = 10
        for k in range(0, 8):
            newkey = []
            newkey.append(keys_copy[k][0] + str(octave - 1))
            newkey.append(keys_copy[k][1])
            newpoints = []
            for point in keys_copy[k][2]:
                newpoint = []
                for coord in point:
                    newpoint.append(coord)
                newpoints.append(newpoint)
            newkey.append(newpoints)
            self.full_keyboard.append(newkey)
            for point in range(0, len(keys_copy[k][2])):
                keys_copy[k][2][point][0] = keys_copy[k][2][point][0] + self.right
        key_ids = []
        for k in range(0, len(self.full_keyboard)):
            the_width = "1.0"
            if (60 == k):
                the_width = "4.0"
            key_ids.append(self.canvas.create_polygon(self.full_keyboard[k][2], fill=self.full_keyboard[k][1],
                activefill='yellow', disabledfill='gray', width=the_width, outline='black', tags=(self.full_keyboard[k][0])))

    def set_keys_from_selects(self):
        for k in range(0, len(self.midi_key_selects)):
            if (self.midi_key_selects[k]):
                if (self.full_keyboard[k][1] == 'white'):
                    self.canvas.itemconfigure(tagOrId=k + 1, fill='red', stipple='gray50')
                else:
                    self.canvas.itemconfigure(tagOrId=k + 1, fill='red', stipple='gray50')
            else:
                self.canvas.itemconfigure(tagOrId=k + 1, fill=self.full_keyboard[k][1], stipple='')

    def set_selects(self, selects):
        if (len(selects) == len(self.midi_key_selects)):
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
        self.frame = ttk.Frame(self, padding="1 1 1 1")
        self.frame.grid()
        self.create_widgets()
        self.title('Keyboard')

    def create_widgets(self):
        self.keyboard_frame = KeyboardFrame(self.frame)
        self.keyboard_frame.grid(sticky="we", row=0, column=0)
        self.keyboard_frame.rowconfigure(index=0, weight=1)
        self.keyboard_frame.columnconfigure(index=0, weight=1)

        self.keyboard_buttons_frame = KeyboardButtons(self.frame, self.keyboard_frame)
        self.keyboard_buttons_frame.grid(sticky="we", row=1, column=0)
        self.keyboard_buttons_frame.rowconfigure(index=0, weight=1)
        self.keyboard_buttons_frame.columnconfigure(index=0, weight=1)

if __name__ == '__main__':
    root = tkinter.Tk()
    keyboard_window = KeyboardWindow()

    root.title('keyboard')

    keyboard_window.mainloop()
