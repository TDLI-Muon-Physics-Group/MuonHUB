cmake_minimum_required(VERSION 3.12)
project(uconvert)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ROOT (adjust path if needed)
#set(ROOTSYS "/lustre/collider/siew/root/install")  # Modify if ROOT is elsewhere
#find_package(ROOT REQUIRED COMPONENTS Geom TMVA Eve Gui RGL EG)
find_package(ROOT REQUIRED)

# Include directories
include_directories(
    ${ROOT_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/plugins
    ${CMAKE_SOURCE_DIR}/misc
)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -Wall -Wextra -Wpedantic -shared -fPIC -Wno-unused-variable")

# Find all source files
file(GLOB_RECURSE SRCS 
    src/*.cpp 
    plugins/*.cpp
)

# Generate ROOT dictionary
#set(DICT_FILE Dict.cxx)
#set(LINKDEF_FILE LinkDef.h)
#ROOT_GENERATE_DICTIONARY(
#    ${DICT_FILE}
#    ${LINKDEF_FILE}
#    OPTIONS "-I${CMAKE_SOURCE_DIR}/src -I${CMAKE_SOURCE_DIR}/plugins"
#)

# Add executable
add_executable(gm2 ${SRCS} ${DICT_FILE})
target_link_libraries(gm2 ${ROOT_LIBRARIES})

# Output in build/bin
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
