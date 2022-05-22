# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DEBUG_DIR = bin/debug

# Release build settings
EXE = $(BIN_DIR)/wish
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
CPPFLAGS = -Wall -I $(SRC_DIR) -MMD -MP

# Debug build settings
DEBUG_EXE = $(DEBUG_DIR)/wish-debug
DEBUG_OBJ = $(patsubst $(SRC_DIR)/%.c, $(DEBUG_DIR)/%.o, $(SRC))
DEBUG_FLAGS = -Wall -I $(SRC_DIR) -g

.PHONY: all debug clean prep remake

# Default build
all: prep $(EXE)

# Release rules
$(EXE): $(OBJ)
	$(CC) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) -c $< -o $@

-include $(OBJ:.o=.d)

# Debug rules
debug: $(DEBUG_EXE)

$(DEBUG_EXE): $(DEBUG_OBJ)
	$(CC) $(CPPFlags) -g $^ -o $@

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFlags) -c -g $< -o $@

-include $(DEBUG_OBJ:.o=.d)

# Other rules
clean:
	@$(RM) $(EXE) $(OBJ_DIR)/* $(DEBUG_DIR)/*

prep:
	@mkdir -p $(SRC_DIR) $(OBJ_DIR) $(BIN_DIR) $(DEBUG_DIR)

remake: clean all
