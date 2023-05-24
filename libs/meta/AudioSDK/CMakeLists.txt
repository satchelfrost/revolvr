add_library(ovraudio SHARED IMPORTED GLOBAL)

set_target_properties(
  ovraudio
  PROPERTIES IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/Libs/Android/${ANDROID_ABI}/libovraudio64.so
)

target_include_directories(ovraudio SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
