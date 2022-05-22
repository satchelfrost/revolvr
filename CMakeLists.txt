cmake_minimum_required(VERSION 3.10)

option(BUILD_LOADER "Build loader" OFF)
add_library(openxr_loader SHARED IMPORTED)
set_property(
 TARGET
 openxr_loader
 PROPERTY
 IMPORTED_LOCATION
 ${CMAKE_SOURCE_DIR}/lib/meta/android/${ANDROID_ABI}/${CMAKE_BUILD_TYPE}/libopenxr_loader.so
)

add_subdirectory(lib/khronos/OpenXR-SDK-Source)
