#pragma once
#include "Core/ApplicationModel.hpp"
#include "Core/Graphics/Font.hpp"
#include "GameResource/ResourceManager.h"
#include "GameObject/GameObjectPool.h"

namespace LuaSTGPlus
{
    /// Application Status
    enum class AppStatus
    {
        NotInitialized,
        Initializing,
        Initialized,
        Running,
        Aborted,
        Destroyed,
    };

    struct ApplicationSetting
    {
        // Display mode - Canvas size (not window size)
        Core::Vector2U canvas_size{ 640,480 };
        // Display mode - Fullscreen
        bool fullscreen{ false };
        // Display mode - Vsync
        bool vsync{ false };

        // Show cursor
        bool show_cursor{ true };

        // Target framerate
        uint32_t target_fps{ 60 };

        // Window title
        std::string window_title{ LUASTG_INFO };

        // Volume: SFX
        float volume_sound_effect{ 1.0f };
        // Volume: BGM
        float volume_music{ 1.0f };
    };

    struct IRenderTargetManager
    {
        // Render Target Stack

        virtual bool BeginRenderTargetStack() = 0;
        virtual bool EndRenderTargetStack() = 0;
        virtual bool PushRenderTarget(IResourceTexture* rt) = 0;
        virtual bool PopRenderTarget() = 0;
        virtual bool CheckRenderTargetInUse(IResourceTexture* rt) = 0;
        virtual Core::Vector2U GetTopRenderTargetSize() = 0;

        // Maintain auto-size render targets

        virtual void AddAutoSizeRenderTarget(IResourceTexture* rt) = 0;
        virtual void RemoveAutoSizeRenderTarget(IResourceTexture* rt) = 0;
        virtual Core::Vector2U GetAutoSizeRenderTargetSize() = 0;
        virtual bool ResizeAutoSizeRenderTarget(Core::Vector2U size) = 0;
    };

    /// Application Framework
    class AppFrame
        : public Core::IApplicationEventListener
        , public Core::Graphics::IWindowEventListener
        , public Core::Graphics::ISwapChainEventListener
        , public IRenderTargetManager
    {
    private:
        AppStatus m_iStatus = AppStatus::NotInitialized;

        // Application Framework
        Core::ScopeObject<Core::IApplicationModel> m_pAppModel;
        Core::ScopeObject<Core::Graphics::ITextRenderer> m_pTextRenderer;

        // Resource Manager
        ResourceMgr m_ResourceMgr;

        // Object pool
        std::unique_ptr<GameObjectPool> m_GameObjectPool;

        // Lua VM
        lua_State* L = nullptr;

        // Settings
        ApplicationSetting m_Setting;

        // Measured FPS
        double m_fFPS = 0.;
        double m_fAvgFPS = 0.;

        // Rendering state
        bool m_bRenderStarted = false;

    public:
        /// Protected mode script execution
        /// Framework-only, called from the outermost level of main logic.
        /// If an error occurs while the script is running, this function is responsible for intercepting the error and issuing an error message.
        bool SafeCallScript(const char* source, size_t len, const char* desc)noexcept;
        
        /// Unprotected calls to global functions
        /// Framework-only, called from the outermost level of main logic.
        bool UnsafeCallGlobalFunction(const char* name, int retc = 0)noexcept;
        
        /// Protected calls to global functions
        /// Framework-only, called from the outermost level of main logic.
        /// If an error occurs while the script is running, this function is responsible for intercepting the error and issuing an error message.
        /// The caller is responsible for maintaining stack balance.
        bool SafeCallGlobalFunction(const char* name, int retc = 0)noexcept;

        /// Protected calls to global functions
        /// Framework-only, called from the outermost level of main logic.
        /// If an error occurs while the script is running, this function is responsible for intercepting the error and issuing an error message.
        /// The caller is responsible for maintaining stack balance.
        bool SafeCallGlobalFunctionB(const char* name, int argc = 0, int retc = 0)noexcept;
        
        /// Execute files in package
        /// This function is used by the scripting system
        void LoadScript(lua_State* L, const char* path, const char* packname);
        
        bool OnOpenLuaEngine();
        bool OnLoadLaunchScriptAndFiles();
        bool OnLoadMainScriptAndFiles();
        
    public:  // Input system interface

        void OpenInput();
        void CloseInput();
        void UpdateInput();
        void ResetKeyboardInput();
        void ResetMouseInput();

        // Check that buttons are pressed
        bool GetKeyState(int VKCode)noexcept;

        /// Get last keystroke
        int GetLastKey()noexcept;

        /// Get mouse position (from the bottom left corner, unless `no_flip = true`)
        Core::Vector2F GetMousePosition(bool no_flip = false)noexcept;

        Core::Vector2F GetCurrentWindowSizeF();
        Core::Vector4F GetMousePositionTransformF();

        /// Get mouse wheel delta
        int32_t GetMouseWheelDelta()noexcept;
        
        /// Check if mouse buttons are pressed
        bool GetMouseState_legacy(int button) noexcept;
        bool GetMouseState(int button) noexcept;
        
    public: // Script call interface, see API docs for more info

        void SetTitle(const char* v) noexcept;
        void SetSplash(bool v) noexcept;
        void SetSEVolume(float v);
        void SetBGMVolume(float v);
        float GetSEVolume() const noexcept { return m_Setting.volume_sound_effect; }
        float GetBGMVolume() const noexcept { return m_Setting.volume_music; }

    public: // Window and swapchains

        // Sets display mode to windowed.  
        bool SetDisplayModeWindow(Core::Vector2U window_size, bool vsync, uint32_t monitor_idx, bool borderless);

        // Sets display mode to fullscreen (borderless).  
        bool SetDisplayModeBorderlessFullscreen(Core::Vector2U window_size, uint32_t monitor_idx, bool vsync);

        // Sets display mode to fullscreen (exclusive).  
        // When refresh_rate is 0, a refresh rate is automatically selected.  
        bool SetDisplayModeExclusiveFullscreen(Core::Vector2U window_size, bool vsync, Core::Rational refresh_rate);

        // Update display mode
        bool UpdateDisplayMode();

        bool InitializationApplySettingStage1();

        bool InitializationApplySettingStage2();

        void SetWindowed(bool v);

        void SetVsync(bool v);

        void SetResolution(uint32_t width, uint32_t height);

    public: // Other framework methods

        // Set target FPS
        void SetFPS(uint32_t v) noexcept;

        // Get current average FPS
        double GetFPS() const noexcept { return m_fAvgFPS; }

        // Read a text file from a resource package.  
        // It is possible to read other files, but you may get meaningless results.
        int LoadTextFile(lua_State* L, const char* path, const char *packname) noexcept;
        int LoadCompressedTextFile(lua_State* L, const char* path, const char *packname) noexcept;

        // TODO: LoadBinaryFile?

    public: // Renderer interface
        
        // Apply blend modes
        void updateGraph2DBlendMode(BlendMode m);

        /// Render particles
        bool Render(IParticlePool* p, float hscale = 1, float vscale = 1)noexcept;
        
        /// Render text
        bool RenderText(IResourceFont* p, char* strBuf, Core::RectF rect, Core::Vector2F scale, FontAlignHorizontal halign, FontAlignVertical valign, bool bWordBreak)noexcept;
        Core::Vector2F CalcuTextSize(IResourceFont* p, const char* strBuf, Core::Vector2F scale)noexcept;
        bool RenderText(const char* name, const char* str, float x, float y, float scale, FontAlignHorizontal halign, FontAlignVertical valign)noexcept;
        bool RenderTTF(const char* name, const char* str, float left, float right, float bottom, float top, float scale, int format, Core::Color4B c)noexcept;

        void SnapShot(const char* path)noexcept;
        void SaveTexture(const char* tex_name, const char* path)noexcept;

        // ---------- Draw common shapes ----------

    public:
        void DebugSetGeometryRenderState();
        void DebugDrawCircle(float x, float y, float r, Core::Color4B color);
        void DebugDrawRect(float x, float y, float a, float b, float rot, Core::Color4B color);
        void DebugDrawEllipse(float x, float y, float a, float b, float rot, Core::Color4B color);

        // ---------- Render target management ----------

    private:
        std::vector<Core::ScopeObject<IResourceTexture>> m_stRenderTargetStack;
        std::set<IResourceTexture*> m_AutoSizeRenderTarget;
        Core::Vector2U m_AutoSizeRenderTargetSize;
    private:
        // Render target stack

        bool BeginRenderTargetStack() override;
        bool EndRenderTargetStack() override;
        bool PushRenderTarget(IResourceTexture* rt) override;
        bool PopRenderTarget() override;
        bool CheckRenderTargetInUse(IResourceTexture* rt) override;
        Core::Vector2U GetTopRenderTargetSize() override;

        // Maintain auto-size render targets

        void AddAutoSizeRenderTarget(IResourceTexture* rt) override;
        void RemoveAutoSizeRenderTarget(IResourceTexture* rt) override;
        Core::Vector2U GetAutoSizeRenderTargetSize() override;
        bool ResizeAutoSizeRenderTarget(Core::Vector2U size) override;

    public:
        // Event listener

        void onSwapChainCreate() override;
        void onSwapChainDestroy() override;

        // Interface

        IRenderTargetManager* GetRenderTargetManager();

    public:
        // Text rendering
        bool FontRenderer_SetFontProvider(const char* name);
        void FontRenderer_SetScale(const Core::Vector2F& s);
        
        Core::RectF FontRenderer_MeasureTextBoundary(const char* str, size_t len);
        Core::Vector2F FontRenderer_MeasureTextAdvance(const char* str, size_t len);
        bool FontRenderer_RenderText(const char* str, size_t len, Core::Vector2F& pos, float z, BlendMode blend, Core::Color4B const& color);
        bool FontRenderer_RenderTextInSpace(const char* str, size_t len, Core::Vector3F& pos, Core::Vector3F const& rvec, Core::Vector3F const& dvec, BlendMode blend, Core::Color4B const& color);
        
        float FontRenderer_GetFontLineHeight();
        float FontRenderer_GetFontAscender();
        float FontRenderer_GetFontDescender();
        
    public:
        // Getting framework objects
        lua_State* GetLuaEngine()noexcept { return L; }

        ResourceMgr& GetResourceMgr()noexcept { return m_ResourceMgr; }

        GameObjectPool& GetGameObjectPool()noexcept{ return *m_GameObjectPool; }

        Core::IApplicationModel* GetAppModel() { return m_pAppModel.get(); }
        Core::Graphics::IRenderer* GetRenderer2D() { return m_pAppModel->getRenderer(); }

    public:
        /// Initialize framework
        /// This function must only be called once, at the beginning.
        /// Returns false on failure.
        bool Init()noexcept;
        /// Terminate framework and free resources
        /// This function can be called by the framework itself and must only be called once.
        void Shutdown()noexcept;
        
        /// Execute framework, and enter game loop.
        void Run()noexcept;
        
    protected:
        std::atomic_int m_window_active_changed{ 0 };
        Core::Vector2I m_sdl_window_size;

        void onWindowCreate() override;
        void onWindowDestroy() override;
        void onWindowActive() override;
        void onWindowInactive() override;
        void onWindowSize(Core::Vector2I size) override;
        void onDeviceChange() override;

        bool onUpdate() override;
        bool onRender() override;
    public:
        AppFrame()noexcept;
        ~AppFrame()noexcept;
    public:
        static AppFrame& GetInstance();
    };
}

#define LAPP (LuaSTGPlus::AppFrame::GetInstance())
#define LPOOL (LAPP.GetGameObjectPool())
#define LRES (LAPP.GetResourceMgr())
