add_executable(test_shmem test_shmem.cpp)

target_link_libraries(test_shmem shmem)

install(TARGETS test_shmem
  # IMPORTANT: Add the test_shmem executable to the "export-set"
  EXPORT shmemLibraryDepends
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
