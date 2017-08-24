# copyright Nils Deppe 2017
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

enable_testing(true)

include_directories(${CMAKE_SOURCE_DIR})

include(SetupCompilationFailureTests)
include(SetupCxxVersion)


