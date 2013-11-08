find_package(Doxygen)

if(DOXYGEN_FOUND)
	set(DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
	set(DOXYOUT "${CMAKE_CURRENT_BINARY_DIR}/html")
	string(REPLACE ";" " " DOXYGEN_INPUTS "${PUBLIC_HEADERS}")

	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
		"${DOXYFILE}"
		@ONLY
	)

	add_custom_command(
		OUTPUT "${DOXYOUT}"
		WORKING_DIRECTORY "${RUMINATE_SOURCE_DIR}"
		COMMENT "Building documentation"
		COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYFILE}"
		DEPENDS ${PUBLIC_HEADERS} ${DOXYFILE}
	)

	add_custom_target(build_doc ALL DEPENDS "${DOXYOUT}")
endif(DOXYGEN_FOUND)
