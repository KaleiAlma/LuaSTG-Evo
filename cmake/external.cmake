include(FetchContent)

set(LSTGEXT_LIB "SHARED" CACHE STRING "Whether to use static or shared libraries for the engine.")
set(LSTGEXT_REPO "KaleiAlma/LSTG-Evo-External" CACHE STRING "Evo external repository.")

string(TOUPPER ${LSTGEXT_LIB} LSTGEXT_LIB)

if(NOT LSTGEXT_RELEASE)
    file(DOWNLOAD https://api.github.com/repos/${LSTGEXT_REPO}/releases/latest ${CMAKE_CURRENT_LIST_DIR}/latest.tmp)
    file(READ ${CMAKE_CURRENT_LIST_DIR}/latest.tmp LSTGEXT_LATEST_RESPONSE)
    # message("${LSTGEXT_LATEST_RESPONSE}")

    string(JSON LSTGEXT_RELEASE GET "${LSTGEXT_LATEST_RESPONSE}" "name")
    file(REMOVE ${CMAKE_CURRENT_LIST_DIR}/latest.tmp)

    message("Using release: ${LSTGEXT_RELEASE}")
endif()


set(LSTGEXT_SYSTEM ${CMAKE_SYSTEM_NAME})

if(LSTGEXT_SYSTEM STREQUAL "Darwin")
    set(LSTGEXT_SYSTEM macOS)
endif()

set(LSTGEXT_PROCESSOR ${TARGET_ARCHITECTURE})

if(LSTGEXT_PROCESSOR STREQUAL "aarch64")
    set(LSTGEXT_PROCESSOR arm64)
endif()

string(TOLOWER ${LSTGEXT_LIB} LSTGEXT_LIB_LOWER)

if(LSTGEXT_COMPILE)
    FetchContent_Declare(
        lstgext
        GIT_REPOSITORY https://github.com/${LSTGEXT_REPO}
        GIT_TAG ${LSTGEXT_RELEASE}
    )
else()
    FetchContent_Declare(
        lstgext
        URL https://github.com/${LSTGEXT_REPO}/releases/download/${LSTGEXT_RELEASE}/lstgext-${LSTGEXT_SYSTEM}-${LSTGEXT_PROCESSOR}-${TARGET_ARCH_REV_WIN}-${LSTGEXT_LIB_LOWER}.zip
    )
endif()

FetchContent_MakeAvailable(lstgext)

# set(lstgext_SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/lstgext-src/)
# set(lstgext_SOURCE_DIR ${CMAKE_BINARY_DIR}/lstgext/)

if(NOT LSTGEXT_COMPILE)
    link_directories(${lstgext_SOURCE_DIR}/lstgext/lib)
    include_directories(
        ${lstgext_SOURCE_DIR}/lstgext/include
        ${lstgext_SOURCE_DIR}/lstgext/include/imgui
        ${lstgext_SOURCE_DIR}/lstgext/include/imgui/misc/cpp
        ${lstgext_SOURCE_DIR}/lstgext/include/imgui/misc/freetype
        ${lstgext_SOURCE_DIR}/lstgext/include/pcg
        ${lstgext_SOURCE_DIR}/lstgext/include/xmath
        ${lstgext_SOURCE_DIR}/lstgext/include/luajit
        ${lstgext_SOURCE_DIR}/lstgext/include/tracy
        ${lstgext_SOURCE_DIR}/lstgext/include/freetype2
    )
endif()

find_package(SDL3)
