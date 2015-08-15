build: init $(OBJ_FILES)

init:
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	
clean:
	rm -rf $(OBJ_DIR)
