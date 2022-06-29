# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.10.2)
project("fts")
set(CMAKE_CXX_STANDARD 17)

#Creating variables containing source locations
file(GLOB LOCAL_SOURCE "src/main/cpp/*.cpp")
file(GLOB LOCAL_ASSEMSBLY "src/main/cpp/*.s")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_library(fts SHARED ${LOCAL_SOURCE} ${LOCAL_ASSEMSBLY})

# Adding the include directory to fts.
target_include_directories(
        fts
        PRIVATE
        src/main/cpp/include
        ../libs/khronos/openxr/OpenXR-SDK/include
)

# Add openxr_loader as a library target
add_library(openxr_loader SHARED IMPORTED)

# Set the location of the openxr_loader library
set_target_properties(
        openxr_loader
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../libs/meta/OpenXR/Libs/Android/${ANDROID_ABI}/${CMAKE_BUILD_TYPE}/libopenxr_loader.so
)

# Add the android native app glue
add_library(
        app-glue
        STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

# Add the header to android antive app glue
target_include_directories(app-glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue/)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.
find_library(log-lib log)
find_library(android-lib android)
find_library(vulkan-lib vulkan)
#find_library(app-glue-lib android_native_app_glue)

# Specifies libraries CMake should link to your target library.
target_link_libraries(fts openxr_loader app-glue ${log-lib} ${android-lib} ${vulkan-lib})