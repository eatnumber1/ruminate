option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)

if(BUILD_DOCUMENTATION)
	find_package(Doxygen)

	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR
			"Doxygen not found"
		)
	endif(NOT DOXYGEN_FOUND)

	set(DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
	string(REPLACE ";" " " DOXYGEN_INPUTS "${PUBLIC_HEADERS}")

	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
		"${DOXYFILE}"
		@ONLY
	)

	add_custom_target(docs
		"${DOXYGEN_EXECUTABLE}" "${DOXYFILE}"
		DEPENDS ${PUBLIC_HEADERS} "${DOXYFILE}"
		WORKING_DIRECTORY "${RUMINATE_SOURCE_DIR}"
		COMMENT "Building documentation"
	)
endif(BUILD_DOCUMENTATION)
