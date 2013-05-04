CC := clang
CXX := clang++
DEPGEN := gcc
SLICE2CPP := slice2cpp
SLICE2PY := slice2py

PKG_CONFIG ?= pkg-config

-include config.mk

SUBDIRS := ice include src test

CSCOPE_FILES := cscope.out cscope.po.out cscope.in.out

TOPDIR := $(PWD)
INCDIR := $(TOPDIR)/include
SRCDIR := $(TOPDIR)/src
ICEDIR := $(TOPDIR)/ice

CPPFLAGS := $(CPPFLAGS) -ggdb -fno-omit-frame-pointer -O1 -fno-optimize-sibling-calls
CFLAGS := $(CFLAGS) -fPIC -Wall -Wnonnull -std=c11 -ferror-limit=3 -Werror
CXXFLAGS := $(CXXFLAGS) -std=c++11 -Wall -Wnonnull -fPIC -ferror-limit=3 -Werror
LIBRARIES := $(LIBRARIES) -lIce -lIceUtil

GTHREAD_CPPFLAGS ?= $(shell $(PKG_CONFIG) --cflags gthread-2.0)
GTHREAD_LIBRARIES ?= $(shell $(PKG_CONFIG) --libs gthread-2.0)
GLIB_CPPFLAGS ?= $(shell $(PKG_CONFIG) --cflags glib-2.0)
GLIB_LIBRARIES ?= $(shell $(PKG_CONFIG) --libs glib-2.0)

CPPFLAGS := $(CPPFLAGS) $(GLIB_CPPFLAGS) $(GTHREAD_CPPFLAGS)
LIBRARIES := $(LIBRARIES) $(GLIB_LIBRARIES) $(GTHREAD_LIBRARIES)

CLEAN_TARGETS := $(SUBDIRS:=/clean)
DEPCLEAN_TARGETS := $(SUBDIRS:=/depclean)
ALL_TARGETS := $(SUBDIRS:=/all)

.PHONY: all clean depclean cscope pristine cscope-clean
.DEFAULT_GOAL: all

all: $(ALL_TARGETS)
clean: $(CLEAN_TARGETS)
depclean: clean $(DEPCLEAN_TARGETS)
pristine: depclean cscope-clean

cscope-clean:
	$(RM) $(CSCOPE_FILES)

cscope:
	$(CSCOPE) -R -b -q

# Define default hooks so a subdir doesn't need to define them.
$(CLEAN_TARGETS):
$(DEPCLEAN_TARGETS):
$(ALL_TARGETS):

define variableRule
 CURDIR := $$(TOPDIR)/$$$(1)
 include $$(CURDIR)/variables.mk
endef
$(foreach subdir, $(SUBDIRS), $(eval $(call variableRule, $(subdir))))

# This defines the following for every dir in SUBDIRS:
#   Sets CURDIR to the $(TOPDIR)/$(dir)
#   Includes a makefile in $(CURDIR)/Makefile
define subdirRule
 CURDIR := $$(TOPDIR)/$$$(1)
 $$$(1)/all: CURDIR := $$(CURDIR)
 $$$(1)/clean: CURDIR := $$(CURDIR)
 $$$(1)/depclean: CURDIR := $$(CURDIR)
 include $$(CURDIR)/Makefile
endef
# This is what actually does the work.
# The "call" command replaces every $(1) variable reference in subdirRule with $(subdir)
# The "eval" command parses the result of the "call" command as make syntax
$(foreach subdir, $(SUBDIRS), $(eval $(call subdirRule, $(subdir))))
# Reset CURDIR back to what it should be.
CURDIR := $(TOPDIR)

%.exe:
	$(CC) $(LDFLAGS) -o $@ $(EXE_OBJECTS) $(LIBRARIES)

%.d: %.c
	$(DEPGEN) -MM $(CPPFLAGS) -MQ $(@:.d=.o) -MQ $@ -MF $*.d $<

%.d: %.cpp
	$(DEPGEN) -MM $(CPPFLAGS) -MQ $(@:.d=.o) -MQ $@ -MF $*.d $<

%.d: %.ice
	$(SLICE2CPP) $(SLICE2CPPFLAGS) --depend $< > $@

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

%.cpp %.h: %.ice
	$(SLICE2CPP) $(SLICE2CPP_FLAGS) $<

%_ice.py: %.ice
	$(SLICE2PY) $(SLICE2PY_FLAGS) $<

# vim:tw=80
