if(JANSSON_FOUND)
	add_executable(stdout
		stdout.c
	)
	set_target_properties(stdout PROPERTIES COMPILE_FLAGS "-std=c99 -g")
	target_link_libraries(stdout
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
