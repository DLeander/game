# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g -std=c++11

# Include paths
INCLUDES = -Iinclude -Iinclude/glm -I/usr/include/GLFW -I/usr/include/boost -Iinclude/server

# Source files
CPP_SRCS = main.cpp src/server/server.cpp
C_SRCS = src/glad/gl.c

# Object files
CPP_OBJS = $(CPP_SRCS:.cpp=.o)
C_OBJS = $(C_SRCS:.c=.o)

# Libraries
# For linus use -lglfw -ldl -lGL
# For mac use -lglfw -framework OpenGL
LDFLAGS = -lglfw -ldl -lGL
# LDFLAGS = -lglfw -framework OpenGL

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
