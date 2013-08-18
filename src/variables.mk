SRC_SOURCES := \
	gettid.c \
	errors.c \
	common.cpp \
	string.cpp \
	type.cpp \
	builtin_type.cpp \
	pointer_type.cpp \
	typedef_type.cpp \
	tag_type.cpp \
	record_type.cpp \
	record_member.cpp \
	frame.cpp \
	rumination.cpp
SRC_SOURCES := $(SRC_SOURCES:%=$(CURDIR)/%)

SRC_OBJECTS := $(SRC_SOURCES:.c=.o)
SRC_OBJECTS := $(SRC_OBJECTS:.cpp=.o)

SRC_DEPS := $(SRC_OBJECTS:.o=.d)

SRC_FLAGS_LINKER := $(SRC_LIBRARIES) -lIce -lIceUtil

LIBRUMINATE_FLAGS_LINKER := -L$(CURDIR) -lruminate
LIBRUMINATE := $(CURDIR)/libruminate.$(SO_SUFFIX)

SRC_PYTHON := \
	src/dbgsvr.py \
	src/debugger_impl.py \
	src/lldb_utils.py \
	src/type_member_impl.py \
	src/debugger_factory_impl.py \
	src/lldb_em.py \
	src/type_impl.py

SRC_PYTHON_OBJECTS := $(SRC_PYTHON:%=%c)
