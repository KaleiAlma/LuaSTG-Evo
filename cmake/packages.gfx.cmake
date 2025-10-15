# sdl3

CPMAddPackage(
    NAME SDL3
    GITHUB_REPOSITORY libsdl-org/SDL
    GIT_TAG release-3.2.22
    OPTIONS
        "SDL_INSTALL_CPACK OFF"
        "SDL_INSTALL_DOCS OFF"
        "SDL_INSTALL_TESTS OFF"
        "SDL_UNINSTALL OFF"
        "SDL_TEST OFF"
        "SDL_TEST_LIBRARY OFF"
        "SDL_SHARED OFF"
        "SDL_STATIC ON"
        "SDL_ASSEMBLY ON"
        "SDL_MMX ${LSTG_x86_64}"
        "SDL_SSE ${LSTG_x86_64}"
        "SDL_SSE2 ${LSTG_x86_64}"
        "SDL_SSE3 ${LSTG_SSE4_2}"
        "SDL_SSE4_1 ${LSTG_SSE4_2}"
        "SDL_SSE4_2 ${LSTG_SSE4_2}"
        "SDL_AVX ${LSTG_AVX}"
        "SDL_AVX2 ${LSTG_AVX2}"
        "SDL_AVX512F ${LSTG_AVX512F}"
        "SDL_RENDER OFF"
)

# g-truc glm
# OpenGL math library

CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.1
    OPTIONS
        "GLM_ENABLE_CXX_20 ON"
        "GLM_ENABLE_SIMD_SSE2 ${LSTG_x86_64}"
        "GLM_ENABLE_SIMD_SSE4_2 ${LSTG_SSE4_2}"
        "GLM_ENABLE_SIMD_AVX ${LSTG_AVX}"
        "GLM_ENABLE_SIMD_AVX2 ${LSTG_AVX2}"
        "GLM_BUILD_LIBRARY ON"
)
lstgext_tgtopts_full(glm)

# tinygltf
# Parser for gltf 2.0 files

CPMAddPackage(
    NAME tinygltf
    VERSION 2.8.14
    GITHUB_REPOSITORY syoyo/tinygltf
    DOWNLOAD_ONLY YES
)

file(WRITE ${CMAKE_BINARY_DIR}/tinygltf/tiny_gltf.h "PLACEHOLD")
file(REMOVE
    ${CMAKE_BINARY_DIR}/tinygltf/tiny_gltf.h
)
file(COPY_FILE
    ${tinygltf_SOURCE_DIR}/tiny_gltf.h
    ${CMAKE_BINARY_DIR}/tinygltf/tiny_gltf.h
)

add_library(tinygltf INTERFACE)
target_include_directories(tinygltf INTERFACE
    ${CMAKE_BINARY_DIR}/tinygltf/
    ${nlohmann_json_SOURCE_DIR}/include/nlohmann/
)

# freetype
# Font utilities

CPMAddPackage(
    NAME freetype
    GITHUB_REPOSITORY freetype/freetype
    GIT_TAG VER-2-13-3
    OPTIONS
        "FT_DISABLE_ZLIB ON"
        "FT_DISABLE_BZIP2 ON"
        "FT_DISABLE_PNG ON"
        "FT_DISABLE_HARFBUZZ ON"
        "FT_DISABLE_BROTLI ON"
)

lstgext_tgtopts_full(freetype)
if(MSVC)
    target_compile_options(freetype PRIVATE
        "/utf-8" # Unicode warning
    )
endif()
set_target_properties(freetype PROPERTIES FOLDER external)

