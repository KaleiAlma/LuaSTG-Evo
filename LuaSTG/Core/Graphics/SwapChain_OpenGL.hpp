#pragma once
#include "Core/Object.hpp"
#include "Core/Graphics/SwapChain.hpp"
#include "Core/Graphics/Window_SDL.hpp"
#include "Core/Graphics/Device_OpenGL.hpp"
#include "glad/gl.h"

namespace Core::Graphics
{
	class SwapChain_OpenGL
		: public Object<ISwapChain>
		, public IWindowEventListener
		, public IDeviceEventListener
	{
	private:
		ScopeObject<Window_SDL> m_window;
		ScopeObject<Device_OpenGL> m_device;

		GLuint opengl_framebuffer = 0;
		GLuint opengl_depthstencilbuffer = 0;
		GLuint opengl_texture = 0;

		bool m_swap_chain_vsync{ false };

		bool m_init{ false };

	private:
		void onDeviceCreate();
		void onDeviceDestroy();
		void onWindowCreate();
		void onWindowDestroy();
		void onWindowActive();
		void onWindowInactive();
		void onWindowSize(Core::Vector2I size);
		// void onWindowFullscreenStateChange(bool state);

	private:
		Vector2U m_canvas_size{ 640,480 };
	private:
		bool createSwapChainRenderTarget();
		void destroySwapChainRenderTarget();
		bool createRenderAttachment();
		void destroyRenderAttachment();

	private:
		bool handleSwapChainWindowSize(Vector2I size);

	private:
		enum class EventType
		{
			SwapChainCreate,
			SwapChainDestroy,
		};
		bool m_is_dispatch_event{ false };
		std::vector<ISwapChainEventListener*> m_eventobj;
		std::vector<ISwapChainEventListener*> m_eventobj_late;
		void dispatchEvent(EventType t);
	public:
		void addEventListener(ISwapChainEventListener* e);
		void removeEventListener(ISwapChainEventListener* e);

		bool setWindowMode(Vector2U size);

		bool setCanvasSize(Vector2U size);
		Vector2U getCanvasSize() { return m_canvas_size; }

		void clearRenderAttachment();
		void applyRenderAttachment();
		// void waitFrameLatency();
		void setVSync(bool enable);
		bool present();

		bool saveSnapshotToFile(StringView path);

	public:
		SwapChain_OpenGL(Window_SDL* p_window, Device_OpenGL* p_device);
		~SwapChain_OpenGL();
	public:
		static bool create(Window_SDL* p_window, Device_OpenGL* p_device, SwapChain_OpenGL** pp_swapchain);
	};
}
