# luajit

CPMAddPackage(
    NAME luajit
    GITHUB_REPOSITORY KaleiAlma/LuaJIT-Evo
    GIT_TAG c56c9d4c9ac0f94eec0f19c2be52be945de3eda2
)

CPMAddPackage(
    NAME lua-cjson
    GIT_TAG 2.1.0.9
    GITHUB_REPOSITORY openresty/lua-cjson
    DOWNLOAD_ONLY YES
)

add_library(lua_cjson STATIC)
luastg_target_common_options(lua_cjson)
target_compile_definitions(lua_cjson PRIVATE
    DISABLE_INVALID_NUMBERS
    ENABLE_CJSON_GLOBAL
)
target_include_directories(lua_cjson PUBLIC
    ${CMAKE_SOURCE_DIR}/external/include
)
target_include_directories(lua_cjson PRIVATE
    ${lua-cjson_SOURCE_DIR}
)
target_sources(lua_cjson PRIVATE
    ${CMAKE_SOURCE_DIR}/external/include/lua_cjson.h
    ${lua-cjson_SOURCE_DIR}/lua_cjson.c
    ${lua-cjson_SOURCE_DIR}/strbuf.h
    ${lua-cjson_SOURCE_DIR}/strbuf.c
    ${lua-cjson_SOURCE_DIR}/fpconv.h
    ${lua-cjson_SOURCE_DIR}/fpconv.c
)
target_link_libraries(lua_cjson PUBLIC
    luajit
)

set_target_properties(lua_cjson PROPERTIES FOLDER lualib)

CPMAddPackage(
    NAME luasocket
    VERSION 3.1.0
    GITHUB_REPOSITORY lunarmodules/luasocket
    DOWNLOAD_ONLY YES
)

set(LUA_SOCKET_ROOT ${luasocket_SOURCE_DIR})
add_library(luasocket STATIC)
luastg_target_common_options(luasocket)
target_compile_definitions(luasocket
PRIVATE
    LUASOCKET_API= # static
    $<$<CONFIG:DEBUG>:LUASOCKET_DEBUG>
)
target_include_directories(luasocket
PRIVATE
    ${LUA_SOCKET_ROOT}/src
)
set(luasocket_src
    ${LUA_SOCKET_ROOT}/src/auxiliar.c
    ${LUA_SOCKET_ROOT}/src/auxiliar.h
    ${LUA_SOCKET_ROOT}/src/buffer.c
    ${LUA_SOCKET_ROOT}/src/buffer.h
    ${LUA_SOCKET_ROOT}/src/compat.c
    ${LUA_SOCKET_ROOT}/src/compat.h
    ${LUA_SOCKET_ROOT}/src/except.c
    ${LUA_SOCKET_ROOT}/src/except.h
    ${LUA_SOCKET_ROOT}/src/inet.c
    ${LUA_SOCKET_ROOT}/src/inet.h
    ${LUA_SOCKET_ROOT}/src/io.c
    ${LUA_SOCKET_ROOT}/src/io.h
    ${LUA_SOCKET_ROOT}/src/luasocket.c
    ${LUA_SOCKET_ROOT}/src/luasocket.h
    ${LUA_SOCKET_ROOT}/src/mime.c
    ${LUA_SOCKET_ROOT}/src/mime.h
    ${LUA_SOCKET_ROOT}/src/options.c
    ${LUA_SOCKET_ROOT}/src/options.h
    #${LUA_SOCKET_ROOT}/src/pierror.h
    ${LUA_SOCKET_ROOT}/src/select.c
    ${LUA_SOCKET_ROOT}/src/select.h
    #${LUA_SOCKET_ROOT}/src/serial.c
    ${LUA_SOCKET_ROOT}/src/tcp.c
    ${LUA_SOCKET_ROOT}/src/tcp.h
    ${LUA_SOCKET_ROOT}/src/timeout.c
    ${LUA_SOCKET_ROOT}/src/timeout.h
    ${LUA_SOCKET_ROOT}/src/udp.c
    ${LUA_SOCKET_ROOT}/src/udp.h
    #${LUA_SOCKET_ROOT}/src/unix.c
    #${LUA_SOCKET_ROOT}/src/unix.h
    #${LUA_SOCKET_ROOT}/src/unixdgram.c
    #${LUA_SOCKET_ROOT}/src/unixdgram.h
    #${LUA_SOCKET_ROOT}/src/unixstream.c
    #${LUA_SOCKET_ROOT}/src/unixstream.h
    #${LUA_SOCKET_ROOT}/src/url.lua
    #${LUA_SOCKET_ROOT}/src/usocket.c
    #${LUA_SOCKET_ROOT}/src/usocket.h
    # ${LUA_SOCKET_ROOT}/src/wsocket.c
    # ${LUA_SOCKET_ROOT}/src/wsocket.h
)
if(WIN32)
    list(APPEND luasocket_src
        ${LUA_SOCKET_ROOT}/src/wsocket.c
        ${LUA_SOCKET_ROOT}/src/wsocket.h
    )
elseif(UNIX)
    list(APPEND luasocket_src
        ${LUA_SOCKET_ROOT}/src/unix.c
        ${LUA_SOCKET_ROOT}/src/unix.h
        ${LUA_SOCKET_ROOT}/src/unixdgram.c
        ${LUA_SOCKET_ROOT}/src/unixdgram.h
        ${LUA_SOCKET_ROOT}/src/unixstream.c
        ${LUA_SOCKET_ROOT}/src/unixstream.h
        ${LUA_SOCKET_ROOT}/src/usocket.c
        ${LUA_SOCKET_ROOT}/src/usocket.h
    )
endif()

target_sources(luasocket PRIVATE ${luasocket_src})
target_link_libraries(luasocket PUBLIC
    luajit
    "$<$<BOOL:${WIN32}>:ws2_32.lib>"
)

set_target_properties(luasocket PROPERTIES FOLDER lualib)

CPMAddPackage(
    NAME luafilesystem
    VERSION 1_8_0
    GITHUB_REPOSITORY lunarmodules/luafilesystem
    DOWNLOAD_ONLY YES
)

add_library(lua_filesystem STATIC)
luastg_target_common_options(lua_filesystem)
luastg_target_more_warning(lua_filesystem)
target_include_directories(lua_filesystem PUBLIC
    ${luafilesystem_SOURCE_DIR}/src
)
target_sources(lua_filesystem PRIVATE
    ${luafilesystem_SOURCE_DIR}/src/lfs.h
    ${luafilesystem_SOURCE_DIR}/src/lfs.c
)
target_link_libraries(lua_filesystem PUBLIC
    luajit
)

set_target_properties(lua_filesystem PROPERTIES FOLDER lualib)


# sol2
# Lua C++ utility library

CPMAddPackage(
    NAME sol2
    GITHUB_REPOSITORY ThePhD/sol2
    VERSION 3.3.0
    OPTIONS
        "SOL2_LUA_VERSION LuaJIT"
)
