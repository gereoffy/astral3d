# LINUX Makefile made by A'rpi / Astral

SRCS = script2.c
OBJS = script2.o
# CC   = wgcc
CFLAGS = $(OPTFLAGS) -Wall

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
	makedepend -- $(CFLAGS) -- $(SRCS) &>/dev/null

# DO NOT DELETE
