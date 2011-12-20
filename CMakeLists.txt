cmake_minimum_required(VERSION 2.8)
project(shmem CXX)

set(SHMEM_MAJOR_VERSION 0)
set(SHMEM_MINOR_VERSION 1)
set(SHMEM_PATCH_VERSION 0)
set(SHMEM_VERSION
  ${SHMEM_MAJOR_VERSION}.${SHMEM_MINOR_VERSION}.${SHMEM_PATCH_VERSION})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/shmem)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# set up include-directories
include_directories(
  "${PROJECT_SOURCE_DIR}"   # to find shmem/shmem.h
  "${PROJECT_BINARY_DIR}")  # to find shmem/config.h

# Add sub-directories
add_subdirectory(shmem)
add_subdirectory(test_shmem)

# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
export(TARGETS shmem test_shmem
  FILE "${PROJECT_BINARY_DIR}/shmemLibraryDepends.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE shmem)

# Create a shmemBuildTreeSettings.cmake file for the use from the build tree
configure_file(shmemBuildTreeSettings.cmake.in
  "${PROJECT_BINARY_DIR}/shmemBuildTreeSettings.cmake" @ONLY)

# Create the shmemConfig.cmake and shmemConfigVersion files
file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
configure_file(shmemConfig.cmake.in
  "${PROJECT_BINARY_DIR}/shmemConfig.cmake" @ONLY)
configure_file(shmemConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/shmemConfigVersion.cmake" @ONLY)

# Install the export set for use with the install-tree
install(EXPORT shmemLibraryDepends DESTINATION
  "${INSTALL_BIN_DIR}/shmem/CMake"
  COMPONENT dev)
