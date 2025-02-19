cmake_minimum_required(VERSION 3.22)
project(LuaSTGEvo LANGUAGES C CXX)

if(APPLE)
    set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)
    set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/opt/llvm@18/lib/c++ -Wl,-rpath,/usr/local/opt/llvm@18/lib/c++")
endif()

#option(LINK_STEAM_API "Enable Steam API" OFF)
#option(LUASTG_RESDIR "Custom configurations" "res")

# set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include(cmake/TargetCommonOptions.cmake)
include(cmake/loader.cpm.cmake)

CPMInitialize()

include(cmake/packages.cpm.cmake)
include(cmake/packages.xiph.cmake)
include(cmake/packages.z.cmake)
add_subdirectory(external)
include(cmake/packages.lua.cmake)

add_subdirectory(imgui)

add_subdirectory(LuaSTG)
