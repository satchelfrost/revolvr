# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.10.2)
project("fts" CXX ASM)
set(CMAKE_CXX_STANDARD 17)

# Creating compile definintions
add_definitions(
  -DANDROID_NDK
  -DXR_USE_PLATFORM_ANDROID
  -DXR_USE_GRAPHICS_API_VULKAN
)

# Add openxr_loader as a library target
add_library(openxr_loader SHARED IMPORTED)

# Set the location of the openxr_loader library
set_target_properties(
  openxr_loader
  PROPERTIES IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/libs/meta/OpenXR/Libs/Android/${ANDROID_ABI}/${CMAKE_BUILD_TYPE}/libopenxr_loader.so
)

# Find android libraries
find_library(android-log log REQUIRED)
find_library(android android REQUIRED)
find_library(vulkan vulkan REQUIRED)

# Add the FTS subdirectory
add_subdirectory("FTS")