#! /bin/sh

./clean.sh

# export OPTFLAGS="-O4 -fomit-frame-pointer -pipe -march=i686 -ffast-math"
# export OPTFLAGS="-O4 -fomit-frame-pointer -pipe -march=k6 -mcpu=k6 -ffast-math"
export OPTFLAGS="-O3 -ffast-math"
export CC=gcc
export AR=ar
export LD=ld

cd UTIL;c;cd ..

cd mp3lib;c;cd ..
cd timer;c;cd ..
cd script;c;cd ..

cd agl;c;cd ..
cd afs;c;cd ..
cd loadmap;c;cd ..

cd fx;c;cd ..

cd 3dslib;c;cd ..
cd render;c;cd ..
cd MAX;c;cd ..

./demo-l.sh

