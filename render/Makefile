# LINUX Makefile made by A'rpi / Astral

SRCS = render.c particle.c
OBJS = render.o particle.o
CC   = gcc
# CFLAGS = -g -Wall
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
render.o: /usr/include/bits/types.h /usr/include/libio.h
render.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
render.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
render.o: /usr/include/endian.h /usr/include/bits/endian.h
render.o: /usr/include/sys/select.h /usr/include/bits/select.h
render.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
render.o: /usr/include/alloca.h /usr/include/math.h
render.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
render.o: /usr/include/bits/mathcalls.h rawfile.c ../agl/agl.h
render.o: /usr/include/GL/gl.h /usr/include/GL/glu.h ../3dslib/ast3d.h
render.o: ../3dslib/vector.h ../3dslib/matrix.h render.h ../prof.h triangle.c
render.o: light.c vertlits.c laser.c 2_mater.c 3_light.c 4_textur.c 4a_bump.c
render.o: 5_envmap.c 6_ltmap.c 7_speclr.c renderlm.c lmapmake.c lightvtx.c
particle.o: /usr/include/stdio.h /usr/include/features.h
particle.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
particle.o: /usr/include/bits/types.h /usr/include/libio.h
particle.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
particle.o: /usr/include/stdlib.h /usr/include/sys/types.h
particle.o: /usr/include/time.h /usr/include/endian.h
particle.o: /usr/include/bits/endian.h /usr/include/sys/select.h
particle.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
particle.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
particle.o: /usr/include/math.h /usr/include/bits/huge_val.h
particle.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
particle.o: ../agl/agl.h /usr/include/GL/gl.h /usr/include/GL/glu.h
particle.o: ../3dslib/ast3d.h render.h
