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
CMAKE_SOURCE_DIR = /home/quake/Projects/fluidsynth/FluidLite

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/quake/Projects/fluidsynth/FluidLite/build

# Include any dependencies generated for this target.
include CMakeFiles/fluidlite.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/fluidlite.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/fluidlite.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/fluidlite.dir/flags.make

CMakeFiles/fluidlite.dir/src/fluid_init.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_init.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_init.c
CMakeFiles/fluidlite.dir/src/fluid_init.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/fluidlite.dir/src/fluid_init.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_init.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_init.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_init.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_init.c

CMakeFiles/fluidlite.dir/src/fluid_init.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_init.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_init.c > CMakeFiles/fluidlite.dir/src/fluid_init.c.i

CMakeFiles/fluidlite.dir/src/fluid_init.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_init.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_init.c -o CMakeFiles/fluidlite.dir/src/fluid_init.c.s

CMakeFiles/fluidlite.dir/src/fluid_chan.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_chan.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_chan.c
CMakeFiles/fluidlite.dir/src/fluid_chan.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/fluidlite.dir/src/fluid_chan.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_chan.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_chan.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_chan.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_chan.c

CMakeFiles/fluidlite.dir/src/fluid_chan.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_chan.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_chan.c > CMakeFiles/fluidlite.dir/src/fluid_chan.c.i

CMakeFiles/fluidlite.dir/src/fluid_chan.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_chan.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_chan.c -o CMakeFiles/fluidlite.dir/src/fluid_chan.c.s

CMakeFiles/fluidlite.dir/src/fluid_chorus.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_chorus.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_chorus.c
CMakeFiles/fluidlite.dir/src/fluid_chorus.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/fluidlite.dir/src/fluid_chorus.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_chorus.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_chorus.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_chorus.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_chorus.c

CMakeFiles/fluidlite.dir/src/fluid_chorus.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_chorus.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_chorus.c > CMakeFiles/fluidlite.dir/src/fluid_chorus.c.i

CMakeFiles/fluidlite.dir/src/fluid_chorus.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_chorus.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_chorus.c -o CMakeFiles/fluidlite.dir/src/fluid_chorus.c.s

CMakeFiles/fluidlite.dir/src/fluid_conv.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_conv.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_conv.c
CMakeFiles/fluidlite.dir/src/fluid_conv.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/fluidlite.dir/src/fluid_conv.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_conv.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_conv.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_conv.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_conv.c

CMakeFiles/fluidlite.dir/src/fluid_conv.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_conv.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_conv.c > CMakeFiles/fluidlite.dir/src/fluid_conv.c.i

CMakeFiles/fluidlite.dir/src/fluid_conv.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_conv.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_conv.c -o CMakeFiles/fluidlite.dir/src/fluid_conv.c.s

CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_defsfont.c
CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_defsfont.c

CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_defsfont.c > CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.i

CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_defsfont.c -o CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.s

CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_dsp_float.c
CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_dsp_float.c

CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_dsp_float.c > CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.i

CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_dsp_float.c -o CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.s

CMakeFiles/fluidlite.dir/src/fluid_gen.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_gen.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_gen.c
CMakeFiles/fluidlite.dir/src/fluid_gen.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/fluidlite.dir/src/fluid_gen.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_gen.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_gen.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_gen.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_gen.c

CMakeFiles/fluidlite.dir/src/fluid_gen.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_gen.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_gen.c > CMakeFiles/fluidlite.dir/src/fluid_gen.c.i

CMakeFiles/fluidlite.dir/src/fluid_gen.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_gen.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_gen.c -o CMakeFiles/fluidlite.dir/src/fluid_gen.c.s

CMakeFiles/fluidlite.dir/src/fluid_hash.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_hash.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_hash.c
CMakeFiles/fluidlite.dir/src/fluid_hash.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/fluidlite.dir/src/fluid_hash.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_hash.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_hash.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_hash.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_hash.c

CMakeFiles/fluidlite.dir/src/fluid_hash.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_hash.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_hash.c > CMakeFiles/fluidlite.dir/src/fluid_hash.c.i

CMakeFiles/fluidlite.dir/src/fluid_hash.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_hash.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_hash.c -o CMakeFiles/fluidlite.dir/src/fluid_hash.c.s

CMakeFiles/fluidlite.dir/src/fluid_list.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_list.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_list.c
CMakeFiles/fluidlite.dir/src/fluid_list.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/fluidlite.dir/src/fluid_list.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_list.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_list.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_list.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_list.c

CMakeFiles/fluidlite.dir/src/fluid_list.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_list.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_list.c > CMakeFiles/fluidlite.dir/src/fluid_list.c.i

CMakeFiles/fluidlite.dir/src/fluid_list.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_list.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_list.c -o CMakeFiles/fluidlite.dir/src/fluid_list.c.s

CMakeFiles/fluidlite.dir/src/fluid_mod.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_mod.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_mod.c
CMakeFiles/fluidlite.dir/src/fluid_mod.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/fluidlite.dir/src/fluid_mod.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_mod.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_mod.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_mod.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_mod.c

CMakeFiles/fluidlite.dir/src/fluid_mod.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_mod.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_mod.c > CMakeFiles/fluidlite.dir/src/fluid_mod.c.i

CMakeFiles/fluidlite.dir/src/fluid_mod.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_mod.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_mod.c -o CMakeFiles/fluidlite.dir/src/fluid_mod.c.s

CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_ramsfont.c
CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_ramsfont.c

CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_ramsfont.c > CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.i

CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_ramsfont.c -o CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.s

CMakeFiles/fluidlite.dir/src/fluid_rev.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_rev.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_rev.c
CMakeFiles/fluidlite.dir/src/fluid_rev.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object CMakeFiles/fluidlite.dir/src/fluid_rev.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_rev.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_rev.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_rev.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_rev.c

CMakeFiles/fluidlite.dir/src/fluid_rev.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_rev.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_rev.c > CMakeFiles/fluidlite.dir/src/fluid_rev.c.i

CMakeFiles/fluidlite.dir/src/fluid_rev.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_rev.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_rev.c -o CMakeFiles/fluidlite.dir/src/fluid_rev.c.s

CMakeFiles/fluidlite.dir/src/fluid_settings.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_settings.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_settings.c
CMakeFiles/fluidlite.dir/src/fluid_settings.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object CMakeFiles/fluidlite.dir/src/fluid_settings.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_settings.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_settings.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_settings.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_settings.c

CMakeFiles/fluidlite.dir/src/fluid_settings.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_settings.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_settings.c > CMakeFiles/fluidlite.dir/src/fluid_settings.c.i

CMakeFiles/fluidlite.dir/src/fluid_settings.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_settings.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_settings.c -o CMakeFiles/fluidlite.dir/src/fluid_settings.c.s

CMakeFiles/fluidlite.dir/src/fluid_synth.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_synth.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_synth.c
CMakeFiles/fluidlite.dir/src/fluid_synth.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building C object CMakeFiles/fluidlite.dir/src/fluid_synth.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_synth.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_synth.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_synth.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_synth.c

CMakeFiles/fluidlite.dir/src/fluid_synth.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_synth.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_synth.c > CMakeFiles/fluidlite.dir/src/fluid_synth.c.i

CMakeFiles/fluidlite.dir/src/fluid_synth.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_synth.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_synth.c -o CMakeFiles/fluidlite.dir/src/fluid_synth.c.s

CMakeFiles/fluidlite.dir/src/fluid_sys.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_sys.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_sys.c
CMakeFiles/fluidlite.dir/src/fluid_sys.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building C object CMakeFiles/fluidlite.dir/src/fluid_sys.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_sys.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_sys.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_sys.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_sys.c

CMakeFiles/fluidlite.dir/src/fluid_sys.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_sys.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_sys.c > CMakeFiles/fluidlite.dir/src/fluid_sys.c.i

CMakeFiles/fluidlite.dir/src/fluid_sys.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_sys.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_sys.c -o CMakeFiles/fluidlite.dir/src/fluid_sys.c.s

CMakeFiles/fluidlite.dir/src/fluid_tuning.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_tuning.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_tuning.c
CMakeFiles/fluidlite.dir/src/fluid_tuning.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building C object CMakeFiles/fluidlite.dir/src/fluid_tuning.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_tuning.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_tuning.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_tuning.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_tuning.c

CMakeFiles/fluidlite.dir/src/fluid_tuning.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_tuning.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_tuning.c > CMakeFiles/fluidlite.dir/src/fluid_tuning.c.i

CMakeFiles/fluidlite.dir/src/fluid_tuning.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_tuning.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_tuning.c -o CMakeFiles/fluidlite.dir/src/fluid_tuning.c.s

CMakeFiles/fluidlite.dir/src/fluid_voice.c.o: CMakeFiles/fluidlite.dir/flags.make
CMakeFiles/fluidlite.dir/src/fluid_voice.c.o: /home/quake/Projects/fluidsynth/FluidLite/src/fluid_voice.c
CMakeFiles/fluidlite.dir/src/fluid_voice.c.o: CMakeFiles/fluidlite.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building C object CMakeFiles/fluidlite.dir/src/fluid_voice.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/fluidlite.dir/src/fluid_voice.c.o -MF CMakeFiles/fluidlite.dir/src/fluid_voice.c.o.d -o CMakeFiles/fluidlite.dir/src/fluid_voice.c.o -c /home/quake/Projects/fluidsynth/FluidLite/src/fluid_voice.c

CMakeFiles/fluidlite.dir/src/fluid_voice.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fluidlite.dir/src/fluid_voice.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/quake/Projects/fluidsynth/FluidLite/src/fluid_voice.c > CMakeFiles/fluidlite.dir/src/fluid_voice.c.i

CMakeFiles/fluidlite.dir/src/fluid_voice.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fluidlite.dir/src/fluid_voice.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/quake/Projects/fluidsynth/FluidLite/src/fluid_voice.c -o CMakeFiles/fluidlite.dir/src/fluid_voice.c.s

# Object files for target fluidlite
fluidlite_OBJECTS = \
"CMakeFiles/fluidlite.dir/src/fluid_init.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_chan.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_chorus.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_conv.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_gen.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_hash.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_list.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_mod.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_rev.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_settings.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_synth.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_sys.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_tuning.c.o" \
"CMakeFiles/fluidlite.dir/src/fluid_voice.c.o"

# External object files for target fluidlite
fluidlite_EXTERNAL_OBJECTS =

libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_init.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_chan.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_chorus.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_conv.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_defsfont.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_dsp_float.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_gen.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_hash.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_list.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_mod.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_ramsfont.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_rev.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_settings.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_synth.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_sys.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_tuning.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/src/fluid_voice.c.o
libfluidlite.so: CMakeFiles/fluidlite.dir/build.make
libfluidlite.so: /usr/lib/x86_64-linux-gnu/libm.so
libfluidlite.so: CMakeFiles/fluidlite.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Linking C shared library libfluidlite.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fluidlite.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/fluidlite.dir/build: libfluidlite.so
.PHONY : CMakeFiles/fluidlite.dir/build

CMakeFiles/fluidlite.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/fluidlite.dir/cmake_clean.cmake
.PHONY : CMakeFiles/fluidlite.dir/clean

CMakeFiles/fluidlite.dir/depend:
	cd /home/quake/Projects/fluidsynth/FluidLite/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/quake/Projects/fluidsynth/FluidLite /home/quake/Projects/fluidsynth/FluidLite /home/quake/Projects/fluidsynth/FluidLite/build /home/quake/Projects/fluidsynth/FluidLite/build /home/quake/Projects/fluidsynth/FluidLite/build/CMakeFiles/fluidlite.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/fluidlite.dir/depend

