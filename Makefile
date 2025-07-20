UNAME_S := $(shell uname -s)

ifeq ($(OS),Windows_NT)
    PLATFORM := WINDOWS
else ifeq ($(UNAME_S),Linux)
    PLATFORM := LINUX
else ifeq ($(UNAME_S),Darwin)
    $(error macOS is not supported by this Makefile)
else
    $(error Unknown OS: $(UNAME_S))
endif

$(info Platform: $(PLATFORM))

CC = gcc
CFLAGS = -Wall -g -DGLEW_STATIC -DGLFW_INCLUDE_NONE

ifeq ($(PLATFORM), WINDOWS)
	LIB=-L./lib -lglew32 -lglfw3 -lfreetype -lgdi32 -lopengl32
else ifeq ($(PLATFORM), LINUX)
	# Assumes you have the necessary libraries installed
	LIB=-lGLEW -lglfw -lfreetype -lGL -lm
endif

INC=-I./include -I./include/freetype

SRC_DIR = src
BUILD_DIR = build
RES_DIR = res

SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/**/**/*.c)
HEADERS = $(wildcard $(SRC_DIR)/*.h) $(wildcard $(SRC_DIR)/**/*.h) $(wildcard $(SRC_DIR)/**/**/*.h)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

ifeq ($(PLATFORM), WINDOWS)
	TARGET_EXEC = $(BUILD_DIR)/chess.exe
else ifeq ($(PLATFORM), LINUX)
	TARGET_EXEC = $(BUILD_DIR)/chess.out
endif

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
