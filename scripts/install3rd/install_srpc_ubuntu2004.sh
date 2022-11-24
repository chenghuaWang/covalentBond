#!/bin/bash
sudo tar -zxvf ../tmp/protobuf-all-3.19.4.tar.gz
cd protobuf-3.19.4
sudo ./autogen.sh
sudo ./configure
sudo make CC=clang
sudo make check
sudo make install
sudo ldconfig
cd ..
git clone --recursive https://github.com/sogou/srpc.git
cd srpc
make CC=clang
sudo make CC=clang install
cd ..