
echo Compiling $1...
cd $1
$CC -c $OPTFLAGS $1.c
$AR r ../libastFX.a $1.o
echo \#include \"$1/$1.h\" >>../libastFX.h
