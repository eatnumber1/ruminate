.PHONY: python/clean python/all python/install

python/all: $(ICE_IN_PYDIR_PY_SOURCES) $(ICE_IN_PYDIR_PY_MODDIR) $(PYTHON_DBGSVR)

$(PYTHON_DBGSVR): $(PYTHON_DBGSVR_INPUT)
	$(subst_vars)
	$(CHMOD) +x $(PYTHON_DBGSVR) || $(RM) $(PYTHON_DBGSVR)

python/clean:
	$(RM) $(PYTHON_OBJECTS) $(ICE_IN_PYDIR_PY_OBJECTS) $(ICE_IN_PYDIR_PY_SOURCES) $(PYTHON_DBGSVR)
	$(RM) -r $(ICE_IN_PYDIR_PY_MODDIR) $(CURDIR)/build

python/install: $(PYTHON_SOURCES) $(ICE_IN_PYDIR_PY_SOURCES) $(ICE_IN_PYDIR_PY_MODDIR) $(PYTHON_DBGSVR)
	cd $(PYDIR) && $(PYTHON) setup.py install --prefix $(PREFIX)
