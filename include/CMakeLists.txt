configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/config.h
)

set(HEADERS
	ruminate.h
)

install(FILES ${HEADERS} DESTINATION include)

add_subdirectory(ruminate)
