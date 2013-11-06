SRC_SOURCES := \
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
SRC_SOURCES := $(SRC_SOURCES:%=$(CURDIR)/%)

SRC_PKG_CONFIG_IN := ruminate.pc.in
SRC_PKG_CONFIG_IN := $(SRC_PKG_CONFIG_IN:%=$(CURDIR)/%)
SRC_PKG_CONFIG_FILES := $(SRC_PKG_CONFIG_IN:.pc.in=.pc)

SRC_OBJECTS := $(SRC_SOURCES:.c=.o)
SRC_OBJECTS := $(SRC_OBJECTS:.cpp=.o)

SRC_DEPS := $(SRC_OBJECTS:.o=.d)

SRC_FLAGS_LINKER := $(SRC_LIBRARIES) -lIce -lIceUtil -lpthread
SRC_FLAGS_PREPROC_AND_COMPILER := $(SRC_FLAGS_PREPROC_AND_COMPILER) -DRUMINATE_DEBUGGER_CONTROLLER_PATH="\"$(PREFIX)/bin/ruminate-dbgsvr\""

LIBRUMINATE := $(CURDIR)/libruminate.$(SO_SUFFIX)
LIBRUMINATE_FLAGS_LINKER := -L$(CURDIR) -lruminate

include $(TOPDIR)/mk/compile_flags.mk
