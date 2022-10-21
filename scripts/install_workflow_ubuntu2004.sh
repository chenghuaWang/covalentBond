#!/bin/bash
git clone https://github.com/sogou/workflow
cd workflow
make CC=clang
sudo make CC=clang install
cd ..