#include "Core/Graphics/Device_OpenGL.hpp"
#include "Core/FileManager.hpp"
#include "Core/Type.hpp"
#include "Core/i18n.hpp"

#include <cstdint>
#include <memory>

#include "glad/gl.h"
#include "spdlog/spdlog.h"
// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace Core::Graphics
{
	Device_OpenGL::Device_OpenGL()
	{
		// Create graphics components...
		// ... but, OpenGL doesn't actually have a concept of a "device".

		spdlog::info("[core] created OpenGL Dummy Device");
	}
	Device_OpenGL::~Device_OpenGL()
	{
		assert(m_eventobj.size() == 0);
		assert(m_eventobj_late.size() == 0);
	}

	void Device_OpenGL::dispatchEvent(EventType t)
	{
		// callback
		m_is_dispatch_event = true;
		switch (t)
		{
		case EventType::DeviceCreate:
			for (auto& v : m_eventobj)
			{
				if (v) v->onDeviceCreate();
			}
			break;
		case EventType::DeviceDestroy:
			for (auto& v : m_eventobj)
			{
				if (v) v->onDeviceDestroy();
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

	void Device_OpenGL::addEventListener(IDeviceEventListener* e)
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
	void Device_OpenGL::removeEventListener(IDeviceEventListener* e)
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

	bool Device_OpenGL::recreate()
	{
		// function placeholder due to OpenGL transition
		return true;
	}

	bool Device_OpenGL::createTextureFromFile(StringView path, bool mipmap, ITexture2D** pp_texture)
	{
		try
		{
			*pp_texture = new Texture2D_OpenGL(this, path, mipmap);
			return true;
		}
		catch (...)
		{
			*pp_texture = nullptr;
			return false;
		}
	}
	//bool createTextureFromMemory(void const* data, size_t size, bool mipmap, ITexture2D** pp_texture);
	bool Device_OpenGL::createTexture(Vector2U size, ITexture2D** pp_texture)
	{
		try
		{
			*pp_texture = new Texture2D_OpenGL(this, size, false);
			return true;
		}
		catch (...)
		{
			*pp_texture = nullptr;
			return false;
		}
	}

	bool Device_OpenGL::createRenderTarget(Vector2U size, IRenderTarget** pp_rt)
	{
		try
		{
			*pp_rt = new RenderTarget_OpenGL(this, size);
			return true;
		}
		catch (...)
		{
			*pp_rt = nullptr;
			return false;
		}
	}
	bool Device_OpenGL::createDepthStencilBuffer(Vector2U size, IDepthStencilBuffer** pp_ds)
	{
		try
		{
			*pp_ds = new DepthStencilBuffer_OpenGL(this, size);
			return true;
		}
		catch (...)
		{
			*pp_ds = nullptr;
			return false;
		}
	}

	bool Device_OpenGL::create(Device_OpenGL** p_device)
	{
		try
		{
			*p_device = new Device_OpenGL();
			return true;
		}
		catch (...)
		{
			*p_device = nullptr;
			return false;
		}
	}

	bool IDevice::create(IDevice** p_device)
	{
		try
		{
			*p_device = new Device_OpenGL();
			return true;
		}
		catch (...)
		{
			*p_device = nullptr;
			return false;
		}
	}
}

namespace Core::Graphics
{
	// Texture2D

	bool Texture2D_OpenGL::setSize(Vector2U size)
	{
		if (!(m_dynamic || m_isrt))
		{
			spdlog::error("[core] Cannot modify size of static texture");
			return false;
		}
		onDeviceDestroy();
		m_size = size;
		return createResource();
	}

	bool Texture2D_OpenGL::uploadPixelData(RectU rc, void const* data, uint32_t pitch)
	{
		if (!m_dynamic)
		{
			return false;
		}

		glBindTexture(GL_TEXTURE_2D, opengl_texture2d);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch / 4);
		glTexSubImage2D(GL_TEXTURE_2D, 0, rc.a.x, rc.a.y, rc.width(), rc.height(), GL_RGBA, GL_UNSIGNED_BYTE, data);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		return true;
	}

	bool Texture2D_OpenGL::saveToFile(StringView path)
	{
		std::string spath(path);

		std::unique_ptr<uint8_t> data(new uint8_t[m_size.x * m_size.y * 4]);

		glBindTexture(GL_TEXTURE_2D, opengl_texture2d);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

		return (bool)stbi_write_png(spath.c_str(), m_size.x, m_size.y, 4, data.get(), m_size.x * 4);
	}

	void Texture2D_OpenGL::onDeviceCreate()
	{
		createResource();
	}
	void Texture2D_OpenGL::onDeviceDestroy()
	{
		glDeleteTextures(1, &opengl_texture2d);
	}

	bool Texture2D_OpenGL::createResource()
	{
		if (m_data)
		{
			glGenTextures(1, &opengl_texture2d);
			if (opengl_texture2d == 0) {
				i18n_core_system_call_report_error("glGenTextures");
				return false;
			}
			glBindTexture(GL_TEXTURE_2D, opengl_texture2d);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_size.x, m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data->data());
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else if (!source_path.empty())
		{
			std::vector<uint8_t> src;
			if (!GFileManager().loadEx(source_path, src))
			{
				spdlog::error("[core] Unable to load file '{}'", source_path);
				return false;
			}

			// Load pictures
			Vector2I size;
			uint8_t* data = stbi_load_from_memory(src.data(), src.size(), &size.x, &size.y, NULL, 4);
			if (data == NULL)
			{
				spdlog::error("[core] Unable to parse file '{}'", source_path);
				return false;
			}
			// image size will never be negative
			m_size.x = size.x;
			m_size.y = size.y;

			glGenTextures(1, &opengl_texture2d);
			if (opengl_texture2d == 0) {
				i18n_core_system_call_report_error("glGenTextures");
				return false;
			}
			glBindTexture(GL_TEXTURE_2D, opengl_texture2d);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_size.x, m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
			stbi_image_free(data);
		}
		else
		{
			glGenTextures(1, &opengl_texture2d);
			if (opengl_texture2d == 0) {
				i18n_core_system_call_report_error("glGenTextures");
				return false;
			}
			glBindTexture(GL_TEXTURE_2D, opengl_texture2d);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_size.x, m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glGenerateMipmap(GL_TEXTURE_2D);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
		}

		return true;
	}

	Texture2D_OpenGL::Texture2D_OpenGL(Device_OpenGL* device, StringView path, bool mipmap)
		: m_device(device)
		, source_path(path)
		, m_dynamic(false)
		, m_premul(false)
		, m_mipmap(mipmap)
		, m_isrt(false)
	{
		if (path.empty())
			throw std::runtime_error("Texture2D::Texture2D(1)");
		if (!createResource())
			throw std::runtime_error("Texture2D::Texture2D(2)");
		m_device->addEventListener(this);
	}
	Texture2D_OpenGL::Texture2D_OpenGL(Device_OpenGL* device, Vector2U size, bool rendertarget)
		: m_device(device)
		, m_size(size)
		, m_dynamic(true)
		, m_premul(rendertarget)
		, m_mipmap(false)
		, m_isrt(rendertarget)
	{
		if (!createResource())
			throw std::runtime_error("Texture2D::Texture2D");
		if (!m_isrt)
			m_device->addEventListener(this);
	}
	Texture2D_OpenGL::~Texture2D_OpenGL()
	{
		if (!m_isrt)
			m_device->removeEventListener(this);
		glDeleteTextures(1, &opengl_texture2d);
	}

	// RenderTarget

	bool RenderTarget_OpenGL::setSize(Vector2U size)
	{
		if (!m_texture->setSize(size)) return false;
		return createResource();
	}

	void RenderTarget_OpenGL::onDeviceCreate()
	{
		if (m_texture->createResource())
			createResource();
	}
	void RenderTarget_OpenGL::onDeviceDestroy()
	{
		m_texture->onDeviceDestroy();
	}

	bool RenderTarget_OpenGL::createResource()
	{
		glGenFramebuffers(1, &opengl_framebuffer);
		if (opengl_framebuffer == 0) {
			spdlog::error("[core] glGenFramebuffers failed");
			return false;
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, opengl_framebuffer);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthstencilbuffer->GetResource());
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture->GetResource(), 0);
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);

		if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			spdlog::error("[core] Failed to create rendertarget framebuffer");
			return false;
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		return true;
	}

	RenderTarget_OpenGL::RenderTarget_OpenGL(Device_OpenGL* device, Vector2U size)
		: m_device(device)
	{
		m_texture.attach(new Texture2D_OpenGL(device, size, true));
		m_depthstencilbuffer.attach(new DepthStencilBuffer_OpenGL(device, size));
		if (!createResource())
			throw std::runtime_error("RenderTarget::RenderTarget");
		m_device->addEventListener(this);
	}
	RenderTarget_OpenGL::~RenderTarget_OpenGL()
	{
		m_device->removeEventListener(this);
	}

	// DepthStencil

	bool DepthStencilBuffer_OpenGL::setSize(Vector2U size)
	{
		glDeleteRenderbuffers(1, &opengl_depthstencilbuffer);
		m_size = size;
		return createResource();
	}

	void DepthStencilBuffer_OpenGL::onDeviceCreate()
	{
		createResource();
	}
	void DepthStencilBuffer_OpenGL::onDeviceDestroy()
	{
		glDeleteRenderbuffers(1, &opengl_depthstencilbuffer);
	}

	bool DepthStencilBuffer_OpenGL::createResource()
	{
		glGenRenderbuffers(1, &opengl_depthstencilbuffer);
		if (opengl_depthstencilbuffer == 0) {
			spdlog::error("[core] glGenRenderbuffers failed");
			return false;
		}
		glBindRenderbuffer(GL_RENDERBUFFER, opengl_depthstencilbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, m_size.x, m_size.y);
		return true;
	}

	DepthStencilBuffer_OpenGL::DepthStencilBuffer_OpenGL(Device_OpenGL* device, Vector2U size)
		: m_device(device)
		, m_size(size)
	{
		if (!createResource())
			throw std::runtime_error("DepthStencilBuffer::DepthStencilBuffer");
		m_device->addEventListener(this);
	}
	DepthStencilBuffer_OpenGL::~DepthStencilBuffer_OpenGL()
	{
		m_device->removeEventListener(this);
	}
}
