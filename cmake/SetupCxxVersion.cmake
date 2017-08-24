
option(CXX_VERSION "Use either C++11 (default) or C++14" OFF)

if(NOT ${CXX_VERSION})
  set(CXX_VERSION "11")
endif()

macro(use_cxx_version)
  if(CMAKE_VERSION VERSION_LESS "3.1")
    set (CMAKE_CXX_FLAGS "-std=c++${CXX_VERSION} ${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_STANDARD ${CXX_VERSION})
  endif()
endmacro(use_cxx_version)

use_cxx_version()
