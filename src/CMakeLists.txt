configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/ruminate.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/ruminate.pc
	@ONLY
)
add_custom_target(build_ruminate_pkg_config ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ruminate.pc)

link_directories(${ICE_LIBRARY_DIR})

set(c_sources
	gettid.c
	errors.c
)

set(cxx_sources
	common.cpp
	string.cpp
	type.cpp
	builtin_type.cpp
	array_type.cpp
	pointer_type.cpp
	typedef_type.cpp
	function_type.cpp
	aggregate_type.cpp
	enum_member.cpp
	aggregate_member.cpp
	type_member.cpp
	frame.cpp
	ruminate.cpp
	memory.cpp
)

#message(WARNING "${SLICE2CPP_OUTPUT_FILES}")
#set_source_files_properties(${SLICE2CPP_OUTPUT_FILES} PROPERTIES COMPILE_FLAGS -w)

add_library(ruminate
	${c_sources}
	${cxx_sources}
	${SLICE2CPP_OUTPUT_FILES}
)

set_source_files_properties(${c_sources} PROPERTIES COMPILE_FLAGS -std=c99)
set_source_files_properties(${cxx_sources} ${SLICE2CPP_OUTPUT_FILES} PROPERTIES COMPILE_FLAGS -std=c++98)
set_source_files_properties(${SLICE2CPP_OUTPUT_FILES} PROPERTIES GENERATED 1)
add_dependencies(ruminate generate_ice_cpp)

target_include_directories(ruminate
	PUBLIC ${RUMINATE_SOURCE_DIR}/include ${GLIB2_INCLUDE_DIRS}
	PRIVATE ${RUMINATE_BINARY_DIR}/include ${RUMINATE_BINARY_DIR}/ice ${RUMINATE_BINARY_DIR} ${ICE_INCLUDE_DIR}
)
target_link_libraries(ruminate
	LINK_PRIVATE Ice IceUtil pthread ${GLIB2_LIBRARIES}
)
#set_target_properties(ruminate PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
#set_target_properties(ruminate PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${GLIB2_INCLUDE_DIRS}")

install(TARGETS ruminate LIBRARY DESTINATION lib)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ruminate.pc" DESTINATION lib/pkgconfig)
