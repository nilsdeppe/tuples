# copyright Nils Deppe 2017
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

find_package(PythonInterp REQUIRED)

# Main function - the only one designed to be called from outside this module.
function(add_compilation_tests TEST_TARGET)
    get_target_property(SOURCE_FILES ${TEST_TARGET} SOURCES)

    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tmp")

    foreach (SOURCE_FILE ${SOURCE_FILES})
      if (NOT IS_ABSOLUTE ${SOURCE_FILE})
        set(SOURCE_FILE ${CMAKE_CURRENT_LIST_DIR}/${SOURCE_FILE})
      endif()
      set(ABSOLUTE_SOURCE_FILES "${ABSOLUTE_SOURCE_FILES};${SOURCE_FILE}")
    endforeach()

    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/cmake/ParseCompilationTests.py
        ${ABSOLUTE_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tmp)

    foreach (SOURCE_FILE ${ABSOLUTE_SOURCE_FILES})
        compilation_tests_parse_file(${SOURCE_FILE} ${TEST_TARGET})
    endforeach ()
endfunction()


function(compilation_tests_parse_file SOURCE_FILE TEST_TARGET)
  if(NOT EXISTS ${SOURCE_FILE})
    message(WARNING "Could not find source file:\n\"${SOURCE_FILE}\"\nfor \
        tests\n.")
    return()
  endif()

  file(STRINGS ${SOURCE_FILE} CONTENTS NEWLINE_CONSUME)
  string(REGEX MATCHALL
    "#ifdef COMPILATION_TEST_([^\n]+)"
    TESTS
    "${CONTENTS}")

  foreach(IFDEF_TEST_NAME ${TESTS})
    string(REGEX REPLACE "#ifdef (COMPILATION_TEST_[^\n]+)" "\\1"
      TEST_NAME "${IFDEF_TEST_NAME}")
    
    if(EXISTS "${CMAKE_BINARY_DIR}/tmp/${TEST_NAME}.${CMAKE_CXX_COMPILER_ID}")
      FILE(READ "${CMAKE_BINARY_DIR}/tmp/${TEST_NAME}.${CMAKE_CXX_COMPILER_ID}" OUTPUT_REGEX)
      FILE(REMOVE "${CMAKE_BINARY_DIR}/tmp/${TEST_NAME}.${CMAKE_CXX_COMPILER_ID}")

      string(REGEX REPLACE "Tags: ([a-z0-9_ ]+) (.*)" "\\1"
        TEST_TAGS "${OUTPUT_REGEX}")

      string(REGEX REPLACE "Tags: ${TEST_TAGS} ([0-9]+.[0-9]+): (.*)" "\\1"
        COMPILER_VERSION "${OUTPUT_REGEX}")

      string(REGEX REPLACE "Tags: ${TEST_TAGS} ${COMPILER_VERSION}: (.*)" "\\1"
        OUTPUT_REGEX "${OUTPUT_REGEX}")
    elseif(EXISTS "${CMAKE_BINARY_DIR}/tmp/${TEST_NAME}.all")
      FILE(READ "${CMAKE_BINARY_DIR}/tmp/${TEST_NAME}.all" OUTPUT_REGEX)
      FILE(REMOVE "${CMAKE_BINARY_DIR}/tmp/${TEST_NAME}.all")
      
      string(REGEX REPLACE "Tags: ([a-z0-9 ]+) Regex:(.*)" "\\1"
        TEST_TAGS "${OUTPUT_REGEX}")

      string(REGEX REPLACE "Tags: ${TEST_TAGS} Regex: (.*)" "\\1"
        OUTPUT_REGEX "${OUTPUT_REGEX}")
    else()
      message(FATAL_ERROR "Could not find a regex to match for test: ${TEST_NAME}")
    endif()
    string(REGEX REPLACE " " ";" TEST_TAGS "${TEST_TAGS}")

    add_test(
        NAME "${TEST_NAME}"
        COMMAND make WHICH_TEST="-D${TEST_NAME}" ${TEST_TARGET}
        )
    set_tests_properties(
      "${TEST_NAME}"
      PROPERTIES
      TIMEOUT 5
      LABELS "${TEST_TAGS}"
      PASS_REGULAR_EXPRESSION ${OUTPUT_REGEX}
      )
  endforeach()
endfunction()
