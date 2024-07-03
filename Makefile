CC = gcc
CFLAGS = -Wall -g -DGLEW_STATIC -DGLFW_INCLUDE_NONE

LIB=-L./lib -lglew32 -lglfw3 -lcglm -lfreetype -lgdi32 -lopengl32
INC=-I./include -I./include/freetype -I./src

SRC_DIR = src
BUILD_DIR = build
RES_DIR = res

SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
HEADERS = $(wildcard $(SRC_DIR)/*.h) $(wildcard $(SRC_DIR)/**/*.h)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
TARGET_EXEC = $(BUILD_DIR)/chess.exe

dir_guard=@mkdir -p $(@D)

.phony: all clean

all: $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJECTS)
	$(dir_guard)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LIB)

#TODO: Improve recompilation strategy when headers change

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c $(HEADERS)
	$(dir_guard)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
