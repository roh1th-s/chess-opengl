CC = gcc
CFLAGS = -Wall -g -DGLEW_STATIC -DGLFW_INCLUDE_NONE
EMCC_FLAGS = -sUSE_GLFW=3 -sUSE_FREETYPE=1 -sFULL_ES3=1 -sALLOW_MEMORY_GROWTH=1

LIB=-L./lib -lglew32 -lglfw3 -lcglm -lfreetype -lgdi32 -lopengl32
INC=-I./include -I./include/freetype
EMCC_INC = -I./include/STB

SRC_DIR = src
BUILD_DIR = build
NATIVE_BUILD_DIR = $(BUILD_DIR)/native
WEB_BUILD_DIR = $(BUILD_DIR)/web
RES_DIR = res

SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/**/**/*.c)
HEADERS = $(wildcard $(SRC_DIR)/*.h) $(wildcard $(SRC_DIR)/**/*.h) $(wildcard $(SRC_DIR)/**/**/*.h)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(NATIVE_BUILD_DIR)/%.o, $(SOURCES))

NATIVE_TARGET = $(NATIVE_BUILD_DIR)/chess.exe
WEB_TARGET = $(WEB_BUILD_DIR)/index.html

dir_guard=@mkdir -p $(@D)

.phony: all native web clean

all: $(NATIVE_TARGET) $(WEB_TARGET)

native: $(NATIVE_TARGET)

web: $(WEB_TARGET)

$(WEB_TARGET): $(SOURCES)
	$(dir_guard)
	emcc $(SOURCES) $(EMCC_FLAGS) -o $@ $(EMCC_INC) --preload-file $(RES_DIR)

$(NATIVE_TARGET): $(OBJECTS)
	$(dir_guard)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LIB)

#TODO: Improve recompilation strategy when headers change

$(NATIVE_BUILD_DIR)/%.o : $(SRC_DIR)/%.c $(HEADERS)
	$(dir_guard)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
