#!/bin/csh

cd render;make;cd ..
cd 3dslib;make;cd ..
cd loadmap;make;cd ..
cd script;make;cd ..

echo "Compiling DEMO"

setenv XLIBS "-L/usr/X11/lib -lX11 -lXext -lXmu -lXi"
setenv GL_LIBS "-lGL -L/usr/src/Mesa-3.1/src-glut -lglut -L/usr/src/Mesa-3.1/src-glu -lGLU"
setenv LIBS "-lm -Ljpeglib -ljpeg -L3dslib -last3d -Lmp3lib -lMP3"
setenv OBJ2 "loadmap/load_map.o loadmap/loadmaps.o loadmap/loadmat.o loadmap/loadtxtr.o"
setenv OBJA "afs/afs3.o afs/unesplib.o"
setenv OBJS "script/script.o blob/blob.o fdtunnel/fdtunnel.o render/render.o render/particle.o timer/timer-lx.o"
setenv INCL "-I/usr/include/glide"

gcc -g -O2 demo-lin.c -o demo $INCL $OBJA $OBJS $OBJ2 $GL_LIBS $XLIBS $LIBS >& errors
cat errors
strip demo
