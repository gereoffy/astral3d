rem wcl386 /l=nt play.c sr1.obj

del mp3.lib
wcc386 -otexan -s -fp5 -fpi87 -5 sr1.c
wcc386 -otexan -s -fp5 -fpi87 -5 audio_win.c
wlib mp3.lib +audio_win.obj +sr1.obj


