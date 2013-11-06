configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/ruminate.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/ruminate.pc
	@ONLY
)
add_custom_target(build_ruminate_pkg_config ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ruminate.pc)

include_directories(
	${RUMINATE_SOURCE_DIR}/include
	${RUMINATE_BINARY_DIR}/include
	${RUMINATE_BINARY_DIR}/ice
	${RUMINATE_BINARY_DIR}
	${ICE_INCLUDE_DIR}
)
link_directories(${ICE_LIBRARY_DIR})

add_library(ruminate
	gettid.c
	errors.c
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
	${SLICE2CPP_OUTPUT_FILES}
)

set_source_files_properties(${SLICE2CPP_OUTPUT_FILES} PROPERTIES GENERATED 1)
add_dependencies(ruminate generate_ice_cpp)

target_link_libraries(ruminate ${GLIB2_LIBRARIES})
target_link_libraries(ruminate Ice IceUtil pthread)

install(TARGETS ruminate LIBRARY DESTINATION lib)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ruminate.pc" DESTINATION lib/pkgconfig)
