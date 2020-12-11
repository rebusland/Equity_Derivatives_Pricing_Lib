cmake_minimum_required(VERSION 3.18)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)

# diagnostic information on CMake project variables are printed
message("**************\n")
message("Diagnostic info\n")

project(EQPricer)

# search all .h and .cpp source files through regexp
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)

# PROJECT TARGET
# specify the project's target name and the sources to be built from
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Add the given directories to those the compiler uses to search for include files,
# when compiling the given target.
# Relative paths are interpreted as relative to the current source directory.
set(_PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party" CACHE STRING "Include directories for my project's target")
target_include_directories(${PROJECT_NAME} PUBLIC ${_PROJECT_INCLUDE_DIR})

# NB Visual Studio already supports multiple build configurations (MINGW e.g. do not)
# CMAKE_BUILD_TYPE only makes sense for single-target generators, like Makefiles. 
# It is not used for multi-target generators as those simply generate a build system
# capable of building all build types (debug, release, etc)
# For multi-configuration generators use CMAKE_CONFIGURATION_TYPES to specify the build types
# to add to the tree, e.g. 
# cmake .. -G"Visual Studio 12 2017 Win64" -D CMAKE_CONFIGURATION_TYPES="Release;Debug"
# will add the tree for both release and debug mode
#
# Commands to specify the build folder and the build type for single-config generators (MINGW e.g.)
# cmake -Bbuild/Debug -DCMAKE_BUILD_TYPE=Debug
# cmake -Bbuild/Release -DCMAKE_BUILD_TYPE=Release
# cmake -Bbuild/ReleaseWithDebugInfo -DCMAKE_BUILD_TYPE=RelWithDebInfo
#
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
	message(NOTICE "\nBuild type not specified, defaulted to: " ${CMAKE_BUILD_TYPE} "\n")
else()
	message("\nBuild type: " ${CMAKE_BUILD_TYPE} "\n")
endif()

#
# !! Comment copied from the CMakeLists.txt of my-sql server project !!
#
# C/CXX compiler flags consist of:
# CPPFLAGS        Taken from environment, see above.
# ADD_DEFINITIONS In each individual CMakeLists.txt
# CMAKE_CXX_FLAGS From command line.
# ENV{CXXFLAGS}   From environment, but environment is ignored if
#                 CMAKE_CXX_FLAGS is also given on command line
# CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}, typical initial cmake values:
#      DEBUG          -g
#      RELWITHDEBINFO -O2 -g -DNDEBUG
#      RELEASE        -O3 -DNDEBUG
#      MINSIZEREL     -Os -DNDEBUG
#
# The compiler command line is
# g++ DEFINITIONS CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_<configuration>
#
# So, if you want to use '-O3' for a RELWITHDEBINFO build, you should do:
# cmake -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O3 -g -DNDEBUG"
#
# Note that CMakeCache.txt contains cmake builtins for these variables,
# *not* the values that will actually be used, which we report here:
# executable folder for different build types
# see CMakeCache file to have an idea of the flags set for these build configurations

#
# SET COMPILER OPTIONS FOR THE PROJECT'S TARGET
#	- target_compile_options allows to set different compilation settings for different targets
#	  e.g. the target for my own files (project target) can display warnings
#	  while "external" target, as for external libraries, could not display warnings at all.
# 	- Enforce compliance to ISO standard:
#		/permessive- (MSVC) and -pedantic-errors (GCC) (tries to) enforce conformance to the ISO C++ standard
#	- Enable (lots of) warnings if requested
#	- Set compiler flags for each build type (release, debug etc.)
#

#
# PROJECT's TARGET compilation
#
# are warning compilation flags enabled by the user?
option(_ENABLE_WARN "Should warning compilation flags be enabled?" OFF)
if (_ENABLE_WARN)
	message(NOTICE "--> Compiler's warning flags are enabled\n")
endif()

# WARNING flags: since these could be annoying and noisy, they are added only if requested
set(_FLAGS_WARNING)

# ISO COMPLIANCE flags: to enforce compliance to ISO C++ standard, this eases cross-platform compatibility
set(_FLAGS_ISO_COMPLIANCE)

# additional flags for DEBUG mode to improve debugging of project's target
set(_ADDITIONAL_FLAGS_DEBUG)

# if we want to overwrite the default options for a build type
# (for single build compilers) we can do something like this
# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${_CUSTOM_FLAGS_RELWITHDEBINFO}), for instance:
# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -DNDEBUG")

if(MSVC)
	message("Compiler is MSVC")
	# NB: Visual Studio already supports multiple build configurations
	list(APPEND _FLAGS_WARNING "/W4") # lots of warning
	list(APPEND _FLAGS_ISO_COMPLIANCE "/permissive-")
	# Define my custom build type, which I can invoke from command line with: cmake -DCMAKE_BUILD_TYPE=Custom
	# TODO if needed add compiler settings for CUSTOM build types for MVSC
	# set(CMAKE_CXX_FLAGS_CUSTOM "" CACHE STRING "C++ compiler's flags for custom build type" FORCE)

	# TODO add custom flags to debug mode to remove any level of compiler's optimization
	# list(APPEND _ADDITIONAL_FLAGS_DEBUG "some_flag_01" "some_flag_02")

elseif(MINGW)
    message("Compiler is MINGW")
	list(APPEND _FLAGS_WARNING "-Wall" "-Wextra") # lots of warning
	list(APPEND _FLAGS_ISO_COMPLIANCE "-pedantic-errors")

	# Define my custom build type, which I can invoke from command line with: cmake -DCMAKE_BUILD_TYPE=Custom
	set(CMAKE_CXX_FLAGS_CUSTOM "-g0 -O2" CACHE STRING "C++ compiler's flags for custom build type" FORCE)

	# remove any level of compiler optimization when debug mode is on
	list(APPEND _ADDITIONAL_FLAGS_DEBUG "-O0")

	# Some useful compiler flags could be:
	# -xdebuginfo=no%decl Reduce size of debug binaries, by omitting function declarations.
	# -g0 is the same as -g, except that inlining is enabled.
	# e.g. STRING_APPEND(CMAKE_CXX_FLAGS_DEBUG " -g0 -xdebuginfo=no%decl")

else()
	message("Compiler is " ${CMAKE_CXX_COMPILER_ID})
	message(WARNING "For this compiler, no compiler option has been set!")
endif()

target_compile_options(
	${PROJECT_NAME} PUBLIC 
	${_FLAGS_ISO_COMPLIANCE} 
	$<$<CONFIG:DEBUG>:${_ADDITIONAL_FLAGS_DEBUG}> # further flags in debug mode for this target
	"$<$<BOOL:${_ENABLE_WARN}>:${_FLAGS_WARNING}>" # warning flags added only if requested
)

# allows to specify the location for the executable file
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# more diagnostic info
message("System name: " ${CMAKE_SYSTEM_NAME})
message("System version: " ${CMAKE_SYSTEM_VERSION})
message("System processor: " ${CMAKE_SYSTEM_PROCESSOR})
message("")

# a somewhat involved way to print the compiler flags, but it works
string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPER)
string(CONCAT cxx_flags CMAKE_CXX_FLAGS "_" ${CMAKE_BUILD_TYPE_UPPER})
message("General Compiler options(" ${cxx_flags} "): ${${cxx_flags}}")
get_target_property(PROJ_TARGET_COMPILE_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS)
message("Compile options for project's target: " "${PROJ_TARGET_COMPILE_FLAGS}")

message("C++ Standard: " ${CMAKE_CXX_STANDARD})
message("C++ compiler: " ${CMAKE_CXX_COMPILER_ID})
message("C++ compiler version: " ${CMAKE_CXX_COMPILER_VERSION})
message("")
message("CMake source dir: " ${CMAKE_SOURCE_DIR})
message("General CMake include directories: " ${CMAKE_INCLUDE_PATH})
get_target_property(project_include_dir ${PROJECT_NAME} INCLUDE_DIRECTORIES)
message("Project's target include directories: ${project_include_dir}")
message("CMake binary dir: " ${CMAKE_BINARY_DIR})
message("CMake runtime output directory: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message("CMake executable output path: " ${EXECUTABLE_OUTPUT_PATH})
message("\n**************\n")
