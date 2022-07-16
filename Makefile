CC = g++
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
BUILD_DIR = build
OBJ_NAME = render
INCLUDE_PATHS = -Iinclude
COMPILER_FLAGS = -std=c++11 -O0 -g

all:
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATHS) $(SRC_FILES) -o $(BUILD_DIR)/$(OBJ_NAME)

clean:
	rm $(BUILD_DIR)/$(OBJ_NAME)
