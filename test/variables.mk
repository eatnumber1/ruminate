TEST_SOURCES := ruminate.c
TEST_SOURCES := $(TEST_SOURCES:%=$(CURDIR)/%)

TEST_OBJECTS := $(TEST_SOURCES:.c=.o)
TEST_DEPS := $(TEST_SOURCES:.c=.d)

ALL_OBJECTS := $(ALL_OBJECTS) $(TEST_OBJECTS)