.PHONY: test/test test/clean test/depclean

test/test: test/backtrace.exe test/json.exe test/main.exe

test/json.exe: $(LIBRUMINATE) $(JSON_TEST_OBJECTS)
test/json.exe: EXE_OBJECTS := $(JSON_TEST_OBJECTS)
test/josn.exe: FLAGS_PREPROCESSOR := $(FLAGS_PREPROCESSOR) $(TEST_FLAGS_PREPROCESSOR)
test/json.exe: FLAGS_LINKER := $(FLAGS_LINKER) $(LIBRUMINATE_FLAGS_LINKER)

test/main.exe: $(LIBRUMINATE) $(TEST_OBJECTS)
test/main.exe: EXE_OBJECTS := $(TEST_OBJECTS)
test/main.exe: FLAGS_PREPROCESSOR := $(FLAGS_PREPROCESSOR) $(TEST_FLAGS_PREPROCESSOR)
test/main.exe: FLAGS_LINKER := $(FLAGS_LINKER) $(LIBRUMINATE_FLAGS_LINKER)

test/backtrace.exe: $(LIBRUMINATE) $(BACKTRACE_TEST_OBJECTS)
test/backtrace.exe: EXE_OBJECTS := $(BACKTRACE_TEST_OBJECTS)
test/backtrace.exe: FLAGS_PREPROCESSOR := $(FLAGS_PREPROCESSOR) $(TEST_FLAGS_PREPROCESSOR)
test/backtrace.exe: FLAGS_LINKER := $(FLAGS_LINKER) $(LIBRUMINATE_FLAGS_LINKER)

test/clean:
	$(RM) $(TEST_OBJECTS) $(JSON_TEST_OBJECTS) $(BACKTRACE_TEST_OBJECTS) test/main.exe test/backtrace.exe test/json.exe

test/depclean:
	$(RM) $(TEST_DEPS) $(JSON_TEST_DEPS) $(BACKTRACE_TEST_DEPS)

-include $(TEST_DEPS) $(JSON_TEST_DEPS) $(BACKTRACE_TEST_DEPS)

$(JSON_TEST_DEPS) $(TEST_DEPS) $(BACKTRACE_TEST_DEPS): $(ICE_HEADERS)
