# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)
project("fts" CXX ASM)
set(CMAKE_CXX_STANDARD 17)

# Creating compile definintions
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

# Add 3rd party libs
add_subdirectory(libs)

# Creating variables containing source locations
file(
  GLOB_RECURSE
  LOCAL_SOURCE
  CONFIGURE_DEPENDS
  "engine/*.cpp"
  "FTS/src/main/cpp/*.cpp"
)

file(
  GLOB_RECURSE
  LOCAL_ASSEMBLY
  CONFIGURE_DEPENDS
  "engine/*.s"
)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_library(fts SHARED ${LOCAL_ASSEMBLY} ${LOCAL_SOURCE})

# Adding the include directory to fts.
target_include_directories(
  fts
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/engine/include
  ${CMAKE_CURRENT_SOURCE_DIR}/FTS/src/main/cpp/fts-game/include
  ${ANDROID_NDK}/sources/android/native_app_glue/
)

# Adding libraries to link to fts.
target_link_libraries(fts openxr_loader ovraudio glm ${android-log} ${android} ${vulkan} ${aaudio})
