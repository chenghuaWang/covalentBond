project(util_test)

include_directories(../../src/core/utils)

add_precompiled_header(util_test ../../src/core/pch.hpp)
add_executable(util_test main.cpp ${UTILS_FILE_SRC} ${UTILS_FILE_HEAD})
add_test(NAME util_test COMMAND util_test)