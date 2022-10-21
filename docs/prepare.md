# Prepare

```
Author: chenghua Wang.
```

## Install WSL Ubuntu20.04.

Our development is on WSL-Ubuntu20.04. If you has a physical machine runs Ubuntu20.04 already, just skip this section.

You can find all information on the WSL doc [how-to-install](https://learn.microsoft.com/en-us/windows/wsl/install)

## Dependencies

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

### Frontend components
