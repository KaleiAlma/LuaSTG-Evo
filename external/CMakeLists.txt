
# ==================== steam api ====================

add_subdirectory(steam_api)

# ==================== glad ====================

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
add_library(glad STATIC)
luastg_target_common_options(glad)
target_sources(glad PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/gl.c
)
