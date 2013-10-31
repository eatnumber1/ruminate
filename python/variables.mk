PYTHON_SOURCES := \
	array_member_impl.py \
	debugger_factory_impl.py \
	debugger_impl.py \
	lldb_em.py \
	lldb_utils.py \
	stopped_thread.py \
	type_impl.py \
	type_impl_factory.py \
	type_member_impl.py
PYTHON_SOURCES := $(PYTHON_SOURCES:%=$(CURDIR)/%)

PYTHON_OBJECTS := $(PYTHON_SOURCES:.py=.pyc)

PYTHON_DBGSVR := $(CURDIR)/ruminate-dbgsvr
