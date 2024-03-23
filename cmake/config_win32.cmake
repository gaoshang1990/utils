# win32下的配置
# 在查找到所有的c文件、cpp文件、头文件后，包含在CMakeLists.txt中

if (MSVC)
    message(STATUS "Configuring for Win32")
    
    remove_matches_from_lists(C_FILES CPP_FILES H_FILES MATCHES "/linux")

    # 添加链接库
    auto_sources(LIB_FILES "*.lib" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src")
    find_include_dir(LIB_FILES_PATHS "${LIB_FILES}")
    link_directories(${LIB_FILES_PATHS})

    set(LINK_LIB_FILES 
        legacy_stdio_definitions
        msvcrtd
    )

    set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:"MSVCRT.lib")

    # 设置VS默认启动工程
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

    # vs工程目录结构
    auto_source_group(source ${CMAKE_CURRENT_SOURCE_DIR}/src ${C_FILES} ${CPP_FILES} ${H_FILES})

endif (MSVC)
