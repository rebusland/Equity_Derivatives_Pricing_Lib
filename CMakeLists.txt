cmake_minimum_required(VERSION 3.18)

project(EQPricer)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)

# enable all warnings (appends the -Wall flag to the previous definition of the CMAKE_CXX_FLAGS variable)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# source files and build directories
# add_subdirectory(src build)

# search all .h and .cpp source files through regexp
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)

# allows to specify the location for the executable file
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# specify the executable's name and the sources to be built from
add_executable(${PROJECT_NAME} ${SOURCE_FILES})


# diagnostic information on CMake project variables
message("**************\n")
message("Diagnostic info\n")
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
