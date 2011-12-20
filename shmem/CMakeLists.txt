configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

add_library(shmem STATIC shmem.cpp shmem.h config.h.in)

set_target_properties(shmem PROPERTIES
  PUBLIC_HEADER "shmem.h;${CMAKE_CURRENT_BINARY_DIR}/config.h")

install(TARGETS shmem
  # IMPORTANT: Add the shmem library to the "export-set"
  EXPORT shmemLibraryDepends
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/shmem"
    COMPONENT dev)
