# TODO: GMSL http://gmsl.sourceforge.net/ can simplify things
CC := clang
CXX := clang++
SLICE2CPP := slice2cpp
SLICE2PY := slice2py

PREFIX ?= /usr/local

PKG_CONFIG ?= pkg-config
INSTALL ?= install
SED ?= sed
MKDIR ?= mkdir
FLOCK ?= flock
LN ?= ln -f
CP ?= cp
PYTHON ?= python
CHMOD ?= chmod

SUBDIRS := ice include src test doc include/ruminate python

CSCOPE_FILES := cscope.out cscope.po.out cscope.in.out

NULL :=
SPACE := $(NULL) $(NULL)

TOPDIR := $(PWD)
INCDIR := $(TOPDIR)/include
SRCDIR := $(TOPDIR)/src
ICEDIR := $(TOPDIR)/ice
PYDIR := $(TOPDIR)/python

SLICE2PY_LOCKFILE := $(TOPDIR)/.slice2py-lock

FLAGS_ALL := $(FLAGS_ALL) -ferror-limit=3
FLAGS_PREPROC_AND_COMPILER := $(FLAGS_PREPROC_AND_COMPILER) -Wall -Wextra -Wnonnull -Werror -pedantic
FLAGS_PREPROC_AND_COMPILER_CXX := $(FLAGS_PREPROC_AND_COMPILER_CXX) -std=c++98
FLAGS_PREPROC_AND_COMPILER_C := $(FLAGS_PREPROC_AND_COMPILER_C) -std=c11
FLAGS_COMPILER := $(FLAGS_COMPILER) -g -fno-omit-frame-pointer -O0 -fno-optimize-sibling-calls -fPIC -fvisibility=hidden -pipe

FLAGS_PREPROC_AND_COMPILER := $(FLAGS_PREPROC_AND_COMPILER) $(shell $(PKG_CONFIG) --cflags 'glib-2.0 >= 2.38')
FLAGS_LINKER := $(FLAGS_LINKER) $(shell $(PKG_CONFIG) --libs 'glib-2.0 >= 2.38')

ifeq ($(shell uname),Darwin)
SO_SUFFIX := dylib
#SO_LINK_FLAGS := $(FLAGS_LINKER) -dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/libfoo.1.dylib
#SO_LINK_FLAGS := $(FLAGS_LINKER) -dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0
SO_LINK_FLAGS := $(FLAGS_LINKER) -dynamiclib
else
SO_SUFFIX := so
SO_LINK_FLAGS := $(FLAGS_LINKER) -shared -Wl,-no-undefined
endif

-include config.mk

CLEAN_TARGETS := $(SUBDIRS:=/clean)
DEPCLEAN_TARGETS := $(SUBDIRS:=/depclean)
ALL_TARGETS := $(SUBDIRS:=/all)
INSTALL_TARGETS := $(SUBDIRS:=/install)
TEST_TARGETS := $(SUBDIRS:=/test)
DOC_TARGETS := $(SUBDIRS:=/doc)

.PHONY: all clean depclean cscope pristine cscope-clean install test doc
.DEFAULT_GOAL: all

all: $(ALL_TARGETS)
depclean: clean $(DEPCLEAN_TARGETS)
pristine: depclean cscope-clean
install: $(INSTALL_TARGETS)
test: $(TEST_TARGETS)
doc: $(DOC_TARGETS)

clean: $(CLEAN_TARGETS)
	$(RM) $(SLICE2PY_LOCKFILE)

cscope-clean:
	$(RM) $(CSCOPE_FILES)

cscope:
	$(CSCOPE) -R -b -q

# Define default hooks so a subdir doesn't need to define them.
$(CLEAN_TARGETS):
$(DEPCLEAN_TARGETS):
$(ALL_TARGETS):
$(INSTALL_TARGETS):
$(TEST_TARGETS):
$(DOC_TARGETS):

define variableRule
 CURDIR := $$(TOPDIR)/$$$(1)
 CURDIR_SHORT := $$$(1)
 COMPILE_FLAGS_PREFIX := $(shell echo "$$$(1)" | tr '[:lower:]' '[:upper:]' | tr '/' '_')_
 include $$(CURDIR)/variables.mk
endef
$(foreach subdir, $(SUBDIRS), $(eval $(call variableRule, $(subdir))))
CURDIR := $(TOPDIR)
CURDIR_SHORT :=
COMPILE_FLAGS_PREFIX :=

include $(TOPDIR)/mk/compile_flags.mk

define subst_vars
$(SED) -e "s,@prefix@,$(PREFIX:,=\,),g" $< > $@ || $(RM) $@
endef

# This defines the following for every dir in SUBDIRS:
#   Sets CURDIR to the $(TOPDIR)/$(dir)
#   Includes a makefile in $(CURDIR)/Makefile
define subdirRule
 CURDIR := $$(TOPDIR)/$$$(1)
 CURDIR_SHORT := $$$(1)
 $$$(1)/all: CURDIR := $$(CURDIR)
 $$$(1)/clean: CURDIR := $$(CURDIR)
 $$$(1)/depclean: CURDIR := $$(CURDIR)
 $$$(1)/install: CURDIR := $$(CURDIR)
 $$$(1)/doc: CURDIR := $$(CURDIR)
 $$$(1)/test: CURDIR := $$(CURDIR)
 include $$(CURDIR)/Makefile
endef
# This is what actually does the work.
# The "call" command replaces every $(1) variable reference in subdirRule with $(subdir)
# The "eval" command parses the result of the "call" command as make syntax
$(foreach subdir, $(SUBDIRS), $(eval $(call subdirRule, $(subdir))))
# Reset CURDIR back to what it should be.
CURDIR := $(TOPDIR)
CURDIR_SHORT :=
COMPILE_FLAGS_PREFIX :=

%.exe:
	$(CXX) -o $@ $(EXE_OBJECTS) $(FLAGS_LINKER)

%.d: %.c
	$(CC) -MM -MQ $(@:.d=.o) -MQ $@ -MF $*.d $< $(FLAGS_PREPROCESSOR_C) $(FLAGS_PREPROC_AND_COMPILER_C) -w

%.d: %.cpp
	$(CXX) -MM -MQ $(@:.d=.o) -MQ $@ -MF $*.d $< $(FLAGS_PREPROCESSOR_CXX) $(FLAGS_PREPROC_AND_COMPILER_CXX) -w

%_ice.py.d: %.ice
	( $(SLICE2PY) --depend $< $(FLAGS_SLICE2PY) | $(SED) -e '1 s,\([^ ]\+[ :]\),$(@D)/\1,g' -e '1 s,\([^ ]\+\)\.py[ :],\1.py.d &,1' > $@ ) || $(RM) $@

%.cpp.d: %.ice
	( $(SLICE2CPP) --depend $< $(FLAGS_SLICE2CPP) | $(SED) -e '1 s,\([^ ]\+[ :]\),$(@D)/\1,g' -e '1 s,\([^ ]\+\).h[ :],\1.cpp.d &,1' > $@ ) || $(RM) $@

%.o: %.c
	$(CC) -c -o $@ $< $(FLAGS_PREPROCESSOR_C) $(FLAGS_COMPILER_C) $(FLAGS_PREPROC_AND_COMPILER_C)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(FLAGS_PREPROCESSOR_CXX) $(FLAGS_COMPILER_CXX) $(FLAGS_PREPROC_AND_COMPILER_CXX)

%.cpp %.h: %.ice
	$(SLICE2CPP) $(FLAGS_SLICE2CPP) $<

%_ice.py: %.ice
	$(FLOCK) $(SLICE2PY_LOCKFILE) -c "$(SLICE2PY) $(FLAGS_SLICE2PY) $<"

%.$(SO_SUFFIX):
	$(CXX) $(SO_LINK_FLAGS) -o $@ $(SO_OBJECTS) $(FLAGS_LINKER)

%: %.in
	$(subst_vars)
