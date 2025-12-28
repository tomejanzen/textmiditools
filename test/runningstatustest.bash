#!/usr/bin/env bash

function runningstatustest
{
    midifilename=${1}
    echo ${midifilename}
    temporary_directory=$(mktemp -d)
    bn=$(basename ${midifilename} .mid)
    echo ${bn}
    /usr/bin/env miditext --midi ${midifilename} --textmidi ${temporary_directory}/${bn}.txt
    echo textmidi --textmidi ${temporary_directory}/${bn}.txt --midi ${temporary_directory}/${bn}.mid --runningstatus standard
    /usr/bin/env textmidi      --textmidi ${temporary_directory}/${bn}.txt --midi ${temporary_directory}/${bn}.mid --runningstatus standard
    /usr/bin/env diff -s ${temporary_directory}/${bn}.mid $midifilename
    echo textmidi --textmidi ${temporary_directory}/${bn}.txt --midi ${temporary_directory}/${bn}.mid --runningstatus never
    /usr/bin/env textmidi      --textmidi ${temporary_directory}/${bn}.txt --midi ${temporary_directory}/${bn}.mid --runningstatus never
    /usr/bin/env diff -s ${temporary_directory}/${bn}.mid $midifilename
    echo textmidi --textmidi ${temporary_directory}/${bn}.txt --midi ${temporary_directory}/${bn}.mid --runningstatus persistentaftermeta
    /usr/bin/env textmidi      --textmidi ${temporary_directory}/${bn}.txt --midi ${temporary_directory}/${bn}.mid --runningstatus persistentaftermeta
    /usr/bin/env diff -s ${temporary_directory}/${bn}.mid $midifilename
    echo textmidi --textmidi ${temporary_directory}/${bn}.txt --midi ${temporary_directory}/${bn}.mid --runningstatus persistentaftersysex
    /usr/bin/env textmidi      --textmidi ${temporary_directory}/${bn}.txt --midi ${temporary_directory}/${bn}.mid --runningstatus persistentaftersysex
    /usr/bin/env diff -s ${temporary_directory}/${bn}.mid $midifilename
    echo textmidi --textmidi ${temporary_directory}/${bn}.txt --midi ${temporary_directory}/${bn}.mid --runningstatus persistentaftersysexormeta
    /usr/bin/env textmidi      --textmidi ${temporary_directory}/${bn}.txt --midi ${temporary_directory}/${bn}.mid --runningstatus persistentaftersysexormeta
    /usr/bin/env diff -s ${temporary_directory}/${bn}.mid $midifilename
    /usr/bin/env rm ${temporary_directory}/${bn}.mid ${temporary_directory}/${bn}.txt
    /usr/bin/env rmdir ${temporary_directory}
}

if [[ ${BASH_ARGC} == 1 ]] ; then
      midifilename=${BASH_ARGV[0]};
fi

runningstatustest $midifilename

