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
		// DWORD win32_window_style{ WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX) };
		// DWORD win32_window_style_ex{ 0 };
		uint32_t sdl_window_flags{ SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI };
		bool m_hidewindow{ true };
		bool m_redirect_bitmap{ true };
		SDL_Rect m_last_window_rect{};
		bool m_alt_down{ false };
		FullscreenMode m_fullscreen_mode{ FullscreenMode::Windowed };
		int32_t m_monitor_idx{};
		bool m_ignore_size_message{ false }; // 在 SetWindowLongPtr 修改窗口样式时，可以忽略 WM_SIZE

		// bool win32_window_is_sizemove{ false };
		// bool win32_window_is_menu_loop{ false };
		// bool win32_window_want_track_focus{ false };
		// bool enable_track_window_focus{ false };

		// Platform::WindowSizeMoveController m_sizemove;
		// Platform::MonitorList m_monitors;
		// Platform::RuntimeLoader::DesktopWindowManager dwmapi_loader;

		// LRESULT onMessage(HWND window, UINT message, WPARAM arg1, LPARAM arg2);

		// bool createWindowClass();
		// void destroyWindowClass();
		bool createWindow();
		void destroyWindow();

		IApplicationModel* m_framework{};

	public:
		// 内部方法

		SDL_Window* GetWindow() { return sdl_window; }

		// void convertTitleText();

		RectI getRect();
		bool setRect(RectI v);
		// RectI getClientRect();
		// bool setClientRect(RectI v);
		// uint32_t getDPI();
		// void setRedirectBitmapEnable(bool enable);
		// bool getRedirectBitmapEnable();
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
			// WindowDpiChanged,

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
		// void setNativeIcon(void* id);

		// void setIMEState(bool enable);
		// bool getIMEState();

		void setTitleText(StringView str);
		StringView getTitleText();

		bool setFrameStyle(WindowFrameStyle style);
		WindowFrameStyle getFrameStyle();

		Vector2U getSize();
		bool setSize(Vector2U v);

		WindowLayer getLayer();
		bool setLayer(WindowLayer layer);

		// float getDPIScaling();

		void setWindowMode(Vector2U size);
		void setExclusiveFullScreenMode();
		void setBorderlessFullScreenMode();

		uint32_t getMonitorCount();
		RectI getMonitorRect(uint32_t index);
		void setMonitorCentered(uint32_t index);
		void setMonitorFullScreen(uint32_t index);

		// void setCustomSizeMoveEnable(bool v);
		// void setCustomMinimizeButtonRect(RectI v);
		// void setCustomCloseButtonRect(RectI v);
		// void setCustomMoveButtonRect(RectI v);

		bool setCursor(WindowCursor type);
		WindowCursor getCursor();

		// void setWindowCornerPreference(bool allow);

	public:
		Window_SDL();
		~Window_SDL();

	public:
		static bool create(Window_SDL** pp_window);
		static bool create(Vector2U size, StringView title_text, WindowFrameStyle style, bool show, Window_SDL** pp_window);
	};
}
