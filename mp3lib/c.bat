rem wcl386 /l=nt play.c sr1.obj

del mp3.lib
wcc386 -otexan -s -fp5 -fpi87 -5 sr1.c
wcc386 -otexan -s -fp5 -fpi87 -5 audio_w.c
wlib mp3.lib +audio_w.obj +sr1.obj


