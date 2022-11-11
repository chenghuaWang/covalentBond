project(lru_test)

include_directories(../../src/core/server/mem)

add_precompiled_header(lru_test ../../src/core/pch.hpp)
add_executable(lru_test main.cpp ../../src/core/server/mem/cbLRUCache.hpp ../../src/core/server/mem/cbLRUCache.cpp)
# target_link_libraries(lru_test  ${WORKFLOW_LIB})
add_test(NAME lru_test COMMAND lru_test)