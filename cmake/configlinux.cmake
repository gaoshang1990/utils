# linux�µ�����
# �ڲ��ҵ����е�c�ļ���cpp�ļ���ͷ�ļ��󣬰�����CMakeLists.txt��

if (NOT MSVC)
  message(STATUS "Configuring for Linux")

  # ɾ������Ҫ���ļ�
  remove_matches_from_lists(C_FILES CPP_FILES H_FILES MATCHES "/win32")
  remove_matches_from_lists(C_FILES CPP_FILES H_FILES MATCHES "/pthread")

  # ������ӿ�
  auto_sources(LIB_FILES "*.so" "RECURSE" "${CMAKE_CURRENT_SOURCE_DIR}/src")
  find_include_dir(LIB_FILES_PATHS "${LIB_FILES}") # �������п��ļ�����Ŀ¼
  link_directories(${LIB_FILES_PATHS})
  set(LINK_LIB_FILES 
      pthread
  )

endif()