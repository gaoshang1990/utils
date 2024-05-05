cmake_minimum_required (VERSION 3.16)

set(PROJECT_NAME "utils")
project(${PROJECT_NAME})

option(BUILD_FOR_HUAWEI_TERMINAL "Configure different compilers" OFF)
message(STATUS "BUILD_FOR_HUAWEI_TERMINAL: ${BUILD_FOR_HUAWEI_TERMINAL}")

option(USE_ENC "encode decode" OFF)
option(USE_INI "ini parser"    OFF)
option(USE_OSP "os port"       ON)
option(USE_UTL "utils"         ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(functions)

message(STATUS "Find all source and header files")
if(USE_ENC)
    auto_sources(C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/encoding")
    auto_sources(H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/encoding")
endif()
if(USE_INI)
    auto_sources(C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/iniparser")
    auto_sources(H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/iniparser")
endif()
if(USE_OSP)
    auto_sources(C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/OSPort")
    auto_sources(H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/OSPort")
endif()
if(USE_UTL)
    auto_sources(C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/utils")
    auto_sources(H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/utils")
endif()

if (MSVC)
    message(STATUS "Configuring for Win32")

    add_compile_options(/utf-8)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})           # 设置VS默认启动工程
    remove_matches_from_lists(C_FILES CPP_FILES H_FILES MATCHES "/linux")                                     # 删除不需要的文件
    auto_source_group(source ./ ${C_FILES} ${CPP_FILES} ${H_FILES} ${HPP_FILES}) # vs工程目录结构
    add_compile_options(/wd4819)                                                                              # 忽略4819告警
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -NODEFAULTLIB:MSVCRT -SAFESEH:NO")

    # 添加链接库
    auto_sources(LIB_FILES "*.lib" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/lib")
    list(APPEND LIB_FILES 
        legacy_stdio_definitions
        msvcrtd
    )
else()
    message(STATUS "Configuring for Linux")

    remove_matches_from_lists(C_FILES CPP_FILES H_FILES MATCHES "/win32") # 删除不需要的文件
    remove_matches_from_lists(C_FILES CPP_FILES H_FILES MATCHES "/pthread")

    # 添加链接库
    auto_sources(LIB_FILES "*.so" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/lib")
    auto_sources(LIB_FILES "*.a"  "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/lib")
    list(APPEND LIB_FILES 
        pthread
        dl
    )
endif(MSVC)

message(STATUS "find all lib directories")
find_include_dir(LIB_FILES_PATHS "${LIB_FILES}") # 查找所有库文件所在目录
list(REMOVE_DUPLICATES LIB_FILES_PATHS)          # 删除重复路径

message(STATUS "Find all include directories")
find_include_dir(ALL_INC_DIRS "${H_FILES}; ${HPP_FILES}")
list(APPEND ALL_INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/lib)
list(APPEND ALL_INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party)
list(REMOVE_DUPLICATES ALL_INC_DIRS)

# 指定生成目标
if(MSVC)
    add_library(${PROJECT_NAME} ${C_FILES} ${CPP_FILES} ${H_FILES})
else()
    add_library(${PROJECT_NAME} ${C_FILES} ${CPP_FILES} ${H_FILES})
endif()

# 添加链接库, 库文件目录, 头文件目录
# target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_FILES_PATHS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${ALL_INC_DIRS})

include(config_copy)
include(config_test)
