# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/zhangbin/env/bin/cmake

# The command to remove a file.
RM = /home/zhangbin/env/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zhangbin/binlog/event-router

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zhangbin/binlog/event-router/debug

# Utility rule file for NightlyMemCheck.

# Include the progress variables for this target.
include CMakeFiles/NightlyMemCheck.dir/progress.make

CMakeFiles/NightlyMemCheck:
	/home/zhangbin/env/bin/ctest -D NightlyMemCheck

NightlyMemCheck: CMakeFiles/NightlyMemCheck
NightlyMemCheck: CMakeFiles/NightlyMemCheck.dir/build.make
.PHONY : NightlyMemCheck

# Rule to build all files generated by this target.
CMakeFiles/NightlyMemCheck.dir/build: NightlyMemCheck
.PHONY : CMakeFiles/NightlyMemCheck.dir/build

CMakeFiles/NightlyMemCheck.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/NightlyMemCheck.dir/cmake_clean.cmake
.PHONY : CMakeFiles/NightlyMemCheck.dir/clean

CMakeFiles/NightlyMemCheck.dir/depend:
	cd /home/zhangbin/binlog/event-router/debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zhangbin/binlog/event-router /home/zhangbin/binlog/event-router /home/zhangbin/binlog/event-router/debug /home/zhangbin/binlog/event-router/debug /home/zhangbin/binlog/event-router/debug/CMakeFiles/NightlyMemCheck.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/NightlyMemCheck.dir/depend
