# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.10.2)
project("fts" CXX ASM)
set(CMAKE_CXX_STANDARD 17)

# Creating variables containing source locations
file(GLOB LOCAL_SOURCE "src/main/cpp/*.cpp")
file(GLOB LOCAL_ASSEMBLY "src/main/cpp/*.s")

# Creating compile definintions
add_definitions(
  -DANDROID_NDK
  -DXR_USE_PLATFORM_ANDROID
  -DXR_USE_GRAPHICS_API_VULKAN
)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_library(fts SHARED ${LOCAL_ASSEMBLY} ${LOCAL_SOURCE})

# Add openxr_loader as a library target
add_library(openxr_loader SHARED IMPORTED)

# Set the location of the openxr_loader library
set_target_properties(
  openxr_loader
  PROPERTIES IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs/meta/OpenXR/Libs/Android/${ANDROID_ABI}/${CMAKE_BUILD_TYPE}/libopenxr_loader.so
)

# Find android libraries
find_library(android-log log REQUIRED)
find_library(android android REQUIRED)
find_library(vulkan Vulkan REQUIRED)

# Adding the include directory to fts.
target_include_directories(
  fts
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs/khronos/openxr/OpenXR-SDK/include
  ${ANDROID_NDK}/sources/android/native_app_glue/
)

# Adding libraries to link to fts.
target_link_libraries(fts openxr_loader ${android-log} ${android} ${vulkan})