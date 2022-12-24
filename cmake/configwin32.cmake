# win32�µ�����
# �ڲ��ҵ����е�c�ļ���cpp�ļ���ͷ�ļ��󣬰�����CMakeLists.txt��

if (WIN32)
  message(STATUS "Configuring for Win32")

  # ����VSĬ����������
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

  # ɾ������Ҫ���ļ�
  remove_matches_from_lists(C_FILES CPP_FILES H_FILES MATCHES "/linux")

  # ������ӿ�
  auto_sources(LIB_FILES "*.lib" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src")
  find_include_dir(LIB_FILES_PATHS "${LIB_FILES}") # �������п��ļ�����Ŀ¼
  link_directories(${LIB_FILES_PATHS})
  set(LINK_LIB_FILES 
      legacy_stdio_definitions
  )

  set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:"MSVCRT.lib")

endif(WIN32)