# Compiler
CXX = gcc

# Source files (all .c files in src directory)
SRC = $(wildcard src/*.c)

# Include directory (relative to the Makefile location)
INCLUDE_DIR = $(CURDIR)/include

# Output executable
OUTFILE = outfile

# Compiler flags
CXXFLAGS = -I$(INCLUDE_DIR)
CXXFLAGS += -g

# Static/dynamic linking flags
# Attempt to statically link GLFW and math, dynamically link rest
LIBS = -lglfw -ldl -lGL -lm

# Default target
all: $(OUTFILE)

# Build the executable
$(OUTFILE): $(SRC)
	$(CXX) $(SRC) -o $(OUTFILE) $(CXXFLAGS) $(LIBS)

# Clean up build files
clean:
	rm -f $(OUTFILE)

.PHONY: all clean
