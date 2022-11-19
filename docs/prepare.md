# Prepare

```
Author: chenghua Wang.
```

## Install WSL Ubuntu20.04.

Our development is on WSL-Ubuntu20.04. If you has a physical machine runs Ubuntu20.04 already, just skip this section.

You can find all information on the WSL doc [how-to-install](https://learn.microsoft.com/en-us/windows/wsl/install)

## Dependencies

**❗❗ Before you start, please make sure you have installed `CMake v3.22+` and `clang v12+` ❗❗**

### Core components

**📌📌I have integrated all installation commands in a shell script. If you don't want to know the details or modify the installation settings, just run the commands below in the ./scripts directory.📌📌**

```
bash ./install_devdep_ubuntu2004.sh
```

#### libssl

```shell
sudo apt-get install libssl-dev  # on ubuntu, centos is openssl-devel
```

#### [workflow](https://github.com/sogou/workflow)

```shell
git clone https://github.com/sogou/workflow
cd workflow
make CC=clang
sudo make CC=clang install
```

#### [lz4](https://github.com/lz4/lz4)

```shell
git clone https://github.com/lz4/lz4
cd lz4
make cc=clang
sudo make install
cd ..
```

#### [sRPC](https://github.com/sogou/srpc)

```shell
# install the protobuf3 first.
sudo tar -zxvf ../tmp/protobuf-all-3.19.4.tar.gz
cd protobuf-3.19.4
sudo ./autogen.sh
sudo ./configure
sudo make CC=clang
sudo make check
sudo make install
sudo ldconfig
cd ..
# !!! restart machine if needed !!!
# install the sRPC framework.
git clone --recursive https://github.com/sogou/srpc.git
cd srpc
make CC=clang
sudo make CC=clang install
```

#### Install Redis

**📌📌For manager service Only.📌📌**

```shell
sudo apt install redis-server
```

#### gLog

```shell
sudo apt-get install libgoogle-glog-dev
```

#### fmt

```shell
git clone https://github.com/fmtlib/fmt.git
cd fmt
mkdir build
cd build
cmake ..
sudo make cc=clang install
cd ..
cd ..
```

#### lua jit

```shell
git clone https://github.com/LuaJIT/LuaJIT
cd LuaJIT
make CC=clang
sudo make install
cd ..
```

#### sol2

```shell
git clone https://github.com/ThePhD/sol2.git
cd sol2
mkdir build
cd build
cmake ..
sudo make
sudo make install
cd ..
cd ..
```

#### wfrest

```shell
git clone --recursive https://github.com/wfrest/wfrest
cd wfrest
make CC=clang
sudo make install
```

### Frontend components

We adopt [flutter](https://flutter.dev) to build our front end. The dart packages we used is illustrated bellow:

* foo 1
* foo 2
