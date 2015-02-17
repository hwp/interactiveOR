# - Try to find LibSVM
# Once done this will define
#  LIBSVM_FOUND - System has LibSVM
#  LIBSVM_INCLUDE_DIRS - The LibSVM include directories
#  LIBSVM_LIBRARIES - The libraries needed to use LibSVM
#  LIBSVM_DEFINITIONS - Compiler switches required for using LibSVM

find_package(PkgConfig)
pkg_check_modules(PC_LIBSVM QUIET libsvm)
set(LIBSVM_DEFINITIONS ${PC_LIBSVM_CFLAGS_OTHER})

find_path(LIBSVM_INCLUDE_DIR libsvm/svm.h
          HINTS ${PC_LIBSVM_INCLUDEDIR} ${PC_LIBSVM_INCLUDE_DIRS}
          PATH_SUFFIXES libsvm)

find_library(LIBSVM_LIBRARY NAMES svm libsvm
             HINTS ${PC_LIBSVM_LIBDIR} ${PC_LIBSVM_LIBRARY_DIRS})

set(LIBSVM_LIBRARIES ${LIBSVM_LIBRARY})
set(LIBSVM_INCLUDE_DIRS ${LIBSVM_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBSVM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibSVM DEFAULT_MSG
                                  LIBSVM_LIBRARY LIBSVM_INCLUDE_DIR)

mark_as_advanced(LIBSVM_INCLUDE_DIR LIBSVM_LIBRARY)

