# imgui

CPMAddPackage(
    NAME imgui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG v1.92.4-docking
    DOWNLOAD_ONLY YES
)

CPMAddPackage(
    NAME implot
    GITHUB_REPOSITORY epezent/implot
    GIT_TAG 3da8bd34299965d3b0ab124df743fe3e076fa222
    DOWNLOAD_ONLY YES
)

set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/external/imgui/imconfig.h

    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_rect_pack.cpp

    ${imgui_SOURCE_DIR}/imstb_textedit.h
    ${imgui_SOURCE_DIR}/imstb_truetype.h

    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    # ${imgui_SOURCE_DIR}/imgui_draw.cpp # imgui_rect_pack.cpp
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui.h

    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
    # ${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp # imgui_rect_pack.cpp
    ${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.h

    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3_shaders.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h

    # implot

    ${implot_SOURCE_DIR}/implot_demo.cpp
    ${implot_SOURCE_DIR}/implot_internal.h
    ${implot_SOURCE_DIR}/implot_items.cpp
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot.h
)

include_directories(
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/misc/cpp
    ${imgui_SOURCE_DIR}/misc/freetype
    ${implot_SOURCE_DIR}
)

add_compile_definitions(IMGUI_USER_CONFIG="${CMAKE_SOURCE_DIR}/external/imgui/imconfig.h")
