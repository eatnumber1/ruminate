if(JANSSON_FOUND)
	add_executable(ruminate-jansson-test
		ruminate-jansson-test.c
	)
	set_target_properties(ruminate-jansson-test PROPERTIES COMPILE_FLAGS "-std=c11 -g")
	target_link_libraries(ruminate-jansson-test
		ruminate
		ruminate-jansson
		jansson
		${GLIB2_LIBRARIES}
		${JANSSON_LIBRARIES}
	)
	include_directories(
		${JANSSON_INCLUDE_DIRS}
		${GLIB2_INCLUDE_DIRS}
	)
endif(JANSSON_FOUND)
