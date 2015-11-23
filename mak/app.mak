build: $(DEPENDS) init $(OBJ_FILES)
	$(CXX) -o $(ROOT)/$(APP) $(OBJ_FILES) $(LIBRARIES_PATHS) $(LXXFLAGS)

include $(ROOT)/mak/common.mak
