#! /bin/sh

./clean.sh

export OPTFLAGS="-O4 -fomit-frame-pointer -pipe -march=i686 -ffast-math"

cd UTIL;c;cd ..
cd blob;c;cd ..
cd mp3lib;c;cd ..
cd timer;c;cd ..
cd fdtunnel;c;cd ..
cd spline;c;cd ..
cd smoke;c;cd ..
cd sinpart;c;cd ..
cd greets;c;cd ..
cd agl;c;cd ..
cd hjbtunel;c;cd ..
cd bsptunel;c;cd ..
cd swirl;c;cd ..
cd sinzoom;c;cd ..
cd fdwater;c;cd ..
cd MAX;c;cd ..
cd gears;c;cd ..

cd afs;c;cd ..
cd 3dslib;c;cd ..
cd loadmap;c;cd ..
cd render;c;cd ..
cd script;c;cd ..

# ./linux.sh
./demo-l.sh

