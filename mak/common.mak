init:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LIBS_DIR)

$(OBJ_FILES): $(DEPENDS)
$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(CLEAN_FILES)
