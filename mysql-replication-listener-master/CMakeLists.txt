project (mysql-5.6-labs-binary-log-api)
cmake_minimum_required (VERSION 2.6)

set(MySQL_BINLOG_VERSION_MAJOR "0")
set(MySQL_BINLOG_VERSION_MINOR "0.1")
set(MRL_VERSION "${MySQL_BINLOG_VERSION_MAJOR}.${MySQL_BINLOG_VERSION_MINOR}")

set(CMAKE_VERSION_STRING "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
set(CMAKE_CXX_FLAGS -g ${CMAKE_CXX_FLAGS})

# Options for building
option(WITH_SERVER_TESTS
  "Build the unit test suite with tests requiring a server"
  OFF)

# GTest download variables
set(GTEST_VERSION "1.5.0")
set(GTEST_PACKAGE_NAME "gtest-${GTEST_VERSION}")
set(GTEST_TARBALL "${GTEST_PACKAGE_NAME}.tar.gz")
set(GTEST_DOWNLOAD_URL "http://googletest.googlecode.com/files/${GTEST_TARBALL}")
if(NOT DOWNLOAD_ROOT)
  set(DOWNLOAD_ROOT ${CMAKE_SOURCE_DIR}/source_downloads)
endif()
set(GTEST_SOURCE_DIR ${DOWNLOAD_ROOT}/${GTEST_PACKAGE_NAME})

# General settings
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
include_directories(include)
link_directories(${PROJECT_BINARY_DIR}/lib)

# ---------- Find Boost Headers/Libraries -----------------------
SET(Boost_DEBUG FALSE)
SET(Boost_FIND_REQUIRED TRUE)
SET(Boost_FIND_QUIETLY TRUE)
SET(Boost_USE_STATIC_LIBS FALSE)
SET(Boost_ADDITIONAL_VERSIONS "1.41" "1.41.0")
FIND_PACKAGE(Boost REQUIRED system thread)

# --------- Find crypt
FIND_LIBRARY(LIB_CRYPTO crypto /opt/local/lib /opt/lib /usr/lib /usr/local/lib)
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
MESSAGE(STATUS "Boost Library_dirs --> ${Boost_LIBRARY_DIRS}")
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

# Locate Google Test package and enable tests if it is found
find_package(GTest ${GTEST_VERSION} QUIET)

if (NOT GTEST_FOUND)
  if (NOT ENABLE_DOWNLOADS)
    # Give one-time warning
    if (NOT ONETIME_GTEST_WARNING)
      message(STATUS 
        "Googletest was not found. gtest-based unit tests will be disabled. "
        "You can run cmake . -DENABLE_DOWNLOADS=1 to automatically download and "
        "build required components from source.")
      SET(ONETIME_GTEST_WARNING 1 CACHE INTERNAL "")
    endif (NOT ONETIME_GTEST_WARNING)
  else (NOT ENABLE_DOWNLOADS)
    # Download gtest source
    if (NOT EXISTS ${GTEST_SOURCE_DIR} AND
        NOT EXISTS ${DOWNLOAD_ROOT}/${GTEST_TARBALL})
      if (${CMAKE_VERSION_STRING} LESS "2.8")
        # In versions earlier than 2.8, try wget for downloading
        find_program(WGET_EXECUTABLE wget)
        mark_as_advanced(WGET_EXECUTABLE)
        if (WGET_EXECUTABLE)
          if (NOT EXISTS ${DOWNLOAD_ROOT})
            make_directory(${DOWNLOAD_ROOT})
          endif (NOT EXISTS ${DOWNLOAD_ROOT})
          execute_process(COMMAND ${WGET_EXECUTABLE} -T 30 ${GTEST_DOWNLOAD_URL}
            WORKING_DIRECTORY ${DOWNLOAD_ROOT} RESULT_VARIABLE ERR)
          if (ERR EQUAL 0)
            SET(DOWNLOAD_SUCCEEDED 1)
          endif (ERR EQUAL 0)
        endif (WGET_EXECUTABLE)
      else (${CMAKE_VERSION_STRING} LESS "2.8")
        # Use CMake builtin download capabilities
        file(DOWNLOAD ${GTEST_DOWNLOAD_URL} ${DOWNLOAD_ROOT}/${GTEST_TARBALL}
          TIMEOUT 30
          STATUS ERR)
        if (ERR EQUAL 0)
          SET(DOWNLOAD_SUCCEEDED 1)
        endif (ERR EQUAL 0)
      endif(${CMAKE_VERSION_STRING} LESS "2.8")

      if (NOT DOWNLOAD_SUCCEEDED)
        message(STATUS 
          "To enable google test, please download ${GTEST_DOWNLOAD_URL} "
          "to the directory ${DOWNLOAD_ROOT}")
      else (NOT DOWNLOAD_SUCCEEDED)
        message(STATUS 
          "Successfully downloaded ${GTEST_DOWNLOAD_URL} to ${DOWNLOAD_ROOT}")
        # Unpack tarball
        execute_process (
          COMMAND ${CMAKE_COMMAND} -E tar xfz  "${DOWNLOAD_ROOT}/${GTEST_TARBALL}"
          WORKING_DIRECTORY "${DOWNLOAD_ROOT}"
          OUTPUT_QUIET
          ERROR_QUIET
          )
        set(GTEST_DOWNLOADED 1 CACHE INTERNAL "")
        set(GTEST_FOUND 1 CACHE INTERNAL "")
      endif (NOT DOWNLOAD_SUCCEEDED)
    else(NOT EXISTS ${GTEST_SOURCE_DIR} AND NOT EXISTS ${DOWNLOAD_ROOT}/${GTEST_TARBALL})
      set(GTEST_DOWNLOADED 1 CACHE INTERNAL "")
      set(GTEST_FOUND 1 CACHE INTERNAL "")
    endif(NOT EXISTS ${GTEST_SOURCE_DIR} AND NOT EXISTS ${DOWNLOAD_ROOT}/${GTEST_TARBALL})
  endif (NOT ENABLE_DOWNLOADS)
endif (NOT GTEST_FOUND)

if (GTEST_DOWNLOADED)
  # Build gtest library
  include_directories(
    ${GTEST_SOURCE_DIR} 
    ${GTEST_SOURCE_DIR}/include
  )
  add_library(gtest STATIC ${GTEST_SOURCE_DIR}/src/gtest-all.cc)
  
  # Set CMake variables to make FindPackage(GTest) happy next time.
  SET(GTEST_FOUND 1 CACHE INTERNAL "")
  SET(GTEST_LIBRARY gtest CACHE INTERNAL "")
  SET(GTEST_LIBRARIES gtest CACHE INTERNAL "")
  SET(GTEST_MAIN_LIBRARY no_gtest_main_library CACHE INTERNAL "")
  SET(GTEST_INCLUDE_DIRS ${GTEST_SOURCE_DIR}/include CACHE INTERNAL "")
  SET(GTEST_INCLUDE_DIR "${GTEST_SOURCE_DIR}/include" CACHE INTERNAL "")
endif (GTEST_DOWNLOADED)

if(GTEST_FOUND)
  message(STATUS "Tests from subdirectory 'tests' added")
  enable_testing(true)
  include_directories(${GTEST_INCLUDE_DIRS})
  add_subdirectory(tests)
endif(GTEST_FOUND)

add_subdirectory(src)

# -- Build the examples
add_subdirectory(examples EXCLUDE_FROM_ALL)

# Configure installation
install(DIRECTORY include DESTINATION . FILES_MATCHING PATTERN "*.h")

include(InstallRequiredSystemLibraries)

# Configure packaging
SET(CPACK_PACKAGE_NAME "mysql-5.6-labs-binary-log-api")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MySQL_BINLOG_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MySQL_BINLOG_VERSION_MINOR}")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "mysql-5.6-labs-binary-log-api: a MySQL client library for interfacing with the binary log mechanism.")
 
SET(CPACK_GENERATOR "STGZ;TGZ;TZ;DEB;RPM")
 
# Get package name correctly formatted with name, version, and platform
execute_process(COMMAND uname -m OUTPUT_VARIABLE SYSTEM_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
SET(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}.${MRL_VERSION}.${CMAKE_SYSTEM_NAME}.${SYSTEM_ARCH}")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Oracle Corporation")

include(CPack)
