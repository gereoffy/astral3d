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

