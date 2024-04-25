#pragma once
#include "Core/Object.hpp"
#include "Core/Graphics/Window_SDL.hpp"
#include "Core/Graphics/Device_OpenGL.hpp"

namespace Core::Graphics
{
	class DearImGui_Backend
		: public Object<IObject>
		, public IWindowEventListener
		, public IDeviceEventListener
	{
	private:
		ScopeObject<Window_SDL> m_window;
		ScopeObject<Device_OpenGL> m_device;
		// LARGE_INTEGER     m_time = {};
		// LARGE_INTEGER     m_freq = {};
		// HWND              m_mouse_window = NULL;
		bool              m_is_mouse_tracked = false;
		int               m_mouse_button_down = 0;
		int               m_last_cursor = 0;
		// Win32MessageQueue m_queue;
	private:
		bool createWindowResources();
		void onWindowCreate();
		void onWindowDestroy();
		NativeWindowMessageResult onNativeWindowMessage(void* arg1, uint32_t arg2, uintptr_t arg3, intptr_t arg4);
	private:
		void processMessage();
		bool updateMouseCursor();
	public:
		void update();
	public:
		DearImGui_Backend(Window_SDL* p_window, Device_OpenGL* p_device);
		~DearImGui_Backend();
	};
}
