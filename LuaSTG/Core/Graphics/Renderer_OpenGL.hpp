#pragma once
#include "Core/Graphics/Device.hpp"
#include "Core/Object.hpp"
#include "Core/Graphics/Renderer.hpp"
#include "Core/Graphics/Device_OpenGL.hpp"
#include "Core/Graphics/Model_OpenGL.hpp"
#include "glad/gl.h"

#define IDX(x) (size_t)static_cast<uint8_t>(x)

namespace Core::Graphics
{
	struct RendererStateSet
	{
		BoxF viewport = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
		RectF scissor_rect = { 0.0f, 0.0f, 1.0f, 1.0f };
		float fog_near_or_density = 0.0f;
		float fog_far = 0.0f;
		Color4B fog_color;
		IRenderer::VertexColorBlendState vertex_color_blend_state = IRenderer::VertexColorBlendState::Mul;
		IRenderer::SamplerState sampler_state = IRenderer::SamplerState::LinearClamp;
		IRenderer::FogState fog_state = IRenderer::FogState::Disable;
		IRenderer::TextureAlphaType texture_alpha_type = IRenderer::TextureAlphaType::Normal;
		IRenderer::DepthState depth_state = IRenderer::DepthState::Disable;
		IRenderer::BlendState blend_state = IRenderer::BlendState::Alpha;
	};

	struct CameraStateSet
	{
		BoxF ortho = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
		Vector3F eye = { 0.0f, 0.0f, 0.0f };
		Vector3F lookat = { 0.0f, 0.0f, 1.0f };
		Vector3F headup = { 0.0f, 1.0f, 0.0f };
		float fov = 0.0f;
		float aspect = 0.0f;
		float znear = 0.0f;
		float zfar = 0.0f;
		bool is_3D = false;

		bool isEqual(BoxF const& box)
		{
			return !is_3D && ortho == box;
		}
		bool isEqual(Vector3F const& eye_, Vector3F const& lookat_, Vector3F const& headup_, float fov_, float aspect_, float znear_, float zfar_)
		{
			return is_3D
				&& eye == eye_
				&& lookat == lookat_
				&& headup == headup_
				&& fov == fov_
				&& aspect == aspect_
				&& znear == znear_
				&& zfar == zfar_;
		}
	};

	struct VertexIndexBuffer
	{
		GLuint vertex_buffer = 0;
		GLuint index_buffer = 0;

		GLint vertex_offset = 0;
		GLuint index_offset = 0;
	};

	struct DrawCommand
	{
		ScopeObject<Texture2D_OpenGL> texture;
		uint16_t vertex_count = 0;
		uint16_t vertex_offset = 0;
		uint16_t index_count = 0;
	};

	struct DrawList
	{
		struct VertexBuffer
		{
			const size_t capacity = 32768;
			size_t size = 0;
			IRenderer::DrawVertex data[32768] = {};
		} vertex;
		struct IndexBuffer
		{
			const size_t capacity = 32768;
			size_t size = 0;
			IRenderer::DrawIndex data[32768] = {};
		} index;
		struct DrawCommandBuffer
		{
			const size_t capacity = 2048;
			size_t size = 0;
			DrawCommand data[2048] = {};
		} command;
	};

	class PostEffectShader_OpenGL
		: public Object<IPostEffectShader>
		, IDeviceEventListener
	{
	private:
		struct LocalVariable
		{
			GLuint offset{};
			GLuint size{};
		};
		struct LocalConstantBuffer
		{
			GLuint index{};
			std::vector<uint8_t> buffer;
			GLuint opengl_buffer = 0;
			std::unordered_map<std::string, LocalVariable> variable;
		};
		struct LocalTexture2D
		{
			GLuint index{};
			ScopeObject<Texture2D_OpenGL> texture;
		};
	private:
		ScopeObject<Device_OpenGL> m_device;
		// GLuint opengl_frag;
		GLuint opengl_prgm;
		std::unordered_map<std::string, LocalConstantBuffer> m_buffer_map;
		std::unordered_map<std::string, LocalTexture2D> m_texture2d_map;
		std::string source;
		bool is_path{ false };

		bool createResources();
		void onDeviceCreate();
		void onDeviceDestroy();
		bool findVariable(StringView name, LocalConstantBuffer*& buf, LocalVariable*& val);

	public:
		GLuint GetShader() const noexcept { return opengl_prgm; }

	public:
		bool setFloat(StringView name, float value);
		bool setFloat2(StringView name, Vector2F value);
		bool setFloat3(StringView name, Vector3F value);
		bool setFloat4(StringView name, Vector4F value);
		bool setTexture2D(StringView name, ITexture2D* p_texture);
		bool apply(IRenderer* p_renderer);

	public:
		PostEffectShader_OpenGL(Device_OpenGL* p_device, StringView path, bool is_path_);
		~PostEffectShader_OpenGL();
	};

	class Renderer_OpenGL
		: public Object<IRenderer>
		, IDeviceEventListener
	{
	private:
		ScopeObject<Device_OpenGL> m_device;
		ScopeObject<ModelSharedComponent_OpenGL> m_model_shared;

		GLuint _fx_vbuffer = 0;
		GLuint _fx_ibuffer = 0;
		GLuint _vao = 0;
		VertexIndexBuffer _vi_buffer[1];
		size_t _vi_buffer_index = 0;
		const size_t _vi_buffer_count = 1;
		DrawList _draw_list;

		void setVertexIndexBuffer(size_t index = 0xFFFFFFFFu);
		bool uploadVertexIndexBuffer(bool discard);
		void clearDrawList();

		GLuint _vp_matrix_buffer = 0;
		GLuint _world_matrix_buffer = 0;
		GLuint _camera_pos_buffer = 0; // Changed with postEffect
		GLuint _fog_data_buffer = 0; // Also used to store texture size and range of postEffect
		GLuint _user_float_buffer = 0; // Used with postEffect

		// Microsoft::WRL::ComPtr<ID3D11InputLayout> _input_layout;
		// GLuint _vertex_shader[IDX(FogState::MAX_COUNT)]; // FogState
		// GLuint _pixel_shader[IDX(VertexColorBlendState::MAX_COUNT)][IDX(FogState::MAX_COUNT)][IDX(TextureAlphaType::MAX_COUNT)]; // VertexColorBlendState, FogState, TextureAlphaType
		GLuint _program;
		// Microsoft::WRL::ComPtr<ID3D11RasterizerState> _raster_state;
		Graphics::SamplerState _sampler_state[IDX(SamplerState::MAX_COUNT)];
		// Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depth_state[IDX(DepthState::MAX_COUNT)];
		// Microsoft::WRL::ComPtr<ID3D11BlendState> _blend_state[IDX(BlendState::MAX_COUNT)];
		
		ScopeObject<Texture2D_OpenGL> _state_texture;
		CameraStateSet _camera_state_set;
		RendererStateSet _state_set;
		bool _state_dirty = false;
		bool _batch_scope = false;

		bool createBuffers();
		bool createStates();
		bool createShaders();
		void initState();
		bool uploadVertexIndexBufferFromDrawList();
		void bindTextureSamplerState(ITexture2D* texture);
		void bindTextureAlphaType(ITexture2D* texture);
		bool batchFlush(bool discard = false);

		bool createResources();
		void onDeviceCreate();
		void onDeviceDestroy();

	public:
		void setSamplerState(IRenderer::SamplerState state, GLuint index);
		void setSamplerState(Graphics::SamplerState state, GLuint index);

	public:
		bool beginBatch();
		bool endBatch();
		bool isBatchScope() { return _batch_scope; }
		bool flush();

		void clearRenderTarget(Color4B const& color);
		void clearDepthBuffer(float zvalue);
		void setRenderAttachment(IRenderTarget* p_rt);

		void setOrtho(BoxF const& box);
		void setPerspective(Vector3F const& eye, Vector3F const& lookat, Vector3F const& headup, float fov, float aspect, float znear, float zfar);

		inline BoxF getViewport() { return _state_set.viewport; }
		void setViewport(BoxF const& box);
		void setScissorRect(RectF const& rect);
		void setViewportAndScissorRect();

		void setVertexColorBlendState(VertexColorBlendState state);
		void setFogState(FogState state, Color4B const& color, float density_or_znear, float zfar);
		void setDepthState(DepthState state);
		void setBlendState(BlendState state);
		void setTexture(ITexture2D* texture);

		bool drawTriangle(DrawVertex const& v1, DrawVertex const& v2, DrawVertex const& v3);
		bool drawTriangle(DrawVertex const* pvert);
		bool drawQuad(DrawVertex const& v1, DrawVertex const& v2, DrawVertex const& v3, DrawVertex const& v4);
		bool drawQuad(DrawVertex const* pvert);
		bool drawRaw(DrawVertex const* pvert, uint16_t nvert, DrawIndex const* pidx, uint16_t nidx);
		bool drawRequest(uint16_t nvert, uint16_t nidx, DrawVertex** ppvert, DrawIndex** ppidx, uint16_t* idxoffset);

		bool createPostEffectShader(StringView path, IPostEffectShader** pp_effect);
		bool drawPostEffect(
			IPostEffectShader* p_effect,
			BlendState blend,
			ITexture2D* p_tex, SamplerState rtsv,
			Vector4F const* cv, size_t cv_n,
			ITexture2D* const* p_tex_arr, SamplerState const* sv, size_t tv_sv_n);
		bool drawPostEffect(IPostEffectShader* p_effect, BlendState blend);

		bool createModel(StringView path, IModel** pp_model);
		bool drawModel(IModel* p_model);

		Graphics::SamplerState getKnownSamplerState(SamplerState state);

	public:
		Renderer_OpenGL(Device_OpenGL* p_device);
		~Renderer_OpenGL();

	public:
		static bool create(Device_OpenGL* p_device, Renderer_OpenGL** pp_renderer);
	};
}

#undef IDX
