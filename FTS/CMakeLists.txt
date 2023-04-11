# Creating variables containing source locations
file(
  GLOB_RECURSE
  LOCAL_SOURCE
  CONFIGURE_DEPENDS
  "src/main/cpp/*.cpp"
)

file(
  GLOB_RECURSE
  LOCAL_ASSEMBLY
  CONFIGURE_DEPENDS
  "src/main/cpp/*.s"
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
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/engine
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/engine/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/fts-game
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/fts-game/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs/khronos/openxr/OpenXR-SDK/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs/meta/AudioSDK/include
  ${ANDROID_NDK}/sources/android/native_app_glue/
)

# Adding libraries to link to fts.
target_link_libraries(fts openxr_loader ovraudio glm ${android-log} ${android} ${vulkan} ${aaudio})