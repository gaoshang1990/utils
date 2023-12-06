# 复制头文件到指定目录
set(DST_DIR_B64 "${CMAKE_CURRENT_SOURCE_DIR}/inc/base64")
set(DST_DIR_INI "${CMAKE_CURRENT_SOURCE_DIR}/inc/iniparser")
set(DST_DIR_OSP "${CMAKE_CURRENT_SOURCE_DIR}/inc/OSPort")
set(DST_DIR_UTL "${CMAKE_CURRENT_SOURCE_DIR}/inc/utils")

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
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_DIR_UTL}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_B64} ${DST_DIR_B64}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_INI} ${DST_DIR_INI}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_OSP} ${DST_DIR_OSP}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_ALO} ${DST_DIR_UTL}
    COMMAND ${CMAKE_COMMAND} -E copy ${H_FILES_PLF} ${DST_DIR_UTL}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/win32/Release/${PROJECT_NAME}.lib ${CMAKE_CURRENT_SOURCE_DIR}/inc
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/linux/lib${PROJECT_NAME}.a        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# 添加自定义目标，将上述自定义命令作为依赖
add_custom_target(CopyTarget ALL DEPENDS ${PROJECT_NAME})
add_dependencies(CopyTarget ${PROJECT_NAME})
