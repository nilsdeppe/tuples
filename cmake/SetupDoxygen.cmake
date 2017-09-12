find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file(
      docs/Doxyfile.in
      ${PROJECT_BINARY_DIR}/docs/DoxyfileHtml @ONLY IMMEDIATE
  )

  add_custom_target(
      doc
      COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/docs/DoxyfileHtml 2>&1
      DEPENDS
      ${PROJECT_BINARY_DIR}/docs/DoxyfileHtml
      ${SPECTRE_DOXYGEN_GROUPS}
  )
endif()
