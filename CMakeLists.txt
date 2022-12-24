# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.16)

# 项目信息
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
# message(STATUS "ALL_INC_DIRS: ${ALL_INC_DIRS}")

# vs工程目录结构
auto_source_group(source ${CMAKE_CURRENT_SOURCE_DIR}/src ${C_FILES} ${CPP_FILES} ${H_FILES})

# 添加头文件
include_directories(${ALL_INC_DIRS})

# 指定生成目标
add_executable(${PROJECT_NAME} ${C_FILES} ${CPP_FILES} ${H_FILES})

# 添加链接库
target_link_libraries(${PROJECT_NAME} ${LINK_LIB_FILES})
