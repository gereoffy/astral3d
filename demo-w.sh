#!/bin/csh

echo "------------- Compiling DEMO ------------------"

# setenv GL_LIBS "-lGL -L/usr/src/Mesa-3.1/src-glut -lglut -L/usr/src/Mesa-3.1/src-glu -lGLU"
setenv LIBS "-L3dslib -last3d -Lfx -lastFX -Lloadmap -llmap -Lafs -lafs -Lmp3lib -lMP3"
setenv LIBS2 "-lopengl32 -L../libs -lwjpeg -lwinmm -lgdi32 -lglu32 -luser32"
setenv OBJS "MAX/render.o script/script2.o render/render.o render/particle.o render/partic2.o render/partic3.o agl/agl.o timer/timer.o"

wgcc -O2 -Wall demo-win.c win-code.c -o demo.exe $OBJS $LIBS $LIBS2
# wstrip demo.exe
