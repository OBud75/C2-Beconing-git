# Makefile

CXX = g++
CXXFLAGS = -Wall -I$(INCLUDE_DIR)
LDFLAGS = -lcurl
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp) main.cpp
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
EXECUTABLE = $(BIN_DIR)/main

all: directories $(EXECUTABLE)

directories:
	mkdir -p $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJ_FILES)
	$(CXX) $^ -o $@ $(LDFLAGS)
	mv $(EXECUTABLE) .

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)  main

move:
	mv $(EXECUTABLE) .

.PHONY: all clean move
