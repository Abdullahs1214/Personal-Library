# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# Target executable
TARGET = psnotify

# Source files
SRC = psnotify.cc


# Default rule to build the program
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean rule to remove compiled files
clean:
	rm -f $(TARGET)
