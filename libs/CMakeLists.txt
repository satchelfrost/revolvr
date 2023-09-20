add_subdirectory(meta)
add_subdirectory(glm)
add_subdirectory(khronos/ktx)

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
add_subdirectory(tinygltf)