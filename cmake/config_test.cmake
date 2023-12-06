set(TEST_NAME "test_utils")

message(STATUS "Find all source and header files")
auto_sources(TEST_C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")
auto_sources(TEST_H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")
auto_sources(TEST_CPP_FILES "*.cpp" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")

include_directories(${TEST_H_FILES})

if (MSVC)
    add_executable(${TEST_NAME} 
        ${TEST_C_FILES} 
        ${TEST_H_FILES} 
        ${TEST_CPP_FILES} 
        ${C_FILES} 
        ${CPP_FILES} 
        ${H_FILES}
    )
    target_link_libraries(${TEST_NAME} 
        ${LINK_LIB_FILES}
    )
endif ()
