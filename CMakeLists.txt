# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.16)

set(PROJECT_NAME "ucl")
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
    auto_sources(C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/ucl")
    auto_sources(H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src/ucl")
endif()

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
    target_link_libraries(test ${LINK_LIB_FILES})
endif ()


# 复制头文件到指定目录
set(SRC_DIR_B64 "${CMAKE_CURRENT_SOURCE_DIR}/src/base64")
set(SRC_DIR_INI "${CMAKE_CURRENT_SOURCE_DIR}/src/iniparser")
set(SRC_DIR_OSP "${CMAKE_CURRENT_SOURCE_DIR}/src/OSPort")
set(SRC_DIR_UCL "${CMAKE_CURRENT_SOURCE_DIR}/src/ucl")

set(DST_DIR_B64 "${CMAKE_CURRENT_SOURCE_DIR}/inc/base64")
set(DST_DIR_INI "${CMAKE_CURRENT_SOURCE_DIR}/inc/iniparser")
set(DST_DIR_OSP "${CMAKE_CURRENT_SOURCE_DIR}/inc/OSPort")
set(DST_DIR_UCL "${CMAKE_CURRENT_SOURCE_DIR}/inc/ucl")

auto_sources(H_FILES_B64 "*.h" "RECURSE" ${SRC_DIR_B64})
auto_sources(H_FILES_INI "*.h" "RECURSE" ${SRC_DIR_INI})
auto_sources(H_FILES_OSP "*.h" "RECURSE" ${SRC_DIR_OSP})
auto_sources(H_FILES_UCL "*.h" "RECURSE" ${SRC_DIR_UCL})

execute_process(
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_B64}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_INI}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_OSP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_UCL}
    RESULT_VARIABLE result
)

if(${result} EQUAL "0")
    file(COPY ${H_FILES_B64} DESTINATION ${DST_DIR_B64})
    file(COPY ${H_FILES_INI} DESTINATION ${DST_DIR_INI})
    file(COPY ${H_FILES_OSP} DESTINATION ${DST_DIR_OSP})
    file(COPY ${H_FILES_UCL} DESTINATION ${DST_DIR_UCL})
endif()