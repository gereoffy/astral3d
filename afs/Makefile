# LINUX Makefile made by A'rpi / Astral

SRCS = afs3.c unesplib.c
OBJS = afs3.o unesplib.o
CC   = gcc
CFLAGS = -O2 -Wall
LIB  = libafs.a

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

afs3.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
afs3.o: /usr/include/gnu/stubs.h
afs3.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stddef.h
afs3.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stdarg.h
afs3.o: /usr/include/bits/types.h /usr/include/libio.h
afs3.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
afs3.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
afs3.o: /usr/include/endian.h /usr/include/bits/endian.h
afs3.o: /usr/include/sys/select.h /usr/include/bits/select.h
afs3.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
afs3.o: /usr/include/alloca.h /usr/include/string.h unesp.h afs.h
unesplib.o: /usr/include/stdio.h /usr/include/features.h
unesplib.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
unesplib.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stddef.h
unesplib.o: /usr/lib/gcc-lib/i386-slackware-linux/egcs-2.91.66/include/stdarg.h
unesplib.o: /usr/include/bits/types.h /usr/include/libio.h
unesplib.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
unesplib.o: /usr/include/stdlib.h /usr/include/sys/types.h
unesplib.o: /usr/include/time.h /usr/include/endian.h
unesplib.o: /usr/include/bits/endian.h /usr/include/sys/select.h
unesplib.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
unesplib.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
unesplib.o: /usr/include/string.h unesp.h
