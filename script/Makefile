# LINUX Makefile made by A'rpi / Astral

SRCS = script.c
OBJS = script.o
CC   = gcc
CFLAGS = -O2 -mpentiumpro -pipe -Wall

.SUFFIXES: .c .o

# .PHONY: all clean

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

all:	script.o

clean:
	rm -f *~ *.o Makefile.bak

distclean:
	makedepend
	rm -f *~ *.o

dep:	depend

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

# DO NOT DELETE

script.o: /usr/include/stdio.h /usr/include/features.h
script.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
script.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stddef.h
script.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stdarg.h
script.o: /usr/include/bits/types.h /usr/include/libio.h
script.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
script.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
script.o: /usr/include/endian.h /usr/include/bits/endian.h
script.o: /usr/include/sys/select.h /usr/include/bits/select.h
script.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
script.o: /usr/include/alloca.h /usr/include/unistd.h
script.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
script.o: /usr/include/getopt.h /usr/include/string.h /usr/include/math.h
script.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
script.o: /usr/include/bits/mathcalls.h
script.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/float.h
script.o: /usr/include/sys/time.h /usr/include/bits/time.h ../mp3lib/mp3.h
script.o: /usr/include/GL/gl.h /usr/include/GL/glu.h ../3dslib/ast3d.h
script.o: ../timer/timer.h ../blob/blob.h ../fdtunnel/fdtunnel.h
script.o: ../render/render.h ../loadmap/loadmaps.h scr_var.c scr_event.c
script.o: scr_file.c scr_init.c vars.h scr_draw.c scr_exec.c
