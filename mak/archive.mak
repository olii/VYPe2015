build: $(DEPENDS) init $(OBJ_FILES)
	ar rcs $(LIBS_DIR)/lib$(ARCHIVE).a $(OBJ_FILES) $(LIBRARIES_PATHS)

include $(ROOT)/mak/common.mak
