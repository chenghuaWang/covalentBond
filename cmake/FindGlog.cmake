macro(GLOG_RESET_FIND_LIBRARY_PREFIX)
  if (MSVC)
    set(CMAKE_FIND_LIBRARY_PREFIXES "${CALLERS_CMAKE_FIND_LIBRARY_PREFIXES}")
  endif (MSVC)
endmacro(GLOG_RESET_FIND_LIBRARY_PREFIX)

# Called if we failed to find glog or any of it's required dependencies,
# unsets all public (designed to be used externally) variables and reports
# error message at priority depending upon [REQUIRED/QUIET/<NONE>] argument.
macro(GLOG_REPORT_NOT_FOUND REASON_MSG)
  unset(GLOG_FOUND)
  unset(GLOG_INCLUDE_DIRS)
  unset(GLOG_LIBRARIES)
  # Make results of search visible in the CMake GUI if glog has not
  # been found so that user does not have to toggle to advanced view.
  mark_as_advanced(CLEAR GLOG_INCLUDE_DIR
                         GLOG_LIBRARY)

  glog_reset_find_library_prefix()

  # Note <package>_FIND_[REQUIRED/QUIETLY] variables defined by FindPackage()
  # use the camelcase library name, not uppercase.
  if (Glog_FIND_QUIETLY)
    message(STATUS "Failed to find glog - " ${REASON_MSG} ${ARGN})
  elseif (Glog_FIND_REQUIRED)
    message(FATAL_ERROR "Failed to find glog - " ${REASON_MSG} ${ARGN})
  else()
    # Neither QUIETLY nor REQUIRED, use no priority which emits a message
    # but continues configuration and allows generation.
    message("-- Failed to find glog - " ${REASON_MSG} ${ARGN})
  endif ()
endmacro(GLOG_REPORT_NOT_FOUND)

# Handle possible presence of lib prefix for libraries on MSVC, see
# also GLOG_RESET_FIND_LIBRARY_PREFIX().
if (MSVC)
  # Preserve the caller's original values for CMAKE_FIND_LIBRARY_PREFIXES
  # s/t we can set it back before returning.
  set(CALLERS_CMAKE_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES}")
  # The empty string in this list is important, it represents the case when
  # the libraries have no prefix (shared libraries / DLLs).
  set(CMAKE_FIND_LIBRARY_PREFIXES "lib" "" "${CMAKE_FIND_LIBRARY_PREFIXES}")
endif (MSVC)

# Search user-installed locations first, so that we prefer user installs
# to system installs where both exist.
list(APPEND GLOG_CHECK_INCLUDE_DIRS
  /usr/local/include
  /usr/local/homebrew/include # Mac OS X
  /opt/local/var/macports/software # Mac OS X.
  /opt/local/include
  /usr/include)
# Windows (for C:/Program Files prefix).
list(APPEND GLOG_CHECK_PATH_SUFFIXES
  glog/include
  glog/Include
  Glog/include
  Glog/Include)

list(APPEND GLOG_CHECK_LIBRARY_DIRS
  /usr/local/lib
  /usr/local/homebrew/lib # Mac OS X.
  /opt/local/lib
  /usr/lib)
# Windows (for C:/Program Files prefix).
list(APPEND GLOG_CHECK_LIBRARY_SUFFIXES
  glog/lib
  glog/Lib
  Glog/lib
  Glog/Lib)

# Search supplied hint directories first if supplied.
find_path(GLOG_INCLUDE_DIR
  NAMES glog/logging.h
  PATHS ${GLOG_INCLUDE_DIR_HINTS}
  ${GLOG_CHECK_INCLUDE_DIRS}
  PATH_SUFFIXES ${GLOG_CHECK_PATH_SUFFIXES})
if (NOT GLOG_INCLUDE_DIR OR
    NOT EXISTS ${GLOG_INCLUDE_DIR})
  glog_report_not_found(
    "Could not find glog include directory, set GLOG_INCLUDE_DIR "
    "to directory containing glog/logging.h")
endif (NOT GLOG_INCLUDE_DIR OR
       NOT EXISTS ${GLOG_INCLUDE_DIR})

find_library(GLOG_LIBRARY NAMES glog
  PATHS ${GLOG_LIBRARY_DIR_HINTS}
  ${GLOG_CHECK_LIBRARY_DIRS}
  PATH_SUFFIXES ${GLOG_CHECK_LIBRARY_SUFFIXES})
if (NOT GLOG_LIBRARY OR
    NOT EXISTS ${GLOG_LIBRARY})
  glog_report_not_found(
    "Could not find glog library, set GLOG_LIBRARY "
    "to full path to libglog.")
endif (NOT GLOG_LIBRARY OR
       NOT EXISTS ${GLOG_LIBRARY})

# Mark internally as found, then verify. GLOG_REPORT_NOT_FOUND() unsets
# if called.
set(GLOG_FOUND TRUE)

# Glog does not seem to provide any record of the version in its
# source tree, thus cannot extract version.

# Catch case when caller has set GLOG_INCLUDE_DIR in the cache / GUI and
# thus FIND_[PATH/LIBRARY] are not called, but specified locations are
# invalid, otherwise we would report the library as found.
if (GLOG_INCLUDE_DIR AND
    NOT EXISTS ${GLOG_INCLUDE_DIR}/glog/logging.h)
  glog_report_not_found(
    "Caller defined GLOG_INCLUDE_DIR:"
    " ${GLOG_INCLUDE_DIR} does not contain glog/logging.h header.")
endif (GLOG_INCLUDE_DIR AND
       NOT EXISTS ${GLOG_INCLUDE_DIR}/glog/logging.h)
string(TOLOWER "${GLOG_LIBRARY}" LOWERCASE_GLOG_LIBRARY)
if (GLOG_LIBRARY AND
    NOT "${LOWERCASE_GLOG_LIBRARY}" MATCHES ".*glog[^/]*")
  glog_report_not_found(
    "Caller defined GLOG_LIBRARY: "
    "${GLOG_LIBRARY} does not match glog.")
endif (GLOG_LIBRARY AND
       NOT "${LOWERCASE_GLOG_LIBRARY}" MATCHES ".*glog[^/]*")

# Set standard CMake FindPackage variables if found.
if (GLOG_FOUND)
  set(GLOG_INCLUDE_DIRS ${GLOG_INCLUDE_DIR})
  set(GLOG_LIBRARIES ${GLOG_LIBRARY})
endif (GLOG_FOUND)

glog_reset_find_library_prefix()

# Handle REQUIRED / QUIET optional arguments.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Glog DEFAULT_MSG
  GLOG_INCLUDE_DIRS GLOG_LIBRARIES)

# Only mark internal variables as advanced if we found glog, otherwise
# leave them visible in the standard GUI for the user to set manually.
if (GLOG_FOUND)
  mark_as_advanced(FORCE GLOG_INCLUDE_DIR
                         GLOG_LIBRARY)
endif (GLOG_FOUND)