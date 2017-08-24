# copyright Nils Deppe 2017
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

## Compilation failure tests, e.g. static_assert and SFINAE
set(EXECUTABLE test_compilation)

set(EXECUTABLE_SOURCES compilation_tests.cpp)

add_executable(${EXECUTABLE} ${EXECUTABLE_SOURCES})

set_target_properties(
  ${EXECUTABLE}
  PROPERTIES
  COMPILE_FLAGS "\${WHICH_TEST} -DFILE_IS_COMPILATION_TEST=\"static_assert(false, \\\"File is compilation test\\\");\""
  )

add_compilation_tests(test_compilation)

## Functionality tests
set(EXECUTABLE run_tests)

set(EXECUTABLE_SOURCES runtime_tests.cpp)

add_executable(${EXECUTABLE} ${EXECUTABLE_SOURCES})

add_catch_tests(run_tests)

# Setup code coverage for unit tests
if(COVERAGE)
  # Setup unit test coverage target.
  setup_target_for_coverage(
      Unit
      ${CMAKE_BINARY_DIR}/docs/html
      unit-test-coverage
      ${CMAKE_CTEST_COMMAND} -L unit
      DEPENDS ${EXECUTABLE}
  )
endif()

