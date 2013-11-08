find_package(Jansson)
find_package(Talloc)

if(JANSSON_FOUND AND TALLOC_FOUND)
	add_library(ruminate-jansson
		ruminate-jansson.c
	)
	set_target_properties(ruminate-jansson PROPERTIES COMPILE_FLAGS -std=c99)
	target_link_libraries(ruminate-jansson
		LINK_PRIVATE ruminate jansson ${GLIB2_LIBRARIES} ${TALLOC_LIBRARIES}
	)
	target_include_directories(ruminate-jansson
		PUBLIC ${JANSSON_INCLUDE_DIRS} ${GLIB2_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}
		PRIVATE ${TALLOC_INCLUDE_DIRS}
	)
endif(JANSSON_FOUND AND TALLOC_FOUND)
