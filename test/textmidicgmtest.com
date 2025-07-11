../textmidicgm --xmlform textmidicgmtest_base.xml
../textmidicgm --xmlform textmidicgmtest_arrangements.xml
../textmidicgm --xmlform textmidicgmtest_pulse4.xml
../textmidicgm --xmlform textmidicgmtest_tempo90.xml
../textmidicgm --xmlform textmidicgmtest_halfbeat.xml
gvim -o textmidicgmtest_*.txt
