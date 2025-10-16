# minizip
# Load zip files

if(WIN32 AND LSTG_ARM64 AND CMAKE_CROSSCOMPILING)
    set(CMAKE_C_COMPILER cl)
    set(CMAKE_CXX_COMPILER cl)
endif()

CPMAddPackage(
    NAME minizip_ng
    GITHUB_REPOSITORY zlib-ng/minizip-ng
    GIT_TAG 4.0.10
    OPTIONS
        "ZLIB_ENABLE_TESTS OFF"
        "ZLIBNG_ENABLE_TESTS OFF"
        "WITH_GTEST OFF"
        "MZ_FETCH_LIBS ON"
        "ZLIB_REPOSITORY https://github.com/zlib-ng/zlib-ng"
        "ZLIB_TAG 2.2.5"
        "ZSTD_TAG v1.5.7"
        "WITH_SSE2 ${LSTG_x86_64}"
        "WITH_SSSE3 ${LSTG_SSE4_2}"
        "WITH_SSE42 ${LSTG_SSE4_2}"
        "WITH_AVX2 ${LSTG_AVX2}"
        "WITH_AVX512 ${LSTG_AVX512F}"
)


if(WIN32 AND LSTG_ARM64 AND CMAKE_CROSSCOMPILING)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
endif()
# if(LINUX AND LSTG_ARM64 AND CMAKE_CROSSCOMPILING) # lld is broken when cross-compiling
#     set_target_properties(minizip PROPERTIES LINKER_TYPE BFD)
# endif()

# thankfully i figured out how to make zlib-ng/minizip-ng play nice ig