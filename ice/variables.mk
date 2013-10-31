ICE_SOURCES := \
	debugger.ice \
	type.ice \
	debugger_factory.ice \
	exceptions.ice \
	type_member.ice \
	frame.ice \
	debugee.ice
ICE_SOURCES := $(ICE_SOURCES:%=$(CURDIR)/%)

ICE_CPP_DEPS := $(ICE_SOURCES:.ice=.cpp.d)
ICE_PY_DEPS := $(ICE_SOURCES:.ice=.py.d)
ICE_DEPS := $(ICE_CPP_DEPS) $(ICE_PY_DEPS)

ICE_CPP_SOURCES := $(ICE_SOURCES:.ice=.cpp)
ICE_PY_SOURCES := $(ICE_SOURCES:%.ice=%_ice.py)
ICE_HEADERS := $(ICE_SOURCES:.ice=.h)
ICE_OBJECTS := $(ICE_SOURCES:.ice=.o)

ICE_FLAGS_SLICE := -I$(ICEDIR) --output-dir $(ICEDIR)

FLAGS_PREPROCESSOR := $(FLAGS_PREPROCESSOR) -I$(TOPDIR) -I$(ICEDIR)
