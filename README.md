# Data Middle Office

Our Data Middle Office(dbo) is named covalent bond. A tool for managing distributed database, gathering/cleaning data, etc. This tool is a final project for 2022-2023 Fall Software Engineering lecture.

## Architecture

## Conventional

Our implementation of dbo is highly rely on cpp and python. The dev platform is Windows10/Ubuntu20.04(for server and client), Ubuntu20.04(for manager).

For cpp coding style, we provide a `.clang-format` file to reformat all cpp based codes. And all codes should pass the cpp-lint check. The compiler we used are `clang v12/10(on ubuntu20.04)`, `clang(GNU CLI) v14(on windows10)`. **Cpp std=17**. For each component, there must be a test module attach with it, we adopt GTest here.

For python code style, we follow the PEP8.

We use Git to manage our code.

Go to [DETAILS](./docs/prepare.md) for more information.

## Docs

* [Preparing-all-dependencies](./docs/prepare.md).

## Dependencies

### Core

* [workflow](https://github.com/sogou/workflow) Sogou`s C++ server engine which can satisfy most C++ back-end development requirements.
* [sRPC](https://github.com/sogou/srpc) An enterprise-level RPC system
* [gLog](https://github.com/google/glog) Google logging module
* [fmt](https://github.com/fmtlib/fmt) A modern formatting library
* [protobuf](https://github.com/protocolbuffers/protobuf) Protocol Buffers - Google's data interchange format
* [redis](https://github.com/redis/redis) in-memory database that persists on disk.
* [rapidJson](https://github.com/Tencent/rapidjson) A fast JSON parser/generator for C++ with both SAX/DOM style API
* [zeroMQ](https://github.com/zeromq/libzmq) attention👀, this lib may not be used. `workflow` provide a message queue already.
* [Zlib](https://github.com/madler/zlib)
* [LuaJit](https://github.com/LuaJIT/LuaJIT) Integrate in. For hot update.
* [lz4](https://github.com/lz4/lz4) Extremely fast compression
* [sol2](https://github.com/ThePhD/sol2) bind C++ and lua.

### Frontend

## Install

1. Follow the steps in [prepare](./docs/prepare.md).

## Members

### [chenghuaWang](https://github.com/chenghuaWang)

Hi there 👋

- 🖼️ I’m currently working on computer vision & (a little)computer graphics
- 🌱 I’m currently learning HPC/Computer Architecture/CG

![Wang's GitHub stats](https://github-readme-stats.vercel.app/api?username=chenghuaWang&bg_color=30,e96443,904e95&title_color=fff&text_color=fff)

### [caibo Feng](https://github.com/caibofeng)

Yahooo

## Reference

## License
