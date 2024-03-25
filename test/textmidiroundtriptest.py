#!/usr/bin/env python3
# textmidiform.py 1.0
# Copyright © 2021 Thomas E. Janzen
# License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
# This is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#

import os
import re
import time
import math
import sys
import tempfile
from pathlib import Path

midi_differ_count = 0
midi_identical_count = 0
txt_differ_count = 0
txt_identical_count = 0
verbose = False
lazy = False
simplecontinuedfraction = False

def testmidi(filename, an_option = "standard"):
    global midi_differ_count
    global midi_identical_count
    global txt_differ_count
    global txt_identical_count
    global verbose
    global lazy
    global simplecontinuedfraction

    miditext_base_cmd = "miditext --verbose "
    textmidi_base_cmd = "textmidi --runningstatus " + an_option
    if lazy:
        miditext_base_cmd += " --lazy"
    if simplecontinuedfraction:
        miditext_base_cmd += " --rhythmexpression simplecontinuedfraction "
    temp_directory_name = tempfile.TemporaryDirectory()
    filenamepath=Path(filename)
    textmidifilepath = Path(temp_directory_name.name)
    textmidifilepath = textmidifilepath.joinpath(filenamepath.with_suffix('.txt').name)
    miditext_cmd = miditext_base_cmd + " --midi " + filenamepath.as_posix() + " --textmidi " + textmidifilepath.as_posix()
    miditext_log = os.popen(miditext_cmd)
    if verbose:
        print(miditext_cmd, "\n")
    miditext_lines = miditext_log.readlines()
    miditext_log.close()
    if verbose:
        for log in miditext_lines:
            print(log, end='')
    tempmidifilepath = Path(temp_directory_name.name)
    tempmidifilepath = tempmidifilepath.joinpath(filenamepath.name)

    retextmidifilepath = Path(temp_directory_name.name)
    retextmidifilepath = retextmidifilepath.joinpath('_B' + filenamepath.with_suffix('.txt').name)

    textmidi_cmd = textmidi_base_cmd + " --textmidi " + textmidifilepath.as_posix() + " --midi " + tempmidifilepath.as_posix()
    if verbose:
        print(textmidi_cmd, "\n")
    textmidi_log = os.popen(textmidi_cmd)
    textmidi_lines = textmidi_log.readlines()
    textmidi_log.close()
    if verbose:
        for log in textmidi_lines:
            print(log, end='')

    remiditext_cmd = miditext_base_cmd + " --midi " + tempmidifilepath.as_posix() + " --textmidi " + retextmidifilepath.as_posix()
    if verbose:
        print(remiditext_cmd, "\n")
    remiditext_log = os.popen(remiditext_cmd)
    remiditext_lines = remiditext_log.readlines()
    remiditext_log.close()
    if verbose:
        for log in remiditext_lines:
            print(log, end='')

    diff_midi_cmd = "/usr/bin/env diff -s " + filenamepath.as_posix() + ' ' + tempmidifilepath.as_posix()
    diff_midi_log = os.popen(diff_midi_cmd, 'r')
    diff_midi_lines = diff_midi_log.readlines()
    diff_midi_log.close()
    diffpat = re.compile("^.*(are identical|differ)$")
    for line in diff_midi_lines:
        line = line.rstrip()
        search_rtn = re.match(diffpat, line)
        if (search_rtn):
            if search_rtn.group(1) == "differ":
                midi_differ_count = midi_differ_count + 1
                print(line, end='')
            else:
                if search_rtn.group(1) == "are identical":
                    midi_identical_count = midi_identical_count + 1
                if verbose:
                    print(log, end='')

    diff_txt_cmd = "/usr/bin/env diff -q -bBw -s " + textmidifilepath.as_posix() + ' ' + retextmidifilepath.as_posix()
    diff_txt_log = os.popen(diff_txt_cmd, 'r')
    diff_txt_lines = diff_txt_log.readlines()
    diff_txt_log.close()
    for line in diff_txt_lines:
        line = line.rstrip()
        search_rtn = re.match(diffpat, line)
        if (search_rtn):
            if search_rtn.group(1) == "differ":
                txt_differ_count = txt_differ_count + 1
                print(line)
            else:
                if search_rtn.group(1) == "are identical":
                    txt_identical_count = txt_identical_count + 1
                if verbose:
                    print(line, end='')

    textmidifilepath.unlink()
    tempmidifilepath.unlink()
    retextmidifilepath.unlink()

if __name__ == '__main__':
    midi_count = 0
    if len(sys.argv) > 1:
        list_file_name = sys.argv[1]
        list_handle = open(list_file_name, 'r')
        file_list = list_handle.readlines()
        list_handle.close()
        if len(sys.argv) > 2:
            for opt in sys.argv:
                if opt == "verbose":
                    verbose = True
                else:
                    if opt == "lazy":
                        lazy = True
                    else:
                        if opt == "simplecontinuedfraction":
                            simplecontinuedfraction = True
    starttime = time.asctime()
    for f in file_list:
        f = f.rstrip()
        filename_and_running_status = f.split(',')
        filename = filename_and_running_status[0]

        running_status_policy = "standard"
        midi_count += 1
        if len(filename_and_running_status) == 2:
            running_status_policy = filename_and_running_status[1]
        testmidi(filename, running_status_policy)

    print ("Total: " , midi_count , " ; Identical text files : " , txt_identical_count , " Identical MIDI files: " , midi_identical_count , "\n")
    print ("Percentage identical MIDI files: " , ((midi_identical_count / midi_count)  * 100.0) , " ; Percentage identical text files: " , ((txt_identical_count / midi_count) * 100.0) , "\n")
    print("Start time: ", starttime, " ; Finished Time: ", time.asctime(), '\n')
