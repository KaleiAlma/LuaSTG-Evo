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

    Core/Window.hpp
    Core/Window_SDL.hpp
    Core/Window_SDL.cpp
    Core/Graphics/Format.hpp
    Core/Graphics/Interface/Device.hpp
    Core/Graphics/Interface/SwapChain.hpp
    Core/Graphics/Interface/Renderer.hpp
    Core/Graphics/Interface/Sprite.hpp
    Core/Graphics/Interface/Font.hpp
    Core/Graphics/Common/Sprite.hpp
    Core/Graphics/Common/Sprite.cpp
    Core/Graphics/Common/Font.hpp
    Core/Graphics/Common/Font.cpp
    Core/Graphics/OpenGL/Device.hpp
    Core/Graphics/OpenGL/Device.cpp
    Core/Graphics/OpenGL/SwapChain.hpp
    Core/Graphics/OpenGL/SwapChain.cpp
    Core/Graphics/OpenGL/Renderer.hpp
    Core/Graphics/OpenGL/Renderer.cpp
    Core/Graphics/OpenGL/Renderer_Shader.cpp
    Core/Graphics/OpenGL/Model.hpp
    Core/Graphics/OpenGL/Model.cpp
    Core/Graphics/OpenGL/Model_Shader.cpp
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
    # tracy
    # imgui
    # implot
    # util
    # utility
    PlatformAPI
    # gfx
    # libqoi
    # SDL3
    # nothings_stb
    glad
    # math
    # xmath
    glm
    # file
    minizip
    # text
    freetype
    uni-algo
    # model
    # tinygltf
    # audio
    # dr_libs
    # database
    # nlohmann_json
)

target_link_libraries(Core INTERFACE SDL3)

# add_dependencies(Core fuck_zlib_ng_and_minizip_ng)
