# App/CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
# Define the MapGenerator as a library as a console application
add_library(MapGenerator STATIC)

# Set the language and C++ standard
set_target_properties(MapGenerator PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Specify the output directories for the compiled library and object files
if(CMAKE_GENERATOR_PLATFORM)
    set(OutputDir "${CMAKE_SYSTEM_NAME}-${CMAKE_GENERATOR_PLATFORM}/$<CONFIG>")
else()
    set(OutputDir "${CMAKE_SYSTEM_NAME}/$<CONFIG>")
endif()

set_target_properties(MapGenerator PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Binaries/${OutputDir}/MapGenerator"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Binaries/${OutputDir}/MapGenerator"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Binaries/${OutputDir}/MapGenerator"
    OBJECT_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Binaries/Intermediates/${OutputDir}/MapGenerator"
)

target_include_directories(MapGenerator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source")

# Specify the source files
file(GLOB_RECURSE GENERATOR_SOURCES "source/*.cpp" "source/*.h" "source/*.hpp")
target_sources(MapGenerator PRIVATE ${GENERATOR_SOURCES})


# Platform-specific settings for Windows
if(WIN32)
    target_compile_definitions(MapGenerator PRIVATE "_WINDOWS")
    set_target_properties(MapGenerator PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"  # Matches /MD for Release, /MDd for Debug
    )
endif()

add_subdirectory(thirdparty/)
target_include_directories(MapGenerator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/")
# Add Libraries


# Configuration-specific settings
# Debug configuration
target_compile_definitions(MapGenerator PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)
target_compile_options(MapGenerator PRIVATE
    $<$<CONFIG:Debug>:-g>
)
set_target_properties(MapGenerator PROPERTIES
    DEBUG_POSTFIX "d"
)

# Release configuration
target_compile_definitions(MapGenerator PRIVATE
    $<$<CONFIG:Release>:RELEASE>
)
set_target_properties(MapGenerator PROPERTIES
    OPTIMIZE_POSTFIX "opt"
)

# Distribution configuration
target_compile_definitions(MapGenerator PRIVATE
    $<$<CONFIG:Dist>:DIST>
)
target_compile_options(MapGenerator PRIVATE
    $<$<CONFIG:Dist>:-O3>
    $<$<CONFIG:Dist>:-DNDEBUG>
)
set_target_properties(MapGenerator PROPERTIES
    DIST_POSTFIX "dist"
)