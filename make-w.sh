#! /bin/sh

./clean.sh

# export OPTFLAGS="-O3 -fomit-frame-pointer -fno-strict-aliasing -pipe -march=i686"
export OPTFLAGS="-O2"

cd UTIL;cw;cd ..
cd blob;cw;cd ..
cd mp3lib;cw;cd ..
cd timer;cw;cd ..
cd fdtunnel;cw;cd ..
cd spline;cw;cd ..
cd smoke;cw;cd ..
cd sinpart;cw;cd ..
cd greets;cw;cd ..
cd agl;cw;cd ..
cd hjbtunel;cw;cd ..
cd bsptunel;cw;cd ..
cd swirl;cw;cd ..
cd sinzoom;cw;cd ..
cd fdwater;cw;cd ..
cd MAX;cw;cd ..
cd gears;cw;cd ..

cd afs;cw;cd ..
cd 3dslib;cw;cd ..
cd loadmap;cw;cd ..
cd render;cw;cd ..
cd script;cw;cd ..

# ./linux.sh
./demo-w.sh

