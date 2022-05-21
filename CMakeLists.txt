cmake_minimum_required(VERSION 3.0)
project(OPENXR)

find_package(PythonInterp 3)

# Enable IDE GUI folders.  "Helper targets" that don't have interesting source code should set their FOLDER property to this
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake predefined targets")
set(LOADER_FOLDER "Loader")
set(HELPER_FOLDER "Helpers")
set(CODEGEN_FOLDER "Generated")
set(TESTS_FOLDER "Tests")
set(API_LAYERS_FOLDER "Layers")
set(SAMPLES_FOLDER "Samples")

string(TOUPPER "${CMAKE_GENERATOR_PLATFORM}" CMAKE_GENERATOR_PLATFORM_UPPER)

add_subdirectory(include)
add_subdirectory(src)
