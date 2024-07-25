#pragma once
#include "Core/Object.hpp"
#include "Core/ApplicationModel.hpp"
#include "Core/Graphics/Window.hpp"
#include "Core/Type.hpp"
#include "SDL_events.h"
// #include "Platform/Monitor.hpp"
// #include "Platform/WindowSizeMoveController.hpp"
// #include "Platform/RuntimeLoader/DesktopWindowManager.hpp"
#include <SDL_rect.h>
#include <SDL_surface.h>
#include <SDL_video.h>
#include <cstdint>
#include <string>

namespace Core::Graphics
{
    class Window_SDL : public Object<IWindow>
    {
    private:

        SDL_Window* sdl_window{ nullptr };

        uint32_t sdl_window_width{ 640 };
        uint32_t sdl_window_height{ 480 };

        SDL_Surface* sdl_window_icon{ nullptr };

        std::string sdl_window_text{ "Window" };

        WindowCursor m_cursor{ WindowCursor::Arrow };
        SDL_Cursor* sdl_window_cursor{ nullptr };

        WindowFrameStyle m_framestyle{ WindowFrameStyle::Fixed };
        uint32_t sdl_window_flags{ SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI };
        bool m_hidewindow{ true };
        bool m_redirect_bitmap{ true };
        SDL_Rect m_last_window_rect{};
        bool m_alt_down{ false };
        FullscreenMode m_fullscreen_mode{ FullscreenMode::Windowed };
        int32_t m_monitor_idx{};

        std::string m_text_input;
        std::string m_ime_comp;
        uint32_t m_text_cursor_pos{ 0 };
        int32_t m_ime_cursor_pos{ -1 };
        bool m_return_enable{ false };

        bool createWindow();
        void destroyWindow();

        IApplicationModel* m_framework{};

    public:
        // Internal Method

        SDL_Window* GetWindow() { return sdl_window; }

        RectI getRect();
        bool setRect(RectI v);
        bool recreateWindow();
        void _toggleFullScreenMode();
        void _setWindowMode(Vector2U size, bool ignore_size);
        void _setBorderlessFullScreenMode();
        void _setFullScreenMode();

        void implSetApplicationModel(IApplicationModel* p_framework) { m_framework = p_framework; }

        void handleEvents();

    private:
        enum class EventType
        {
            WindowCreate,
            WindowDestroy,

            WindowActive,
            WindowInactive,

            WindowClose,

            WindowSize,
            WindowFullscreenStateChange,

            NativeWindowMessage,

            DeviceChange,
        };
        union EventData
        {
            Vector2I window_size;
            bool window_fullscreen_state;
            SDL_Event event;
        };
        bool m_is_dispatch_event{ false };
        std::vector<IWindowEventListener*> m_eventobj;
        std::vector<IWindowEventListener*> m_eventobj_late;
        void dispatchEvent(EventType t, EventData d = {});

    public:
        void addEventListener(IWindowEventListener* e);
        void removeEventListener(IWindowEventListener* e);

        void* getNativeHandle();

        void setTitleText(StringView str);
        StringView getTitleText();

        bool setFrameStyle(WindowFrameStyle style);
        WindowFrameStyle getFrameStyle();

        Vector2U getSize();
        bool setSize(Vector2U v);

        WindowLayer getLayer();
        bool setLayer(WindowLayer layer);

        void setWindowMode(Vector2U size);
        void setExclusiveFullScreenMode();
        void setBorderlessFullScreenMode();

        uint32_t getMonitorCount();
        RectI getMonitorRect(uint32_t index);
        void setMonitorCentered(uint32_t index);
        void setMonitorFullScreen(uint32_t index);

        bool setCursor(WindowCursor type);
        WindowCursor getCursor();

        void setTextInputEnable(bool enable);
        std::string getTextInput();
        std::string getIMEComp();
        void setTextInput(StringView text);
        void clearTextInput();
        uint32_t getTextInputLength();
        uint32_t getTextCursorPos();
        uint32_t getTextCursorPosRaw();
        int32_t getIMECursorPos();
        bool setTextCursorPos(uint32_t pos);
        void insertInputTextAtCursor(StringView text, bool move_cursor = true);
        bool insertInputText(StringView text, uint32_t pos);
        uint32_t removeInputTextAtCursor(uint32_t length, bool after);
        int32_t removeInputText(uint32_t length, uint32_t pos);
        void setTextInputReturnEnable(bool enable);
        void setTextInputRect(RectI rect);

        std::string getClipboardText();
        bool setClipboardText(StringView text);

    public:
        Window_SDL();
        ~Window_SDL();

    public:
        static bool create(Window_SDL** pp_window);
        static bool create(Vector2U size, StringView title_text, WindowFrameStyle style, bool show, Window_SDL** pp_window);
    };
}
