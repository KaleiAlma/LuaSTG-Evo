# Windows Platform

# find_package(SDL2 REQUIRED)

set(PLATFORM_SOURCES
    Platform/Shared.hpp
    Platform/CommandLineArguments.cpp
    Platform/CommandLineArguments.hpp
    Platform/All.cpp

    Platform/AdapterPolicy.hpp
    Platform/AdapterPolicy.cpp
    Platform/MessageBox.hpp
    Platform/MessageBox.cpp
)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PLATFORM_SOURCES})

add_library(PlatformAPI STATIC)
luastg_target_common_options(PlatformAPI)
luastg_target_more_warning(PlatformAPI)
target_include_directories(PlatformAPI PUBLIC
    .
    ${CMAKE_SOURCE_DIR}/external/sdl2/include/
)
target_sources(PlatformAPI PRIVATE
    ${PLATFORM_SOURCES}
)
target_link_libraries(PlatformAPI PUBLIC SDL2-static spdlog)
