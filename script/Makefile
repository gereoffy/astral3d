# LINUX Makefile made by A'rpi / Astral

SRCS = script2.c
OBJS = script2.o
CC   = gcc
CFLAGS = -g -O2 -Wall

.SUFFIXES: .c .o

# .PHONY: all clean

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

all:	$(OBJS)

clean:
	rm -f *~ *.o Makefile.bak

distclean:
	makedepend
	rm -f *~ *.o

dep:	depend

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

# DO NOT DELETE

script2.o: /usr/include/stdio.h /usr/include/features.h
script2.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
script2.o: /usr/include/bits/types.h /usr/include/libio.h
script2.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
script2.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
script2.o: /usr/include/endian.h /usr/include/bits/endian.h
script2.o: /usr/include/sys/select.h /usr/include/bits/select.h
script2.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
script2.o: /usr/include/alloca.h /usr/include/unistd.h
script2.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
script2.o: /usr/include/getopt.h /usr/include/string.h /usr/include/math.h
script2.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
script2.o: /usr/include/bits/mathcalls.h /usr/include/sys/time.h
script2.o: /usr/include/bits/time.h ../agl/agl.h /usr/include/GL/gl.h
script2.o: /usr/include/GL/glu.h ../3dslib/ast3d.h ../timer/timer.h
script2.o: ../mp3lib/mp3.h ../afs/afs.h ../loadmap/loadtxtr.h ../blob/blob.h
script2.o: ../fdtunnel/fdtunnel.h ../spline/spline.h ../smoke/smoke.h
script2.o: ../sinpart/sinpart.h ../greets/greets.h ../render/render.h vars.h
script2.o: cmds.h scr_evnt.c scr_draw.c file.c
