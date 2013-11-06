.PHONY: doc/html doc/doc

doc/doc: doc/html

doc/html: $(DOXYFILE) $(PUBLIC_HEADERS) $(DOXYDEPS)
	( \
		cat $<; \
		echo "INPUT += $(PUBLIC_HEADERS)" \
	) | $(DOXYGEN) -
