set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARM)

if(MINGW OR CYGWIN OR WIN32)
    set(UTIL_SEARCH_CMD where)
elseif(UNIX OR APPLE)
    set(UTIL_SEARCH_CMD which)
endif()

set(TOOLCHAIN_PREFIX arm-linux-gnueabihf-)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

execute_process(
  COMMAND ${UTIL_SEARCH_CMD} ${TOOLCHAIN_PREFIX}gcc
  OUTPUT_VARIABLE BINUTILS_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)
# Without that flag CMake is not able to pass test compilation check
if (${CMAKE_VERSION} VERSION_EQUAL "3.6.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.6")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
else()
    set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs")
endif()

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}gcc-ar)
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}gcc-ranlib)

# skip compiler check
set (CMAKE_C_COMPILER_WORKS   1)
set (CMAKE_CXX_COMPILER_WORKS 1)

# Default C compiler flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-g3 -Og -Wall -pedantic -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG_INIT}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -Wall")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE_INIT}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "-Os -Wall")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL_INIT}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT  "-O2 -g -Wall")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "" FORCE)

# Default C++ compiler flags
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g3 -Og -Wall -pedantic -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG_INIT}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Os -Wall")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL_INIT}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT  "-O2 -g -Wall")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "" FORCE)

# set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
# set(CMAKE_SIZE_UTIL ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size tool")

set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)