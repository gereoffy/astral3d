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
afs3.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
afs3.o: /usr/include/libio.h /usr/include/_G_config.h
afs3.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
afs3.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
afs3.o: /usr/include/bits/endian.h /usr/include/sys/select.h
afs3.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
afs3.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
afs3.o: /usr/include/string.h unesp.h afs.h
unesplib.o: /usr/include/stdio.h /usr/include/features.h
unesplib.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
unesplib.o: /usr/include/bits/types.h /usr/include/libio.h
unesplib.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
unesplib.o: /usr/include/stdlib.h /usr/include/sys/types.h
unesplib.o: /usr/include/time.h /usr/include/endian.h
unesplib.o: /usr/include/bits/endian.h /usr/include/sys/select.h
unesplib.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
unesplib.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
unesplib.o: /usr/include/string.h unesp.h
