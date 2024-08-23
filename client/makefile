# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g -std=c++11

# Include paths
INCLUDES = -Iinclude -Iinclude/glm -Iinclude/stb -I/usr/include/GLFW -I/usr/include/boost -Iinclude/client -Iinclude/shader -Iinclude/objects -Iinclude/texture

# Source files
CPP_SRCS = main.cpp src/stb/stb.cpp src/client/client.cpp src/shader/shaderClass.cpp src/objects/EBO.cpp src/objects/VAO.cpp src/objects/VBO.cpp src/client/camera.cpp src/client/player.cpp src/client/client_network.cpp src/texture/texture.cpp
C_SRCS = src/glad/gl.c

# Object files
CPP_OBJS = $(CPP_SRCS:.cpp=.o)
C_OBJS = $(C_SRCS:.c=.o)

# Libraries
LDFLAGS = -lglfw -ldl -lGL

# Output executable
TARGET = run

# Build target
$(TARGET): $(CPP_OBJS) $(C_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(CPP_OBJS) $(C_OBJS) $(LDFLAGS)

# Compile C++ source files
%.o: %.cpp clean
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile C source files
%.o: %.c clean
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean target
.PHONY: clean
clean:
	rm -f $(TARGET) $(CPP_OBJS) $(C_OBJS)
