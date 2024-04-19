#include "Core/Graphics/Window_SDL.hpp"
#include "Core/ApplicationModel_SDL.hpp"
#include "Core/Graphics/Window.hpp"
#include "Core/InitializeConfigure.hpp"
#include "Core/Type.hpp"
#include "Core/i18n.hpp"
#include "Platform/WindowsVersion.hpp"
// #include "Platform/HighDPI.hpp"
// #include "Platform/WindowTheme.hpp"
#include "utf8.hpp"
#include <SDL2/SDL.h>
#include <cstdint>

// constexpr int const LUASTG_WM_UPDAE_TITLE = WM_USER + 64;
// constexpr int const LUASTG_WM_RECREATE = LUASTG_WM_UPDAE_TITLE + 1;
// constexpr int const LUASTG_WM_SETICON = LUASTG_WM_RECREATE + 1;
// constexpr int const LUASTG_WM_SET_WINDOW_MODE = LUASTG_WM_SETICON + 1;
// constexpr int const LUASTG_WM_SET_FULLSCREEN_MODE = LUASTG_WM_SET_WINDOW_MODE + 1;
// constexpr int const LUASTG_WM_SET_BORDERLESS_FULLSCREEN_MODE = LUASTG_WM_SET_FULLSCREEN_MODE + 1;

namespace Core::Graphics
{
#define APPMODEL ((ApplicationModel_SDL*)m_framework)

	// LRESULT CALLBACK Window_SDL::win32_window_callback(HWND window, UINT message, WPARAM arg1, LPARAM arg2)
	// {
	// 	if (Window_SDL* self = (Window_SDL*)GetWindowLongPtrW(window, GWLP_USERDATA))
	// 	{
	// 		return self->onMessage(window, message, arg1, arg2);
	// 	}
	// 	switch (message)
	// 	{
	// 	case WM_NCCREATE:
	// 		Platform::HighDPI::EnableNonClientDpiScaling(window);
	// 		break;
	// 	case WM_CREATE:
	// 		SetLastError(0);
	// 		SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCTW*)arg2)->lpCreateParams);
	// 		if (DWORD const err = GetLastError())
	// 		{
	// 			spdlog::error("[luastg] (LastError = {}) SetWindowLongPtrW -> #GWLP_USERDATA failed", err);
	// 			return -1;
	// 		}
	// 		return 0;
	// 	}
	// 	return DefWindowProcW(window, message, arg1, arg2);
	// }

	// LRESULT Window_SDL::onMessage(HWND window, UINT message, WPARAM arg1, LPARAM arg2)
	// {
	// 	if (enable_track_window_focus && win32_window_want_track_focus)
	// 	{
	// 		HWND focus_window = GetForegroundWindow();
	// 		if (focus_window && focus_window != window)
	// 		{
	// 			win32_window_want_track_focus = FALSE;
	// 			std::array<WCHAR, 256> buffer1{};
	// 			std::array<WCHAR, 256> buffer2{};
	// 			LRESULT const length1 = SendMessageW(focus_window, WM_GETTEXT, 256, (LPARAM)buffer1.data());
	// 			int const length2 = GetClassNameW(focus_window, buffer2.data(), 256);
	// 			spdlog::info("[core] 窗口焦点已改变，新的焦点窗口为：[{}] ({}) {}", (void*)focus_window,
	// 				utf8::to_string(std::wstring_view(buffer2.data(), (size_t)length2)),
	// 				utf8::to_string(std::wstring_view(buffer1.data(), (size_t)length1))
	// 			);
	// 		}
	// 	}

	// 	// 窗口挪动器
	// 	auto rResult = m_sizemove.handleSizeMove(window, message, arg1, arg2);
	// 	if (rResult.bReturn)
	// 		return rResult.lResult;
	// 	// 特殊
	// 	for (auto& v : m_eventobj)
	// 	{
	// 		if (v)
	// 		{
	// 			auto r = v->onNativeWindowMessage(window, message, arg1, arg2);
	// 			if (r.should_return)
	// 			{
	// 				return r.result;
	// 			}
	// 		}
	// 	}
	// 	// 其他消息
	// 	switch (message)
	// 	{
	// 	case WM_ACTIVATEAPP:
	// 		if (arg1 /* == TRUE */)
	// 		{
	// 			win32_window_want_track_focus = FALSE;
	// 			Platform::WindowTheme::UpdateColorMode(window, TRUE);
	// 			dispatchEvent(EventType::WindowActive);
	// 		}
	// 		else
	// 		{
	// 			win32_window_want_track_focus = TRUE; // 要开始抓内鬼了
	// 			Platform::WindowTheme::UpdateColorMode(window, FALSE);
	// 			dispatchEvent(EventType::WindowInactive);
	// 		}
	// 		m_alt_down = FALSE; // 无论如何，清除该按键状态
	// 		break;
	// 	case WM_SIZE:
	// 		if (!m_ignore_size_message)
	// 		{
	// 			EventData d = {};
	// 			d.window_size = Vector2U(LOWORD(arg2), HIWORD(arg2));
	// 			dispatchEvent(EventType::WindowSize, d);
	// 		}
	// 		break;
	// 	case WM_ENTERSIZEMOVE:
	// 		win32_window_is_sizemove = TRUE;
	// 		InvalidateRect(window, NULL, FALSE); // 标记窗口区域为需要重新绘制，以便产生 WM_PAINT 消息
	// 		return 0;
	// 	case WM_EXITSIZEMOVE:
	// 		win32_window_is_sizemove = FALSE;
	// 		return 0;
	// 	case WM_ENTERMENULOOP:
	// 		win32_window_is_menu_loop = TRUE;
	// 		InvalidateRect(window, NULL, FALSE); // 标记窗口区域为需要重新绘制，以便产生 WM_PAINT 消息
	// 		return 0;
	// 	case WM_EXITMENULOOP:
	// 		win32_window_is_menu_loop = FALSE;
	// 		return 0;
	// 	case WM_PAINT:
	// 		if (win32_window_is_sizemove || win32_window_is_menu_loop)
	// 		{
	// 			APPMODEL->runFrame();
	// 		}
	// 		else
	// 		{
	// 			ValidateRect(window, NULL); // 正常情况下，WM_PAINT 忽略掉
	// 		}
	// 		return 0;
	// 	case WM_SYSKEYDOWN:
	// 	case WM_KEYDOWN:
	// 		if (arg1 == VK_MENU)
	// 		{
	// 			m_alt_down = TRUE;
	// 			return 0;
	// 		}
	// 		if (m_alt_down && arg1 == VK_RETURN)
	// 		{
	// 			_toggleFullScreenMode();
	// 			return 0;
	// 		}
	// 		break;
	// 	case WM_SYSKEYUP:
	// 	case WM_KEYUP:
	// 		if (arg1 == VK_MENU)
	// 		{
	// 			m_alt_down = FALSE;
	// 			return 0;
	// 		}
	// 		break;
	// 	case WM_GETMINMAXINFO:
	// 		{
	// 			MINMAXINFO* info = (MINMAXINFO*)arg2;
	// 			RECT rect_min = { 0, 0, 320, 240 };
	// 			//RECT rect = { 0, 0, (LONG)win32_window_width, (LONG)win32_window_height };
	// 			UINT dpi = Platform::HighDPI::GetDpiForWindow(win32_window);
	// 			if (Platform::HighDPI::AdjustWindowRectExForDpi(&rect_min, win32_window_style, FALSE, win32_window_style_ex, dpi))
	// 			{
	// 				info->ptMinTrackSize.x = rect_min.right - rect_min.left;
	// 				info->ptMinTrackSize.y = rect_min.bottom - rect_min.top;
	// 			}
	// 			//if (Platform::HighDPI::AdjustWindowRectExForDpi(&rect, win32_window_style, FALSE, win32_window_style_ex, dpi))
	// 			//{
	// 			//	info->ptMaxTrackSize.x = rect.right - rect.left;
	// 			//	info->ptMaxTrackSize.y = rect.bottom - rect.top;
	// 			//}
	// 		}
	// 		return 0;
	// 	case WM_DPICHANGED:
	// 		if (getFrameStyle() != WindowFrameStyle::None)
	// 		{
	// 			setSize(getSize()); // 刷新一次尺寸（因为非客户区可能会变化）
	// 			dispatchEvent(EventType::WindowDpiChanged);
	// 			return 0;
	// 		}
	// 		dispatchEvent(EventType::WindowDpiChanged); // 仍然需要通知
	// 		break;
	// 	case WM_SETTINGCHANGE:
	// 	case WM_THEMECHANGED:
	// 		Platform::WindowTheme::UpdateColorMode(window, TRUE);
	// 		break;
	// 	case WM_DEVICECHANGE:
	// 		if (arg1 == 0x0007 /* DBT_DEVNODES_CHANGED */)
	// 		{
	// 			dispatchEvent(EventType::DeviceChange);
	// 		}
	// 		break;
	// 	case WM_SETCURSOR:
	// 		if (LOWORD(arg2) == HTCLIENT)
	// 		{
	// 			SetCursor(win32_window_cursor);
	// 			return TRUE;
	// 		}
	// 		break;
	// 	case WM_MENUCHAR:
	// 		// 快捷键能不能死全家
	// 		return MAKELRESULT(0, MNC_CLOSE);
	// 	case WM_SYSCOMMAND:
	// 		// 鼠标左键点击标题栏图标或者 Alt+Space 不会出现菜单
	// 		switch (arg1 & 0xFFF0)
	// 		{
	// 		case SC_KEYMENU:
	// 		case SC_MOUSEMENU:
	// 			return 0;
	// 		}
	// 		break;
	// 	case WM_CLOSE:
	// 		dispatchEvent(EventType::WindowClose);
	// 		PostQuitMessage(EXIT_SUCCESS);
	// 		return 0;
	// 	case LUASTG_WM_UPDAE_TITLE:
	// 		SetWindowTextW(window, win32_window_text_w.data());
	// 		return 0;
	// 	case LUASTG_WM_RECREATE:
	// 		{
	// 			BOOL result = FALSE;
	// 			WINDOWPLACEMENT last_window_placement = {};

	// 			assert(win32_window);
	// 			result = GetWindowPlacement(win32_window, &last_window_placement);
	// 			assert(result); (void)result;

	// 			destroyWindow();
	// 			if (!createWindow()) return false;

	// 			assert(win32_window);
	// 			result = SetWindowPlacement(win32_window, &last_window_placement);
	// 			assert(result); (void)result;
	// 		}
	// 		return 0;
	// 	case LUASTG_WM_SETICON:
	// 		{
	// 			HICON hIcon = LoadIcon(win32_window_class.hInstance, MAKEINTRESOURCE(win32_window_icon_id));
	// 			SendMessageW(win32_window, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIcon);
	// 			SendMessageW(win32_window, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon);
	// 			DestroyIcon(hIcon);
	// 		}
	// 		return 0;
	// 	case LUASTG_WM_SET_WINDOW_MODE:
	// 		_setWindowMode(Vector2U(LOWORD(arg1), HIWORD(arg1)), arg2);
	// 		return 0;
	// 	case LUASTG_WM_SET_BORDERLESS_FULLSCREEN_MODE:
	// 		_setBorderlessFullscreenMode();
	// 		return 0;
	// 	case LUASTG_WM_SET_FULLSCREEN_MODE:
	// 		_setFullScreenMode();
	// 		return 0;
	// 	}
	// 	return DefWindowProcW(window, message, arg1, arg2);
	// }
	// bool Window_SDL::createWindowClass()
	// {
	// 	HINSTANCE hInstance = GetModuleHandleW(NULL);
	// 	assert(hInstance); // 如果 hInstance 为 NULL 那肯定是见鬼了

	// 	std::memset(win32_window_class_name, 0, sizeof(win32_window_class_name));
	// 	std::swprintf(win32_window_class_name, std::size(win32_window_class_name), L"LuaSTG::Core::Window[%p]", this);

	// 	auto& cls = win32_window_class;
	// 	cls.style = CS_HREDRAW | CS_VREDRAW;
	// 	cls.lpfnWndProc = &win32_window_callback;
	// 	cls.hInstance = hInstance;
	// 	cls.hCursor = LoadCursor(NULL, IDC_ARROW);
	// 	cls.lpszClassName = win32_window_class_name;

	// 	win32_window_class_atom = RegisterClassExW(&cls);
	// 	if (win32_window_class_atom == 0)
	// 	{
	// 		spdlog::error("[luastg] (LastError = {}) RegisterClassExW failed", GetLastError());
	// 		return false;
	// 	}

	// 	return true;
	// }
	// void Window_SDL::destroyWindowClass()
	// {
	// 	if (win32_window_class_atom != 0)
	// 	{
	// 		UnregisterClassW(win32_window_class.lpszClassName, win32_window_class.hInstance);
	// 	}
	// 	win32_window_class_atom = 0;
	// }
	bool Window_SDL::createWindow()
	{
		// if (win32_window_class_atom == 0)
		// {
		// 	return false;
		// }

		// 直接创建窗口

		// convertTitleText();
		// if (!m_redirect_bitmap)
		// {
		// 	win32_window_style_ex |= WS_EX_NOREDIRECTIONBITMAP;
		// }
		// else
		// {
		// 	win32_window_style_ex &= ~DWORD(WS_EX_NOREDIRECTIONBITMAP);
		// }
		// win32_window = CreateWindowExW(
		// 	win32_window_style_ex,
		// 	win32_window_class.lpszClassName,
		// 	win32_window_text_w.data(),
		// 	win32_window_style,
		// 	0, 0, (int)win32_window_width, (int)win32_window_height,
		// 	NULL, NULL, win32_window_class.hInstance, this);
		sdl_window = SDL_CreateWindow(
			sdl_window_text,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			sdl_window_width, sdl_window_height,
			sdl_window_flags
		);
		if (sdl_window == NULL)
		{
			spdlog::error("[luastg] (GetError = {}) SDL_CreateWindow failed", SDL_GetError());
			return false;
		}
		m_monitor_idx = SDL_GetWindowDisplayIndex(sdl_window);

		// 配置输入法

		// win32_window_imc = ImmAssociateContext(win32_window, NULL);
		// if (win32_window_ime_enable)
		// {
		// 	ImmAssociateContext(win32_window, win32_window_imc);
		// }

		// 配置窗口挪动器

		// m_sizemove.setWindow(win32_window);

		// 窗口样式

		// Platform::WindowTheme::UpdateColorMode(win32_window, TRUE);
		// setWindowCornerPreference(m_allow_windows_11_window_corner);

		return true;
	}
	void Window_SDL::destroyWindow()
	{
		// m_sizemove.setWindow(NULL);
		if (sdl_window)
		{
			// if (win32_window_imc)
			// 	ImmAssociateContext(win32_window, win32_window_imc);
			// DestroyWindow(win32_window);
			SDL_DestroyWindow(sdl_window);
		}
		sdl_window = NULL;
		// win32_window_imc = NULL;
	}
	bool Window_SDL::recreateWindow()
	{
		dispatchEvent(EventType::WindowDestroy);
		// SendMessageW(win32_window, LUASTG_WM_RECREATE, 0, 0);
		// ShowWindow(win32_window, SW_SHOWDEFAULT);
		uint32_t flags = SDL_GetWindowFlags(sdl_window);
		Vector2I pos{};
		SDL_GetWindowPosition(sdl_window, &pos.x, &pos.y);
		SDL_DestroyWindow(sdl_window);
		sdl_window = SDL_CreateWindow(sdl_window_text, pos.x, pos.y, sdl_window_width, sdl_window_height, flags);
		
		if (sdl_window == NULL)
		{
			spdlog::error("[luastg] (GetError = {}) SDL_CreateWindow failed", SDL_GetError());
			return false;
		}

		dispatchEvent(EventType::WindowCreate);
		return true;
	}
	void Window_SDL::_toggleFullScreenMode()
	{
		if (m_fullscreen_mode != FullscreenMode::Windowed)
			_setWindowMode(Vector2U(sdl_window_width, sdl_window_height), true);
		else
			_setBorderlessFullScreenMode();
	}
	void Window_SDL::_setWindowMode(Vector2U size, bool ignore_size)
	{
		// HMONITOR win32_monitor = MonitorFromWindow(win32_window, MONITOR_DEFAULTTONEAREST);
		// assert(win32_monitor);
		// MONITORINFO monitor_info = {};
		// monitor_info.cbSize = sizeof(monitor_info);
		// BOOL const get_monitor_info_result = GetMonitorInfoW(win32_monitor, &monitor_info);
		// assert(get_monitor_info_result); (void)get_monitor_info_result;
		// assert(monitor_info.rcMonitor.right > monitor_info.rcMonitor.left);
		// assert(monitor_info.rcMonitor.bottom > monitor_info.rcMonitor.top);

		// RECT rect = { 0, 0, (int32_t)size.x, (int32_t)size.y };
		// Platform::HighDPI::AdjustWindowRectExForDpi(
		// 	&rect, WS_OVERLAPPEDWINDOW, FALSE, 0,
		// 	Platform::HighDPI::GetDpiForWindow(win32_window));

		//m_ignore_size_message = TRUE;
		// SetLastError(0);
		// SetWindowLongPtrW(win32_window, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		// DWORD const set_style_result = GetLastError();
		// assert(set_style_result == 0); (void)set_style_result;
		//SetLastError(0);
		//SetWindowLongPtrW(win32_window, GWL_EXSTYLE, 0);
		//DWORD const set_style_ex_result = GetLastError();
		//assert(set_style_ex_result == 0); (void)set_style_ex_result;
		//m_ignore_size_message = FALSE;

		SDL_SetWindowFullscreen(sdl_window, 0);

		bool want_restore_placement = false;

		if (m_fullscreen_mode != FullscreenMode::Windowed && ignore_size)
		{
			want_restore_placement = true;
		}
		// else
		// {
		// 	// BOOL const set_window_pos_result = SetWindowPos(
		// 	// 	win32_window,
		// 	// 	HWND_NOTOPMOST,
		// 	// 	(monitor_info.rcMonitor.right + monitor_info.rcMonitor.left) / 2 - (int32_t)size.x / 2,
		// 	// 	(monitor_info.rcMonitor.bottom + monitor_info.rcMonitor.top) / 2 - (int32_t)size.y / 2,
		// 	// 	rect.right - rect.left,
		// 	// 	rect.bottom - rect.top,
		// 	// 	SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		// 	// assert(set_window_pos_result); (void)set_window_pos_result;
		// }

		// RECT client_rect = {};
		// BOOL get_client_rect_result = GetClientRect(win32_window, &client_rect);
		

		// assert(get_client_rect_result); (void)get_client_rect_result;

		// m_framestyle = WindowFrameStyle::Normal;
		setFrameStyle(WindowFrameStyle::Normal);
		m_fullscreen_mode = FullscreenMode::Windowed;
		// win32_window_style = WS_OVERLAPPEDWINDOW;
		// win32_window_style_ex = 0;
		// win32_window_width = UINT(client_rect.right - client_rect.left);
		// win32_window_height = UINT(client_rect.bottom - client_rect.top);


		EventData event_data{};
		event_data.window_fullscreen_state = false;
		dispatchEvent(EventType::WindowFullscreenStateChange, event_data);

		if (want_restore_placement)
		{
			SDL_SetWindowPosition(sdl_window, m_last_window_rect.x, m_last_window_rect.y);
			SDL_SetWindowSize(sdl_window, m_last_window_rect.w, m_last_window_rect.h);
			sdl_window_width = m_last_window_rect.w;
			sdl_window_height = m_last_window_rect.h;
		}
		else
		{
			int32_t monitor = m_monitor_idx == -1 ? SDL_GetWindowDisplayIndex(sdl_window) : m_monitor_idx;
			SDL_SetWindowPosition(sdl_window, SDL_WINDOWPOS_CENTERED_DISPLAY(m_monitor_idx), SDL_WINDOWPOS_CENTERED_DISPLAY(m_monitor_idx));
			SDL_SetWindowSize(sdl_window, size.x, size.y);
			sdl_window_width = size.x;
			sdl_window_height = size.y;
		}
	}
	void Window_SDL::_setBorderlessFullScreenMode()
	{
		if (m_fullscreen_mode == FullscreenMode::Windowed)
		{
			SDL_GetWindowPosition(sdl_window, &m_last_window_rect.x, &m_last_window_rect.y);
			SDL_GetWindowSize(sdl_window, &m_last_window_rect.w, &m_last_window_rect.h);
		}

		// HMONITOR win32_monitor = MonitorFromWindow(win32_window, MONITOR_DEFAULTTONEAREST);
		// assert(win32_monitor);
		// MONITORINFO monitor_info = {};
		// monitor_info.cbSize = sizeof(monitor_info);
		// BOOL const get_monitor_info_result = GetMonitorInfoW(win32_monitor, &monitor_info);
		// assert(get_monitor_info_result); (void)get_monitor_info_result;
		// assert(monitor_info.rcMonitor.right > monitor_info.rcMonitor.left);
		// assert(monitor_info.rcMonitor.bottom > monitor_info.rcMonitor.top);

		// RECT rect = { 0, 0, (int32_t)size.x, (int32_t)size.y };
		// Platform::HighDPI::AdjustWindowRectExForDpi(
		// 	&rect, WS_OVERLAPPEDWINDOW, FALSE, 0,
		// 	Platform::HighDPI::GetDpiForWindow(win32_window));

		//m_ignore_size_message = TRUE;
		// SetLastError(0);
		// SetWindowLongPtrW(win32_window, GWL_STYLE, WS_POPUP);
		// DWORD const set_style_result = GetLastError();
		// assert(set_style_result == 0); (void)set_style_result;
		//SetLastError(0);
		//SetWindowLongPtrW(win32_window, GWL_EXSTYLE, 0);
		//DWORD const set_style_ex_result = GetLastError();
		//assert(set_style_ex_result == 0); (void)set_style_ex_result;
		//m_ignore_size_message = FALSE;

		int32_t monitor = m_monitor_idx == -1 ? SDL_GetWindowDisplayIndex(sdl_window) : m_monitor_idx;
		SDL_Rect r;
		SDL_GetDisplayBounds(monitor, &r);
		sdl_window_width = r.w;
		sdl_window_height = r.h;

		SDL_SetWindowSize(sdl_window, r.w, r.h);
		SDL_SetWindowPosition(sdl_window, SDL_WINDOWPOS_CENTERED_DISPLAY(monitor), SDL_WINDOWPOS_CENTERED_DISPLAY(monitor));
		SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);

		// BOOL const set_window_pos_result = SetWindowPos(
		// 	win32_window,
		// 	HWND_NOTOPMOST,
		// 	monitor_info.rcMonitor.left,
		// 	monitor_info.rcMonitor.top,
		// 	monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
		// 	monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
		// 	SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		// assert(set_window_pos_result); (void)set_window_pos_result;

		// RECT client_rect = {};
		// BOOL get_client_rect_result = GetClientRect(win32_window, &client_rect);
		// assert(get_client_rect_result); (void)get_client_rect_result;

		// m_framestyle = WindowFrameStyle::None;
		setFrameStyle(WindowFrameStyle::None);
		m_fullscreen_mode = FullscreenMode::Borderless;
		// win32_window_style = WS_POPUP;
		// win32_window_style_ex = 0;
		// win32_window_width = UINT(monitor_info.rcMonitor.right - monitor_info.rcMonitor.left);
		// win32_window_height = UINT(monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top);

		EventData event_data{};
		event_data.window_fullscreen_state = false;
		dispatchEvent(EventType::WindowFullscreenStateChange, event_data);
	}
	void Window_SDL::_setFullScreenMode()
	{
		if (m_fullscreen_mode == FullscreenMode::Windowed)
		{
			SDL_GetWindowPosition(sdl_window, &m_last_window_rect.x, &m_last_window_rect.y);
			SDL_GetWindowSize(sdl_window, &m_last_window_rect.w, &m_last_window_rect.h);
		}

		// HMONITOR win32_monitor = MonitorFromWindow(win32_window, MONITOR_DEFAULTTONEAREST);
		// assert(win32_monitor);
		// MONITORINFO monitor_info = {};
		// monitor_info.cbSize = sizeof(monitor_info);
		// BOOL const get_monitor_info_result = GetMonitorInfoW(win32_monitor, &monitor_info);
		// assert(get_monitor_info_result); (void)get_monitor_info_result;
		// assert(monitor_info.rcMonitor.right > monitor_info.rcMonitor.left);
		// assert(monitor_info.rcMonitor.bottom > monitor_info.rcMonitor.top);

		//m_ignore_size_message = TRUE;
		// SetLastError(0);
		// SetWindowLongPtrW(win32_window, GWL_STYLE, WS_POPUP);
		// DWORD const set_style_result = GetLastError();
		// assert(set_style_result == 0); (void)set_style_result;
		//SetLastError(0);
		//SetWindowLongPtrW(win32_window, GWL_EXSTYLE, 0);
		//DWORD const set_style_ex_result = GetLastError();
		//assert(set_style_ex_result == 0); (void)set_style_ex_result;
		//m_ignore_size_message = FALSE;

		int32_t monitor = m_monitor_idx == -1 ? SDL_GetWindowDisplayIndex(sdl_window) : m_monitor_idx;
		SDL_Rect r;
		SDL_GetDisplayBounds(monitor, &r);
		sdl_window_width = r.w;
		sdl_window_height = r.h;

		// SDL_SetWindowSize(sdl_window, r.w, r.h);
		SDL_SetWindowPosition(sdl_window, SDL_WINDOWPOS_CENTERED_DISPLAY(monitor), SDL_WINDOWPOS_CENTERED_DISPLAY(monitor));
		SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN);

		// BOOL const set_window_pos_result = SetWindowPos(
		// 	win32_window,
		// 	HWND_NOTOPMOST,
		// 	monitor_info.rcMonitor.left,
		// 	monitor_info.rcMonitor.top,
		// 	monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
		// 	monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
		// 	SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		// assert(set_window_pos_result); (void)set_window_pos_result;

		setFrameStyle(WindowFrameStyle::None);
		m_fullscreen_mode = FullscreenMode::Exclusive;
		// win32_window_style = WS_POPUP;
		// win32_window_style_ex = 0;
		// win32_window_width = UINT(monitor_info.rcMonitor.right - monitor_info.rcMonitor.left);
		// win32_window_height = UINT(monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top);

		EventData event_data{};
		event_data.window_fullscreen_state = true;
		dispatchEvent(EventType::WindowFullscreenStateChange, event_data);
	}

	// void Window_SDL::convertTitleText()
	// {
	// 	win32_window_text_w[0] = L'\0';
	// 	int const size = MultiByteToWideChar(CP_UTF8, 0, win32_window_text.data(), (int)win32_window_text.size(), NULL, 0);
	// 	if (size <= 0 || size > (int)(win32_window_text_w.size() - 1))
	// 	{
	// 		assert(false); return;
	// 	}
	// 	win32_window_text_w[size] = L'\0';
	// 	int const result = MultiByteToWideChar(CP_UTF8, 0, win32_window_text.data(), (int)win32_window_text.size(), win32_window_text_w.data(), size);
	// 	if (result <= 0 || result != size)
	// 	{
	// 		assert(false); return;
	// 	}
	// 	win32_window_text_w[result] = L'\0';
	// }

	RectI Window_SDL::getRect()
	{
		// RECT rc = {};
		// GetWindowRect(win32_window, &rc);
		SDL_Rect rc;
		SDL_GetWindowPosition(sdl_window, &rc.x, &rc.y);
		SDL_GetWindowSize(sdl_window, &rc.w, &rc.h);
		return RectI(rc.x, rc.y, rc.x + rc.w, rc.y + rc.h);
	}
	bool Window_SDL::setRect(RectI v)
	{
		SDL_SetWindowPosition(sdl_window, v.a.x, v.a.y);
		SDL_SetWindowSize(sdl_window, v.b.x - v.a.x, v.b.y - v.a.y);
	}
	// RectI Window_SDL::getClientRect()
	// {
	// 	RECT rc = {};
	// 	GetClientRect(win32_window, &rc);
	// 	return RectI(rc.left, rc.top, rc.right, rc.bottom);
	// }
	// bool Window_SDL::setClientRect(RectI v)
	// {
	// 	// 更新 DPI
	// 	win32_window_dpi = Platform::HighDPI::GetDpiForWindow(win32_window);
	// 	// 计算包括窗口框架的尺寸
	// 	RECT rc = { v.a.x , v.a.y , v.b.x , v.b.y };
	// 	Platform::HighDPI::AdjustWindowRectExForDpi(
	// 		&rc,
	// 		win32_window_style,
	// 		FALSE,
	// 		win32_window_style_ex,
	// 		win32_window_dpi);
	// 	// 获取最近的显示器的位置
	// 	if (HMONITOR monitor = MonitorFromWindow(win32_window, MONITOR_DEFAULTTONEAREST))
	// 	{
	// 		MONITORINFO moninfo = { sizeof(MONITORINFO), {}, {}, 0 };
	// 		if (GetMonitorInfoA(monitor, &moninfo))
	// 		{
	// 			// 偏移到该显示器0点位置
	// 			rc.left += moninfo.rcMonitor.left;
	// 			rc.right += moninfo.rcMonitor.left;
	// 			rc.top += moninfo.rcMonitor.top;
	// 			rc.bottom += moninfo.rcMonitor.top;
	// 		}
	// 	}
	// 	// 最后再应用
	// 	return SetWindowPos(win32_window, NULL,
	// 		rc.left,
	// 		rc.top,
	// 		rc.right - rc.left,
	// 		rc.bottom - rc.top,
	// 		SWP_NOZORDER) != FALSE;
	// }
	// uint32_t Window_SDL::getDPI()
	// {
	// 	win32_window_dpi = Platform::HighDPI::GetDpiForWindow(win32_window);
	// 	return win32_window_dpi;
	// }
	// void Window_SDL::setRedirectBitmapEnable(bool enable)
	// {
	// 	m_redirect_bitmap = enable ? TRUE : FALSE;
	// }
	// bool Window_SDL::getRedirectBitmapEnable()
	// {
	// 	return m_redirect_bitmap;
	// }

	void Window_SDL::dispatchEvent(EventType t, EventData d)
	{
		// 回调
		m_is_dispatch_event = true;
		switch (t)
		{
		case EventType::WindowCreate:
			for (auto& v : m_eventobj)
			{
				if (v) v->onWindowCreate();
			}
			break;
		case EventType::WindowDestroy:
			for (auto& v : m_eventobj)
			{
				if (v) v->onWindowDestroy();
			}
			break;
		case EventType::WindowActive:
			for (auto& v : m_eventobj)
			{
				if (v) v->onWindowActive();
			}
			break;
		case EventType::WindowInactive:
			for (auto& v : m_eventobj)
			{
				if (v) v->onWindowInactive();
			}
			break;
		case EventType::WindowClose:
			for (auto& v : m_eventobj)
			{
				if (v) v->onWindowClose();
			}
			break;
		case EventType::WindowSize:
			for (auto& v : m_eventobj)
			{
				if (v) v->onWindowSize(d.window_size);
			}
			break;
		case EventType::WindowFullscreenStateChange:
			for (auto& v : m_eventobj)
			{
				if (v) v->onWindowFullscreenStateChange(d.window_fullscreen_state);
			}
			break;
		case EventType::WindowDpiChanged:
			for (auto& v : m_eventobj)
			{
				if (v) v->onWindowDpiChange();
			}
			break;
		case EventType::DeviceChange:
			for (auto& v : m_eventobj)
			{
				if (v) v->onDeviceChange();
			}
			break;
		}
		m_is_dispatch_event = false;
		// 处理那些延迟的对象
		removeEventListener(nullptr);
		for (auto& v : m_eventobj_late)
		{
			m_eventobj.emplace_back(v);
		}
		m_eventobj_late.clear();
	}
	void Window_SDL::Window_SDL::addEventListener(IWindowEventListener* e)
	{
		removeEventListener(e);
		if (m_is_dispatch_event)
		{
			m_eventobj_late.emplace_back(e);
		}
		else
		{
			m_eventobj.emplace_back(e);
		}
	}
	void Window_SDL::Window_SDL::removeEventListener(IWindowEventListener* e)
	{
		if (m_is_dispatch_event)
		{
			for (auto& v : m_eventobj)
			{
				if (v == e)
				{
					v = nullptr; // 不破坏遍历过程
				}
			}
		}
		else
		{
			for (auto it = m_eventobj.begin(); it != m_eventobj.end();)
			{
				if (*it == e)
					it = m_eventobj.erase(it);
				else
					it++;
			}
		}
	}

	void* Window_SDL::getNativeHandle() { return sdl_window; }
	// void Window_SDL::setNativeIcon(void* id)
	// {
	// 	win32_window_icon_id = (INT_PTR)id;
	// 	SendMessageW(win32_window, LUASTG_WM_SETICON, 0, 0);
	// }

	// void Window_SDL::setIMEState(bool enable)
	// {
	// 	win32_window_ime_enable = enable;
	// 	if (win32_window_ime_enable)
	// 		ImmAssociateContext(win32_window, win32_window_imc);
	// 	else
	// 		ImmAssociateContext(win32_window, NULL);
	// }
	// bool Window_SDL::getIMEState()
	// {
	// 	return win32_window_ime_enable;
	// }

	void Window_SDL::setTitleText(StringView str)
	{
		sdl_window_text = str;
		SDL_SetWindowTitle(sdl_window, str)
		// convertTitleText();
		// PostMessageW(win32_window, LUASTG_WM_UPDAE_TITLE, 0, 0);
	}
	StringView Window_SDL::getTitleText()
	{
		return sdl_window_text;
	}

	bool Window_SDL::setFrameStyle(WindowFrameStyle style)
	{
		m_framestyle = style;
		switch (style)
		{
		default:
			assert(false); return false;
		case WindowFrameStyle::None:
			sdl_window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS;
			SDL_SetWindowBordered(sdl_window, SDL_FALSE);
			SDL_SetWindowResizable(sdl_window, SDL_FALSE);
			break;
		case WindowFrameStyle::Fixed:
			sdl_window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
			SDL_SetWindowBordered(sdl_window, SDL_TRUE);
			SDL_SetWindowResizable(sdl_window, SDL_FALSE);
			break;
		case WindowFrameStyle::Normal:
			sdl_window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
			SDL_SetWindowBordered(sdl_window, SDL_TRUE);
			SDL_SetWindowResizable(sdl_window, SDL_TRUE);
			break;
		}
		// SetWindowLongPtrW(win32_window, GWL_STYLE, win32_window_style);
		//SetWindowLongPtrW(win32_window, GWL_EXSTYLE, win32_window_style_ex);
		// UINT const flags = (SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE) | (!m_hidewindow ? SWP_SHOWWINDOW : 0);
		// SetWindowPos(win32_window, NULL, 0, 0, 0, 0, flags);
		return true;
	}
	WindowFrameStyle Window_SDL::getFrameStyle()
	{
		return m_framestyle;
	}

	Vector2U Window_SDL::getSize()
	{
		return { sdl_window_width, sdl_window_height };
	}
	bool Window_SDL::setSize(Vector2U v)
	{
		sdl_window_width = v.x;
		sdl_window_height = v.y;
		// return setClientRect(RectI(0, 0, (int)v.x, (int)v.y));
		SDL_SetWindowSize(sdl_window, (int)v.x, (int)v.y);
		return true;
	}

	WindowLayer Window_SDL::getLayer()
	{
		if (m_hidewindow)
			return WindowLayer::Invisible;
		// if (WS_EX_TOPMOST & GetWindowLongPtrW(win32_window, GWL_EXSTYLE))
			// return WindowLayer::TopMost;
		if (SDL_GetWindowFlags(sdl_window) & SDL_WINDOW_ALWAYS_ON_TOP)
			return WindowLayer::Top;
		return WindowLayer::Normal;
	}
	bool Window_SDL::setLayer(WindowLayer layer)
	{
		// HWND pLayer = NULL;
		switch (layer)
		{
		default:
		case WindowLayer::Unknown:
			assert(false); return false;
		case WindowLayer::Invisible:
			// pLayer = NULL;
			SDL_HideWindow(sdl_window);
			break;
		// case WindowLayer::Bottom:
			// pLayer = HWND_BOTTOM;
			// break;
		case WindowLayer::Normal:
			// pLayer = HWND_NOTOPMOST;
			SDL_ShowWindow(sdl_window);
			SDL_SetWindowAlwaysOnTop(sdl_window, SDL_FALSE);
			break;
		case WindowLayer::Top:
			// pLayer = HWND_TOP;
			// break;
		// case WindowLayer::TopMost:
			// pLayer = HWND_TOPMOST;
			SDL_ShowWindow(sdl_window);
			SDL_SetWindowAlwaysOnTop(sdl_window, SDL_TRUE);
			break;
		}
		// UINT flags = (SWP_NOMOVE | SWP_NOSIZE);
		// if (layer == WindowLayer::Invisible)
		// {
		// 	// flags |= (SWP_NOZORDER | SWP_HIDEWINDOW);
		// 	m_hidewindow = true;
		// }
		// else
		// {
		// 	// flags |= (SWP_SHOWWINDOW);
		// 	m_hidewindow = false;
		// }
		m_hidewindow = layer == WindowLayer::Invisible;
		return true; //SetWindowPos(win32_window, pLayer, 0, 0, 0, 0, flags) != FALSE;
	}

	// float Window_SDL::getDPIScaling()
	// {
	// 	return (float)getDPI() / (float)USER_DEFAULT_SCREEN_DPI;
	// }

	void Window_SDL::setWindowMode(Vector2U size)
	{
		// SendMessageW(win32_window, LUASTG_WM_SET_WINDOW_MODE, MAKEWPARAM(size.x, size.y), FALSE);
		_setWindowMode(size, false);
	}
	void Window_SDL::setExclusiveFullScreenMode()
	{
		// SendMessageW(win32_window, LUASTG_WM_SET_FULLSCREEN_MODE, 0, 0);
		_setFullScreenMode();
	}
	void Window_SDL::setBorderlessFullScreenMode()
	{
		// SendMessageW(win32_window, LUASTG_WM_SET_BORDERLESS_FULLSCREEN_MODE, 0, 0);
		_setBorderlessFullScreenMode();
	}
	
	uint32_t Window_SDL::getMonitorCount()
	{
		// m_monitors.Refresh();
		// return (uint32_t)m_monitors.GetCount();
		return SDL_GetNumVideoDisplays();
	}
	RectI Window_SDL::getMonitorRect(uint32_t index)
	{
		// RECT const rc = m_monitors.GetRect(index);
		SDL_Rect rc;
		SDL_GetDisplayBounds(index, &rc);
		return RectI(rc.x, rc.y, rc.x + rc.w, rc.y + rc.h);
	}
	void Window_SDL::setMonitorCentered(uint32_t index)
	{
		// m_monitors.MoveWindowToCenter(index, win32_window);
		SDL_SetWindowPosition(sdl_window, SDL_WINDOWPOS_CENTERED_DISPLAY(index), SDL_WINDOWPOS_CENTERED_DISPLAY(index));
	}
	void Window_SDL::setMonitorFullScreen(uint32_t index)
	{
		// auto const rect = getMonitorRect(index);
		// win32_window_width = rect.b.x - rect.a.x;
		// win32_window_height = rect.b.y - rect.a.y;
		// m_monitors.ResizeWindowToFullScreen(index, win32_window);
		
		SDL_Rect rc;
		SDL_GetDisplayBounds(index, &rc);
		SDL_SetWindowPosition(sdl_window, rc.x, rc.y);
		SDL_SetWindowSize(sdl_window, rc.w, rc.h);
	}

	// void Window_SDL::setCustomSizeMoveEnable(bool v)
	// {
	// 	m_sizemove.setEnable(v ? TRUE : FALSE);
	// }
	// void Window_SDL::setCustomMinimizeButtonRect(RectI v)
	// {
	// 	m_sizemove.setMinimizeButtonRect(RECT{
	// 		.left   = v.a.x,
	// 		.top    = v.a.y,
	// 		.right  = v.b.x,
	// 		.bottom = v.b.y,
	// 	});
	// }
	// void Window_SDL::setCustomCloseButtonRect(RectI v)
	// {
	// 	m_sizemove.setCloseButtonRect(RECT{
	// 		.left   = v.a.x,
	// 		.top    = v.a.y,
	// 		.right  = v.b.x,
	// 		.bottom = v.b.y,
	// 	});
	// }
	// void Window_SDL::setCustomMoveButtonRect(RectI v)
	// {
	// 	m_sizemove.setTitleBarRect(RECT{
	// 		.left   = v.a.x,
	// 		.top    = v.a.y,
	// 		.right  = v.b.x,
	// 		.bottom = v.b.y,
	// 	});
	// }

	bool Window_SDL::setCursor(WindowCursor type)
	{
		m_cursor = type;
		switch (type)
		{
		default:
			assert(false); return false;

		case WindowCursor::None:
			// win32_window_cursor = NULL;
			SDL_SetCursor(NULL);
			break;

		case WindowCursor::Arrow:
			// win32_window_cursor = LoadCursor(NULL, IDC_ARROW);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
			break;
		case WindowCursor::Hand:
			// win32_window_cursor = LoadCursor(NULL, IDC_HAND);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
			break;

		case WindowCursor::Cross:
			// win32_window_cursor = LoadCursor(NULL, IDC_CROSS);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR));
			break;
		case WindowCursor::TextInput:
			// win32_window_cursor = LoadCursor(NULL, IDC_IBEAM);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM));
			break;
		
		case WindowCursor::Resize:
			// win32_window_cursor = LoadCursor(NULL, IDC_SIZEALL);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL));
			break;
		case WindowCursor::ResizeEW:
			// win32_window_cursor = LoadCursor(NULL, IDC_SIZEWE);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE));
			break;
		case WindowCursor::ResizeNS:
			// win32_window_cursor = LoadCursor(NULL, IDC_SIZENS);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS));
			break;
		case WindowCursor::ResizeNESW:
			// win32_window_cursor = LoadCursor(NULL, IDC_SIZENESW);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW));
			break;
		case WindowCursor::ResizeNWSE:
			// win32_window_cursor = LoadCursor(NULL, IDC_SIZENWSE);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE));
			break;

		case WindowCursor::NotAllowed:
			// win32_window_cursor = LoadCursor(NULL, IDC_NO);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO));
			break;
		case WindowCursor::Wait:
			// win32_window_cursor = LoadCursor(NULL, IDC_WAIT);
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT));
			break;
		}
		// POINT pt = {};
		// GetCursorPos(&pt);
		// SetCursorPos(pt.x, pt.y);
		return true;
	}
	WindowCursor Window_SDL::getCursor()
	{
		return m_cursor;
	}

	// void Window_SDL::setWindowCornerPreference(bool allow)
	// {
	// 	m_allow_windows_11_window_corner = allow;

	// 	if (!Platform::WindowsVersion::Is11())
	// 	{
	// 		return;
	// 	}

	// 	assert(win32_window);

	// 	DWM_WINDOW_CORNER_PREFERENCE attr = allow ? DWMWCP_DEFAULT : DWMWCP_DONOTROUND;
	// 	HRESULT hr = gHR = dwmapi_loader.SetWindowAttribute(
	// 		win32_window, DWMWA_WINDOW_CORNER_PREFERENCE, &attr, sizeof(attr));
	// 	if (FAILED(hr))
	// 	{
	// 		std::string msg;
	// 		msg.reserve(64);
	// 		msg.append("DwmSetWindowAttribute -> ");
	// 		msg.append(allow ? "DWMWCP_DEFAULT" : "DWMWCP_DONOTROUND");
	// 		i18n_core_system_call_report_error(msg);
	// 	}
	// }

	Window_SDL::Window_SDL()
	{
		InitializeConfigure config;
		config.loadFromFile("config.json");
		// if (config.debug_track_window_focus) {
		// 	enable_track_window_focus = true;
		// }
		// win32_window_text_w.fill(L'\0');
		// if (!createWindowClass())
		// 	throw std::runtime_error("createWindowClass failed");
		if (!createWindow())
			throw std::runtime_error("createWindow failed");
	}
	Window_SDL::~Window_SDL()
	{
		destroyWindow();
		// destroyWindowClass();
	}

	bool Window_SDL::create(Window_SDL** pp_window)
	{
		try
		{
			*pp_window = new Window_SDL();
			return true;
		}
		catch (...)
		{
			*pp_window = nullptr;
			return false;
		}
	}
	bool Window_SDL::create(Vector2U size, StringView title_text, WindowFrameStyle style, bool show, Window_SDL** pp_window)
	{
		try
		{
			auto* p = new Window_SDL();
			*pp_window = p;
			p->setSize(size);
			p->setTitleText(title_text);
			p->setFrameStyle(style);
			if (show)
				p->setLayer(WindowLayer::Normal);
			return true;
		}
		catch (...)
		{
			*pp_window = nullptr;
			return false;
		}
	}

	bool IWindow::create(IWindow** pp_window)
	{
		try
		{
			*pp_window = new Window_SDL();
			return true;
		}
		catch (...)
		{
			*pp_window = nullptr;
			return false;
		}
	}
	bool IWindow::create(Vector2U size, StringView title_text, WindowFrameStyle style, bool show, IWindow** pp_window)
	{
		try
		{
			auto* p = new Window_SDL();
			*pp_window = p;
			p->setSize(size);
			p->setTitleText(title_text);
			p->setFrameStyle(style);
			if (show)
				p->setLayer(WindowLayer::Normal);
			return true;
		}
		catch (...)
		{
			*pp_window = nullptr;
			return false;
		}
	}
}
