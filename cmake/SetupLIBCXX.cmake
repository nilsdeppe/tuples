
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if (LIBCXX_ROOT)
    find_package(LIBCXX REQUIRED)
    set(
        CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -L${LIBCXX_LIBRARIES}"
    )
    include_directories(${LIBCXX_INCLUDE_DIRS})
    message(STATUS "libc++ include: ${LIBCXX_INCLUDE_DIRS}")
    message(STATUS "libc++ libraries: ${LIBCXX_LIBRARIES}")
  endif()

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
  set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}")
endif()
