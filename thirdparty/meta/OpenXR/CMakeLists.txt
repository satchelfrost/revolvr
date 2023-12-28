add_library(openxr_loader SHARED IMPORTED GLOBAL)

set_target_properties(
  openxr_loader
  PROPERTIES IMPORTED_LOCATION
  "${CMAKE_CURRENT_SOURCE_DIR}/Libs/Android/${ANDROID_ABI}/${CMAKE_BUILD_TYPE}/libopenxr_loader.so"
)

target_include_directories(openxr_loader SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../../khronos)

