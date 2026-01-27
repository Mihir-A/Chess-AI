cmake_minimum_required(VERSION 3.14)

project(chess)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "Release C++ flags" FORCE)


# Use modern CMake for include directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

option(CHESS_VENDORED_SDL2 "Build SDL2 from source via FetchContent" ON)

if(CHESS_VENDORED_SDL2)
    include(FetchContent)

    # Keep the SDL build minimal and self-contained.
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST OFF CACHE BOOL "" FORCE)
    set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG SDL2
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL2)
else()
    # Fallback to a system SDL2 if the user prefers it.
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
endif()


# Collect all .cpp files from src/
file(GLOB SOURCES "src/*.cpp")

# Define the executables
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# SDL2main may be required on some platforms (e.g., Windows GUI apps).
if(TARGET SDL2::SDL2main)
    # SDL2main must appear before SDL2 on the link line.
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
endif()

if(TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
elseif(TARGET SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static)
else()
    message(FATAL_ERROR "SDL2 target not found. Disable CHESS_VENDORED_SDL2 or install SDL2.")
endif()

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
