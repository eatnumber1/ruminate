UTIL_SOURCES := \
	gettid.c \
	errors.c \
	common.cpp \
	string.cpp \
	type.cpp \
	builtin_type.cpp \
	array_type.cpp \
	pointer_type.cpp \
	typedef_type.cpp \
	function_type.cpp \
	aggregate_type.cpp \
	enum_member.cpp \
	aggregate_member.cpp \
	type_member.cpp \
	frame.cpp \
	ruminate.cpp
UTIL_SOURCES := $(UTIL_SOURCES:%=$(CURDIR)/%)

UTIL_PKG_CONFIG_IN := ruminate.pc.in
UTIL_PKG_CONFIG_IN := $(UTIL_PKG_CONFIG_IN:%=$(CURDIR)/%)
UTIL_PKG_CONFIG_FILES := $(UTIL_PKG_CONFIG_IN:.pc.in=.pc)

UTIL_OBJECTS := $(UTIL_SOURCES:.c=.o)
UTIL_OBJECTS := $(UTIL_OBJECTS:.cpp=.o)

UTIL_DEPS := $(UTIL_OBJECTS:.o=.d)

UTIL_FLAGS_LINKER := $(UTIL_LIBRARIES) -lIce -lIceUtil -lpthread
UTIL_FLAGS_PREPROC_AND_COMPILER := $(UTIL_FLAGS_PREPROC_AND_COMPILER)

LIBRUMINATE := $(CURDIR)/libruminate.$(SO_SUFFIX)
LIBRUMINATE_FLAGS_LINKER := -L$(CURDIR) -lruminate

include $(TOPDIR)/mk/compile_flags.mk
