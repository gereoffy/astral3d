# LINUX Makefile made by A'rpi / Astral

SRCS = render.c particle.c
OBJS = render.o particle.o
CC   = gcc
CFLAGS = -O2 -mpentiumpro -pipe

.SUFFIXES: .c .o

# .PHONY: all clean

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

all:	render.o particle.o

clean:
	rm -f *~ *.o Makefile.bak

distclean:
	makedepend
	rm -f *~ *.o

dep:	depend

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

# DO NOT DELETE

render.o: /usr/include/stdio.h /usr/include/features.h
render.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
render.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stddef.h
render.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stdarg.h
render.o: /usr/include/bits/types.h /usr/include/libio.h
render.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
render.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
render.o: /usr/include/endian.h /usr/include/bits/endian.h
render.o: /usr/include/sys/select.h /usr/include/bits/select.h
render.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
render.o: /usr/include/alloca.h /usr/include/math.h
render.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
render.o: /usr/include/bits/mathcalls.h
render.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/float.h
render.o: /usr/include/GL/gl.h /usr/include/GL/glu.h ../3dslib/ast3d.h
render.o: ../3dslib/vector.h ../3dslib/matrix.h render.h particle.h ../prof.h
render.o: triangle.c frustum.c light.c 1_transf.c 1a_fcull.c 2_mater.c
render.o: 3_light.c 4_textur.c 5_envmap.c 6_lightmap.c 7_speclr.c
particle.o: /usr/include/stdio.h /usr/include/features.h
particle.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
particle.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stddef.h
particle.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stdarg.h
particle.o: /usr/include/bits/types.h /usr/include/libio.h
particle.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
particle.o: /usr/include/stdlib.h /usr/include/sys/types.h
particle.o: /usr/include/time.h /usr/include/endian.h
particle.o: /usr/include/bits/endian.h /usr/include/sys/select.h
particle.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
particle.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
particle.o: /usr/include/math.h /usr/include/bits/huge_val.h
particle.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
particle.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/float.h
particle.o: /usr/include/GL/gl.h ../3dslib/ast3d.h render.h
