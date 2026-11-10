cmake_minimum_required(VERSION 3.20)
project(myproject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Picks up every .cpp under src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# 1. Create a shared library instead of an executable
add_library(mylib SHARED ${SOURCES})

# 2. Required for .so files on Linux (compiles with -fPIC)
set_property(TARGET mylib PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(mylib PRIVATE src include)
target_compile_options(mylib PRIVATE
  -Wall -Wextra -Weffc++ -Wconversion -Wsign-conversion
  -pedantic-errors -Werror)