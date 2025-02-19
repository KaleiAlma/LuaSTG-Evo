#include "Core/Graphics/SwapChain_OpenGL.hpp"
#include "Core/Graphics/Device_OpenGL.hpp"
#include "Core/Graphics/Renderer.hpp"
#include "Core/Graphics/Window_SDL.hpp"
#include "Core/Type.hpp"
#include "Core/i18n.hpp"
#include "SDL_video.h"
#include "glad/gl.h"
#include "SDL.h"
#include "spdlog/spdlog.h"
#include "stb_image_write.h"

//#define _log(x) OutputDebugStringA(x "\n")
#define _log(x)
#define ReportError(x) spdlog::error("[core] ", x)

// Default Fragment Shader
const GLchar sc_frag[]{R"(
#version 410 core
uniform sampler2D sampler0;

layout(location = 0) in vec2 TexCoord;
out vec4 FragColor;

void main()
{
    FragColor = texture(sampler0, TexCoord);
}
)"};

// Default Vertex Shader
const GLchar sc_vert[]{R"(
#version 410 core
layout(location = 0) in vec2 pos;
layout(location = 0) out vec2 TexCoord;

void main()
{
    TexCoord = pos;

    gl_Position = vec4(pos.x * 2 - 1, pos.y * 2 - 1, 0.0, 1.0);
}
)"};


static bool compileShaderMacro(const GLchar* data, GLint size, GLenum shadertype, GLuint& shader)
{
	shader = glCreateShader(shadertype);
	glShaderSource(shader, 1, &data, &size);
	glCompileShader(shader);

	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		GLchar log[1024];
		int32_t log_len;
		glGetShaderInfoLog(shader, 1024, &log_len, log);
		spdlog::error("[core] Failed to compile shader: {}", log);
		glDeleteShader(shader);
		return false;
	}

	return true;
}
static bool compileVertexShaderMacro(const GLchar* data, GLint size, GLuint& shader)
{
	return compileShaderMacro(data, size, GL_VERTEX_SHADER, shader);
}
static bool compileFragmentShaderMacro(const GLchar* data, GLint size, GLuint& shader)
{
	return compileShaderMacro(data, size, GL_FRAGMENT_SHADER, shader);
}

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

		glGenRenderbuffers(1, &rdr_depthstencilbuffer);
		if (rdr_depthstencilbuffer == 0) {
			spdlog::error("[core] (SwapChain) glGenRenderbuffers failed");
			return false;
		}
		glBindRenderbuffer(GL_RENDERBUFFER, rdr_depthstencilbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, m_canvas_size.x, m_canvas_size.y);

		glGenTextures(1, &rdr_tex);
		if (rdr_tex == 0) {
			spdlog::error("[core] (SwapChain) glGenTextures failed");
			return false;
		}
		glBindTexture(GL_TEXTURE_2D, rdr_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_canvas_size.x, m_canvas_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glGenFramebuffers(1, &rdr_fbo);
		if (rdr_fbo == 0) {
			spdlog::error("[core] (SwapChain) glGenFramebuffers failed");
			return false;
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rdr_fbo);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rdr_depthstencilbuffer);
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rdr_tex, 0);
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);

		GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

		if(status != GL_FRAMEBUFFER_COMPLETE)
		{
			spdlog::error("[core] Failed to create swapchain framebuffer: {}, {}", glGetError(), status);
			assert(false);
			return false;
		}

		glDrawBuffers(1, DrawBuffers);

		status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

		if(status != GL_FRAMEBUFFER_COMPLETE)
		{
			spdlog::error("[core] Failed to create swapchain framebuffer: {}, {}", glGetError(), status);
			assert(false);
			return false;
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
		// glGenVertexArrays(1, &rdr_vao);
		// glGenBuffers(1, &rdr_vbo);
		// glGenBuffers(1, &rdr_ibo);
		glGenVertexArrays(1, &ex_vao);
		glGenBuffers(1, &ex_vbo);
		glGenBuffers(1, &ex_ibo);

		GLuint frag, vert;
		compileVertexShaderMacro(sc_vert, sizeof(sc_vert), vert);
		compileFragmentShaderMacro(sc_frag, sizeof(sc_frag), frag);
		prgm = glCreateProgram();
		glAttachShader(prgm, vert);
		glAttachShader(prgm, frag);
		glLinkProgram(prgm);
		glDeleteShader(vert);
		glDeleteShader(frag);

		glBindVertexArray(ex_vao);

		glBindBuffer(GL_ARRAY_BUFFER, ex_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), &vertex_data, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ex_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx_data), idx_data, GL_STATIC_DRAW);
		return true;
	}
	void SwapChain_OpenGL::destroySwapChainRenderTarget()
	{
		_log("destroySwapChainRenderTarget");

		glDeleteFramebuffers(1, &rdr_fbo);
		glDeleteRenderbuffers(1, &rdr_depthstencilbuffer);
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

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rdr_fbo);
	}
	void SwapChain_OpenGL::clearRenderAttachment()
	{
		//_log("clearRenderAttachment");

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	bool SwapChain_OpenGL::handleSwapChainWindowSize(Vector2I size)
	{
		_log("handleSwapChainWindowSize");

		if (size.x == 0 || size.y == 0)
		{
			assert(false); return false;
		}

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
		// Vector2U wsize = m_window->getSize();
		Vector2I wsize{};
		SDL_GL_GetDrawableSize(m_window->GetWindow(), &wsize.x, &wsize.y);
		Vector2F scale_dim = Vector2F((float) wsize.x / m_canvas_size.x, (float) wsize.y / m_canvas_size.y);
		float scale = std::min(scale_dim.x, scale_dim.y);
		Vector2F d;

		if (scale_dim.x > scale_dim.y)
		{
			d.x = ((float)wsize.x - m_canvas_size.x * scale) * 0.5;
			d.y = 0;
		}
		else
		{
			d.x = 0;
			d.y = ((float)wsize.y - m_canvas_size.y * scale) * 0.5;
		}
		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, rdr_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, wsize.x, wsize.y);
		glScissor(0, 0, wsize.x, wsize.y);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClearDepth(1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBlitFramebuffer(
			0, 0, m_canvas_size.x, m_canvas_size.y,
			d.x, scale * m_canvas_size.y + d.y, scale * m_canvas_size.x + d.x, d.y,
			GL_COLOR_BUFFER_BIT, GL_LINEAR
		);

		if (!ex_fbos.empty())
		{
			glUseProgram(prgm);
			glBindVertexArray(ex_vao);
			glBindBuffer(GL_ARRAY_BUFFER, ex_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), &vertex_data, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ex_ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx_data), idx_data, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
			glEnableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);

			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);  
			glBlendEquation(GL_FUNC_ADD);

			for (auto& tex : ex_fbos)
			{
				// glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
				

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex);


				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
			}
		}

		// glBlitFramebuffer(
		// 	0, 0, wsize.x, wsize.y,
		// 	0, 0, wsize.x, wsize.y,
		// 	GL_COLOR_BUFFER_BIT, GL_LINEAR
		// );

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(m_window->getNativeHandle()));

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rdr_fbo);

#ifndef NDEBUG
		// spdlog::debug("GL Error: {}", glGetError());
#endif

		return true;
	}

	bool SwapChain_OpenGL::saveSnapshotToFile(StringView path)
	{
		std::string spath(path);

		std::unique_ptr<uint8_t> data(new uint8_t[m_canvas_size.x * m_canvas_size.y * 4]);

		glBindTexture(GL_TEXTURE_2D, rdr_tex);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

		return (bool)stbi_write_png(spath.c_str(), m_canvas_size.x, m_canvas_size.y, 4, data.get(), m_canvas_size.x * 4);
	}

	bool SwapChain_OpenGL::addFramebuffer(GLuint &fbo, GLuint &tex)
	{
		glGenTextures(1, &tex);
		if (tex == 0) {
			spdlog::error("[core] (SwapChain, Extra) glGenTextures failed");
			return false;
		}
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_window->getSize().x, m_window->getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glGenFramebuffers(1, &fbo);
		if (fbo == 0) {
			spdlog::error("[core] (SwapChain, Extra) glGenFramebuffers failed");
			return false;
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);

		if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			spdlog::error("[core] Failed to create SwapChain Extra framebuffer");
			return false;
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		ex_fbos.emplace_back(tex);

		return true;
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
