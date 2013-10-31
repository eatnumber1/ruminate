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
ICE_PY_DEPS := $(ICE_SOURCES:.ice=_ice.py.d)
ICE_DEPS := $(ICE_CPP_DEPS) $(ICE_PY_DEPS)

ICE_CPP_SOURCES := $(ICE_SOURCES:.ice=.cpp)
ICE_PY_SOURCES := $(ICE_SOURCES:%.ice=%_ice.py)
ICE_HEADERS := $(ICE_SOURCES:.ice=.h)
ICE_OBJECTS := $(ICE_SOURCES:.ice=.o)

ICE_FLAGS_SLICE := -I$(ICEDIR) --output-dir $(ICEDIR)

ICE_PY_MODNAME := RuminateBackend
ICE_PY_MODDIR := $(CURDIR)/$(ICE_PY_MODNAME)

ICE_IN_PYDIR_PY_SOURCES := $(ICE_PY_SOURCES:$(CURDIR)/%=$(PYDIR)/%)

ICE_IN_PYDIR_PY_MODNAME := $(ICE_PY_MODNAME)
ICE_IN_PYDIR_PY_MODDIR := $(PYDIR)/$(ICE_IN_PYDIR_PY_MODNAME)

FLAGS_PREPROCESSOR := $(FLAGS_PREPROCESSOR) -I$(TOPDIR) -I$(ICEDIR)
