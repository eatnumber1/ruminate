if(JANSSON_FOUND)
	add_library(ruminate-jansson
		serialize.c
		deserialize.c
		json_state.c
	)
	set_target_properties(ruminate-jansson PROPERTIES COMPILE_FLAGS "-std=c11 -g")
	target_link_libraries(ruminate-jansson
		LINK_PRIVATE ruminate jansson ${GLIB2_LIBRARIES}
	)
	target_include_directories(ruminate-jansson
		PUBLIC ${JANSSON_INCLUDE_DIRS} ${GLIB2_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}
	)
endif(JANSSON_FOUND)
