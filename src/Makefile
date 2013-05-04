.PHONY: src/all src/clean src/depclean

src/clean:
	$(RM) $(SRC_OBJECTS)

src/depclean:
	$(RM) $(SRC_DEPS)

$(SRC_DEPS): $(ICE_HEADERS)

-include $(SRC_DEPS)
