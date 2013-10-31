.PHONY: python/clean python/all python/install

python/all: $(ICE_IN_PYDIR_PY_SOURCES) $(ICE_IN_PYDIR_PY_MODDIR)

python/clean:
	$(RM) $(PYTHON_OBJECTS) $(ICE_IN_PYDIR_PY_SOURCES)
	$(RM) -r $(ICE_IN_PYDIR_PY_MODDIR) $(CURDIR)/build

python/install:
	cd $(PYDIR) && $(PYTHON) setup.py install --prefix $(PREFIX)
