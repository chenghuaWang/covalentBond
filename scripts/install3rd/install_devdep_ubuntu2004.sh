#!/bin/bash
mkdir tmpd
cd tmpd
echo "[ info ] Start Build the Core development envs."
echo "[ info ] Try to install libssl/openssl on system."
sudo apt-get install openssl
sudo apt-get install libssl-dev
echo "[ info ] Try to get workflow lib and installing."
bash ./install_workflow_ubuntu2004.sh
echo "[ info ] Try to get sRPC lib and installing."
git clone https://github.com/lz4/lz4
cd lz4
make cc=clang
sudo make install
cd ..
bash ./install_srpc_ubuntu2004.sh
echo "[ info ] Try to get redis-server lib and installing."
sudo apt install redis-server
echo "[ info ] Try to get glog-dev lib and installing."
sudo apt-get install libgoogle-glog-dev
echo "[ info ] Try to get fmt lib and installing."
bash ./install_fmt_ubuntu2004.sh
echo "[ info ] Try to get luajit lib and installing."
bash ./install_luajit_ubuntu2004.sh