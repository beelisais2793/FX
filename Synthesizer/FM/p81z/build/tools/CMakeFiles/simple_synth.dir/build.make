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
CMAKE_SOURCE_DIR = /home/quake/Projects/AudioDSP/FM/p81z

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/quake/Projects/AudioDSP/FM/p81z/build

# Include any dependencies generated for this target.
include tools/CMakeFiles/simple_synth.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tools/CMakeFiles/simple_synth.dir/compiler_depend.make

# Include the progress variables for this target.
include tools/CMakeFiles/simple_synth.dir/progress.make

# Include the compile flags for this target's objects.
include tools/CMakeFiles/simple_synth.dir/flags.make

tools/CMakeFiles/simple_synth.dir/simple_synth.cpp.o: tools/CMakeFiles/simple_synth.dir/flags.make
tools/CMakeFiles/simple_synth.dir/simple_synth.cpp.o: /home/quake/Projects/AudioDSP/FM/p81z/tools/simple_synth.cpp
tools/CMakeFiles/simple_synth.dir/simple_synth.cpp.o: tools/CMakeFiles/simple_synth.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/AudioDSP/FM/p81z/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tools/CMakeFiles/simple_synth.dir/simple_synth.cpp.o"
	cd /home/quake/Projects/AudioDSP/FM/p81z/build/tools && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tools/CMakeFiles/simple_synth.dir/simple_synth.cpp.o -MF CMakeFiles/simple_synth.dir/simple_synth.cpp.o.d -o CMakeFiles/simple_synth.dir/simple_synth.cpp.o -c /home/quake/Projects/AudioDSP/FM/p81z/tools/simple_synth.cpp

tools/CMakeFiles/simple_synth.dir/simple_synth.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simple_synth.dir/simple_synth.cpp.i"
	cd /home/quake/Projects/AudioDSP/FM/p81z/build/tools && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/quake/Projects/AudioDSP/FM/p81z/tools/simple_synth.cpp > CMakeFiles/simple_synth.dir/simple_synth.cpp.i

tools/CMakeFiles/simple_synth.dir/simple_synth.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simple_synth.dir/simple_synth.cpp.s"
	cd /home/quake/Projects/AudioDSP/FM/p81z/build/tools && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/quake/Projects/AudioDSP/FM/p81z/tools/simple_synth.cpp -o CMakeFiles/simple_synth.dir/simple_synth.cpp.s

# Object files for target simple_synth
simple_synth_OBJECTS = \
"CMakeFiles/simple_synth.dir/simple_synth.cpp.o"

# External object files for target simple_synth
simple_synth_EXTERNAL_OBJECTS =

tools/simple_synth: tools/CMakeFiles/simple_synth.dir/simple_synth.cpp.o
tools/simple_synth: tools/CMakeFiles/simple_synth.dir/build.make
tools/simple_synth: libP81Z.a
tools/simple_synth: tools/CMakeFiles/simple_synth.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/quake/Projects/AudioDSP/FM/p81z/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable simple_synth"
	cd /home/quake/Projects/AudioDSP/FM/p81z/build/tools && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simple_synth.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/CMakeFiles/simple_synth.dir/build: tools/simple_synth
.PHONY : tools/CMakeFiles/simple_synth.dir/build

tools/CMakeFiles/simple_synth.dir/clean:
	cd /home/quake/Projects/AudioDSP/FM/p81z/build/tools && $(CMAKE_COMMAND) -P CMakeFiles/simple_synth.dir/cmake_clean.cmake
.PHONY : tools/CMakeFiles/simple_synth.dir/clean

tools/CMakeFiles/simple_synth.dir/depend:
	cd /home/quake/Projects/AudioDSP/FM/p81z/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/quake/Projects/AudioDSP/FM/p81z /home/quake/Projects/AudioDSP/FM/p81z/tools /home/quake/Projects/AudioDSP/FM/p81z/build /home/quake/Projects/AudioDSP/FM/p81z/build/tools /home/quake/Projects/AudioDSP/FM/p81z/build/tools/CMakeFiles/simple_synth.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/CMakeFiles/simple_synth.dir/depend

