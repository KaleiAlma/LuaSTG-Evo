# zlib

CPMAddPackage(
    NAME zlib_ng # 必须用这个名称，用来适配一些SB库
    VERSION 2.1.6
    GITHUB_REPOSITORY zlib-ng/zlib-ng
    GIT_TAG 2.1.6
    # DOWNLOAD_ONLY YES
)

# minizip
# 读取 zip 文件

CPMAddPackage(
    NAME minizip_ng
    VERSION 4.0.5
    GITHUB_REPOSITORY zlib-ng/minizip-ng
    GIT_TAG 4.0.5
    # DOWNLOAD_ONLY YES
)

# FUCK ZLIB-NG AND MINIZIP-NG
# FUCK ZLIB-NG AND MINIZIP-NG
# FUCK ZLIB-NG AND MINIZIP-NG
# FUCK ZLIB-NG AND MINIZIP-NG
# FUCK ZLIB-NG AND MINIZIP-NG
# FUCK ZLIB-NG AND MINIZIP-NG
# FUCK ZLIB-NG AND MINIZIP-NG
# FUCK ZLIB-NG AND MINIZIP-NG
# FUCK ZLIB-NG AND MINIZIP-NG
# FUCK ZLIB-NG AND MINIZIP-NG
# FUCK ZLIB-NG AND MINIZIP-NG

# if(zlib_ng_ADDED AND minizip_ng_ADDED)
#     file(WRITE ${CMAKE_BINARY_DIR}/install/include/fuck_zlib_ng_and_minizip_ng.h "")
#     file(WRITE ${CMAKE_BINARY_DIR}/install/include/minizip-ng/fuck_zlib_ng_and_minizip_ng.h "")

#     add_custom_target(fuck_zlib_ng_and_minizip_ng
#         COMMAND echo ${CMAKE_GENERATOR}
#         COMMAND echo ${CMAKE_GENERATOR_PLATFORM}
#         COMMAND echo $<CONFIG>
        
#         COMMAND cmake -S ${zlib_ng_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/zlib-ng"$<$<BOOL:${CMAKE_GENERATOR_PLATFORM}>:/${CMAKE_GENERATOR_PLATFORM}>" -G ${CMAKE_GENERATOR} "$<$<BOOL:${CMAKE_GENERATOR_PLATFORM}>:-A ${CMAKE_GENERATOR_PLATFORM}>" -DZLIB_ENABLE_TESTS=OFF -DZLIBNG_ENABLE_TESTS=OFF -DWITH_GTEST=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>
#         COMMAND cmake --build   ${CMAKE_BINARY_DIR}/zlib-ng"$<$<BOOL:${CMAKE_GENERATOR_PLATFORM}>:/${CMAKE_GENERATOR_PLATFORM}>" --config $<CONFIG> --target all
#         COMMAND cmake --install ${CMAKE_BINARY_DIR}/zlib-ng"$<$<BOOL:${CMAKE_GENERATOR_PLATFORM}>:/${CMAKE_GENERATOR_PLATFORM}>" --config $<CONFIG> --prefix ${CMAKE_BINARY_DIR}/install
#         COMMAND cmake -E rm -f ${CMAKE_BINARY_DIR}/install/bin/zlib-ng$<$<CONFIG:Debug>:d>2${CMAKE_SHARED_LIBRARY_SUFFIX}
#         COMMAND cmake -E rm -f ${CMAKE_BINARY_DIR}/install/lib/zlib-ng$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    
#         COMMAND cmake -S ${minizip_ng_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/minizip-ng"$<$<BOOL:${CMAKE_GENERATOR_PLATFORM}>:/${CMAKE_GENERATOR_PLATFORM}>" -G ${CMAKE_GENERATOR} "$<$<BOOL:${CMAKE_GENERATOR_PLATFORM}>:-A ${CMAKE_GENERATOR_PLATFORM}>" -DMZ_COMPAT=OFF -DMZ_BZIP2=OFF -DMZ_LZMA=OFF -DMZ_ZSTD=OFF -DMZ_FETCH_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug> -DZLIB_INCLUDE_DIRS=${CMAKE_BINARY_DIR}/install/include -DZLIBNG_LIBRARY=${CMAKE_BINARY_DIR}/install/lib/zlibstatic-ng$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
#         COMMAND cmake --build   ${CMAKE_BINARY_DIR}/minizip-ng"$<$<BOOL:${CMAKE_GENERATOR_PLATFORM}>:/${CMAKE_GENERATOR_PLATFORM}>" --config $<CONFIG> --target all
#         COMMAND cmake --install ${CMAKE_BINARY_DIR}/minizip-ng"$<$<BOOL:${CMAKE_GENERATOR_PLATFORM}>:/${CMAKE_GENERATOR_PLATFORM}>" --config $<CONFIG> --prefix ${CMAKE_BINARY_DIR}/install
#     )

#     set_target_properties(fuck_zlib_ng_and_minizip_ng PROPERTIES FOLDER external)
    
#     add_library(zlib-ng STATIC IMPORTED GLOBAL)
#     target_include_directories(zlib-ng INTERFACE
#         ${CMAKE_BINARY_DIR}/install/include
#     )
#     set_target_properties(zlib-ng PROPERTIES
#         IMPORTED_LOCATION         ${CMAKE_BINARY_DIR}/lib/libz-ng${CMAKE_STATIC_LIBRARY_SUFFIX}
#         IMPORTED_LOCATION_DEBUG   ${CMAKE_BINARY_DIR}/lib/libz-ngd${CMAKE_STATIC_LIBRARY_SUFFIX}
#         IMPORTED_LOCATION_RELEASE ${CMAKE_BINARY_DIR}/lib/libz-ng${CMAKE_STATIC_LIBRARY_SUFFIX}
#     )
    
#     add_library(minizip-ng STATIC IMPORTED GLOBAL)
#     target_include_directories(minizip-ng INTERFACE
#         ${CMAKE_BINARY_DIR}/install/include/minizip-ng
#     )
#     set_target_properties(minizip-ng PROPERTIES
#         IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/install/lib/libminizip-ng${CMAKE_STATIC_LIBRARY_SUFFIX}
#     )
#     target_link_libraries(minizip-ng INTERFACE
#         zlib-ng
#         bcrypt${CMAKE_STATIC_LIBRARY_SUFFIX}
#     )
    
#     # set(LUAJIT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/luajit")
#     set(zlib_ng_lib "${CMAKE_CURRENT_BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlib_ng$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}")
#     set(minizip_ng_lib "${CMAKE_CURRENT_BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}minizip_ng$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}")

#     ExternalProject_Add(
#         zlib_ng_local
#         PREFIX ${CMAKE_BINARY_DIR}/external/zlib_ng
#         DOWNLOAD_COMMAND ""
#         SOURCE_DIR "${zlib_ng_SOURCE_DIR}"
#         CMAKE_ARGS
#         "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
#         "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}"
#         "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
#         "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
#         "-DCMAKE_DEBUG_POSTFIX=$<$<CONFIG:Debug>:d>"
#         "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
#     BUILD_BYPRODUCTS
#         "${zlib_ng_lib}"
#     )
#     ExternalProject_Add(
#         minizip_ng_local
#         PREFIX ${CMAKE_BINARY_DIR}/external/minizip_ng
#         DOWNLOAD_COMMAND ""
#         SOURCE_DIR "${minizip_ng_SOURCE_DIR}"
#         CMAKE_ARGS
#         "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
#         "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}"
#         "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
#         "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
#         "-DCMAKE_DEBUG_POSTFIX=$<$<CONFIG:Debug>:d>"
#         "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
#     BUILD_BYPRODUCTS
#         "${minizip_ng_lib}"
#     )
# endif()
