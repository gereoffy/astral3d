# LINUX Makefile made by A'rpi / Astral

SRCS = ast3d.c camera.c file_3ds.c quatern.c spline.c
OBJS = ast3d.o camera.o file_3ds.o quatern.o spline.o
CC   = gcc
CFLAGS = -g -O2 -Wall -mpentiumpro -pipe

.SUFFIXES: .c .o

# .PHONY: all clean

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

all:	libast3d.a

libast3d.a: $(OBJS)
	rm -f libast3d.a
	ar -r libast3d.a $(OBJS)

clean:
	rm -f *~ *.o Makefile.bak

distclean:
	makedepend
	rm -f *~ *.o libast3d.a

dep:	depend

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

# DO NOT DELETE

ast3d.o: /usr/include/stdlib.h /usr/include/features.h
ast3d.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ast3d.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stddef.h
ast3d.o: /usr/include/sys/types.h /usr/include/bits/types.h
ast3d.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
ast3d.o: /usr/include/sys/select.h /usr/include/bits/select.h
ast3d.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
ast3d.o: /usr/include/alloca.h /usr/include/stdio.h
ast3d.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stdarg.h
ast3d.o: /usr/include/libio.h /usr/include/_G_config.h
ast3d.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
ast3d.o: /usr/include/ctype.h /usr/include/math.h
ast3d.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
ast3d.o: /usr/include/bits/mathcalls.h
ast3d.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/float.h
ast3d.o: ast3d.h ast3di.h ../afs/afs.h vector.h matrix.h fix_uv.c do_trans.c
ast3d.o: visibchk.c
camera.o: /usr/include/math.h /usr/include/features.h
camera.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
camera.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
camera.o: /usr/include/bits/mathcalls.h
camera.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/float.h
camera.o: /usr/include/stdio.h
camera.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stddef.h
camera.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stdarg.h
camera.o: /usr/include/bits/types.h /usr/include/libio.h
camera.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h ast3d.h
camera.o: ast3di.h ../afs/afs.h vector.h matrix.h
file_3ds.o: /usr/include/string.h /usr/include/features.h
file_3ds.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
file_3ds.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stddef.h
file_3ds.o: /usr/include/stdlib.h /usr/include/sys/types.h
file_3ds.o: /usr/include/bits/types.h /usr/include/time.h
file_3ds.o: /usr/include/endian.h /usr/include/bits/endian.h
file_3ds.o: /usr/include/sys/select.h /usr/include/bits/select.h
file_3ds.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
file_3ds.o: /usr/include/alloca.h /usr/include/stdio.h
file_3ds.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stdarg.h
file_3ds.o: /usr/include/libio.h /usr/include/_G_config.h
file_3ds.o: /usr/include/bits/stdio_lim.h /usr/include/math.h
file_3ds.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
file_3ds.o: /usr/include/bits/mathcalls.h
file_3ds.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/float.h
file_3ds.o: ast3d.h ast3di.h ../afs/afs.h vector.h matrix.h
quatern.o: /usr/include/math.h /usr/include/features.h
quatern.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
quatern.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
quatern.o: /usr/include/bits/mathcalls.h
quatern.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/float.h
quatern.o: /usr/include/stdio.h
quatern.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stddef.h
quatern.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stdarg.h
quatern.o: /usr/include/bits/types.h /usr/include/libio.h
quatern.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h ast3d.h
quatern.o: ast3di.h ../afs/afs.h
spline.o: /usr/include/math.h /usr/include/features.h
spline.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
spline.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
spline.o: /usr/include/bits/mathcalls.h
spline.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/float.h
spline.o: /usr/include/stdio.h
spline.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stddef.h
spline.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stdarg.h
spline.o: /usr/include/bits/types.h /usr/include/libio.h
spline.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h ast3d.h
spline.o: ast3di.h ../afs/afs.h vector.h
