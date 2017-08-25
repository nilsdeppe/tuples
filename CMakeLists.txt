# copyright Nils Deppe 2017
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# In order to use certain code analysis tools like clang-tidy and cppcheck the
# compile commands need to be accessible. CMake can write these to a
# "compile_commands.json" file.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing(true)

include_directories(${CMAKE_SOURCE_DIR}/include)

include(SetupLIBCXX)
include(CodeCoverageDetection)
include(SetupCxxVersion)

include(SetupCompilationFailureTests)
include(SetupCatchTests)

add_subdirectory(test)
