SRC_SOURCES := \
	gettid.c \
	errors.c \
	common.cpp \
	string.cpp \
	memory.cpp \
	type.cpp \
	builtin_type.cpp \
	array_type.cpp \
	pointer_type.cpp \
	typedef_type.cpp \
	tag_type.cpp \
	function_type.cpp \
	aggregate_type.cpp \
	aggregate_member.cpp \
	type_member.cpp \
	frame.cpp \
	ruminate.cpp
SRC_SOURCES := $(SRC_SOURCES:%=$(CURDIR)/%)

SRC_OBJECTS := $(SRC_SOURCES:.c=.o)
SRC_OBJECTS := $(SRC_OBJECTS:.cpp=.o)

SRC_DEPS := $(SRC_OBJECTS:.o=.d)

SRC_FLAGS_LINKER := $(SRC_LIBRARIES) -lIce -lIceUtil

LIBRUMINATE := $(CURDIR)/libruminate.$(SO_SUFFIX)
LIBRUMINATE_FLAGS_LINKER := -L$(CURDIR) -lruminate

ifneq ($(shell uname),Darwin)
FLAGS_LINKER := $(FLAGS_LINKER) -Wl,-rpath,$(CURDIR)
endif

SRC_PYTHON := \
	src/dbgsvr.py \
	src/debugger_impl.py \
	src/lldb_utils.py \
	src/type_member_impl.py \
	src/debugger_factory_impl.py \
	src/lldb_em.py \
	src/type_impl.py

SRC_PYTHON_OBJECTS := $(SRC_PYTHON:%=%c)
