RUMINATE_TEST_SOURCES := ruminate.c
RUMINATE_TEST_SOURCES := $(RUMINATE_TEST_SOURCES:%=$(CURDIR)/%)

RUMINATE_TEST_OBJECTS := $(RUMINATE_TEST_SOURCES:.c=.o)
RUMINATE_TEST_DEPS := $(RUMINATE_TEST_SOURCES:.c=.d)

TEST_SOURCES := main.c
TEST_SOURCES := $(TEST_SOURCES:%=$(CURDIR)/%)

TEST_OBJECTS := $(TEST_SOURCES:.c=.o)
TEST_DEPS := $(TEST_SOURCES:.c=.d)

BACKTRACE_TEST_SOURCES := util.c backtrace.c
BACKTRACE_TEST_SOURCES := $(BACKTRACE_TEST_SOURCES:%=$(CURDIR)/%)

BACKTRACE_TEST_OBJECTS := $(BACKTRACE_TEST_SOURCES:.c=.o)
BACKTRACE_TEST_DEPS := $(BACKTRACE_TEST_SOURCES:.c=.d)

TEST_FLAGS_PREPROCESSOR := -I$(CURDIR)
