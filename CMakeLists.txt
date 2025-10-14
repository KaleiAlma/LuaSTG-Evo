cmake_minimum_required(VERSION 3.22)
project(LuaSTGEvo LANGUAGES C CXX)

if(APPLE)
    set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)
    set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/opt/llvm@18/lib/c++ -Wl,-rpath,/usr/local/opt/llvm@18/lib/c++")
endif()

set(DEFAULT_x86_64_REVISION "westmere" CACHE STRING "Clang x86_64 CPU revision")
set(DEFAULT_x86_64_REVISION_WINDOWS "SSE4.2" CACHE STRING "MSVC x64 architecture flag")
set(DEFAULT_ARM64_REVISION "armv8.2-a" CACHE STRING "Clang ARM64 CPU revision")
set(DEFAULT_ARM64_REVISION_WINDOWS "armv8.2" CACHE STRING "MSVC ARM64 architecture flag")

include(cmake/architecture.cmake)
target_architecture(TARGET_ARCHITECTURE)
if(NOT DEFINED TARGET_ARCH_REV)
    if(TARGET_ARCHITECTURE STREQUAL "x86_64")
        set(TARGET_ARCH_REV ${DEFAULT_x86_64_REVISION})
    elseif(TARGET_ARCHITECTURE STREQUAL "aarch64")
        set(TARGET_ARCH_REV ${DEFAULT_ARM64_REVISION})
    endif()
endif()
if(NOT DEFINED TARGET_ARCH_REV_WIN)
    if(TARGET_ARCHITECTURE STREQUAL "x86_64")
        set(TARGET_ARCH_REV_WIN ${DEFAULT_x86_64_REVISION_WINDOWS})
    elseif(TARGET_ARCHITECTURE STREQUAL "aarch64")
        set(TARGET_ARCH_REV_WIN ${DEFAULT_ARM64_REVISION_WINDOWS})
    endif()
endif()

#option(LINK_STEAM_API "Enable Steam API" OFF)
#option(LUASTG_RESDIR "Custom configurations" "res")

# set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include(cmake/TargetCommonOptions.cmake)
include(cmake/loader.cpm.cmake)

CPMInitialize()

include(cmake/external.cmake)
include(cmake/pugixml.cmake)
# include(cmake/packages.cpm.cmake)
# include(cmake/packages.xiph.cmake)
# include(cmake/packages.z.cmake)
add_subdirectory(external)
# include(cmake/packages.lua.cmake)

# add_subdirectory(imgui)

add_subdirectory(LuaSTG)
