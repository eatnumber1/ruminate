add_executable(backtrace
	backtrace.c
	util.c
)
set_target_properties(backtrace PROPERTIES COMPILE_FLAGS -std=c99)
target_link_libraries(backtrace ruminate ${GLIB2_LIBRARIES})
include_directories(
	${GLIB2_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
)
