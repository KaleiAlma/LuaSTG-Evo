
target_include_directories(LuaSTG PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)
if(WIN32)
    set(_LuaSTG_res
        ${CMAKE_CURRENT_LIST_DIR}/app.ico
        ${CMAKE_CURRENT_LIST_DIR}/resource.h
        ${CMAKE_CURRENT_LIST_DIR}/resource.rc
    )
elseif(APPLE)
    set(_LuaSTG_res
        ${CMAKE_CURRENT_LIST_DIR}/app.icns
    )
endif()

list(APPEND _LuaSTG_res
    ${CMAKE_CURRENT_LIST_DIR}/Config.h
    ${CMAKE_CURRENT_LIST_DIR}/AppFrameLuaEx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ResourcePassword.cpp
)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} PREFIX LuaSTG/Custom FILES ${_LuaSTG_res})
target_sources(LuaSTG PRIVATE
    ${_LuaSTG_res}
)

if(APPLE)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/app.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    # set(MACOSX_BUNDLE_ICON_FILE app.icns)
    set_target_properties(LuaSTG PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/MacOSXBundleInfo.plist.in
        MACOSX_BUNDLE_ICON_FILE Resources/app.icns
    )
endif()