set(SLICE_FILES
	debugger.ice
	type.ice
	debugger_factory.ice
	exceptions.ice
	type_member.ice
	frame.ice
)

foreach(ice_include_dir ${ICE_INCLUDE_DIR})
	set(SLICE_OPTIONS ${SLICE_OPTIONS} -I${ice_include_dir})
endforeach(ice_include_dir ${ICE_INCLUDE_DIR})

set(SLICE_PY_MODULE_NAME RuminateBackend)
set(SLICE_PY_MODULE "${CMAKE_CURRENT_BINARY_DIR}/${SLICE_PY_MODULE_NAME}/__init__.py")
set(SLICE_PY_MODULE_COPIED "${RUMINATE_BINARY_DIR}/python/${SLICE_PY_MODULE_NAME}/__init__.py")
get_filename_component(SLICE_PY_MODULE_DIR "${SLICE_PY_MODULE}" PATH)
get_filename_component(SLICE_PY_MODULE_COPIED_DIR "${SLICE_PY_MODULE_COPIED}" PATH)

add_custom_target(generate_nothing)
set(PREVIOUS_SLICE2PY_OUTPUT_TARGET generate_nothing)

foreach(slice_file_with_extension ${SLICE_FILES})
	set(slice_file "${CMAKE_CURRENT_SOURCE_DIR}/${slice_file_with_extension}")
	get_filename_component(slice_file_name "${slice_file_with_extension}" NAME_WE)
	set(GENERATED_CPP_FILE "${CMAKE_CURRENT_BINARY_DIR}/${slice_file_name}.cpp")
	set(GENERATED_H_FILE "${CMAKE_CURRENT_BINARY_DIR}/${slice_file_name}.h")
	set(GENERATED_PY_FILE "${CMAKE_CURRENT_BINARY_DIR}/${slice_file_name}_ice.py")
	set(COPIED_PY_FILE "${RUMINATE_BINARY_DIR}/python/${slice_file_name}_ice.py")

	add_custom_command(
		OUTPUT "${GENERATED_CPP_FILE}" "${GENERATED_H_FILE}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMAND "${ICE_SLICE2CPP}"
		ARGS -I "${ICE_SLICE_DIR}" ${SLICE_OPTIONS} -I "${CMAKE_CURRENT_SOURCE_DIR}" "${slice_file}"
		COMMENT "Compiling ${slice_file_with_extension} to c++"
		MAIN_DEPENDENCY "${slice_file}"
	)

	add_custom_command(
		OUTPUT "${GENERATED_PY_FILE}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMAND "${ICE_SLICE2PY}"
		ARGS -I "${ICE_SLICE_DIR}" ${SLICE_OPTIONS} -I "${CMAKE_CURRENT_SOURCE_DIR}" "${slice_file}"
		COMMENT "Compiling ${slice_file_with_extension} to python"
		MAIN_DEPENDENCY "${slice_file}"
		DEPENDS "${PREVIOUS_SLICE2PY_OUTPUT_TARGET}"
	)

	add_custom_command(
		OUTPUT "${COPIED_PY_FILE}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMAND "${CMAKE_COMMAND}"
		ARGS -E copy "${GENERATED_PY_FILE}" "${COPIED_PY_FILE}"
		MAIN_DEPENDENCY "${GENERATED_PY_FILE}"
	)

	set(SLICE2PY_OUTPUT_TARGET "generate_${slice_file_name}")
	add_custom_target("${SLICE2PY_OUTPUT_TARGET}" DEPENDS "${GENERATED_PY_FILE}")

	set(SLICE2PY_COPIED_TARGET "copy_${slice_file_name}")
	add_custom_target("${SLICE2PY_COPIED_TARGET}" DEPENDS "${SLICE2PY_OUTPUT_TARGET}" "${COPIED_PY_FILE}")

	set(PREVIOUS_SLICE2PY_OUTPUT_TARGET "${SLICE2PY_OUTPUT_TARGET}")

	list(APPEND SLICE2CPP_OUTPUT_FILES "${GENERATED_CPP_FILE}" "${GENERATED_H_FILE}")
	list(APPEND SLICE2PY_OUTPUT_TARGETS "${SLICE2PY_OUTPUT_TARGET}")
	list(APPEND SLICE2PY_COPIED_TARGETS "${SLICE2PY_COPIED_TARGET}")
endforeach(slice_file_with_extension ${SLICE_FILES})

add_custom_command(
	OUTPUT "${SLICE_PY_MODULE_COPIED}"
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	COMMAND "${CMAKE_COMMAND}"
	ARGS -E make_directory "${SLICE_PY_MODULE_COPIED_DIR}"
	COMMAND "${CMAKE_COMMAND}"
	ARGS -E copy "${SLICE_PY_MODULE}" "${SLICE_PY_MODULE_COPIED}"
	DEPENDS ${SLICE2PY_OUTPUT_TARGETS}
)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${SLICE_PY_MODULE}")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${SLICE_PY_MODULE_DIR}")

add_custom_target(generate_ice_cpp DEPENDS ${SLICE2CPP_OUTPUT_FILES})
add_custom_target(generate_ice_py DEPENDS "${PREVIOUS_SLICE2PY_OUTPUT_TARGET}" ${SLICE2PY_OUTPUT_TARGETS} "${SLICE_PY_MODULE}")
add_custom_target(copy_ice_py DEPENDS ${SLICE2PY_COPIED_TARGETS} "${SLICE_PY_MODULE_COPIED}")

set(SLICE2CPP_OUTPUT_FILES "${SLICE2CPP_OUTPUT_FILES}" PARENT_SCOPE)
