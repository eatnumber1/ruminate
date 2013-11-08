set(HEADERS
	common.h
	array_type.h
	builtin_type.h
	errors.h
	frame.h
	function_type.h
	pointer_type.h
	aggregate_member.h
	enum_member.h
	aggregate_type.h
	ruminate.h
	string.h
	type.h
	type_member.h
	typedef_type.h
)

install(FILES ${HEADERS} DESTINATION include/ruminate)

foreach(header ${HEADERS})
	list(APPEND PUBLIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
endforeach(header ${HEADERS})

set(PUBLIC_HEADERS ${PUBLIC_HEADERS} PARENT_SCOPE)
