# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)
project("fts" CXX ASM)
set(CMAKE_CXX_STANDARD 17)

# Creating compile definitions
add_definitions(
  -DANDROID_NDK
  -DXR_USE_PLATFORM_ANDROID
  -DXR_USE_GRAPHICS_API_VULKAN
)

# Find android libraries
find_library(android-log log REQUIRED)
find_library(android android REQUIRED)
find_library(vulkan vulkan REQUIRED)
find_library(aaudio aaudio REQUIRED)

# Add the FTS subdirectory
add_subdirectory(FTS)

# Add 3rd party libs
add_subdirectory(libs)