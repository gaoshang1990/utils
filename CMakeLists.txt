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
    add_library(${PROJECT_NAME} ${C_FILES} ${CPP_FILES} ${H_FILES})
endif()

# 添加链接库
target_link_libraries(${PROJECT_NAME} ${LINK_LIB_FILES})


# 生成测试工程
if (MSVC)
    auto_sources(TEST_C_FILES   "*.c"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")
    auto_sources(TEST_H_FILES   "*.h"   "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")
    auto_sources(TEST_CPP_FILES "*.cpp" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/test")
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/win32/Debug)
    add_executable(test ${TEST_C_FILES} ${C_FILES} ${TEST_CPP_FILES} ${CPP_FILES} ${H_FILES} ${TEST_H_FILES})
    target_link_libraries(test ${LINK_LIB_FILES})
endif ()


# 复制头文件到指定目录
set(DST_DIR_B64 "${CMAKE_CURRENT_SOURCE_DIR}/inc/base64")
set(DST_DIR_INI "${CMAKE_CURRENT_SOURCE_DIR}/inc/iniparser")
set(DST_DIR_OSP "${CMAKE_CURRENT_SOURCE_DIR}/inc/OSPort")
set(DST_DIR_ALO "${CMAKE_CURRENT_SOURCE_DIR}/inc/utils/standalone")
set(DST_DIR_PLF "${CMAKE_CURRENT_SOURCE_DIR}/inc/utils/platform")

auto_sources(H_FILES_B64 "*.h" "RECURSE" ${CMAKE_CURRENT_SOURCE_DIR}/src/base64)
auto_sources(H_FILES_INI "*.h" "RECURSE" ${CMAKE_CURRENT_SOURCE_DIR}/src/iniparser)
auto_sources(H_FILES_OSP "*.h" "RECURSE" ${CMAKE_CURRENT_SOURCE_DIR}/src/OSPort)
auto_sources(H_FILES_ALO "*.h" "RECURSE" ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/standalone)
auto_sources(H_FILES_PLF "*.h" "RECURSE" ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/platform)

# 添加自定义命令，将目标文件复制到指定目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_B64}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_INI}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_OSP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_ALO}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_PLF}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_B64} ${DST_DIR_B64}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_INI} ${DST_DIR_INI}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_OSP} ${DST_DIR_OSP}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_ALO} ${DST_DIR_ALO}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_PLF} ${DST_DIR_PLF}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/win32/Release/${PROJECT_NAME}.lib ${CMAKE_CURRENT_SOURCE_DIR}/inc
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/mingw/lib${PROJECT_NAME}.a        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# 添加自定义目标，将上述自定义命令作为依赖
add_custom_target(CopyTarget ALL DEPENDS ${PROJECT_NAME})
add_dependencies(CopyTarget ${PROJECT_NAME} )
