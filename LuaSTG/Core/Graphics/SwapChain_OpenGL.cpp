#include "Core/Graphics/SwapChain_OpenGL.hpp"
#include "Core/Graphics/Device_OpenGL.hpp"
#include "Core/Graphics/Window_SDL.hpp"
#include "Core/Type.hpp"
#include "Core/i18n.hpp"
#include "glad/gl.h"
#include "SDL2/SDL_video.h"
#include "stb_image_write.h"

//#define _log(x) OutputDebugStringA(x "\n")
#define _log(x)
#define ReportError(x) spdlog::error("[core] ", x)

namespace Core::Graphics
{
	void SwapChain_OpenGL::dispatchEvent(EventType t)
	{
		// callback
		m_is_dispatch_event = true;
		switch (t)
		{
		case EventType::SwapChainCreate:
			for (auto& v : m_eventobj)
			{
				if (v) v->onSwapChainCreate();
			}
			break;
		case EventType::SwapChainDestroy:
			for (auto& v : m_eventobj)
			{
				if (v) v->onSwapChainDestroy();
			}
			break;
		}
		m_is_dispatch_event = false;
		// Dealing with delayed objects
		removeEventListener(nullptr);
		for (auto& v : m_eventobj_late)
		{
			m_eventobj.emplace_back(v);
		}
		m_eventobj_late.clear();
	}
	void SwapChain_OpenGL::addEventListener(ISwapChainEventListener* e)
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
	void SwapChain_OpenGL::removeEventListener(ISwapChainEventListener* e)
	{
		if (m_is_dispatch_event)
		{
			for (auto& v : m_eventobj)
			{
				if (v == e)
				{
					v = nullptr; // doesn't break traversal
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

	void SwapChain_OpenGL::onDeviceCreate()
	{
		if (m_init) // has set up
		{
			setWindowMode(m_canvas_size);
		}
	}
	void SwapChain_OpenGL::onDeviceDestroy()
	{
		destroySwapChainRenderTarget();
	}
	void SwapChain_OpenGL::onWindowCreate()
	{
		// Currently window recreations are only triggered by the exchange chain, so leave this blank
	}
	void SwapChain_OpenGL::onWindowDestroy()
	{
		// Currently window recreations are only triggered by the exchange chain, so leave this blank
	}
	void SwapChain_OpenGL::onWindowActive()
	{
		_log("onWindowActive");
	}
	void SwapChain_OpenGL::onWindowInactive()
	{
		_log("onWindowInactive");
	}
	void SwapChain_OpenGL::onWindowSize(Core::Vector2I size)
	{
		if (size.x == 0 || size.y == 0)
			return; // Ignore minimize
		handleSwapChainWindowSize(size);
	}

	bool SwapChain_OpenGL::createSwapChainRenderTarget()
	{
		_log("createSwapChainRenderTarget");

		glGenRenderbuffers(1, &opengl_depthstencilbuffer);
		if (opengl_depthstencilbuffer == 0) {
			spdlog::error("[core] (SwapChain) glGenRenderbuffers failed");
			return false;
		}
		glBindRenderbuffer(GL_RENDERBUFFER, opengl_depthstencilbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, m_canvas_size.x, m_canvas_size.y);

		glGenTextures(1, &opengl_texture);
		if (opengl_texture == 0) {
			spdlog::error("[core] (SwapChain) glGenTextures failed");
			return false;
		}
		glBindTexture(GL_TEXTURE_2D, opengl_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_canvas_size.x, m_canvas_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glGenFramebuffers(1, &opengl_framebuffer);
		if (opengl_framebuffer == 0) {
			spdlog::error("[core] (SwapChain) glGenFramebuffers failed");
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, opengl_framebuffer);
		// glNamedFramebufferRenderbuffer(opengl_framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, opengl_depthstencilbuffer);
		// glNamedFramebufferTexture(opengl_framebuffer, GL_COLOR_ATTACHMENT0, opengl_texture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, opengl_depthstencilbuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, opengl_texture, 0);
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);

		// GLenum status = glCheckNamedFramebufferStatus(opengl_framebuffer, GL_FRAMEBUFFER);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if(status != GL_FRAMEBUFFER_COMPLETE)
		{
			spdlog::error("[core] Failed to create swapchain framebuffer: {}, {}", glGetError(), status);
			assert(false);
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}
	void SwapChain_OpenGL::destroySwapChainRenderTarget()
	{
		_log("destroySwapChainRenderTarget");

		glDeleteFramebuffers(1, &opengl_framebuffer);
		glDeleteRenderbuffers(1, &opengl_depthstencilbuffer);
	}
	bool SwapChain_OpenGL::createRenderAttachment()
	{
		_log("createRenderAttachment");

		if (!createSwapChainRenderTarget()) return false;

		return true;
	}
	void SwapChain_OpenGL::destroyRenderAttachment()
	{
		_log("destroyRenderAttachment");

		destroySwapChainRenderTarget();
	}
	void SwapChain_OpenGL::applyRenderAttachment()
	{
		//_log("applyRenderAttachment");

		glBindFramebuffer(GL_FRAMEBUFFER, opengl_framebuffer);
	}
	void SwapChain_OpenGL::clearRenderAttachment()
	{
		//_log("clearRenderAttachment");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	bool SwapChain_OpenGL::handleSwapChainWindowSize(Vector2I size)
	{
		_log("handleSwapChainWindowSize");

		if (size.x == 0 || size.y == 0)
		{
			assert(false); return false;
		}

		// Because of differences between DirectX and OpenGL, this function is
		// now effectively empty.

		return true;
	}

	bool SwapChain_OpenGL::setWindowMode(Vector2U size)
	{
		_log("setWindowMode");

		if (size.x < 1 || size.y < 1)
		{
			i18n_log_error_fmt("[core].SwapChain_OpenGL.create_swapchain_failed_invalid_size_fmt", size.x, size.y);
			assert(false); return false;
		}

		dispatchEvent(EventType::SwapChainDestroy);
		destroyRenderAttachment();

		m_canvas_size = size;
		if (!createSwapChainRenderTarget()) // Create render attachments
		{
			return false;
		}
		
		// Update data

		m_init = true;

		// Notify other components of swapchain recreation

		dispatchEvent(EventType::SwapChainCreate);

		return true;
	}

	bool SwapChain_OpenGL::setCanvasSize(Vector2U size)
	{
		_log("setCanvasSize");

		if (size.x == 0 || size.y == 0)
		{
			i18n_log_error_fmt("[core].SwapChain_OpenGL.resize_canvas_failed_invalid_size_fmt",
				size.x, size.y);
			assert(false); return false;
		}

		m_canvas_size = size;

		// TODO: if size didn't change, can we just return?

		dispatchEvent(EventType::SwapChainDestroy);

		destroySwapChainRenderTarget();

		if (!createSwapChainRenderTarget()) return false;

		dispatchEvent(EventType::SwapChainCreate);

		return true;
	}

	void SwapChain_OpenGL::setVSync(bool enable)
	{
		m_swap_chain_vsync = enable;
		if (enable)
		{
			if (SDL_GL_SetSwapInterval(-1))
				SDL_GL_SetSwapInterval(1);
		}
		else
		{
			SDL_GL_SetSwapInterval(0);
		}
	}
	bool SwapChain_OpenGL::present()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, opengl_framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClearDepth(1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Vector2U wsize = m_window->getSize();

		glBlitFramebuffer(
			0, 0, m_canvas_size.x, m_canvas_size.y,
			(wsize.x - m_canvas_size.x) / 2, (wsize.y - m_canvas_size.y) / 2,
			(wsize.x + m_canvas_size.x) / 2, (wsize.y + m_canvas_size.y) / 2,
			GL_COLOR_BUFFER_BIT, GL_LINEAR
		);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, opengl_framebuffer);

		SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(m_window->getNativeHandle()));

		return true;
	}

	bool SwapChain_OpenGL::saveSnapshotToFile(StringView path)
	{
		std::string spath(path);

		std::unique_ptr<uint8_t> data(new uint8_t[m_canvas_size.x * m_canvas_size.y * 4]);

		glBindTexture(GL_TEXTURE_2D, opengl_texture);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

		return (bool)stbi_write_png(spath.c_str(), m_canvas_size.x, m_canvas_size.y, 4, data.get(), m_canvas_size.x * 4);
	}

	SwapChain_OpenGL::SwapChain_OpenGL(Window_SDL* p_window, Device_OpenGL* p_device)
		: m_window(p_window)
		, m_device(p_device)
	{
		assert(p_window);
		assert(p_device);
		m_window->addEventListener(this);
		m_device->addEventListener(this);
	}
	SwapChain_OpenGL::~SwapChain_OpenGL()
	{
		m_window->removeEventListener(this);
		m_device->removeEventListener(this);
		destroySwapChainRenderTarget();
		assert(m_eventobj.size() == 0);
		assert(m_eventobj_late.size() == 0);
	}

	bool SwapChain_OpenGL::create(Window_SDL* p_window, Device_OpenGL* p_device, SwapChain_OpenGL** pp_swapchain)
	{
		try
		{
			*pp_swapchain = new SwapChain_OpenGL(p_window, p_device);
			return true;
		}
		catch (...)
		{
			*pp_swapchain = nullptr;
			return false;
		}
	}

	bool ISwapChain::create(IWindow* p_window, IDevice* p_device, ISwapChain** pp_swapchain)
	{
		try
		{
			*pp_swapchain = new SwapChain_OpenGL(dynamic_cast<Window_SDL*>(p_window), dynamic_cast<Device_OpenGL*>(p_device));
			return true;
		}
		catch (...)
		{
			*pp_swapchain = nullptr;
			return false;
		}
	}
}
