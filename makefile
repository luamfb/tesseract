LIBGLAD_SRC_PATH=glad/src
LIBGLAD_INCLUDE_PATH=glad/include
GLAD_OBJ := glad.o
GLAD_SRC := $(LIBGLAD_SRC_PATH)/glad.c

GLM_INCLUDE_PATH=glm

BIN := bin
OBJ := main.o shader.o
SRC_PATH := src
INCLUDE_PATH := include

LDLIBS := $(shell pkg-config --libs glfw3 gl)
CFLAGS := -I$(INCLUDE_PATH) -I$(LIBGLAD_INCLUDE_PATH) -I$(GLM_INCLUDE_PATH)
CXXFLAGS := $(CFLAGS)

all : $(BIN) $(OBJ)

$(BIN) : $(OBJ) $(GLAD_OBJ)
	$(CXX) -o $@ $^ $(LDLIBS)

$(GLAD_OBJ) : $(GLAD_SRC)
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ) : %.o: $(SRC_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

clean :
	rm -f $(OBJ) $(GLAD_OBJ) $(SHADER_OBJ)

.PHONY: all clean
