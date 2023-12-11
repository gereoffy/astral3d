#!/bin/sh

echo "------------- Compiling DEMO ------------------"

export OBJS="MAX/render.o script/script2.o render/render.o render/particle.o render/partic2.o render/partic3.o agl/agl.o timer/timer.o"
export LIBS="-L3dslib -last3d -Lfx -lastFX -Lloadmap -llmap -Lafs -lafs -Lmp3lib -lMP3"
export LIBS2="-lm -lGL -ljpeg -lglut -lGLU"
export XLIBS="-L/usr/X11/lib -lX11 -lXext -lXmu -lXi"

gcc -O3 demo-lin.c -o demo $OBJS $LIBS $LIBS2 $XLIBS >& errors
cat errors
# strip demo
