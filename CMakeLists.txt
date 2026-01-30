cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0069 NEW)

project(chess)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG" CACHE STRING "Release C++ flags" FORCE)
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release C++ flags" FORCE)
endif()
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# Optional "ReleaseExtremeOptimization" config (manual use only).
set(CHESS_EXTREME_CONFIG "ReleaseExtremeOptimization")
if(CMAKE_CONFIGURATION_TYPES)
    list(FIND CMAKE_CONFIGURATION_TYPES "${CHESS_EXTREME_CONFIG}" _extreme_index)
    if(_extreme_index EQUAL -1)
        list(APPEND CMAKE_CONFIGURATION_TYPES "${CHESS_EXTREME_CONFIG}")
        list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "" FORCE)
    endif()
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASEEXTREMEOPTIMIZATION "/O2 /Ot /GL /fp:fast /arch:AVX2 /DNDEBUG" CACHE STRING "ReleaseExtremeOptimization C++ flags" FORCE)
else()
    set(CMAKE_CXX_FLAGS_RELEASEEXTREMEOPTIMIZATION "-Ofast -DNDEBUG -flto -march=native -mtune=native -ffast-math -funroll-loops" CACHE STRING "ReleaseExtremeOptimization C++ flags" FORCE)
endif()
set(CMAKE_EXE_LINKER_FLAGS_RELEASEEXTREMEOPTIMIZATION "" CACHE STRING "ReleaseExtremeOptimization linker flags" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_RELEASEEXTREMEOPTIMIZATION "" CACHE STRING "ReleaseExtremeOptimization shared linker flags" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS_RELEASEEXTREMEOPTIMIZATION "" CACHE STRING "ReleaseExtremeOptimization module linker flags" FORCE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASEEXTREMEOPTIMIZATION ON)


# Use modern CMake for include directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

option(CHESS_VENDORED_SDL2 "Build SDL2 from source via FetchContent" ON)

if(EMSCRIPTEN)
    # When targeting WebAssembly, SDL2 is provided by Emscripten's ports.
    set(CHESS_VENDORED_SDL2 OFF CACHE BOOL "" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    message(STATUS "Configuring for Emscripten/WebAssembly (using SDL2 port)")
elseif(CHESS_VENDORED_SDL2)
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
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASEEXTREMEOPTIMIZATION TRUE)

if(EMSCRIPTEN)
    # Use SDL2 via Emscripten ports and bundle assets for runtime loading.
    # Note: -sUSE_SDL must be present on both compile and link steps.
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE
        "-sUSE_SDL=2"
    )
    target_link_options(
        ${PROJECT_NAME}
        PRIVATE
        "-sUSE_SDL=2"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFORCE_FILESYSTEM=1"
        "--preload-file" "${CMAKE_SOURCE_DIR}/assets@/assets"
    )
else()
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
endif()

# Create output directories if they don't exist
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/debug)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/release)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/wasm)

# Use modern target_include_directories
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

if(EMSCRIPTEN)
    # Single-config generators are typical for Emscripten; keep output simple.
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/wasm
    )
else()
    # Set output directories for different build types
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/debug
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/release
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/build/relwithdebinfo
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/build/minsizerel
        RUNTIME_OUTPUT_DIRECTORY_RELEASEEXTREMEOPTIMIZATION ${CMAKE_SOURCE_DIR}/build/releaseextremeoptimization
    )
endif()
