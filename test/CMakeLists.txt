include_directories(${RUMINATE_SOURCE_DIR}/include)

add_executable(json
	json.c
)
set_source_files_properties(json PROPERTIES COMPILE_FLAGS -std=c11)
target_link_libraries(json ruminate)

add_executable(backtrace
	backtrace.c
	util.c
)
set_target_properties(backtrace PROPERTIES COMPILE_FLAGS -std=c99)
target_link_libraries(backtrace ruminate)
