#!/bin/csh

echo "------------- Compiling LIBS ------------------"

cd afs;make;cd ..
cd loadmap;make;cd ..
cd 3dslib;make;cd ..
cd render;make;cd ..
cd script;make;cd ..

# cd mp3lib;./c;cd ..


# cd smoke;./c;cd ..

echo "------------- Compiling DEMO ------------------"

setenv XLIBS "-L/usr/X11/lib -lX11 -lXext -lXmu -lXi"
# setenv GL_LIBS "-lGL -L/usr/src/Mesa-3.1/src-glut -lglut -L/usr/src/Mesa-3.1/src-glu -lGLU"
setenv LIBS "-L3dslib -last3d -Lloadmap -llmap -Lafs -lafs -Lmp3lib -lMP3"
setenv LIBS2 "-lm -lGL -L../libs -ljpeg -lglut -lGLU"
setenv OBJS "MAX/render.o gears/gears.o script/script2.o blob/blob.o sinzoom/sinzoom.o fdtunnel/fdtunnel.o fdwater/fdwater.o swirl/swirl.o hjbtunel/tunnel2.o bsptunel/tunnel.o spline/spline.o smoke/smoke.o greets/greets.o sinpart/sinpart.o render/render.o render/particle.o render/partic2.o agl/agl.o timer/timer-lx.o"

gcc -O2 demo-lin.c -o demo $OBJS $LIBS $LIBS2 $XLIBS >& errors
cat errors
strip demo
