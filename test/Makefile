.PHONY: test/all test/clean test/depclean

test/all: test/ruminate.exe

test/ruminate.exe: $(ALL_OBJECTS)
test/ruminate.exe: EXE_OBJECTS := $(ALL_OBJECTS)

test/clean:
	$(RM) $(TEST_OBJECTS)

test/depclean:
	$(RM) $(TEST_DEPS)

-include $(TEST_DEPS)

$(TEST_DEPS): $(ICE_HEADERS)
