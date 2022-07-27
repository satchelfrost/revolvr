# Creating variables containing source locations
file(GLOB LOCAL_SOURCE
  "src/main/cpp/*.cpp"
  "src/main/cpp/rvr_parser/*.cpp"
)
file(GLOB LOCAL_ASSEMBLY "src/main/cpp/*.s")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_library(fts SHARED ${LOCAL_ASSEMBLY} ${LOCAL_SOURCE})

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
target_link_libraries(fts openxr_loader glm ${android-log} ${android} ${vulkan})