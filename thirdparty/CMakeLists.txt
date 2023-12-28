add_subdirectory(meta)
add_subdirectory(glm)
add_subdirectory(khronos/ktx)
add_subdirectory(happly)

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
add_subdirectory(tinygltf)