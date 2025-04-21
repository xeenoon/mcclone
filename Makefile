# Compiler
CXX = g++

# Source files (all .cpp and .c files in src directory)
SRC = $(wildcard src/*.c) # Compile all .c files

# Include directory (replace with absolute path)
INCLUDE_DIR = $(HOME)/Downloads/tutorial/include

# Output executable
OUTFILE = outfile

# Compiler flags
CXXFLAGS = -I$(INCLUDE_DIR)
CXXFLAGS += -g

# Libraries
LIBS = -lglfw -ldl -lGL

# Default target
all: $(OUTFILE)

# Build the executable
$(OUTFILE): $(SRC)
	$(CXX) $(SRC) -o $(OUTFILE) $(CXXFLAGS) $(LIBS)

# Clean up build files
clean:
	rm -f $(OUTFILE)

.PHONY: all clean
