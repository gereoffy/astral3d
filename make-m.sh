#! /bin/sh

./clean.sh

export OPTFLAGS="-Os -ffast-math -DGL_SILENCE_DEPRECATION -I/opt/local/include/"
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

./demo-m.sh

