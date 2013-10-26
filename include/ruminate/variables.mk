INCLUDE_RUMINATE_PUBLIC_HEADERS := \
	array_type.h \
	builtin_type.h \
	errors.h \
	frame.h \
	function_type.h \
	pointer_type.h \
	aggregate_member.h \
	aggregate_type.h \
	ruminate.h \
	string.h \
	tag_type.h \
	type.h \
	type_member.h \
	typedef_type.h
INCLUDE_RUMINATE_PUBLIC_HEADERS := $(INCLUDE_RUMINATE_PUBLIC_HEADERS:%=$(CURDIR)/%)
PUBLIC_HEADERS := $(PUBLIC_HEADERS) $(INCLUDE_RUMINATE_PUBLIC_HEADERS)
