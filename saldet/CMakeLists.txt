# Salient Object Detection
 
# Require
CMAKE_MINIMUM_REQUIRED (VERSION 2.8 FATAL_ERROR)
 
# Set your project name and version
PROJECT (SalDet)
SET(PROJECT_VERSION 0.0.1)
 
# Build flags for different configurations
# Use is determined by the option CMAKE_BUILD_TYPE
set(CMAKE_C_FLAGS_DEBUG "-g -Wall ")
set(CMAKE_C_FLAGS_RELEASE "-O2 ")
 
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall ")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 ")

# Required package
find_package(OpenCV REQUIRED)
 
# Add an uninstall target (for "make uninstall")
# CONFIGURE_FILE(
# "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
# "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
# IMMEDIATE @ONLY)
# ADD_CUSTOM_TARGET(uninstall
# "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
 
# Add subdirectories so those CMakeLists.txt files are parsed
ADD_SUBDIRECTORY (src)
# ADD_SUBDIRECTORY (doc)

