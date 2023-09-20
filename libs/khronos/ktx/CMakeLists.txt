add_library(ktx SHARED IMPORTED GLOBAL)

set_target_properties(
  ktx
  PROPERTIES IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/libktx.so
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/libktx_read.so
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/libobjUtil.so
)

target_include_directories(ktx SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
