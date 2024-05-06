# 复制头文件到指定目录
set(DST_DIR_ENC "${CMAKE_CURRENT_SOURCE_DIR}/inc/encoding")
set(DST_DIR_INI "${CMAKE_CURRENT_SOURCE_DIR}/inc/iniparser")
set(DST_DIR_OSP "${CMAKE_CURRENT_SOURCE_DIR}/inc/OSPort")
set(DST_DIR_UTL "${CMAKE_CURRENT_SOURCE_DIR}/inc/utils")

auto_sources(H_FILES_ENC "*.h" "RECURSE" ${CMAKE_CURRENT_SOURCE_DIR}/src/encoding)
auto_sources(H_FILES_INI "*.h" "RECURSE" ${CMAKE_CURRENT_SOURCE_DIR}/src/iniparser)
auto_sources(H_FILES_OSP "*.h" "RECURSE" ${CMAKE_CURRENT_SOURCE_DIR}/src/OSPort)
auto_sources(H_FILES_UTL "*.h" "RECURSE" ${CMAKE_CURRENT_SOURCE_DIR}/src/utils)

# 添加自定义命令，将目标文件复制到指定目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_ENC}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_INI}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_OSP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_UTL}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_ENC} ${DST_DIR_ENC}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_INI} ${DST_DIR_INI}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_OSP} ${DST_DIR_OSP}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_UTL} ${DST_DIR_UTL}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/win32/Release/${PROJECT_NAME}.lib ${CMAKE_CURRENT_SOURCE_DIR}/inc
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/linux/lib${PROJECT_NAME}.a        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# 添加自定义目标，将上述自定义命令作为依赖
add_custom_target(CopyTarget ALL DEPENDS ${PROJECT_NAME})
add_dependencies(CopyTarget ${PROJECT_NAME})
