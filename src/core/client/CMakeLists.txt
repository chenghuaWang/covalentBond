#[[
@author     chenghua.wang
@startdate  Oct 22, 2022
@file       src/client/CMakeLists.txt
@brief      The main CMakeLists file for server com.
]]
cmake_minimum_required(VERSION 3.22)
project(
    cb-client
    VERSION 1.0
    DESCRIPTION "The client part of CB. Database"
    LANGUAGES C CXX
)

add_precompiled_header(cb-client ../pch.hpp)
add_executable(cb-client main.cpp ${UTILS_FILE_SRC} ${UTILS_FILE_HEAD})
target_link_libraries(cb-client ${WORKFLOW_LIB} ${SRPC_LIB})
if(GLOG_FOUND)
target_link_libraries(cb-client glog fmt::fmt ${LUAJIT_LIBRARY})
endif(GLOG_FOUND)