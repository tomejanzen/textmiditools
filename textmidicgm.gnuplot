#
# RUN WITH by specifying the data file output by textmidicgm:
# /usr/bin/gnuplot -e "thefile='tjcgm.txt'" textmidicgm.gnuplot
#
# AlgoRhythms 3.0 colors
#wavecolor = "green"
#textcolor = "white"
#backgroundcolor = "0x9932CC"
## AlgoRhythms 1.0 colors
wavecolor = "0xAAAAEE"
textcolor = "black"
backgroundcolor = "0xCCCCCC"
set title "textmidicgm" textcolor rgb textcolor
set terminal jpeg background rgb backgroundcolor size 1024,1024
set grid xtics linetype rgb textcolor
set grid ytics linetype rgb textcolor
set border  linetype rgb textcolor
set xtics textcolor rgb textcolor
set ytics textcolor rgb textcolor
set size 1.0,0.5
set multiplot layout 4,1
set size 1.0,0.25
set autoscale y
set title "Pitch Index" textcolor rgb textcolor
plot [] [] thefile every 1 index "pitch" with yerrorbars linecolor rgb wavecolor 
set autoscale y
set title "Duration" textcolor rgb textcolor
plot [] [] thefile every 1 index "rhythm" with yerrorbars linecolor rgb wavecolor
set autoscale y
set title "Velocity" textcolor rgb textcolor
plot [] [] thefile every 1 index "dynamic" with yerrorbars linecolor rgb wavecolor
set autoscale y
set title "Complement" textcolor rgb textcolor
plot [] [] thefile every 1 index "texture" with yerrorbars linecolor rgb wavecolor

unset multiplot
