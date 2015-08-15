build: init $(OBJ_FILES)
	ar rcs $(LIBS_DIR)/lib$(ARCHIVE).a $(OBJ_FILES)

include $(ROOT)/mak/common.mak
