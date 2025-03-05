#!/usr/bin/env bash
thedate=$(/usr/bin/env date +%Y%m%d)
/usr/bin/env date    
./textmidiroundtriptest.py classical_archives0.txt verbose > classical_archives0log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives1.txt verbose > classical_archives1log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives2.txt verbose > classical_archives2log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives3.txt verbose > classical_archives3log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives5.txt verbose > classical_archives5log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives6.txt verbose > classical_archives6log${thedate}.txt 2>&1
./textmidiroundtriptest.py classical_archives_never0.txt verbose > classical_archives_never0log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_never1.txt verbose > classical_archives_never1log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_never2.txt verbose > classical_archives_never2log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_never3.txt verbose > classical_archives_never3log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_never4.txt verbose > classical_archives_never4log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_never6.txt verbose > classical_archives_never6log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_never5.txt verbose > classical_archives_never5log${thedate}.txt 2>&1 
./textmidiroundtriptest.py classical_archives_rigid0.txt lazy verbose > classical_archives_rigid0log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_rigid1.txt lazy verbose > classical_archives_rigid1log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_rigid2.txt lazy verbose > classical_archives_rigid2log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_rigid3.txt lazy verbose > classical_archives_rigid3log${thedate}.txt 2>&1
./textmidiroundtriptest.py classical_archives_rigid0.txt simplecontinuedfraction lazy verbose > classical_archives_rigidsimplecontinuedfraction0log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_rigid1.txt simplecontinuedfraction lazy verbose > classical_archives_rigidsimplecontinuedfraction1log${thedate}.txt 2>&1
./textmidiroundtriptest.py classical_archives_rigid2.txt simplecontinuedfraction lazy verbose > classical_archives_rigidsimplecontinuedfraction2log${thedate}.txt 2>&1 &
./textmidiroundtriptest.py classical_archives_rigid3.txt simplecontinuedfraction lazy verbose > classical_archives_rigidsimplecontinuedfraction3log${thedate}.txt 2>&1
ps -ef | tail
/usr/bin/env date
