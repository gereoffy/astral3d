# LINUX Makefile made by A'rpi / Astral

SRCS = afs4.c unesplib.c ole2.c
OBJS = afs4.o unesplib.o ole2.o
LIB  = libafs.a

# CC   = wgcc
# AR   = war
CFLAGS = $(OPTFLAGS) -Wall

.SUFFIXES: .c .o

# .PHONY: all clean

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

all:	$(LIB)

$(LIB):	$(OBJS)
	rm -f $(LIB)
	$(AR) -r $(LIB) $(OBJS)

clean:
	rm -f *~ *.o Makefile.bak

distclean:
	makedepend
	rm -f *~ *.o $(LIB)

dep:	depend

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

# DO NOT DELETE

