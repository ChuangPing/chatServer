# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/chuanping/c++/chatServer/testmuduo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chuanping/c++/chatServer/testmuduo/build

# Include any dependencies generated for this target.
include CMakeFiles/server.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/server.dir/flags.make

CMakeFiles/server.dir/testmuduo.cpp.o: CMakeFiles/server.dir/flags.make
CMakeFiles/server.dir/testmuduo.cpp.o: ../testmuduo.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chuanping/c++/chatServer/testmuduo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/server.dir/testmuduo.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/testmuduo.cpp.o -c /home/chuanping/c++/chatServer/testmuduo/testmuduo.cpp

CMakeFiles/server.dir/testmuduo.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/testmuduo.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chuanping/c++/chatServer/testmuduo/testmuduo.cpp > CMakeFiles/server.dir/testmuduo.cpp.i

CMakeFiles/server.dir/testmuduo.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/testmuduo.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chuanping/c++/chatServer/testmuduo/testmuduo.cpp -o CMakeFiles/server.dir/testmuduo.cpp.s

CMakeFiles/server.dir/testmuduo.cpp.o.requires:

.PHONY : CMakeFiles/server.dir/testmuduo.cpp.o.requires

CMakeFiles/server.dir/testmuduo.cpp.o.provides: CMakeFiles/server.dir/testmuduo.cpp.o.requires
	$(MAKE) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/testmuduo.cpp.o.provides.build
.PHONY : CMakeFiles/server.dir/testmuduo.cpp.o.provides

CMakeFiles/server.dir/testmuduo.cpp.o.provides.build: CMakeFiles/server.dir/testmuduo.cpp.o


# Object files for target server
server_OBJECTS = \
"CMakeFiles/server.dir/testmuduo.cpp.o"

# External object files for target server
server_EXTERNAL_OBJECTS =

../bin/server: CMakeFiles/server.dir/testmuduo.cpp.o
../bin/server: CMakeFiles/server.dir/build.make
../bin/server: CMakeFiles/server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/chuanping/c++/chatServer/testmuduo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/server.dir/build: ../bin/server

.PHONY : CMakeFiles/server.dir/build

CMakeFiles/server.dir/requires: CMakeFiles/server.dir/testmuduo.cpp.o.requires

.PHONY : CMakeFiles/server.dir/requires

CMakeFiles/server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/server.dir/clean

CMakeFiles/server.dir/depend:
	cd /home/chuanping/c++/chatServer/testmuduo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chuanping/c++/chatServer/testmuduo /home/chuanping/c++/chatServer/testmuduo /home/chuanping/c++/chatServer/testmuduo/build /home/chuanping/c++/chatServer/testmuduo/build /home/chuanping/c++/chatServer/testmuduo/build/CMakeFiles/server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/server.dir/depend

