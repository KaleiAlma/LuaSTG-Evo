# nlohmann json
# JSON parser and writer

CPMAddPackage(
    NAME nlohmann_json
    VERSION 3.12.0
    GITHUB_REPOSITORY nlohmann/json
    OPTIONS
    "JSON_BuildTests OFF"
    "JSON_Install ON"
)

# gabime spdlog
# Logging utility

if(WIN32)
    set(SPDLOG_WIN32_OPTS
        "SPDLOG_WCHAR_FILENAMES ON"
        "SPDLOG_WCHAR_SUPPORT ON"
    )
endif()

CPMAddPackage(
    NAME spdlog
    VERSION 1.15.1
    GITHUB_REPOSITORY gabime/spdlog
    OPTIONS
    ${SPDLOG_WIN32_OPTS}
    "SPDLOG_DISABLE_DEFAULT_LOGGER ON"
    "SPDLOG_INSTALL ON"
)

lstgext_tgtopts_full(spdlog)

if(MSVC)
    target_compile_options(spdlog PUBLIC
        "/DSPDLOG_SHORT_LEVEL_NAMES={\"V\",\"D\",\"I\",\"W\",\"E\",\"F\",\"O\"}"
    )
endif()
set_target_properties(spdlog PROPERTIES FOLDER external)


# nothings stb
# Misc tools

CPMAddPackage(
    NAME nothings_stb
    GITHUB_REPOSITORY nothings/stb
    GIT_TAG 5c205738c191bcb0abc65c4febfa9bd25ff35234
    DOWNLOAD_ONLY YES
)

add_library(nothings_stb INTERFACE ${nothings_stb_SOURCE_DIR}/stb_vorbis.c)
target_include_directories(nothings_stb INTERFACE ${nothings_stb_SOURCE_DIR})


# pcg random
# High-quality RNG

CPMAddPackage(
    NAME pcg_cpp
    GITHUB_REPOSITORY imneme/pcg-cpp
    GIT_TAG 428802d1a5634f96bcd0705fab379ff0113bcf13
    DOWNLOAD_ONLY YES
)

add_library(pcg_cpp INTERFACE)
target_include_directories(pcg_cpp INTERFACE
    ${pcg_cpp_SOURCE_DIR}/include
)


# xxhash
# High-quality high-performance hash lib (not password-secure)

CPMAddPackage(
    NAME xxhash
    VERSION 0.8.3
    GITHUB_REPOSITORY Cyan4973/xxHash
    DOWNLOAD_ONLY YES
)

add_library(xxhash STATIC)
set_target_properties(xxhash PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(xxhash PUBLIC
    ${xxhash_SOURCE_DIR}
)
target_sources(xxhash PRIVATE
    ${xxhash_SOURCE_DIR}/xxhash.c
    ${xxhash_SOURCE_DIR}/xxhash.h
)
set_target_properties(xxhash PROPERTIES FOLDER external)

# uni-algo
# Unicode utilities

CPMAddPackage(
    NAME uni-algo
    VERSION 1.2.0
    GITHUB_REPOSITORY uni-algo/uni-algo
    OPTIONS
        "UNI_ALGO_INSTALL ON"
)
lstgext_tgtopts_full(uni-algo)


# tracy
# Profiler

# hack to fix include dirs
set(_CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/tracy)

CPMAddPackage(
    NAME tracy
    VERSION 0.11.1
    GITHUB_REPOSITORY wolfpld/tracy
    OPTIONS
        "TRACY_ENABLE OFF"
)
lstgext_tgtopts_full(TracyClient)

# reset hack
set(CMAKE_INSTALL_INCLUDEDIR ${_CMAKE_INSTALL_INCLUDEDIR})

CPMAddPackage(
    NAME pugixml
    VERSION 1.13
    GITHUB_REPOSITORY zeux/pugixml
    DOWNLOAD_ONLY YES
)

if(pugixml_ADDED)
    # pugixml's CMake support kinda sucks, so we do it ourself.
    add_library(pugixml STATIC)
    target_include_directories(pugixml PUBLIC
        ${pugixml_SOURCE_DIR}/src
    )
    target_sources(pugixml PRIVATE
        ${pugixml_SOURCE_DIR}/src/pugiconfig.hpp
        ${pugixml_SOURCE_DIR}/src/pugixml.hpp
        ${pugixml_SOURCE_DIR}/src/pugixml.cpp
    )
    set(pugixml_natvis ${pugixml_SOURCE_DIR}/scripts/natvis/pugixml.natvis)
    source_group(TREE ${pugixml_SOURCE_DIR}/scripts FILES ${pugixml_natvis})
    target_sources(pugixml PUBLIC
        ${pugixml_natvis}
    )
    set_target_properties(pugixml PROPERTIES FOLDER external)
endif()

CPMAddPackage(
    NAME xmath
    GITHUB_REPOSITORY Xrysnow/lstgx_Math
    GIT_TAG 7f594caeaff8d14c8032bb246c5d435e0d40c65d
    DOWNLOAD_ONLY YES
)

add_library(xmath STATIC)
luastg_target_common_options(xmath)
target_include_directories(xmath PUBLIC
    ${CMAKE_SOURCE_DIR}/external/xmath-patch
    ${xmath_SOURCE_DIR}
    .
)
target_sources(xmath PRIVATE
    ${CMAKE_SOURCE_DIR}/external/xmath-patch/math/Vec2.h
    ${CMAKE_SOURCE_DIR}/external/xmath-patch/math/Vec2.cpp
    ${xmath_SOURCE_DIR}/meow_fft.c
    ${xmath_SOURCE_DIR}/meow_fft.h
    ${xmath_SOURCE_DIR}/XCollision.cpp
    ${xmath_SOURCE_DIR}/XCollision.h
    ${xmath_SOURCE_DIR}/XComplex.cpp
    ${xmath_SOURCE_DIR}/XComplex.h
    ${xmath_SOURCE_DIR}/XConstant.h
    ${xmath_SOURCE_DIR}/XDistance.cpp
    ${xmath_SOURCE_DIR}/XDistance.h
    ${xmath_SOURCE_DIR}/XEquation.cpp
    ${xmath_SOURCE_DIR}/XEquation.h
    ${xmath_SOURCE_DIR}/XFFT.cpp
    ${xmath_SOURCE_DIR}/XFFT.h
    ${xmath_SOURCE_DIR}/XIntersect.cpp
    ${xmath_SOURCE_DIR}/XIntersect.h
    ${xmath_SOURCE_DIR}/XMath.h
    ${xmath_SOURCE_DIR}/XRandom.cpp
    ${xmath_SOURCE_DIR}/XRandom.h
    ${xmath_SOURCE_DIR}/XSpline.cpp
    ${xmath_SOURCE_DIR}/XSpline.h
    ${xmath_SOURCE_DIR}/XTween.cpp
    ${xmath_SOURCE_DIR}/XTween.h
)

set_target_properties(xmath PROPERTIES FOLDER external)
