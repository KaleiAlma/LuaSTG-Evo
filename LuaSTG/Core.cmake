# Core

add_library(Core STATIC)

luastg_target_common_options(Core)
luastg_target_more_warning(Core)
target_compile_definitions(Core PRIVATE
    LUASTG_CORE_USING_IMGUI
    MA_USE_STDINT
)
target_include_directories(Core PUBLIC
    .
    ${CMAKE_BINARY_DIR}/include/minizip
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/utility
    ${CMAKE_SOURCE_DIR}/external/include
)

set(Core_SRC
    Core/Type.hpp

    Core/i18n.hpp
    Core/i18n.cpp
    Core/framework.hpp
    Core/framework.cpp
    Core/Object.hpp
    Core/DataObject.cpp

    Core/FileManager.hpp
    Core/FileManager.cpp
    Core/InitializeConfigure.hpp
    Core/InitializeConfigure.cpp

    Core/Graphics/Window.hpp
    Core/Graphics/Window_SDL.hpp
    Core/Graphics/Window_SDL.cpp
    Core/Graphics/Format.hpp
    Core/Graphics/Device.hpp
    Core/Graphics/Device_OpenGL.hpp
    Core/Graphics/Device_OpenGL.cpp
    Core/Graphics/SwapChain.hpp
    Core/Graphics/SwapChain_OpenGL.hpp
    Core/Graphics/SwapChain_OpenGL.cpp
    Core/Graphics/Renderer.hpp
    Core/Graphics/Renderer_OpenGL.hpp
    Core/Graphics/Renderer_OpenGL.cpp
    Core/Graphics/Renderer_Shader_OpenGL.cpp
    Core/Graphics/Model_OpenGL.hpp
    Core/Graphics/Model_OpenGL.cpp
    Core/Graphics/Model_Shader_OpenGL.cpp
    Core/Graphics/Sprite.hpp
    Core/Graphics/Sprite_OpenGL.hpp
    Core/Graphics/Sprite_OpenGL.cpp
    Core/Graphics/Font.hpp
    Core/Graphics/Font_OpenGL.hpp
    Core/Graphics/Font_OpenGL.cpp
    Core/Application.hpp
    Core/ApplicationModel.hpp
    Core/ApplicationModel_SDL.hpp
    Core/ApplicationModel_SDL.cpp
    Core/EventDispatcherImpl.hpp

    Core/Audio/Decoder.hpp
    Core/Audio/Decoder_ma.cpp
    Core/Audio/Decoder_ma.hpp
    Core/Audio/Decoder_ALL.cpp
    Core/Audio/Device.hpp
    Core/Audio/Device_SDL.cpp
    Core/Audio/Device_SDL.hpp
)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${Core_SRC})
target_precompile_headers(Core PRIVATE
    Core/framework.hpp
)
target_sources(Core PRIVATE
    ${Core_SRC}
)

target_link_libraries(Core PUBLIC
    # debug
    spdlog
    tracy
    imgui
    implot
    # util
    # utility
    PlatformAPI
    # gfx
    libqoi
    SDL2-static
    nothings_stb
    glad
    # math
    xmath
    glm
    # file
    minizip
    # text
    freetype
    uni-algo::uni-algo
    # model
    tinygltf
    # audio
    dr_libs
    Ogg::ogg
    Vorbis::vorbis
    Vorbis::vorbisfile
    FLAC::FLAC
    # database
    nlohmann_json
)

# add_dependencies(Core fuck_zlib_ng_and_minizip_ng)
