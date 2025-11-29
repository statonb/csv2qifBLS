cmake_minimum_required(VERSION 3.10)

# Set compilers (must be before project() command)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(csv2qifBLS VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    csv2qifBLS.cpp
    cusipBankMap.cpp
    mmSymbols.cpp
)

# Header files (optional, for IDE organization)
set(HEADERS
    cusipBankMap.h
    mmSymbols.h
)

# Create the executable
add_executable(csv2qifBLS ${SOURCES} ${HEADERS})

# Debug build settings
target_compile_options(csv2qifBLS PRIVATE
    $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra -DDEBUG>
)

# Release build settings
target_compile_options(csv2qifBLS PRIVATE
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Print build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

