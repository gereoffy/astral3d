# LINUX Makefile made by A'rpi / Astral

SRCS = ast3d.c camera.c file_3ds.c quatern.c spline.c
OBJS = ast3d.o camera.o file_3ds.o quatern.o spline.o
# CC   = wgcc
# AR   = war
# CFLAGS = -O3 -Wall -mpentiumpro -pipe
CFLAGS = $(OPTFLAGS) -Wall

.SUFFIXES: .c .o

# .PHONY: all clean

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

all:	libast3d.a

libast3d.a: $(OBJS)
	rm -f libast3d.a
	$(AR) -r libast3d.a $(OBJS)

clean:
	rm -f *~ *.o Makefile.bak

distclean:
	makedepend
	rm -f *~ *.o libast3d.a

dep:	depend

depend:
	makedepend -- $(CFLAGS) -- $(SRCS) &>/dev/null

# DO NOT DELETE
