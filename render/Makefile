# LINUX Makefile made by A'rpi / Astral

SRCS = render.c particle.c partic2.c partic3.c
OBJS = render.o particle.o partic2.o partic3.o
# CC   = wgcc
# CFLAGS = -g -Wall
# CFLAGS = -O3 -mpentiumpro -pipe
CFLAGS = $(OPTFLAGS)

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
