add_executable(json json.c)
set_source_files_properties(json PROPERTIES COMPILE_FLAGS -std=c11)
target_link_libraries(json ruminate ${GLIB2_LIBRARIES})
include_directories(${GLIB2_INCLUDE_DIRS})
