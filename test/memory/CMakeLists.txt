add_executable(memory
	memory.c
)
set_target_properties(memory PROPERTIES COMPILE_FLAGS -std=c99)
target_link_libraries(memory ruminate ${GLIB2_LIBRARIES})
include_directories(
	${GLIB2_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
)
