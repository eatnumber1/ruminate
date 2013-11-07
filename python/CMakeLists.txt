set(PYTHON_FILES
	debugger_factory_impl.py
	debugger_impl.py
	lldb_em.py
	lldb_utils.py
	stopped_thread.py
	type_impl.py
	type_impl_factory.py
	type_member_impl.py
)

foreach(python_file_src ${PYTHON_FILES})
	get_filename_component(python_file_name "${python_file_src}" NAME)
	set(python_file_dest ${CMAKE_CURRENT_BINARY_DIR}/${python_file_name})

	add_custom_command(
		OUTPUT "${python_file_dest}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		COMMAND "${CMAKE_COMMAND}"
		ARGS -E copy "${python_file_src}" "${python_file_dest}"
		MAIN_DEPENDENCY "${python_file_src}"
	)

	list(APPEND PYTHON_COPIED_FILES "${python_file_dest}")
endforeach(python_file_src ${PYTHON_FILES})

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/ruminate-dbgsvr.in"
	"${CMAKE_CURRENT_BINARY_DIR}/ruminate-dbgsvr"
	@ONLY
)

list(APPEND PYTHON_COPIED_FILES "${CMAKE_CURRENT_BINARY_DIR}/ruminate-dbgsvr")

add_custom_target(copy_py_files DEPENDS copy_ice_py ${PYTHON_COPIED_FILES})

find_program(PYTHON python)

if(NOT PYTHON)
	MESSAGE(FATAL_ERROR "Python not found")
endif(NOT PYTHON)

set(SETUP_PY "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/build/timestamp")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in" ${SETUP_PY} @ONLY)

add_custom_command(
	OUTPUT ${OUTPUT}
	COMMAND ${PYTHON} ${SETUP_PY} -q build
	COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
	DEPENDS copy_py_files
)

add_custom_target(build_ruminate_backend ALL DEPENDS ${OUTPUT})

install(CODE "execute_process(
	COMMAND ${PYTHON} ${SETUP_PY} install --prefix ${CMAKE_INSTALL_PREFIX}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)")
