.PHONY: doc/doc doc/clean doc/doc

doc/all: doc/doc

doc/clean:
	$(RM) -r doc/html

doc/doc: doc/html

doc/html: $(DOXYFILE) $(PUBLIC_HEADERS) $(DOXYDEPS)
	( \
		cat $<; \
		echo "INPUT += $(PUBLIC_HEADERS)" \
	) | $(DOXYGEN) -
