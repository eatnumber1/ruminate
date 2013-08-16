SRC_SOURCES := \
	gettid.c \
	errors.c \
	common.cpp \
	string.cpp \
	type.cpp \
	tag_type.cpp \
	record_type.cpp \
	record_member.cpp \
	frame.cpp \
	rumination.cpp
SRC_SOURCES := $(SRC_SOURCES:%=$(CURDIR)/%)

SRC_OBJECTS := $(SRC_SOURCES:.c=.o)
SRC_OBJECTS := $(SRC_OBJECTS:.cpp=.o)

SRC_DEPS := $(SRC_OBJECTS:.o=.d)

ALL_OBJECTS := $(ALL_OBJECTS) $(SRC_OBJECTS)
