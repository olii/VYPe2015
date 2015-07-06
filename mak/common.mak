build: $(OBJ_FILES)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
