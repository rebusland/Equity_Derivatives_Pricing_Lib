cmake_minimum_required(VERSION 3.18)

# diagnostic information on CMake project variables are printed
message("**************\n")
message("Diagnostic info\n")

project(EQPricer)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)

# COMPILER OPTIONS
#
# - Enable (lots of) warnings: this could be annoying after a while
# - /permessive- (MSVC) and -pedantic-errors (GCC) (tries to) enforce conformance to the ISO C++ standard
if (MSVC)
	message("Compiler is MSVC, version: " ${MSVC_VERSION})
    add_compile_options(/W4 /permissive-)
elseif(MINGW)
    message("Compiler is MINGW")
    add_compile_options(-g -O0 -Wall -Wextra -pedantic-errors)
else()
	message("Compiler is " ${CMAKE_CXX_COMPILER_ID})
	message("For this compiler, no compiler option has been set!")
endif()

# source files and build directories
# add_subdirectory(src build)

# search all .h and .cpp source files through regexp
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)

# allows to specify the location for the executable file
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# specify the executable's name and the sources to be built from
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# more diagnostic info
message("System name: " ${CMAKE_SYSTEM_NAME})
message("System version: " ${CMAKE_SYSTEM_VERSION})
message("System processor: " ${CMAKE_SYSTEM_PROCESSOR})
message("")
message("CMake source dir: " ${CMAKE_SOURCE_DIR})
message("CMake include path: " ${CMAKE_INCLUDE_PATH})
message("CMake binary dir: " ${CMAKE_BINARY_DIR})
message("CMake runtime output directory: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message("CMake executable output path: " ${EXECUTABLE_OUTPUT_PATH})
message("\n**************\n")
