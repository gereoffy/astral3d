# LINUX Makefile made by A'rpi / Astral

SRCS = load_map.c loadmaps.c loadmat.c loadtxtr.c
OBJS = load_map.o loadmaps.o loadmat.o loadtxtr.o
CC   = gcc
CFLAGS = -O2 -Wall
LIB  = liblmap.a

.SUFFIXES: .c .o

# .PHONY: all clean

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

all:	$(LIB)

$(LIB):	$(OBJS)
	rm -f $(LIB)
	ar -r $(LIB) $(OBJS)

clean:
	rm -f *~ *.o Makefile.bak

distclean:
	makedepend
	rm -f *~ *.o $(LIB)

dep:	depend

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

# DO NOT DELETE

load_map.o: /usr/include/stdio.h /usr/include/features.h
load_map.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
load_map.o: /usr/include/bits/types.h /usr/include/libio.h
load_map.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
load_map.o: /usr/include/stdlib.h /usr/include/sys/types.h
load_map.o: /usr/include/time.h /usr/include/endian.h
load_map.o: /usr/include/bits/endian.h /usr/include/sys/select.h
load_map.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
load_map.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
load_map.o: /usr/include/string.h ../afs/afs.h load_map.h load_raw.c
load_map.o: load_jpg.c ../../libs/jpeglib/jpeglib.h
load_map.o: ../../libs/jpeglib/jconfig.h ../../libs/jpeglib/jmorecfg.h
loadmaps.o: /usr/include/stdio.h /usr/include/features.h
loadmaps.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
loadmaps.o: /usr/include/bits/types.h /usr/include/libio.h
loadmaps.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
loadmaps.o: /usr/include/stdlib.h /usr/include/sys/types.h
loadmaps.o: /usr/include/time.h /usr/include/endian.h
loadmaps.o: /usr/include/bits/endian.h /usr/include/sys/select.h
loadmaps.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
loadmaps.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
loadmaps.o: /usr/include/math.h /usr/include/bits/huge_val.h
loadmaps.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
loadmaps.o: /usr/include/GL/gl.h /usr/include/GL/glu.h ../3dslib/ast3d.h
loadmaps.o: ../render/render.h ../loadmap/load_map.h ../loadmap/loadtxtr.h
loadmaps.o: ../loadmap/loadmaps.h
loadmat.o: /usr/include/stdio.h /usr/include/features.h
loadmat.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
loadmat.o: /usr/include/bits/types.h /usr/include/libio.h
loadmat.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
loadmat.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
loadmat.o: /usr/include/endian.h /usr/include/bits/endian.h
loadmat.o: /usr/include/sys/select.h /usr/include/bits/select.h
loadmat.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
loadmat.o: /usr/include/alloca.h /usr/include/GL/gl.h /usr/include/GL/glu.h
loadmat.o: ../3dslib/ast3d.h ../render/render.h ../loadmap/loadtxtr.h
loadtxtr.o: /usr/include/stdio.h /usr/include/features.h
loadtxtr.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
loadtxtr.o: /usr/include/bits/types.h /usr/include/libio.h
loadtxtr.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
loadtxtr.o: /usr/include/stdlib.h /usr/include/sys/types.h
loadtxtr.o: /usr/include/time.h /usr/include/endian.h
loadtxtr.o: /usr/include/bits/endian.h /usr/include/sys/select.h
loadtxtr.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
loadtxtr.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
loadtxtr.o: /usr/include/string.h /usr/include/GL/gl.h /usr/include/GL/glu.h
loadtxtr.o: ../loadmap/load_map.h ../loadmap/loadtxtr.h
