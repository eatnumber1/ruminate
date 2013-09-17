ICE_SOURCES := \
	debugger.ice \
	type.ice \
	debugger_factory.ice \
	exceptions.ice \
	type_member.ice \
	frame.ice \
	debugee.ice
ICE_SOURCES := $(ICE_SOURCES:%=$(CURDIR)/%)

ICE_DEPS := $(ICE_DEPS:.ice=.cpp.d) $(ICE_DEPS:.ice=.py.d)

ICE_CPP_SOURCES := $(ICE_SOURCES:.ice=.cpp)
ICE_PY_SOURCES := $(ICE_SOURCES:%.ice=%_ice.py)
ICE_HEADERS := $(ICE_SOURCES:.ice=.h)
ICE_OBJECTS := $(ICE_SOURCES:.ice=.o)

ICE_FLAGS_SLICE := -I$(ICEDIR) --output-dir $(ICEDIR)

FLAGS_PREPROCESSOR := $(FLAGS_PREPROCESSOR) -I$(TOPDIR) -I$(ICEDIR)
