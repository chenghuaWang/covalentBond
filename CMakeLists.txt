#[[
@author     chenghua.wang
@startdate  Oct 22, 2022
@file       CMakeLists.txt
@brief      The main CMakeLists file for organizing all project.
@details    Some options are provided when you want to rebuild this
project, the options are shown below:
1. 
]]
cmake_minimum_required(VERSION 3.22)
project(
    covalent-bound
    VERSION 1.0
)

set(CMAKE_INSTALL_MESSAGE LAZY CACHE STRING "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "")

option(USE_CLANG_FORMAT "" ON)
option(USE_CLANG_TIDY "" ON)
option(BUILD_TESTING "" OFF)
option(BUILD_CO_DOC "" OFF)

if(BUILD_CO_DOC)
include(cmake/build_doxygen.cmake)
build_doxygen()
endif()

# Reference:
# https://medium.com/@alasher/colored-c-compiler-output-with-ninja-clang-gcc-10bfe7f2b949
option(CB_FORCE_COLORED_DIAGNOSTICS "Always produce ANSI-colored diagnostics (GNU/Clang only)." ON)

set(CPU_THREADING_RUNTIME "OMP")

set(COMPILER_VERSION_ERROR_MSG "At least gcc 9, clang 12 supported.")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 9)
    message(FATAL_ERROR ${COMPILER_VERSION_ERROR_MSG})
  endif()
  if(CPU_THREADING_RUNTIME STREQUAL "OMP")
    set(OMP_FLAGS "-fopenmp")
  endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # Reference:
  # https://releases.llvm.org/11.0.0/tools/clang/docs/OpenMPSupport.html
  if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 11)
    if(CPU_THREADING_RUNTIME STREQUAL "OMP")
      message(
        FATAL_ERROR
          "libopenmp is not supported under clang10, please use TBB with '-DCPU_THREADING_RUNTIME=TBB'."
      )
    else()
        set(OMP_FLAGS "-openmp")
    endif()
  endif()
  if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 5)
    message(FATAL_ERROR ${COMPILER_VERSION_ERROR_MSG})
  endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 12)
    message(FATAL_ERROR ${COMPILER_VERSION_ERROR_MSG})
  endif()
else()
  message(WARNING "Unknown compiler \"${CMAKE_CXX_COMPILER_ID}\".")
endif()

set(CB_CMAKE_DIR ${PROJECT_SOURCE_DIR}/cmake)

if(CB_FORCE_COLORED_DIAGNOSTICS)
  add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>>
    $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>>
    $<$<COMPILE_LANGUAGE:CUDA>:$<$<CUDA_COMPILER_ID:Clang>:-fcolor-diagnostics>>)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "CMAKE_CXX_COMPILER_VERSION: " ${CMAKE_CXX_COMPILER_VERSION})

# prepare the related libs.

if (WIN32)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /MP /wd4200")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd4200 /std:c++14")
else ()
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -fPIC -pipe -std=gnu90")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -pipe -std=c++17 -fno-exceptions")
endif ()

# workflow
find_library(LIBRT rt)
find_package(OpenSSL REQUIRED)
find_package(workflow REQUIRED CONFIG HINTS ..)
include_directories(${OPENSSL_INCLUDE_DIR} ${WORKFLOW_INCLUDE_DIR})
link_directories(${WORKFLOW_LIB_DIR})
set(WORKFLOW_LIB workflow pthread OpenSSL::SSL OpenSSL::Crypto ${LIBRT})

# glog
include(${CB_CMAKE_DIR}/FindGlog.cmake)

# fmt
find_package(fmt)

# luaJit51
include(${CB_CMAKE_DIR}/FindLuaJit.cmake)
include_directories(${LUAJIT_INCLUDE_DIR})

# wfrest
find_package(wfrest REQUIRED CONFIG HINTS ..)
include_directories(${WFREST_INCLUDE_DIR})
if (APPLE)
	set(WFREST_LIB wfrest workflow pthread OpenSSL::SSL OpenSSL::Crypto protobuf z)
else ()
	set(WFREST_LIB wfrest)
endif ()

# srpc
find_package(srpc REQUIRED CONFIG HINTS ..)
set(protobuf_MODULE_COMPATIBLE ON CACHE BOOL "")
if (WIN32)
	find_package(Protobuf CONFIG REQUIRED)
	find_library(LZ4_LIBRARY NAMES lz4)
	find_package(Snappy CONFIG REQUIRED)
else ()
	find_package(Protobuf REQUIRED)
endif ()
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/third_party/lz4/lib/lz4.h")
	set(LZ4_LIB lz4)
endif ()
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/third_party/snappy/cmake")
	set(SNAPPY_LIB snappy)
endif ()
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/workflow/workflow-config.cmake.in")
	find_package(Workflow REQUIRED CONFIG HINTS ../workflow)
endif ()
find_package(ZLIB REQUIRED)
if (WIN32)
	link_directories(${SRPC_LIB_DIR} ${WORKFLOW_LIB_DIR} ${Protobuf_LIB_DIR})
	set(SRPC_GEN_PROGRAM ${SRPC_BIN_DIR}/Debug/srpc_generator.exe)
else ()
	get_filename_component(Protobuf_LIB_DIR ${Protobuf_LIBRARY} DIRECTORY)
	link_directories(${SRPC_LIB_DIR} ${WORKFLOW_LIB_DIR} ${Protobuf_LIB_DIR})
	set(SRPC_GEN_PROGRAM ${SRPC_BIN_DIR}/srpc_generator)
endif ()
if (APPLE)
	set(SRPC_LIB
		srpc
		workflow
		pthread
		OpenSSL::SSL
		OpenSSL::Crypto
		protobuf
		z
		)
elseif (WIN32)
	set(SRPC_LIB
		srpc
		workflow
		ws2_32
		wsock32
		OpenSSL::SSL
		OpenSSL::Crypto
		protobuf::libprotobuf
		ZLIB::ZLIB
		Snappy::snappy
		${LZ4_LIBRARY}
		)
else ()
	set(SRPC_LIB
		srpc
		workflow
		pthread
		OpenSSL::SSL
		OpenSSL::Crypto
		protobuf
		z
		${SNAPPY_LIB}
		${LZ4_LIB}
		)
endif ()

# set the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Processing the pre-compiled head.
include_directories(${PROJECT_SOURCE_DIR}/third_party/)
include_directories(${GLOG_INCLUDE_DIR})
add_library(cb-pch src/core/pch.cpp src/core/pch.hpp)
if(GLOG_FOUND)
target_link_libraries(cb-pch glog)
endif(GLOG_FOUND)
include(${CB_CMAKE_DIR}/pre_compiled_head.cmake)

# prepare the files from public files' directories
file(GLOB_RECURSE UTILS_FILE_SRC ${PROJECT_SOURCE_DIR}/src/core/utils/*.cpp)
file(GLOB_RECURSE UTILS_FILE_HEAD ${PROJECT_SOURCE_DIR}/src/core/utils/*.hpp)

add_subdirectory(src/core/server)
add_subdirectory(src/core/client)

# Add CTest.
enable_testing()
add_subdirectory(test)