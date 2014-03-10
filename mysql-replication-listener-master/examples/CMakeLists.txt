project(examples)

link_directories(../lib)
include_directories(../include)

# Create build rules for all the simple examples that only require a
# single file.
foreach(prog basic-3)
  ADD_EXECUTABLE(${prog} ${prog}.cpp)
  TARGET_LINK_LIBRARIES(${prog} replication)
endforeach()

#add_subdirectory(mysql2lucene EXCLUDE_FROM_ALL)
