# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.16)

set(PROJECT_NAME "utils")
project(${PROJECT_NAME})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(functions)

message(STATUS "Find all source and header files")
if(USE_B64)
    auto_sources(C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/base64")
    auto_sources(H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/base64")
endif()
if(USE_INI)
    auto_sources(C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/INI")
    auto_sources(H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/INI")
endif()
if(USE_OSP)
    auto_sources(C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/OSPort")
    auto_sources(H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/OSPort")
endif()
if(USE_UCL)
    auto_sources(C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/utils")
    auto_sources(H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/utils")
endif()

include(config_win32)
include(config_linux)

find_include_dir(ALL_INC_DIRS "${H_FILES}") 
list(REMOVE_DUPLICATES ALL_INC_DIRS)
include_directories(${ALL_INC_DIRS})

# 指定生成目标
if(MSVC)
    add_library(${PROJECT_NAME} ${C_FILES} ${CPP_FILES} ${H_FILES})
else()
    add_library(${PROJECT_NAME} ${C_FILES} ${CPP_FILES} ${H_FILES})
endif()

# 添加链接库
target_link_libraries(${PROJECT_NAME} ${LINK_LIB_FILES})

include(config_copy)

include(config_test)
