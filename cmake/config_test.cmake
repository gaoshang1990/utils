set(TEST_NAME "test_utils")

auto_sources(TEST_C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")
auto_sources(TEST_H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")
auto_sources(TEST_CPP_FILES "*.cpp" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")

find_include_dir(TEST_INC_DIRS "${TEST_H_FILES}") 
list(REMOVE_DUPLICATES TEST_INC_DIRS)
include_directories(${TEST_INC_DIRS})

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
        ${LIB_FILES}
    )

    target_include_directories(${TEST_NAME}  PRIVATE
        ${TEST_INC_DIRS}
        ${ALL_INC_DIRS}
    )
endif ()
