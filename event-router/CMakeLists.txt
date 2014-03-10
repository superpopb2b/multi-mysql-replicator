
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(event-router)
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR -> ${CMAKE_CURRENT_SOURCE_DIR}")
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "DEBUG")
ENDIF()

SET(CXX_FLAGS
 -g
 -rdynamic
 -fPIC
 -Wall
 -Wextra
 # -Werror
 # -Woverloaded-virtual
 -Wpointer-arith
 -Wwrite-strings
 -Wno-unused-parameter
 -Wno-reorder
 # -Wno-unused-variable
 -DVALGRIND
 -D_FILE_OFFSET_BITS=64
 -DHAVE_STDINT_H
 -DHAVE_NETINET_IN_H
 )

IF(CMAKE_BUILD_BITS EQUAL 32)
  LIST(APPEND CXX_FLAGS "-m32")
ENDIF()

STRING(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -W -ggdb3 -fno-inline -fprofile-arcs -ftest-coverage")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")

# Boost
FIND_PACKAGE(Boost REQUIRED system thread)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../mysql-replication-listener-master/include")

FIND_LIBRARY(GLOG_LIBRARY NAMES libglog.a)
FIND_LIBRARY(GFLAGS_LIBRARY NAMES libgflags.a)
#FIND_LIBRARY(BOOST_SYSTEM_LIBRARY NAMES libboost_system.a)
#FIND_LIBRARY(BOOST_THREAD_LIBRARY NAMES libboost_thread.a)
FIND_LIBRARY(ZOOKEEPER_LIBRARY NAMES libzookeeper_mt.a)
FIND_LIBRARY(BINLOG names libreplication.so HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../mysql-replication-listener-master/lib")
MESSAGE(STATUS "Binlog -> ${BINLOG}")
MESSAGE(STATUS "BOOST_THREAD_LIBRARY -> ${BOOST_THREAD_LIBRARY}")
LINK_LIBRARIES(
  #${BOOST_SYSTEM_LIBRARY} 
  #${BOOST_THREAD_LIBRARY}
  ${GLOG_LIBRARY} ${GFLAGS_LIBRARY}
  ${ZOOKEEPER_LIBRARY}
  pthread
  pthread rt
  ${BINLOG}
)


# Must use google code style to address the include files


STRING(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
MESSAGE(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

SET(target_name event-router)

# Enable testing
IF(${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
  INCLUDE(CTest)
  INCLUDE(Dart)
  ENABLE_TESTING()

  FIND_PACKAGE(GTest REQUIRED)
  FIND_LIBRARY(GTEST_LIBRARY NAMES gtest)
  LINK_LIBRARIES(${GTEST_LIBRARY})

  SET(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --quiet --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=100 --verbose --demangle=yes")
ENDIF(${CMAKE_BUILD_TYPE} MATCHES "DEBUG")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/include/")
FILE(GLOB_RECURSE lib_srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/row_event_reader.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/src/row_event_reader_factory.cc")
ADD_LIBRARY("${target_name}" STATIC ${lib_srcs})

# server
FILE(GLOB_RECURSE sample "${CMAKE_CURRENT_SOURCE_DIR}/src/sample.cc")
ADD_EXECUTABLE("sample" ${sample})
TARGET_LINK_LIBRARIES("sample" "${target_name}")

# client
#FILE(GLOB_RECURSE client_srcs "${CMAKE_CURRENT_SOURCE_DIR}/client/*.cc")
#ADD_EXECUTABLE("client_${target_name}" ${client_srcs})
#TARGET_LINK_LIBRARIES(client_${target_name} "thrift_${target_name}")


