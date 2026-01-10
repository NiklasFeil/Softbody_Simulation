#! /bin/bash

premake5 gmake
cd build
rm SoftbodySimulation/Simulation
make
./SoftbodySimulation/Simulation
