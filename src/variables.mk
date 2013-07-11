SRC_SOURCES := rumination.cpp type.cpp gettid.c errors.c
SRC_SOURCES := $(SRC_SOURCES:%=$(CURDIR)/%)

SRC_OBJECTS := $(SRC_SOURCES:.c=.o)
SRC_OBJECTS := $(SRC_OBJECTS:.cpp=.o)

SRC_DEPS := $(SRC_OBJECTS:.o=.d)

ALL_OBJECTS := $(ALL_OBJECTS) $(SRC_OBJECTS)
