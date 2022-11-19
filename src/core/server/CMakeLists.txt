#[[
@author     chenghua.wang
@startdate  Oct 22, 2022
@file       src/server/CMakeLists.txt
@brief      The main CMakeLists file for server com.
]]
cmake_minimum_required(VERSION 3.22)
project(
    cb-server
    VERSION 1.0
    DESCRIPTION "The server part of CB. Database"
    LANGUAGES C CXX
)

set(TASK_FILES
./api.cpp
./api.hpp
./pipeline.cpp
./pipeline.hpp
./task/luaEngine.cpp
./task/luaEngine.hpp
./task/cbTable.cpp
./task/cbTable.cpp
./task/cbTable.cpp
./task/cbOperator.cpp
./task/cbOperator.hpp
./task/cbComputeGraph.cpp
./task/cbComputeGraph.hpp
./trivial/cfgIO.cpp
./trivial/cfgIO.hpp
./trivial/cbVirtualDevice.cpp
./trivial/cbVirtualDevice.hpp
)

include_directories(../utils)
include_directories(.)

add_precompiled_header(cb-server ../pch.hpp)
add_executable(cb-server main.cpp 
liveThread.hpp
liveThread.cpp
${UTILS_FILE_SRC} 
${UTILS_FILE_HEAD} 
${TASK_FILES})
target_link_libraries(cb-server ${WORKFLOW_LIB} ${SRPC_LIB} ${WFREST_LIB})
if(GLOG_FOUND)
target_link_libraries(cb-server glog fmt::fmt ${LUAJIT_LIBRARY} ${WORKFLOW_LIB} ${SRPC_LIB} ${WFREST_LIB})
endif(GLOG_FOUND)