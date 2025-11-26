cmake_minimum_required(VERSION 3.14)

project(chess)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "Release C++ flags" FORCE)


# Use modern CMake for include directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

include(FetchContent)

set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

FetchContent_MakeAvailable(SFML)


# Collect all .cpp files from src/
file(GLOB SOURCES "src/*.cpp")

# Define the executables
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)

# Create output directories if they don't exist
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/debug)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/release)

# Use modern target_include_directories
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

# Set output directories for different build types
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/release
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/build/relwithdebinfo
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/build/minsizerel
)
