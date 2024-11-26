cmake_minimum_required(VERSION 3.20)
project(C3DExporter)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MAX_SDK_PATH "C:/Program Files/Autodesk/3ds Max 2025 SDK/maxsdk")

# Set output extension to .dle
set(CMAKE_SHARED_LIBRARY_SUFFIX ".dle")

# Add source files
set(SOURCES
    src/C3DExporter.cpp
)

# Create shared library
add_library(C3DExporter SHARED ${SOURCES})

# Include directories
target_include_directories(C3DExporter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MAX_SDK_PATH}/include
    ${MAX_SDK_PATH}/include/maxsdk
)

# Link directories
target_link_directories(C3DExporter PRIVATE
    ${MAX_SDK_PATH}/lib/x64/Release
)

# Link libraries
target_link_libraries(C3DExporter PRIVATE
    core
    geom
    mesh
    maxutil
    paramblk2
)

# Windows-specific settings
if(WIN32)
    # Define required preprocessor macros
    target_compile_definitions(C3DExporter PRIVATE
        WIN32
        _WINDOWS
        _USRDLL
        _WINDLL
        UNICODE
        _UNICODE
    )
    
    # Set module definition file
    set_target_properties(C3DExporter PROPERTIES
        LINK_FLAGS "/DEF:\"${CMAKE_SOURCE_DIR}/src/C3DExporter.def\""
    )
endif()

# Set output directories
set_target_properties(C3DExporter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)