#!/bin/bash
git clone https://github.com/LuaJIT/LuaJIT
cd LuaJIT
make CC=clang
sudo make install
cd ..