# ==================== luajit ====================

add_subdirectory(luajit/luajit)
# set_target_properties(minilua PROPERTIES FOLDER luajit)
# set_target_properties(buildvm PROPERTIES FOLDER luajit)
# set_target_properties(lua51static PROPERTIES FOLDER luajit)
# set_target_properties(lua51 PROPERTIES FOLDER luajit)
# set_target_properties(luajit PROPERTIES FOLDER luajit)


# set(LUAJIT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/luajit")
# set(luajit_lib "${CMAKE_CURRENT_BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}luajit$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}")

# ExternalProject_Add(
#     LuaJIT_local
#     PREFIX ${CMAKE_BINARY_DIR}/external/luajit
#     DOWNLOAD_COMMAND ""
#     SOURCE_DIR "${LUAJIT_SOURCE_DIR}"
#     CMAKE_ARGS
#     "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
#     "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}"
#     "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
#     "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
#     "-DCMAKE_DEBUG_POSTFIX=$<$<CONFIG:Debug>:d>"
#     "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
#     "-DLUAJIT_FORCE_UTF8_FOPEN=ON"
# BUILD_BYPRODUCTS
#     "${luajit_lib}"
# )

# ==================== lua filesystem ====================

add_library(lua_filesystem STATIC)
luastg_target_common_options(lua_filesystem)
luastg_target_more_warning(lua_filesystem)
target_include_directories(lua_filesystem PUBLIC
    luafilesystem/src
)
target_sources(lua_filesystem PRIVATE
    luafilesystem/src/lfs.h
    luafilesystem/src/lfs.c
)
target_link_libraries(lua_filesystem PUBLIC
    # lua51static
    libluajit
)

set_target_properties(lua_filesystem PROPERTIES FOLDER lualib)

# ==================== lua cjson ====================

add_library(lua_cjson STATIC)
luastg_target_common_options(lua_cjson)
if(MSVC)
    target_compile_options(lua_cjson PRIVATE
        "/wd4090" # 不同的const限定符（指针传递）
    )
endif()
target_compile_definitions(lua_cjson PRIVATE
    _CRT_SECURE_NO_WARNINGS
    DISABLE_INVALID_NUMBERS
    ENABLE_CJSON_GLOBAL
)
target_include_directories(lua_cjson PUBLIC
    lua-cjson-patch
)
target_include_directories(lua_cjson PRIVATE
    lua-cjson
)
target_sources(lua_cjson PRIVATE
    lua-cjson-patch/lua_cjson.h
    lua-cjson/lua_cjson.c
    lua-cjson/strbuf.h
    lua-cjson/strbuf.c
    lua-cjson/fpconv.h
    lua-cjson/fpconv.c
)
target_link_libraries(lua_cjson PUBLIC
# lua51static
libluajit
)

set_target_properties(lua_cjson PROPERTIES FOLDER lualib)

# ==================== lua csv ====================

#add_library(lua_xlsx_csv STATIC)
#luastg_target_common_options(lua_xlsx_csv)
#target_include_directories(lua_xlsx_csv PUBLIC
#    lua-csv
#)
#target_sources(lua_xlsx_csv PRIVATE
#    lua-csv/lua_xlsx_csv.h
#    lua-csv/lua_xlsx.cpp
#    lua-csv/lua_csv.cpp
#)
#target_link_libraries(lua_xlsx_csv PUBLIC
#    luajit
#    zip
#    pugixml
#)
#
#set_target_properties(lua_xlsx_csv PROPERTIES FOLDER external)

# ==================== xmath ====================

add_library(xmath STATIC)
luastg_target_common_options(xmath)
target_include_directories(xmath PUBLIC
    xmath-patch
    xmath
    .
)
target_sources(xmath PRIVATE
    xmath-patch/math/Vec2.h
    xmath-patch/math/Vec2.cpp
    xmath/meow_fft.c
    xmath/meow_fft.h
    xmath/XCollision.cpp
    xmath/XCollision.h
    xmath/XComplex.cpp
    xmath/XComplex.h
    xmath/XConstant.h
    xmath/XDistance.cpp
    xmath/XDistance.h
    xmath/XEquation.cpp
    xmath/XEquation.h
    xmath/XFFT.cpp
    xmath/XFFT.h
    xmath/XIntersect.cpp
    xmath/XIntersect.h
    xmath/XMath.h
    xmath/XRandom.cpp
    xmath/XRandom.h
    xmath/XSpline.cpp
    xmath/XSpline.h
    xmath/XTween.cpp
    xmath/XTween.h
)

set_target_properties(xmath PROPERTIES FOLDER external)

# ==================== tracy ====================

option(ENABLE_TRACY_CLIENT "" OFF)

add_library(tracy STATIC)
luastg_target_common_options(tracy)
if(ENABLE_TRACY_CLIENT)
    target_compile_definitions(tracy PUBLIC
        TRACY_ENABLE
    )
endif()
target_include_directories(tracy PUBLIC
    tracy/public/tracy
    tracy-patch
)
target_sources(tracy PRIVATE
    tracy/public/tracy/Tracy.hpp
    # tracy/public/tracy/TracyD3D11.hpp
    tracy/public/tracy/TracyOpenGL.hpp
    tracy/public/TracyClient.cpp
    # tracy-patch/TracyD3D11Ctx.hpp
    # tracy-patch/TracyD3D11Ctx.cpp
    # tracy-patch/TracyOpenGLCtx.hpp
    # tracy-patch/TracyOpenGLCtx.cpp
)

set_target_properties(tracy PROPERTIES FOLDER external)

# ==================== image qoi ====================

add_library(libqoi STATIC)
luastg_target_common_options(libqoi)
target_compile_definitions(libqoi PUBLIC
    QOI_NO_STDIO
    QOI_IMPLEMENTATION
)
target_include_directories(libqoi PUBLIC
    image.qoi
)
target_sources(libqoi PRIVATE
    image.qoi/qoi.h
)
set_target_properties(libqoi PROPERTIES FOLDER external LINKER_LANGUAGE C)

# ==================== steam api ====================

add_subdirectory(steam_api)

# ==================== glad ====================

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
add_library(glad STATIC)
luastg_target_common_options(glad)
target_sources(glad PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/gl.c
)

# ==================== sdl2 ====================

set(SDL2MIXER_MOD OFF)

add_subdirectory(sdl2)
if(WIN32)
    set_target_properties(SDL2-static PROPERTIES OUTPUT_NAME "SDL2-static$<$<CONFIG:Debug>:d>")
endif()
