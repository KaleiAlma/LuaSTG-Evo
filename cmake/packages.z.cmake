set(ZSTD_TAG v1.5.6 CACHE STRING "")
# set(MZ_ZSTD OFF CACHE BOOL "" FORCE)

# zlib
set(ZLIB_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(ZLIBNG_ENABLE_TESTS OFF CACHE BOOL "" FORCE)

# minizip
# Load zip files

set(ZLIB_REPOSITORY https://github.com/zlib-ng/zlib-ng CACHE STRING "")
set(ZLIB_TAG 2.1.6 CACHE STRING "")
set(ZLIBNG_FOUND TRUE CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "")

CPMAddPackage(
    NAME minizip_ng
    VERSION 4.0.5
    GITHUB_REPOSITORY zlib-ng/minizip-ng
    GIT_TAG 4.0.5
    OPTIONS
    ZLIB_ENABLE_TESTS OFF
    ZLIBNG_ENABLE_TESTS OFF
    # ZLIB_REPOSITORY https://github.com/zlib-ng/zlib-ng
    # ZLIB_TAG 2.1.6
    MZ_FETCH_LIBS OFF
    # DOWNLOAD_ONLY YES
)

# thankfully i figured out how to make zlib-ng/minizip-ng play nice ig