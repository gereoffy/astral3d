#! /bin/sh

./clean.sh

# export OPTFLAGS="-O4 -fomit-frame-pointer -pipe -march=i686 -ffast-math"
# export OPTFLAGS="-O3 -fomit-frame-pointer -fno-strict-aliasing -pipe -march=i686"
export OPTFLAGS="-O2"

cd UTIL;cw;cd ..

cd mp3lib;cw;cd ..
cd timer;cw;cd ..
cd script;cw;cd ..

cd agl;cw;cd ..
cd afs;cw;cd ..
cd loadmap;cw;cd ..

cd fx;cw;cd ..

cd 3dslib;cw;cd ..
cd render;cw;cd ..
cd MAX;cw;cd ..

./demo-w.sh

