# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/quake/Projects/TStk/stk

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/quake/Projects/TStk/stk/build

# Include any dependencies generated for this target.
include projects/examples/CMakeFiles/threebees.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include projects/examples/CMakeFiles/threebees.dir/compiler_depend.make

# Include the progress variables for this target.
include projects/examples/CMakeFiles/threebees.dir/progress.make

# Include the compile flags for this target's objects.
include projects/examples/CMakeFiles/threebees.dir/flags.make

projects/examples/CMakeFiles/threebees.dir/threebees.cpp.o: projects/examples/CMakeFiles/threebees.dir/flags.make
projects/examples/CMakeFiles/threebees.dir/threebees.cpp.o: /home/quake/Projects/TStk/stk/projects/examples/threebees.cpp
projects/examples/CMakeFiles/threebees.dir/threebees.cpp.o: projects/examples/CMakeFiles/threebees.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/TStk/stk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object projects/examples/CMakeFiles/threebees.dir/threebees.cpp.o"
	cd /home/quake/Projects/TStk/stk/build/projects/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT projects/examples/CMakeFiles/threebees.dir/threebees.cpp.o -MF CMakeFiles/threebees.dir/threebees.cpp.o.d -o CMakeFiles/threebees.dir/threebees.cpp.o -c /home/quake/Projects/TStk/stk/projects/examples/threebees.cpp

projects/examples/CMakeFiles/threebees.dir/threebees.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/threebees.dir/threebees.cpp.i"
	cd /home/quake/Projects/TStk/stk/build/projects/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/quake/Projects/TStk/stk/projects/examples/threebees.cpp > CMakeFiles/threebees.dir/threebees.cpp.i

projects/examples/CMakeFiles/threebees.dir/threebees.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/threebees.dir/threebees.cpp.s"
	cd /home/quake/Projects/TStk/stk/build/projects/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/quake/Projects/TStk/stk/projects/examples/threebees.cpp -o CMakeFiles/threebees.dir/threebees.cpp.s

# Object files for target threebees
threebees_OBJECTS = \
"CMakeFiles/threebees.dir/threebees.cpp.o"

# External object files for target threebees
threebees_EXTERNAL_OBJECTS =

/home/quake/Projects/TStk/stk/projects/examples/threebees: projects/examples/CMakeFiles/threebees.dir/threebees.cpp.o
/home/quake/Projects/TStk/stk/projects/examples/threebees: projects/examples/CMakeFiles/threebees.dir/build.make
/home/quake/Projects/TStk/stk/projects/examples/threebees: /usr/lib/x86_64-linux-gnu/libjack.so
/home/quake/Projects/TStk/stk/projects/examples/threebees: /usr/lib/x86_64-linux-gnu/libasound.so
/home/quake/Projects/TStk/stk/projects/examples/threebees: libstk.a
/home/quake/Projects/TStk/stk/projects/examples/threebees: /usr/lib/x86_64-linux-gnu/libjack.so
/home/quake/Projects/TStk/stk/projects/examples/threebees: /usr/lib/x86_64-linux-gnu/libasound.so
/home/quake/Projects/TStk/stk/projects/examples/threebees: projects/examples/CMakeFiles/threebees.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/quake/Projects/TStk/stk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/quake/Projects/TStk/stk/projects/examples/threebees"
	cd /home/quake/Projects/TStk/stk/build/projects/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/threebees.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
projects/examples/CMakeFiles/threebees.dir/build: /home/quake/Projects/TStk/stk/projects/examples/threebees
.PHONY : projects/examples/CMakeFiles/threebees.dir/build

projects/examples/CMakeFiles/threebees.dir/clean:
	cd /home/quake/Projects/TStk/stk/build/projects/examples && $(CMAKE_COMMAND) -P CMakeFiles/threebees.dir/cmake_clean.cmake
.PHONY : projects/examples/CMakeFiles/threebees.dir/clean

projects/examples/CMakeFiles/threebees.dir/depend:
	cd /home/quake/Projects/TStk/stk/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/quake/Projects/TStk/stk /home/quake/Projects/TStk/stk/projects/examples /home/quake/Projects/TStk/stk/build /home/quake/Projects/TStk/stk/build/projects/examples /home/quake/Projects/TStk/stk/build/projects/examples/CMakeFiles/threebees.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : projects/examples/CMakeFiles/threebees.dir/depend

