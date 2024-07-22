#include "AppFrame.h"
#include "Core/FileManager.hpp"
#include "Debugger/ImGuiExtension.h"
#include "LuaBinding/LuaAppFrame.hpp"

using namespace LuaSTGPlus;

////////////////////////////////////////////////////////////////////////////////
/// AppFrame
////////////////////////////////////////////////////////////////////////////////

AppFrame& AppFrame::GetInstance()
{
    static AppFrame s_Instance;
    return s_Instance;
}
AppFrame::AppFrame() noexcept = default;
AppFrame::~AppFrame() noexcept
{
    if (m_iStatus != AppStatus::NotInitialized && m_iStatus != AppStatus::Destroyed)
    {
        // If the framework has not been destroyed, destroy it
        Shutdown();
    }
}

#pragma region 脚本接口

void AppFrame::SetFPS(uint32_t v)noexcept
{
    m_Setting.target_fps = (v > 1u) ? v : 1u; // It has to be at least 1 fps
}
void AppFrame::SetSEVolume(float v)
{
    m_Setting.volume_sound_effect = v;
    if (GetAppModel())
        GetAppModel()->getAudioDevice()->setMixChannelVolume(Core::Audio::MixChannel::SoundEffect, v);
}
void AppFrame::SetBGMVolume(float v)
{
    m_Setting.volume_music = v;
    if (GetAppModel())
        GetAppModel()->getAudioDevice()->setMixChannelVolume(Core::Audio::MixChannel::Music, v);
}
void AppFrame::SetTitle(const char* v)noexcept
{
    try
    {
        m_Setting.window_title = v;
        if (m_pAppModel)
            m_pAppModel->getWindow()->setTitleText(v);
    }
    catch (const std::bad_alloc&)
    {
        spdlog::error("[luastg] SetTitle: Out of memory");
    }
}
void AppFrame::SetSplash(bool v)noexcept
{
    m_Setting.show_cursor = v;
    if (m_pAppModel)
    {
        m_pAppModel->getWindow()->setCursor(m_Setting.show_cursor ? Core::Graphics::WindowCursor::Arrow : Core::Graphics::WindowCursor::None);
    }
}

int AppFrame::LoadTextFile(lua_State* L_, const char* path, const char *packname)noexcept
{
    if (ResourceMgr::GetResourceLoadingLog()) {
        if (packname)
            spdlog::info("[luastg] Reading text file '{}' in package '{}'", packname, path);
        else
            spdlog::info("[luastg] Reading text file '{}'", path);
    }
    bool loaded = false;
    std::vector<uint8_t> src;
    if (packname)
    {
        auto& arc = GFileManager().getFileArchive(packname);
        if (!arc.empty())
        {
            loaded = arc.load(path, src);
        }
    }
    else
    {
        loaded = GFileManager().loadEx(path, src);
    }
    if (!loaded) {
        spdlog::error("[luastg] Unable to load file '{}'", path);
        return 0;
    }
    lua_pushlstring(L_, (char*)src.data(), src.size());
    return 1;
}

#pragma endregion

#pragma region 框架函数

bool AppFrame::Init()noexcept
{
    assert(m_iStatus == AppStatus::NotInitialized);
    
    spdlog::info(LUASTG_INFO);
    spdlog::info("[luastg] Initializing Engine");
    m_iStatus = AppStatus::Initializing;

    //////////////////////////////////////// Lua Init
    
    spdlog::info("[luastg] Initializing LuaJIT");
    
    // Initialize Lua VM
    if (!OnOpenLuaEngine())
    {
        spdlog::info("[luastg] Failed to initialize LuaJIT");
        return false;
    }
    
    // Load launch script (optional)
    if (!OnLoadLaunchScriptAndFiles())
    {
        return false;
    }
    
    //////////////////////////////////////// Initialize Engine
    {
        if (!Core::IApplicationModel::create(this, ~m_pAppModel))
            return false;
        if (!Core::Graphics::ITextRenderer::create(m_pAppModel->getRenderer(), ~m_pTextRenderer))
            return false;
        if (!InitializationApplySettingStage1())
            return false;

        // Allocate space for object pools
        spdlog::info("[luastg] Initializing object pool with capacity: {}", LOBJPOOL_SIZE);
        try
        {
            m_GameObjectPool = std::make_unique<GameObjectPool>(L);
        }
        catch (const std::bad_alloc&)
        {
            spdlog::error("[luastg] Unable to allocate memory for object pool");
            return false;
        }
    
        // Renderer adapter
        m_bRenderStarted = false;
        
        OpenInput();

        // Initialize ImGui
        #ifdef USING_DEAR_IMGUI
            imgui::bindEngine();
        #endif
        
        if (!InitializationApplySettingStage2())
            return false;
    }

    // Load main script
    if (!OnLoadMainScriptAndFiles())
    {
        return false;
    }

    //////////////////////////////////////// Initialization Complete
    m_iStatus = AppStatus::Initialized;
    spdlog::info("[luastg] Initialization Complete");
    
    //////////////////////////////////////// Call GameInit
    if (!SafeCallGlobalFunction(LuaSTG::LuaEngine::G_CALLBACK_EngineInit)) {
        return false;
    }
    
    return true;
}
void AppFrame::Shutdown()noexcept
{
    if (L) {
        SafeCallGlobalFunction(LuaSTG::LuaEngine::G_CALLBACK_EngineStop);
    }
    
    m_GameObjectPool = nullptr;
    spdlog::info("[luastg] Object pool destroyed");

    if (L)
    {
        lua_close(L);
        L = nullptr;
        spdlog::info("[luastg] LuaJIT shutdown");
    }

    m_stRenderTargetStack.clear();
    m_ResourceMgr.ClearAllResource();
    spdlog::info("[luastg] Resources freed");
    
    // 卸载ImGui
    #ifdef USING_DEAR_IMGUI
        imgui::unbindEngine();
    #endif

    GFileManager().unloadAllFileArchive();
    spdlog::info("[luastg] Freed packages");
    
    CloseInput();
    m_pTextRenderer = nullptr;
    m_pAppModel = nullptr;

    m_iStatus = AppStatus::Destroyed;
    spdlog::info("[luastg] Engine shutdown.");
}
void AppFrame::Run()noexcept
{
    assert(m_iStatus == AppStatus::Initialized);
    spdlog::info("[luastg] Start Update/Render Loop");
    
    m_pAppModel->getWindow()->addEventListener(this);
    m_pAppModel->getSwapChain()->addEventListener(this);

    m_pAppModel->getFrameRateController()->setTargetFPS(m_Setting.target_fps);
    m_pAppModel->run();
    
    m_pAppModel->getSwapChain()->removeEventListener(this);
    m_pAppModel->getWindow()->removeEventListener(this);

    spdlog::info("[luastg] Exiting Update & Render Loop");
}

#pragma endregion

#pragma region 游戏循环

void AppFrame::onWindowCreate()
{
    OpenInput();
}
void AppFrame::onWindowDestroy()
{
    CloseInput();
}
void AppFrame::onWindowActive()
{
    m_window_active_changed.fetch_or(0x1);
}
void AppFrame::onWindowInactive()
{
    m_window_active_changed.fetch_or(0x2);
}
void AppFrame::onWindowSize(Core::Vector2I size)
{
    m_sdl_window_size = size;
}
void AppFrame::onDeviceChange()
{
    m_window_active_changed.fetch_or(0x4);
}

bool AppFrame::onUpdate()
{
    m_fFPS = m_pAppModel->getFrameRateController()->getFPS();
    m_fAvgFPS = m_pAppModel->getFrameRateController()->getAvgFPS();
    m_pAppModel->getFrameRateController()->setTargetFPS(m_Setting.target_fps);

    bool result = true;

    {
        ZoneScopedN("OnUpdate-Event");

        int window_active_changed = m_window_active_changed.exchange(0);
        if (window_active_changed & 0x2)
        {
            lua_pushinteger(L, (lua_Integer)LuaSTG::LuaEngine::EngineEvent::WindowActive);
            lua_pushboolean(L, false);
            SafeCallGlobalFunctionB(LuaSTG::LuaEngine::G_CALLBACK_EngineEvent, 2, 0);

            if (!SafeCallGlobalFunction(LuaSTG::LuaEngine::G_CALLBACK_FocusLoseFunc))
            {
                result = false;
                m_pAppModel->requestExit();
            }
        }
        if (window_active_changed & 0x1)
        {
            lua_pushinteger(L, (lua_Integer)LuaSTG::LuaEngine::EngineEvent::WindowActive);
            lua_pushboolean(L, true);
            SafeCallGlobalFunctionB(LuaSTG::LuaEngine::G_CALLBACK_EngineEvent, 2, 0);

            if (!SafeCallGlobalFunction(LuaSTG::LuaEngine::G_CALLBACK_FocusGainFunc))
            {
                result = false;
                m_pAppModel->requestExit();
            }
        }

        UpdateInput();
    }

#if (defined(_DEBUG) && defined(LuaSTG_enable_GameObjectManager_Debug))
    static uint64_t _frame_count = 0;
    spdlog::debug("[frame] ---------- {} ----------", _frame_count);
    _frame_count += 1;
#endif

    if (result)
    {
        ZoneScopedN("OnUpdate-LuaCallback");
        // Run frame function
        imgui::cancelSetCursor();
        m_GameObjectPool->DebugNextFrame();
        if (!SafeCallGlobalFunction(LuaSTG::LuaEngine::G_CALLBACK_EngineUpdate, 1))
        {
            result = false;
            m_pAppModel->requestExit();
        }
        bool tAbort = lua_toboolean(L, -1) != 0;
        lua_pop(L, 1);
        if (tAbort)
            m_pAppModel->requestExit();
        m_ResourceMgr.UpdateSound();
    }

    ResetKeyboardInput();

    return result;
}
bool AppFrame::onRender()
{
    m_bRenderStarted = true;

    GetRenderTargetManager()->BeginRenderTargetStack();

    // Run render function
    bool result = SafeCallGlobalFunction(LuaSTG::LuaEngine::G_CALLBACK_EngineDraw);
    if (!result)
        m_pAppModel->requestExit();

    GetRenderTargetManager()->EndRenderTargetStack();

    m_bRenderStarted = false;
    return result;
}

#pragma endregion
