# linux下的配置
# 在查找到所有的c文件、cpp文件、头文件后，包含在CMakeLists.txt中

if (NOT MSVC)
  message(STATUS "Configuring for Linux")

  # 删除不需要的文件
  remove_matches_from_lists(C_FILES CPP_FILES H_FILES MATCHES "/win32")
  remove_matches_from_lists(C_FILES CPP_FILES H_FILES MATCHES "/pthread")

  # 添加链接库
  auto_sources(LIB_FILES "*.so" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src")
  find_include_dir(LIB_FILES_PATHS "${LIB_FILES}") # 查找所有库文件所在目录
  link_directories(${LIB_FILES_PATHS})
  set(LINK_LIB_FILES 
      pthread
  )

endif()