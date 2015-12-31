build: $(DEPENDS) init $(OBJ_FILES)
	$(CXX) -static -o $(ROOT)/$(APP) $(OBJ_FILES) $(LIBRARIES_PATHS) $(LXXFLAGS)

include $(ROOT)/mak/common.mak
