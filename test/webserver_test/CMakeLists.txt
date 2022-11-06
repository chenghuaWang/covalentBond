project(wbs_test)

include_directories(../../src/core/utils)

add_precompiled_header(wbs_test ../../src/core/pch.hpp)
add_executable(wbs_test main.cpp ${UTILS_FILE_SRC} ${UTILS_FILE_HEAD})
target_link_libraries(wbs_test  ${WORKFLOW_LIB})
add_test(NAME wbs_test COMMAND wbs_test)