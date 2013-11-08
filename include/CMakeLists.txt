configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/config.h
	@ONLY
)

set(HEADERS
	ruminate.h
)

install(FILES ${PUBLIC_HEADERS} DESTINATION include)

add_subdirectory(ruminate)

foreach(header ${HEADERS})
	list(APPEND PUBLIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
endforeach(header ${HEADERS})
set(PUBLIC_HEADERS ${PUBLIC_HEADERS} PARENT_SCOPE)
