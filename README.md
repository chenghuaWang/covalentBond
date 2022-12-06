[简体中文](./README_CN.md) | [English](./README.md)

# Data Middle Office

Our Data Middle Office(dbo) is named covalent bond. A tool for managing distributed database, gathering/cleaning data, etc. This tool is a final project for 2022-2023 Fall Software Engineering lecture.

You can get more information on [WIKI](https://github.com/chenghuaWang/covalentBond/wiki)

The cpp doxygen doc is activate on [cppDoc](https://chenghuawang.github.io/covalentBond/doxygen/html/annotated.html)

## Architecture


## Usage

There r some options u need to set first before running cb-server.

* `webPort` The port of web server, default to `8888`.
* `webRoot` The root dict of web server, default to `.`.
* `httpPort` The backend http server port, default to `8080`.
* `execSec` Interval between graphs starts, default to `10`.
* `redisHost` The host ip of redis server, DNS is not support, no default.
* `redisPassword` The password of redis server, no default.
* `redisPort` The port of redis server, no default.
* `redisDBNum` The database number of redis server, default to `1`.
* `redisSSL` If using SSL on redis connection, default to `0`(True=1, False=0).

All options can be gave to cb-server using POSIX style cmd.

```shell
cb-server --redisHost x.x.x.x --redisPort 6379 --redisDBNum 1 --redisPassword no
```

If everything is ok, you will get the message in terminal like this:

```shell
Licensed under the Apache License, Version 2.0 (the " License ")
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an " AS IS " BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Welcome to CovalentBond.
**************************************************
[ info ] Backend listener start at port 8080.
[WFREST] GET    /table_at_graph
[WFREST] POST   /add_graph
[WFREST] POST   /add_device
[ info ] Get the ./index.html file. Start HTTP server at http://localhost:8888/index.html
[ info ] Press ^C to stop server.
[ info ] server successfully started.
```

### Add a device.

### Add a graph.

### How to write graph in lua.

### How to overload the default behavior of OPs.

## Conventional

Our implementation of dbo is highly rely on cpp and lua. The dev platform is Windows10/Ubuntu20.04(for server and client), Ubuntu20.04(for manager).

For cpp coding style, we provide a `.clang-format` file to reformat all cpp based codes. And all codes should pass the cpp-lint check. The compiler we used are `clang v12/10(on ubuntu20.04)`, `clang(GNU CLI) v14(on windows10)`. **Cpp std=17**. For each component, there must be a test module attach with it, ~~we adopt GTest here.~~,emmmm..., finally we just using the testing method provided by cmake.

For python code style, we follow the PEP8.

We use Git to manage our code.

Go to [DETAILS](./docs/prepare.md) for more information.

## Docs

* [Preparing-all-dependencies](./docs/prepare.md).
* [WIKI](https://github.com/chenghuaWang/covalentBond/wiki)
* [cppDoc](https://chenghuawang.github.io/covalentBond/doxygen/html/annotated.html)

## Dependencies

### Core

* [workflow](https://github.com/sogou/workflow) Sogou`s C++ server engine which can satisfy most C++ back-end development requirements.
* ~~[sRPC](https://github.com/sogou/srpc) An enterprise-level RPC system~~
* [gLog](https://github.com/google/glog) Google logging module
* [fmt](https://github.com/fmtlib/fmt) A modern formatting library
* ~~[protobuf](https://github.com/protocolbuffers/protobuf) Protocol Buffers - Google's data interchange format~~
* [redis](https://github.com/redis/redis) in-memory database that persists on disk.
* [rapidJson](https://github.com/Tencent/rapidjson) A fast JSON parser/generator for C++ with both SAX/DOM style API
* ~~[zeroMQ](https://github.com/zeromq/libzmq) attention👀, this lib may not be used. `workflow` provide a message queue already.~~
* [Zlib](https://github.com/madler/zlib)
* [LuaJit](https://github.com/LuaJIT/LuaJIT) Integrate in. For hot update.
* [lz4](https://github.com/lz4/lz4) Extremely fast compression
* [sol2](https://github.com/ThePhD/sol2) bind C++ and lua.

### Frontend

We used flutter SDK to develop the web and PC app.

## Install

1. Follow the steps in [prepare](./docs/prepare.md).
2. using the cmd in [install](https://github.com/chenghuaWang/covalentBond/wiki#%E5%A6%82%E4%BD%95%E5%AE%89%E8%A3%85)

## Members

### [chenghuaWang](https://github.com/chenghuaWang)

Hi there 👋

- 🖼️ I’m currently working on computer vision & (a little)computer graphics
- 🌱 I’m currently learning HPC/Computer Architecture/CG

![Wang's GitHub stats](https://github-readme-stats.vercel.app/api?username=chenghuaWang&bg_color=30,e96443,904e95&title_color=fff&text_color=fff)

### [caibo Feng](https://github.com/caibofeng)

- I'm also focus on  computer vision and learn with professor.
- I join in this project to learn from our leader and group.
- I like Jay Zhou and JJ Lin.

### [yunzi Sun](https://github.com/ketsurineko)
_On the internet , U can tell me ice_neko._

- I want to know lastest elctric and how to use it,such as PC.
- I am willing to learn everything i am interested in.
- I am new in computer vision and coding progarm. (really)

## Reference

## Thanks To All Contributors

<a href="https://github.com/chenghuaWang/covalentBond/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=chenghuaWang/covalentBond" />
</a>

Made with [contrib.rocks](https://contrib.rocks).

## License

This project is under Apache2.0 license.
