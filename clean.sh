#! /bin/sh
echo "Deleting object files and libraries..."
find . -name \*.o -exec rm -f {} \;
find . -name \*.a -exec rm -f {} \;
find . -name \*.obj -exec rm -f {} \;
find . -name \*.lib -exec rm -f {} \;
find . -name test -exec rm -f {} \;
find . -name Makefile.bak -exec rm -f {} \;
# cd UTIL;./unix2dos;cd ..
rm -f demo linux make-pak UTIL/dos2unix UTIL/unix2dos errors
