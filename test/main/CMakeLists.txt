add_executable(main
	main.c
)
set_target_properties(main PROPERTIES COMPILE_FLAGS "-std=c99 -g")
target_link_libraries(main ruminate ${GLIB2_LIBRARIES})
include_directories(${GLIB2_INCLUDE_DIRS})
