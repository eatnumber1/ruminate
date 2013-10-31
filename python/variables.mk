PYTHON_SOURCES := \
	src/dbgsvr.py \
	src/debugger_impl.py \
	src/lldb_utils.py \
	src/type_member_impl.py \
	src/debugger_factory_impl.py \
	src/lldb_em.py \
	src/type_impl.py

PYTHON_OBJECTS := $(PYTHON_SOURCES:%=%c)
