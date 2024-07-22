#include "AppFrame.h"
#include "resource.h"

namespace LuaSTGPlus
{
    static std::string const MODE_NAME_WINDOW("Windowed");
    static std::string const MODE_NAME_FULLSCREEN("Fullscreen");

    inline bool isRationalEmpty(Core::Rational const& rational)
    {
        return rational.numerator == 0 || rational.denominator == 0;
    }

    inline bool isRectEmpty(Core::RectI const& rect)
    {
        return (rect.b.x - rect.a.x) > 0
            && (rect.b.y - rect.a.y) > 0
            ;
    }

    inline bool isRectSameSize(Core::RectI const& rect1, Core::RectI const& rect2)
    {
        return (rect1.b.x - rect1.a.x) == (rect2.b.x - rect2.a.x)
            && (rect1.b.y - rect1.a.y) == (rect2.b.y - rect2.a.y)
            ;
    }

    inline uint32_t matchMonitorIndex(Core::Graphics::IWindow* window, Core::RectI const& monitor_rect, bool& find)
    {
        uint32_t const count = window->getMonitorCount();
        // Stage 1: Match Same Rect
        for (uint32_t i = 0; i < count; i += 1)
        {
            Core::RectI const rect = window->getMonitorRect(i);
            if (rect == monitor_rect)
            {
                find = true;
                return i;
            }
        }
        // Stage 2: Match Same Size
        for (uint32_t i = 0; i < count; i += 1)
        {
            Core::RectI const rect = window->getMonitorRect(i);
            if (isRectSameSize(rect, monitor_rect))
            {
                find = true;
                return i;
            }
        }
        // Stage 3: fallback
        find = false;
        return 0;
    }

    inline Core::Vector2I getMonitorSize(Core::Graphics::IWindow* window, uint32_t index)
    {
        Core::RectI const rect = window->getMonitorRect(index);
        return Core::Vector2I(rect.b.x - rect.a.x, rect.b.y - rect.a.y);
    }

    inline std::string_view getFullscreenTypeString(ApplicationSetting const& setting)
    {
        if (setting.fullscreen)
            return MODE_NAME_FULLSCREEN;
        else
            return MODE_NAME_WINDOW;
    }

    inline void logResult(bool ok, ApplicationSetting const& from_mode, std::string_view to_mode)
    {
        if (ok)
            spdlog::info("[luastg] Display mode switched: {} -> {}", getFullscreenTypeString(from_mode), to_mode);
        else
            spdlog::error("[luastg] Display mode switch failed: {} -> {}", getFullscreenTypeString(from_mode), to_mode);
    }

    bool AppFrame::SetDisplayModeWindow(Core::Vector2U window_size, bool vsync, uint32_t monitor_idx, bool borderless)
    {
        auto* window = GetAppModel()->getWindow();
        auto* swapchain = GetAppModel()->getSwapChain();

        swapchain->setVSync(vsync);
        bool const result = swapchain->setCanvasSize(window_size);

        window->setWindowMode(window_size);
        window->setMonitorCentered(monitor_idx);
        
        logResult(result, m_Setting, MODE_NAME_WINDOW);

        m_Setting.canvas_size = window_size;
        m_Setting.fullscreen = false;
        m_Setting.vsync = vsync;
        
        return result;
    }

    bool AppFrame::SetDisplayModeBorderlessFullscreen(Core::Vector2U window_size, uint32_t monitor_idx, bool vsync)
    {
        auto* window = GetAppModel()->getWindow();
        auto* swapchain = GetAppModel()->getSwapChain();

        Core::Vector2I const monitor_size = getMonitorSize(window, monitor_idx);

        swapchain->setVSync(vsync);
        bool const result = swapchain->setCanvasSize(window_size);

        window->setBorderlessFullScreenMode();
        window->setMonitorFullScreen(monitor_idx);

        logResult(result, m_Setting, MODE_NAME_FULLSCREEN);

        m_Setting.canvas_size = window_size;
        m_Setting.fullscreen = true;
        m_Setting.vsync = vsync;
        
        return result;
    }

    bool AppFrame::SetDisplayModeExclusiveFullscreen(Core::Vector2U window_size, bool vsync, Core::Rational)
    {
        auto* window = GetAppModel()->getWindow();
        auto* swapchain = GetAppModel()->getSwapChain();

        swapchain->setVSync(vsync);
        bool const result = swapchain->setCanvasSize(window_size);

        window->setWindowMode(window_size);
        window->setExclusiveFullScreenMode();

        logResult(result, m_Setting, MODE_NAME_FULLSCREEN);

        m_Setting.canvas_size = window_size;
        m_Setting.fullscreen = true;
        m_Setting.vsync = vsync;

        return result;
    }

    bool AppFrame::UpdateDisplayMode()
    {
        if (m_Setting.fullscreen)
            return SetDisplayModeExclusiveFullscreen(
                m_Setting.canvas_size,
                m_Setting.vsync,
                Core::Rational());
        else
            return SetDisplayModeWindow(
                m_Setting.canvas_size,
                m_Setting.vsync,
                0,
                false);
    }

    bool AppFrame::InitializationApplySettingStage1()
    {
        // Configure window
        {
            using namespace Core::Graphics;
            auto* p_window = m_pAppModel->getWindow();
            p_window->setTitleText(m_Setting.window_title);
            p_window->setCursor(m_Setting.show_cursor ? WindowCursor::Arrow : WindowCursor::None);
            p_window->setSize(m_Setting.canvas_size);
        }
        // Set volume
        {
            using namespace Core::Audio;
            auto* p_audio = GetAppModel()->getAudioDevice();
            p_audio->setMixChannelVolume(MixChannel::SoundEffect, m_Setting.volume_sound_effect);
            p_audio->setMixChannelVolume(MixChannel::Music, m_Setting.volume_music);
        }
        return true;
    }

    bool AppFrame::InitializationApplySettingStage2()
    {
        auto* p_swapchain = GetAppModel()->getSwapChain();
        // Init swapchain first
        bool const result = p_swapchain->setWindowMode(m_Setting.canvas_size);
        if (!result) return false;
        // Formally update display mode
        UpdateDisplayMode();
        // Refresh picture first to avoid white screen
        p_swapchain->clearRenderAttachment();
        p_swapchain->present();
        return true;
    }

    void AppFrame::SetWindowed(bool v)
    {
        if (m_iStatus == AppStatus::Initializing)
        {
            m_Setting.fullscreen = !v;
        }
        else if (m_iStatus == AppStatus::Running)
        {
            spdlog::warn("[luastg] SetWindowed: launch-only function called at runtime");
        }
    }

    void AppFrame::SetVsync(bool v)
    {
        if (m_iStatus == AppStatus::Initializing)
        {
            m_Setting.vsync = v;
        }
        else if (m_iStatus == AppStatus::Running)
        {
            spdlog::warn("[luastg] SetVsync: launch-only function called at runtime");
        }
    }

    void AppFrame::SetResolution(uint32_t width, uint32_t height)
    {
        if (m_iStatus == AppStatus::Initializing)
        {
            m_Setting.canvas_size = Core::Vector2U(width, height);
        }
        else if (m_iStatus == AppStatus::Running)
            spdlog::warn("[luastg] SetResolution: launch-only function called at runtime");
    }
}
