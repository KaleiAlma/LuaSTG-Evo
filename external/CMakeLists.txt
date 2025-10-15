
# ==================== steam api ====================

add_subdirectory(steam_api)

# ==================== glad ====================

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
add_library(glad STATIC)
luastg_target_common_options(glad)
target_sources(glad PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/gl.c
)

# ==================== lua imgui ====================

add_library(lua_imgui STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/include/lua_imgui.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/include/lua_imgui_type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_common.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_enum.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_enum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_hash.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_hash.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_ImGui.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_ImGui_ColorX.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_ImGui_DragX.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_ImGui_InputX.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_ImGui_PlotX.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_ImGui_register.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_ImGui_SliderX.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_ImGui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_ImGuiTextBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui_type.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/src/lua_imgui.cpp
)

target_include_directories(lua_imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/binding/include
)

target_link_libraries(lua_imgui PUBLIC luajit)
