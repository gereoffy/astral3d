#!/bin/csh

rm -f linux

echo "------------- Compiling LIBS ------------------"

cd afs;make;cd ..
cd loadmap;make;cd ..
cd 3dslib;make;cd ..
cd render;make;cd ..

echo "------------- Compiling LINUX -----------------"

setenv XLIBS "-L/usr/X11/lib -lX11 -lXext -lXmu -lXi"
setenv LIBS "-L3dslib -last3d -Lloadmap -llmap -Lafs -lafs -Lmp3lib -lMP3"
setenv LIBS2 "-lm -lGL -L../libs -ljpeg -lglut -lGLU"
setenv OBJS "render/render.o render/particle.o agl/agl.o timer/timer-lx.o"

gcc -g -O2 linux.c -o linux $OBJS $LIBS $LIBS2 $XLIBS >& errors
cat errors
# strip linux
