#! /bin/bash

rm -r build
premake5 gmake
cd build
make
./bin/Debug/Softbody
