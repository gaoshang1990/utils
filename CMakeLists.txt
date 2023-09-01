# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.16)

set(PROJECT_NAME "ucl")
project(${PROJECT_NAME})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(functions)

message(STATUS "Find all source and header files")
auto_sources(C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src")
auto_sources(CPP_FILES "*.cpp" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src")
auto_sources(H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src")
include(configwin32)
include(configlinux)

message(STATUS "Find all include directories")
find_include_dir(ALL_INC_DIRS "${H_FILES}") 
list(REMOVE_DUPLICATES ALL_INC_DIRS)

message(STATUS "Add include directories")
include_directories(${ALL_INC_DIRS})

# 指定生成目标
if(MSVC)
    add_library(${PROJECT_NAME} ${C_FILES} ${CPP_FILES} ${H_FILES})
else()
    add_library(${PROJECT_NAME} SHARED ${C_FILES} ${CPP_FILES} ${H_FILES})
endif()

# 添加链接库
target_link_libraries(${PROJECT_NAME} ${LINK_LIB_FILES})


# 生成测试工程
if (MSVC)
    auto_sources(TEST_C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")
    auto_sources(TEST_H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")
    auto_sources(TEST_CPP_FILES "*.cpp" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")
    # 添加链接库
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/win32/Debug)
    # set(TEST_LINK_LIB_FILE ucl.lib)
    add_executable(test ${TEST_C_FILES} ${C_FILES} ${TEST_CPP_FILES} ${CPP_FILES} ${H_FILES} ${TEST_H_FILES})
    # target_link_libraries(test ${TEST_LINK_LIB_FILE})
endif ()