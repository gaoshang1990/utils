# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.16)

set(PROJECT_NAME "test_utils")
project(${PROJECT_NAME})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(functions)

message(STATUS "Find all source and header files")
auto_sources(C_FILES        "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/../src")
auto_sources(H_FILES        "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/../src")
auto_sources(TEST_C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}")
auto_sources(TEST_H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}")
auto_sources(TEST_CPP_FILES "*.cpp" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "Find all include directories")
find_include_dir(ALL_INC_DIRS "${H_FILES}") 
list(REMOVE_DUPLICATES ALL_INC_DIRS)
message(STATUS "Add include directories")
include_directories(${ALL_INC_DIRS})

if (MSVC)
    remove_matches_from_lists(C_FILES CPP_FILES H_FILES MATCHES "/linux")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    
    auto_sources(LIB_FILES "*.lib" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/../src")
    find_include_dir(LIB_FILES_PATHS "${LIB_FILES}")
    link_directories(${LIB_FILES_PATHS})
    set(LINK_LIB_FILES 
        legacy_stdio_definitions
        msvcrtd
    )
    set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:"MSVCRT.lib")

    add_executable(${PROJECT_NAME} ${TEST_C_FILES} ${C_FILES} ${TEST_CPP_FILES} ${CPP_FILES} ${H_FILES} ${TEST_H_FILES})
    target_link_libraries(${PROJECT_NAME} ${LINK_LIB_FILES})
endif ()
