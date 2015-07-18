init:
	@mkdir -p $(OBJ_DIR)

build: $(OBJ_FILES)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	
clean:
	rm -rf $(OBJ_DIR)
