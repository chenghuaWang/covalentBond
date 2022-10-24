#!/bin/bash
git clone https://github.com/fmtlib/fmt.git
cd fmt
mkdir build
cd build
cmake ..
sudo make cc=clang install
cd ..
cd ..