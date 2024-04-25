#include "Core/Graphics/DearImGui_SDL_OpenGL.hpp"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "imgui.h"
// #include <windowsx.h>
#include "SDL.h"
#include "SDL_mouse.h"
#include <SDL2/SDL_keyboard.h>

namespace Core::Graphics
{
	// constexpr UINT const MSG_MOUSE_CAPTURE = WM_USER + 0x20;
	// constexpr UINT const MSG_SET_IME_POS = WM_USER + 0x21;

	// constexpr WPARAM const MSG_MOUSE_CAPTURE_SET = 1;
	// constexpr WPARAM const MSG_MOUSE_CAPTURE_REL = 2;

	// constexpr int const IM_VK_KEYPAD_ENTER = VK_RETURN + 256;

	static bool mapMouseCursor(SDL_SystemCursor* outValue)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		{
			return false;
		}
		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
		if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			*outValue = NULL;
		}
		else
		{
			SDL_SystemCursor sdl_cursor = SDL_SYSTEM_CURSOR_ARROW;
			switch (imgui_cursor)
			{
			case ImGuiMouseCursor_Arrow: sdl_cursor = SDL_SYSTEM_CURSOR_ARROW; break;
			case ImGuiMouseCursor_TextInput: sdl_cursor = SDL_SYSTEM_CURSOR_IBEAM; break;
			case ImGuiMouseCursor_ResizeAll: sdl_cursor = SDL_SYSTEM_CURSOR_SIZEALL; break;
			case ImGuiMouseCursor_ResizeEW: sdl_cursor = SDL_SYSTEM_CURSOR_SIZEWE; break;
			case ImGuiMouseCursor_ResizeNS: sdl_cursor = SDL_SYSTEM_CURSOR_SIZENS; break;
			case ImGuiMouseCursor_ResizeNESW: sdl_cursor = SDL_SYSTEM_CURSOR_SIZENESW; break;
			case ImGuiMouseCursor_ResizeNWSE: sdl_cursor = SDL_SYSTEM_CURSOR_SIZENWSE; break;
			case ImGuiMouseCursor_Hand: sdl_cursor = SDL_SYSTEM_CURSOR_HAND; break;
			case ImGuiMouseCursor_NotAllowed: sdl_cursor = SDL_SYSTEM_CURSOR_NO; break;
			}
			*outValue = sdl_cursor;
		}
		return true;
	}
	// static bool isVkDown(int vk)
	// {
	// 	return (::GetKeyState(vk) & 0x8000) != 0;
	// }
	static bool isKeyDown(SDL_Keycode key)
	{
		return SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(key)] != 0;
	}
	static void addKeyEvent(ImGuiKey key, bool down, int native_keycode, int native_scancode = -1)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddKeyEvent(key, down);
		io.SetKeyEventNativeData(key, native_keycode, native_scancode); // To support legacy indexing (<1.87 user code)
		IM_UNUSED(native_scancode);
	}
	static ImGuiKey mapKeycodeToImGuiKey(SDL_Keycode key)
	{
		switch (key)
		{
		case SDLK_TAB: return ImGuiKey_Tab;
		case SDLK_LEFT: return ImGuiKey_LeftArrow;
		case SDLK_RIGHT: return ImGuiKey_RightArrow;
		case SDLK_UP: return ImGuiKey_UpArrow;
		case SDLK_DOWN: return ImGuiKey_DownArrow;
		case SDLK_PAGEUP: return ImGuiKey_PageUp;
		case SDLK_PAGEDOWN: return ImGuiKey_PageDown;
		case SDLK_HOME: return ImGuiKey_Home;
		case SDLK_END: return ImGuiKey_End;
		case SDLK_INSERT: return ImGuiKey_Insert;
		case SDLK_DELETE: return ImGuiKey_Delete;
		case SDLK_BACKSPACE: return ImGuiKey_Backspace;
		case SDLK_SPACE: return ImGuiKey_Space;
		case SDLK_RETURN: return ImGuiKey_Enter;
		case SDLK_ESCAPE: return ImGuiKey_Escape;
		case SDLK_QUOTE: return ImGuiKey_Apostrophe;
		case SDLK_COMMA: return ImGuiKey_Comma;
		case SDLK_MINUS: return ImGuiKey_Minus;
		case SDLK_PERIOD: return ImGuiKey_Period;
		case SDLK_SLASH: return ImGuiKey_Slash;
		case SDLK_SEMICOLON: return ImGuiKey_Semicolon;
		case SDLK_PLUS: return ImGuiKey_Equal;
		case SDLK_LEFTBRACKET: return ImGuiKey_LeftBracket;
		case SDLK_BACKSLASH: return ImGuiKey_Backslash;
		case SDLK_RIGHTBRACKET: return ImGuiKey_RightBracket;
		case SDLK_BACKQUOTE: return ImGuiKey_GraveAccent;
		case SDLK_CAPSLOCK: return ImGuiKey_CapsLock;
		case SDLK_SCROLLLOCK: return ImGuiKey_ScrollLock;
		case SDLK_NUMLOCKCLEAR: return ImGuiKey_NumLock;
		case SDLK_PRINTSCREEN: return ImGuiKey_PrintScreen;
		case SDLK_PAUSE: return ImGuiKey_Pause;
		case SDLK_KP_0: return ImGuiKey_Keypad0;
		case SDLK_KP_1: return ImGuiKey_Keypad1;
		case SDLK_KP_2: return ImGuiKey_Keypad2;
		case SDLK_KP_3: return ImGuiKey_Keypad3;
		case SDLK_KP_4: return ImGuiKey_Keypad4;
		case SDLK_KP_5: return ImGuiKey_Keypad5;
		case SDLK_KP_6: return ImGuiKey_Keypad6;
		case SDLK_KP_7: return ImGuiKey_Keypad7;
		case SDLK_KP_8: return ImGuiKey_Keypad8;
		case SDLK_KP_9: return ImGuiKey_Keypad9;
		case SDLK_KP_PERIOD: return ImGuiKey_KeypadDecimal;
		case SDLK_KP_DIVIDE: return ImGuiKey_KeypadDivide;
		case SDLK_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
		case SDLK_KP_MINUS: return ImGuiKey_KeypadSubtract;
		case SDLK_KP_PLUS: return ImGuiKey_KeypadAdd;
		case SDLK_KP_ENTER: return ImGuiKey_KeypadEnter;
		case SDLK_LSHIFT: return ImGuiKey_LeftShift;
		case SDLK_LCTRL: return ImGuiKey_LeftCtrl;
		case SDLK_LALT: return ImGuiKey_LeftAlt;
		case SDLK_LGUI: return ImGuiKey_LeftSuper;
		case SDLK_RSHIFT: return ImGuiKey_RightShift;
		case SDLK_RCTRL: return ImGuiKey_RightCtrl;
		case SDLK_RALT: return ImGuiKey_RightAlt;
		case SDLK_RGUI: return ImGuiKey_RightSuper;
		case SDLK_MENU: return ImGuiKey_Menu;
		case '0': return ImGuiKey_0;
		case '1': return ImGuiKey_1;
		case '2': return ImGuiKey_2;
		case '3': return ImGuiKey_3;
		case '4': return ImGuiKey_4;
		case '5': return ImGuiKey_5;
		case '6': return ImGuiKey_6;
		case '7': return ImGuiKey_7;
		case '8': return ImGuiKey_8;
		case '9': return ImGuiKey_9;
		case 'a': return ImGuiKey_A;
		case 'b': return ImGuiKey_B;
		case 'c': return ImGuiKey_C;
		case 'd': return ImGuiKey_D;
		case 'e': return ImGuiKey_E;
		case 'f': return ImGuiKey_F;
		case 'g': return ImGuiKey_G;
		case 'h': return ImGuiKey_H;
		case 'i': return ImGuiKey_I;
		case 'j': return ImGuiKey_J;
		case 'k': return ImGuiKey_K;
		case 'l': return ImGuiKey_L;
		case 'm': return ImGuiKey_M;
		case 'n': return ImGuiKey_N;
		case 'o': return ImGuiKey_O;
		case 'p': return ImGuiKey_P;
		case 'q': return ImGuiKey_Q;
		case 'r': return ImGuiKey_R;
		case 's': return ImGuiKey_S;
		case 't': return ImGuiKey_T;
		case 'u': return ImGuiKey_U;
		case 'v': return ImGuiKey_V;
		case 'w': return ImGuiKey_W;
		case 'x': return ImGuiKey_X;
		case 'y': return ImGuiKey_Y;
		case 'z': return ImGuiKey_Z;
		case SDLK_F1: return ImGuiKey_F1;
		case SDLK_F2: return ImGuiKey_F2;
		case SDLK_F3: return ImGuiKey_F3;
		case SDLK_F4: return ImGuiKey_F4;
		case SDLK_F5: return ImGuiKey_F5;
		case SDLK_F6: return ImGuiKey_F6;
		case SDLK_F7: return ImGuiKey_F7;
		case SDLK_F8: return ImGuiKey_F8;
		case SDLK_F9: return ImGuiKey_F9;
		case SDLK_F10: return ImGuiKey_F10;
		case SDLK_F11: return ImGuiKey_F11;
		case SDLK_F12: return ImGuiKey_F12;
		default: return ImGuiKey_None;
		}
	}
	static void updateKeyModifiers()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddKeyEvent(ImGuiMod_Ctrl, isKeyDown(SDLK_LCTRL) || isKeyDown(SDLK_RCTRL));
		io.AddKeyEvent(ImGuiMod_Shift, isKeyDown(SDLK_LSHIFT) || isKeyDown(SDLK_RSHIFT));
		io.AddKeyEvent(ImGuiMod_Alt, isKeyDown(SDLK_LALT) || isKeyDown(SDLK_RALT));
		io.AddKeyEvent(ImGuiMod_Super, isKeyDown(SDLK_LGUI) || isKeyDown(SDLK_RGUI));
	}
	
	// inline WPARAM convertImVec2ToWPARAM(ImVec2 const& v)
	// {
	// 	return ((int16_t)v.x) & (((int32_t)(int16_t)v.y) << 16);
	// }
	// inline ImVec2 convertWPARAMToImVec2(WPARAM const v)
	// {
	// 	return ImVec2((float)(int16_t)(v & 0xFFFFu), (float)(int16_t)((v & 0xFFFF0000u) >> 16));
	// }
	// static void updateIME(ImGuiViewport* viewport, ImGuiPlatformImeData* data)
	// {
	// 	if (viewport->PlatformHandleRaw)
	// 	{
	// 		PostMessageW((HWND)viewport->PlatformHandleRaw, MSG_SET_IME_POS, convertImVec2ToWPARAM(data->InputPos), 0);
	// 	}
	// }

	bool DearImGui_Backend::createWindowResources()
	{
		// if (!QueryPerformanceFrequency(&m_freq))
		// 	return false;
		// if (!QueryPerformanceCounter(&m_time))
		// 	return false;

		ImGuiIO& io = ImGui::GetIO();
		assert(io.BackendPlatformUserData == nullptr);
		io.BackendPlatformName = "LuaSTG Sub";
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendPlatformUserData = (void*)this;
		// io.SetPlatformImeDataFn = &updateIME;

		ImGui::GetMainViewport()->PlatformHandleRaw = (void*)m_window->GetWindow();

		auto w_size_ =  m_window->getSize();
		io.DisplaySize = ImVec2((float)w_size_.x, (float)w_size_.y);

		return true;
	}
	void DearImGui_Backend::onWindowCreate()
	{
		createWindowResources();
	}
	void DearImGui_Backend::onWindowDestroy()
	{
		// m_mouse_window = NULL;
		m_is_mouse_tracked = false;
		m_mouse_button_down = 0;
		m_last_cursor = (int)ImGuiMouseCursor_Arrow;

		ImGuiIO& io = ImGui::GetIO();
		assert(io.BackendPlatformUserData != nullptr);
		io.BackendPlatformName = nullptr;
		io.BackendPlatformUserData = nullptr;
	}
	IWindowEventListener::NativeWindowMessageResult DearImGui_Backend::onNativeWindowMessage(void* arg1, uint32_t arg2, uintptr_t arg3, intptr_t arg4)
	{
		HWND hWnd = (HWND)arg1; UINT uMsg = arg2; WPARAM wParam = arg3; LPARAM lParam = arg4;

		if (!ImGui::GetCurrentContext())
			return IWindowEventListener::NativeWindowMessageResult();

		switch (uMsg)
		{
			// -------- 需要特殊处理的鼠标消息 --------

		case WM_MOUSEMOVE:
			// 调用 TrackMouseEvent 来接收 WM_MOUSELEAVE 通知
			m_mouse_window = hWnd;
			if (m_is_mouse_tracked)
			{
				m_is_mouse_tracked = true;
				TRACKMOUSEEVENT tme = {
					.cbSize = sizeof(TRACKMOUSEEVENT),
					.dwFlags = TME_LEAVE,
					.hwndTrack = hWnd,
					.dwHoverTime = 0,
				};
				::TrackMouseEvent(&tme);
			}
			m_queue.write({ hWnd, uMsg, wParam, lParam });
			return IWindowEventListener::NativeWindowMessageResult();
		case WM_MOUSELEAVE:
			// 鼠标已经离开了
			if (m_mouse_window == hWnd)
			{
				m_mouse_window = NULL;
			}
			m_is_mouse_tracked = false;
			m_queue.write({ hWnd, uMsg, wParam, lParam });
			return IWindowEventListener::NativeWindowMessageResult();

			// -------- 要交给 ImGui 处理的消息 --------

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		case WM_CHAR:

		case WM_ACTIVATEAPP:
		case WM_SIZE:
			m_queue.write({ hWnd, uMsg, wParam, lParam });
			return IWindowEventListener::NativeWindowMessageResult();

			// -------- 从 ImGui 发送过来的消息（工作线程） --------

		case MSG_MOUSE_CAPTURE:
			switch (wParam)
			{
			case MSG_MOUSE_CAPTURE_SET: if (::GetCapture() == NULL) ::SetCapture(hWnd); break;
			case MSG_MOUSE_CAPTURE_REL: if (::GetCapture() == hWnd) ::ReleaseCapture(); break;
			}
			return IWindowEventListener::NativeWindowMessageResult(0, true);
		case MSG_SET_IME_POS:
			if (HIMC himc = ::ImmGetContext(hWnd))
			{
				ImVec2 const pt = convertWPARAMToImVec2(wParam);
				COMPOSITIONFORM cf = {
					.dwStyle = CFS_FORCE_POSITION,
					.ptCurrentPos = POINT{
						.x = (LONG)pt.x,
						.y = (LONG)pt.y,
					},
					.rcArea = RECT{},
				};
				::ImmSetCompositionWindow(himc, &cf);
				::ImmReleaseContext(hWnd, himc);
			}
			return IWindowEventListener::NativeWindowMessageResult(0, true);

			// -------- 其他消息（目前应该没有更多需要处理的消息） --------
		}

		return IWindowEventListener::NativeWindowMessageResult();
	}

	void DearImGui_Backend::processMessage()
	{
		ImGuiIO& io = ImGui::GetIO();
		Win32Message msg = {};

		auto processInputEvent = [&]() -> bool
		{
			// mouse

			switch (msg.uMsg)
			{
			case WM_MOUSEMOVE:
				io.AddMousePosEvent((float)GET_X_LPARAM(msg.lParam), (float)GET_Y_LPARAM(msg.lParam));
				return true;
			case WM_MOUSELEAVE:
				io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
				return true;
			case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
			case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
				do {
					int button = 0;
					if (msg.uMsg == WM_LBUTTONDOWN || msg.uMsg == WM_LBUTTONDBLCLK) { button = 0; }
					if (msg.uMsg == WM_RBUTTONDOWN || msg.uMsg == WM_RBUTTONDBLCLK) { button = 1; }
					if (msg.uMsg == WM_MBUTTONDOWN || msg.uMsg == WM_MBUTTONDBLCLK) { button = 2; }
					if (msg.uMsg == WM_XBUTTONDOWN || msg.uMsg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(msg.wParam) == XBUTTON1) ? 3 : 4; }
					if (m_mouse_button_down == 0) ::PostMessageW(m_window->GetWindow(), MSG_MOUSE_CAPTURE, MSG_MOUSE_CAPTURE_SET, 0);
					m_mouse_button_down |= 1 << button;
					io.AddMouseButtonEvent(button, true);
				} while (false);
				return true;
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_XBUTTONUP:
				do {
					int button = 0;
					if (msg.uMsg == WM_LBUTTONUP) { button = 0; }
					if (msg.uMsg == WM_RBUTTONUP) { button = 1; }
					if (msg.uMsg == WM_MBUTTONUP) { button = 2; }
					if (msg.uMsg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(msg.wParam) == XBUTTON1) ? 3 : 4; }
					m_mouse_button_down &= ~(1 << button);
					if (m_mouse_button_down == 0) ::PostMessageW(m_window->GetWindow(), MSG_MOUSE_CAPTURE, MSG_MOUSE_CAPTURE_REL, 0);
					io.AddMouseButtonEvent(button, false);
				} while (false);
				return true;
			case WM_MOUSEWHEEL:
				io.AddMouseWheelEvent(0.0f, (float)GET_WHEEL_DELTA_WPARAM(msg.wParam) / (float)WHEEL_DELTA);
				return true;
			case WM_MOUSEHWHEEL:
				io.AddMouseWheelEvent((float)GET_WHEEL_DELTA_WPARAM(msg.wParam) / (float)WHEEL_DELTA, 0.0f);
				return true;
			}

			// keyboard

			switch (msg.uMsg)
			{
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
				if (msg.wParam < 256)
				{
					bool const is_key_down = (msg.uMsg == WM_KEYDOWN || msg.uMsg == WM_SYSKEYDOWN);

					// Submit modifiers
					updateKeyModifiers();

					// Obtain virtual key code
					// (keypad enter doesn't have its own... VK_RETURN with KF_EXTENDED flag means keypad enter, see IM_VK_KEYPAD_ENTER definition for details, it is mapped to ImGuiKey_KeyPadEnter.)
					int vk = (int)msg.wParam;
					if ((msg.wParam == VK_RETURN) && (HIWORD(msg.lParam) & KF_EXTENDED))
						vk = IM_VK_KEYPAD_ENTER;

					// Submit key event
					ImGuiKey const key = mapVirtualKeyToImGuiKey(vk);
					int const scancode = (int)LOBYTE(HIWORD(msg.lParam));
					if (key != ImGuiKey_None)
						addKeyEvent(key, is_key_down, vk, scancode);

					// Submit individual left/right modifier events
					if (vk == VK_SHIFT)
					{
						// Important: Shift keys tend to get stuck when pressed together, missing key-up events are corrected in ImGui_ImplWin32_ProcessKeyEventsWorkarounds()
						if (isVkDown(VK_LSHIFT) == is_key_down) { addKeyEvent(ImGuiKey_LeftShift, is_key_down, VK_LSHIFT, scancode); }
						if (isVkDown(VK_RSHIFT) == is_key_down) { addKeyEvent(ImGuiKey_RightShift, is_key_down, VK_RSHIFT, scancode); }
					}
					else if (vk == VK_CONTROL)
					{
						if (isVkDown(VK_LCONTROL) == is_key_down) { addKeyEvent(ImGuiKey_LeftCtrl, is_key_down, VK_LCONTROL, scancode); }
						if (isVkDown(VK_RCONTROL) == is_key_down) { addKeyEvent(ImGuiKey_RightCtrl, is_key_down, VK_RCONTROL, scancode); }
					}
					else if (vk == VK_MENU)
					{
						if (isVkDown(VK_LMENU) == is_key_down) { addKeyEvent(ImGuiKey_LeftAlt, is_key_down, VK_LMENU, scancode); }
						if (isVkDown(VK_RMENU) == is_key_down) { addKeyEvent(ImGuiKey_RightAlt, is_key_down, VK_RMENU, scancode); }
					}
				}
				return true;
			case WM_SETFOCUS:
			case WM_KILLFOCUS:
				io.AddFocusEvent(msg.uMsg == WM_SETFOCUS);
				return true;
			case WM_CHAR:
				if (msg.wParam > 0 && msg.wParam < 0x10000) io.AddInputCharacterUTF16((uint16_t)msg.wParam);
				return true;
			}

			return false;
		};

		auto processOtherEvent = [&]() -> bool
		{
			switch (msg.uMsg)
			{
			case WM_ACTIVATEAPP:
				io.AddFocusEvent(msg.wParam == TRUE);
				return true;
			case WM_SIZE:
				io.DisplaySize = ImVec2((float)(LOWORD(msg.lParam)), (float)(HIWORD(msg.lParam)));
				return true;
			}

			return false;
		};

		for (size_t cnt = 0; cnt < m_queue.size && m_queue.read(msg); cnt += 1)
		{
			if (!processInputEvent())
			{
				processOtherEvent();
			}
		}

		// Left & right Shift keys: when both are pressed together, Windows tend to not generate the WM_KEYUP event for the first released one.
		if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && !isVkDown(VK_LSHIFT))
			addKeyEvent(ImGuiKey_LeftShift, false, VK_LSHIFT);
		if (ImGui::IsKeyDown(ImGuiKey_RightShift) && !isVkDown(VK_RSHIFT))
			addKeyEvent(ImGuiKey_RightShift, false, VK_RSHIFT);

		// Sometimes WM_KEYUP for Win key is not passed down to the app (e.g. for Win+V on some setups, according to GLFW).
		if (ImGui::IsKeyDown(ImGuiKey_LeftSuper) && !isVkDown(VK_LWIN))
			addKeyEvent(ImGuiKey_LeftSuper, false, VK_LWIN);
		if (ImGui::IsKeyDown(ImGuiKey_RightSuper) && !isVkDown(VK_RWIN))
			addKeyEvent(ImGuiKey_RightSuper, false, VK_RWIN);
	}
	bool DearImGui_Backend::updateMouseCursor()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
			return false;

		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
		if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			m_window->setCursor(WindowCursor::None);
		}
		else
		{
			// Show OS mouse cursor
			switch (imgui_cursor)
			{
			case ImGuiMouseCursor_Arrow:        m_window->setCursor(WindowCursor::Arrow); break;
			case ImGuiMouseCursor_TextInput:    m_window->setCursor(WindowCursor::TextInput); break;
			case ImGuiMouseCursor_ResizeAll:    m_window->setCursor(WindowCursor::Resize); break;
			case ImGuiMouseCursor_ResizeEW:     m_window->setCursor(WindowCursor::ResizeEW); break;
			case ImGuiMouseCursor_ResizeNS:     m_window->setCursor(WindowCursor::ResizeNS); break;
			case ImGuiMouseCursor_ResizeNESW:   m_window->setCursor(WindowCursor::ResizeNESW); break;
			case ImGuiMouseCursor_ResizeNWSE:   m_window->setCursor(WindowCursor::ResizeNWSE); break;
			case ImGuiMouseCursor_Hand:         m_window->setCursor(WindowCursor::Hand); break;
			case ImGuiMouseCursor_NotAllowed:   m_window->setCursor(WindowCursor::NotAllowed); break;
			}
		}

		return true;
	}

	void DearImGui_Backend::update()
	{
		ImGuiIO& io = ImGui::GetIO();
		assert(io.BackendPlatformUserData != nullptr);

		processMessage();

		LARGE_INTEGER current_time = {};
		::QueryPerformanceCounter(&current_time);
		io.DeltaTime = (float)(current_time.QuadPart - m_time.QuadPart) / (float)m_freq.QuadPart;
		m_time = current_time;
		
		auto w_size_ = m_window->getSize();
		io.DisplaySize = ImVec2((float)w_size_.x, (float)w_size_.y);

		ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
		if (m_last_cursor != mouse_cursor)
		{
			m_last_cursor = mouse_cursor;
			updateMouseCursor();
		}
	}

	DearImGui_Backend::DearImGui_Backend(Window_Win32* p_window, Device_D3D11* p_device)
		: m_window(p_window)
		, m_device(p_device)
	{
		if (!createWindowResources())
			throw std::runtime_error("DearImGui_Backend::DearImGui_Backend");
		m_window->addEventListener(this);
		m_device->addEventListener(this);
	}
	DearImGui_Backend::~DearImGui_Backend()
	{
		m_window->removeEventListener(this);
		m_device->removeEventListener(this);
	}
}
