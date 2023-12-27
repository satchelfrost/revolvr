# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)
project("app" CXX ASM)
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

# Add 3rd party libs
add_subdirectory(libs)

# Creating variables containing source locations
file(
  GLOB_RECURSE
  LOCAL_SOURCE
  CONFIGURE_DEPENDS
  "src/*.cpp"
  "examples/app/src/main/cpp/*.cpp"
)

file(
  GLOB_RECURSE
  LOCAL_ASSEMBLY
  CONFIGURE_DEPENDS
  "src/*.s"
)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_library(game SHARED ${LOCAL_ASSEMBLY} ${LOCAL_SOURCE})

# Adding the include directory to game.
target_include_directories(
  game
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/examples/app/include
  ${ANDROID_NDK}/sources/android/native_app_glue/
)

#${CMAKE_CURRENT_SOURCE_DIR}/revolvr/libs/tinygltf

# Adding libraries to link to game.
target_link_libraries(
        game
        openxr_loader
        ovraudio
        glm
        tinygltf
        ktx
        ${android-log}
        ${android}
        ${vulkan}
        ${aaudio}
)
