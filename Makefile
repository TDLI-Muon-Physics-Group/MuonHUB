BASE_DIR=${PWD}

# Compiler
CXX = $(shell root-config --cxx)
#CXX = $(shell /lustre/collider/siew/root/install/bin/root-config --cxx)

# Directories
SRC_DIR = src
PLUGIN_DIR = plugins
BUILD_DIR = build
MISC_DIR = misc

# Compiler flags
# Include directories where header files are located
INCLUDE_DIRS = -I$(ROOTSYS)/include -I./$(SRC_DIR) -I./$(PLUGIN_DIR) -I./$(MISC_DIR)
CXXFLAGS = $(INCLUDE_DIRS) -std=c++17 $(shell root-config --cflags) -g -O3 -Wall -Wextra -Wpedantic -shared -fPIC -Wno-unused-variable

# Linker flags
LDFLAGS = $(shell root-config --libs)

# Executable name
TARGET = build/uconvert

# Find all source and header files
SRCS = $(shell find $(SRC_DIR) $(PLUGIN_DIR)  -name "*.cpp")
HEADERS = $(shell find $(SRC_DIR) $(PLUGIN_DIR) $(MISC_DIR) -name "*.h")
DICTHEADERS = $(shell find $(PLUGIN_DIR) $(SRC_DIR) -name "*.h")

# Object files
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter-out %.cxx,$(SRCS)))
DICT_OBJ = $(BUILD_DIR)/Dict.o

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)
	mkdir -p $(BUILD_DIR)/$(PLUGIN_DIR)

# Generate Dict.cxx and PCM file
#Dict.cxx: LinkDef.h $(DICTHEADERS) | $(BUILD_DIR)
#	@echo "Generating $@"
#	rootcling -f Dict.cxx -s $(BUILD_DIR)/Dict_rdict.pcm -c -I./$(SRC_DIR) -I./$(PLUGIN_DIR) $(DICTHEADERS) LinkDef.h

# Compile the dictionary object
#$(DICT_OBJ): Dict.cxx
#	$(CXX) $(CXXFLAGS) -c Dict.cxx -o $@

# Link the target executable
$(TARGET): $(OBJS)
	@echo "Linking $@"
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compile the source files
$(BUILD_DIR)/%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET) $(BUILD_DIR)/*.o #$(OBJS)
	rm -f Dict.o Dict.cxx Dict_rdict.pcm
	@$(MAKE) clean-artifact
	rm -r $(BASE_DIR)/$(BUILD_DIR)

# Remove only object files after successful compilation
clean-intermediate:
	rm -f $(OBJS)

clean-artifact:
	find . \( -name "*~" -o -name "__*__" -o -name "#*#" \) -print0 | xargs -0 rm -rf

# Ensure make rebuilds when new source files are added
.PHONY: force clean clean-intermediate clean-artifact
force: $(SRCS) $(HEADERS)
	$(MAKE) all
