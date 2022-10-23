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

add_precompiled_header(cb-server ../pch.hpp)
add_executable(cb-server main.cpp)
target_link_libraries(cb-server ${WORKFLOW_LIB} ${SRPC_LIB})
if(GLOG_FOUND)
target_link_libraries(cb-server glog)
endif(GLOG_FOUND)