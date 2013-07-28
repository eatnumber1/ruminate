.PHONY: test/all test/clean test/depclean

test/all: test/ruminate.exe test/main.exe test/backtrace.exe

test/ruminate.exe: $(ALL_OBJECTS) $(RUMINATE_TEST_OBJECTS)
test/ruminate.exe: EXE_OBJECTS := $(ALL_OBJECTS) $(RUMINATE_TEST_OBJECTS)

test/main.exe: $(ALL_OBJECTS) $(TEST_OBJECTS)
test/main.exe: EXE_OBJECTS := $(ALL_OBJECTS) $(TEST_OBJECTS)

test/backtrace.exe: $(ALL_OBJECTS) $(BACKTRACE_TEST_OBJECTS)
test/backtrace.exe: EXE_OBJECTS := $(ALL_OBJECTS) $(BACKTRACE_TEST_OBJECTS)

test/clean:
	$(RM) $(TEST_OBJECTS) $(RUMINATE_TEST_OBJECTS) $(BACKTRACE_TEST_OBJECTS)

test/depclean:
	$(RM) $(TEST_DEPS) $(RUMINATE_TEST_DEPS) $(BACKTRACE_TEST_DEPS)

-include $(TEST_DEPS) $(RUMINATE_TEST_DEPS) $(BACKTRACE_TEST_DEPS)

$(RUMINATE_TEST_DEPS) $(TEST_DEPS) $(BACKTRACE_TEST_DEPS): $(ICE_HEADERS)
