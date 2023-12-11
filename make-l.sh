#! /bin/sh

./clean.sh

# export OPTFLAGS="-O4 -fomit-frame-pointer -pipe -march=i686 -ffast-math"
# export OPTFLAGS="-O4 -fomit-frame-pointer -pipe -march=k6 -mcpu=k6 -ffast-math"
export OPTFLAGS="-Os -ffast-math"
export CC=gcc
export AR=ar
export LD=ld

(cd afs; make clean; make)
(cd agl; ./c)
(cd timer; ./c)
(cd mp3lib; ./c)

(cd 3dslib; make clean; make)
(cd loadmap; make clean; make)
(cd render; make clean; make)

(cd MAX; ./c)
(cd fx; ./c)

(cd script; make clean; make)

./demo-l.sh

