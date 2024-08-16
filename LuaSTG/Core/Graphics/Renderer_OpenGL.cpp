#include "Core/Graphics/Renderer_OpenGL.hpp"
#include "Core/Graphics/Device.hpp"
#include "Core/Graphics/Device_OpenGL.hpp"
#include "Core/Graphics/Model_OpenGL.hpp"
#include "Core/Graphics/Renderer.hpp"
#include "Core/Type.hpp"
#include "TracyOpenGL.hpp"
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>
#include <optional>

#define IDX(x) (size_t)static_cast<uint8_t>(x)
using DrawIndex = uint16_t;


namespace Core::Graphics
{
    inline GLuint get_view(Texture2D_OpenGL* p)
    {
        return p->GetView();
    }
    inline GLuint get_view(ITexture2D* p)
    {
        return get_view(static_cast<Texture2D_OpenGL*>(p));
    }
    inline GLuint get_view(ScopeObject<Texture2D_OpenGL>& p)
    {
        return get_view(p.get());
    }
    inline GLuint get_view(ScopeObject<ITexture2D>& p)
    {
        return get_view(static_cast<Texture2D_OpenGL*>(p.get()));
    }

    // inline GLuint get_sampler(ISamplerState* p_sampler)
    // {
    // 	return static_cast<SamplerState_OpenGL*>(p_sampler)->GetState();
    // }
    // inline GLuint get_sampler(ScopeObject<ISamplerState>& p_sampler)
    // {
    // 	return static_cast<SamplerState_OpenGL*>(p_sampler.get())->GetState();
    // }
}

namespace Core::Graphics
{
    void PostEffectShader_OpenGL::onDeviceCreate()
    {
        createResources();
    }
    void PostEffectShader_OpenGL::onDeviceDestroy()
    {
        glDeleteProgram(opengl_prgm);
        for (auto& v : m_buffer_map)
        {
            glDeleteBuffers(1, &v.second.opengl_buffer);
        }
    }

    bool PostEffectShader_OpenGL::findVariable(StringView name, LocalConstantBuffer*& buf, LocalVariable*& val)
    {
        std::string name_s(name);
        for (auto& b : m_buffer_map)
        {
            auto it = b.second.variable.find(name_s);
            if (it != b.second.variable.end())
            {
                buf = &b.second;
                val = &it->second;
                return true;
            }
        }
        return false;
    }
    bool PostEffectShader_OpenGL::setFloat(StringView name, float value)
    {
        LocalConstantBuffer* b{};
        LocalVariable* v{};
        if (!findVariable(name, b, v)) { return false; }
        if (v->size != sizeof(value)) { assert(false); return false; }
        memcpy(b->buffer.data() + v->offset, &value, v->size);
        return true;
    }
    bool PostEffectShader_OpenGL::setFloat2(StringView name, Vector2F value)
    {
        LocalConstantBuffer* b{};
        LocalVariable* v{};
        if (!findVariable(name, b, v)) { return false; }
        if (v->size != sizeof(value)) { assert(false); return false; }
        memcpy(b->buffer.data() + v->offset, &value, v->size);
        return true;
    }
    bool PostEffectShader_OpenGL::setFloat3(StringView name, Vector3F value)
    {
        LocalConstantBuffer* b{};
        LocalVariable* v{};
        if (!findVariable(name, b, v)) { return false; }
        if (v->size != sizeof(value)) { assert(false); return false; }
        memcpy(b->buffer.data() + v->offset, &value, v->size);
        return true;
    }
    bool PostEffectShader_OpenGL::setFloat4(StringView name, Vector4F value)
    {
        LocalConstantBuffer* b{};
        LocalVariable* v{};
        if (!findVariable(name, b, v)) { return false; }
        if (v->size != sizeof(value)) { assert(false); return false; }
        memcpy(b->buffer.data() + v->offset, &value, v->size);
        return true;
    }
    bool PostEffectShader_OpenGL::setTexture2D(StringView name, ITexture2D* p_texture)
    {
        std::string name_s(name);
        auto it = m_texture2d_map.find(name_s);
        if (it == m_texture2d_map.end()) { return false; }
        it->second.texture = dynamic_cast<Texture2D_OpenGL*>(p_texture);
        if (!it->second.texture) { assert(false); return false; }
        return true;
    }
    bool PostEffectShader_OpenGL::apply(IRenderer* p_renderer)
    {
        assert(p_renderer);

        auto p_sampler = p_renderer->getKnownSamplerState(IRenderer::SamplerState::LinearClamp);

        for (auto& v : m_buffer_map)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, v.second.opengl_buffer);
            glBufferData(GL_UNIFORM_BUFFER, v.second.buffer.size(), v.second.buffer.data(), GL_STATIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, v.second.binding, v.second.opengl_buffer);
        }

        for (auto& v : m_texture2d_map)
        {
            auto p_custom = v.second.texture->getSamplerState();
            glActiveTexture(GL_TEXTURE0 + v.second.index);
            glBindTexture(GL_TEXTURE_2D, v.second.texture->GetResource());
            static_cast<Renderer_OpenGL*>(p_renderer)->setSamplerState(p_custom.value_or(p_sampler), v.second.index);
        }

        return true;
    }

    void PostEffectShader_OpenGL::bind(GLuint engine_data, GLuint user_data)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, m_buffer_map["engine_data"].binding, engine_data);
        glBindBufferBase(GL_UNIFORM_BUFFER, m_buffer_map["user_data"].binding, user_data);
    }

    PostEffectShader_OpenGL::PostEffectShader_OpenGL(Device_OpenGL* p_device, StringView path, bool is_path_)
        : m_device(p_device)
        , source(path)
        , is_path(is_path_)
    {
        if (!createResources())
            throw std::runtime_error("PostEffectShader_OpenGL::PostEffectShader_OpenGL");
        m_device->addEventListener(this);
    }
    PostEffectShader_OpenGL::~PostEffectShader_OpenGL()
    {
        m_device->removeEventListener(this);
    }
}

namespace Core::Graphics
{
    void Renderer_OpenGL::setVertexIndexBuffer(size_t index)
    {
        index = (index == 0xFFFFFFFFu) ? _vi_buffer_index : index;
        auto& vi = _vi_buffer[index];

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, vi.vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vi.index_buffer);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (const GLvoid *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (const GLvoid *)offsetof(DrawVertex, u));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DrawVertex), (const GLvoid *)offsetof(DrawVertex, color));
        glEnableVertexAttribArray(2);
    }
    bool Renderer_OpenGL::uploadVertexIndexBuffer(bool discard)
    {
        ZoneScoped;
        TracyGpuZone("UploadVertexIndexBuffer");
        glBindVertexArray(_vao);
        auto& vi_ = _vi_buffer[_vi_buffer_index];
        GLuint inv = discard ? GL_MAP_INVALIDATE_BUFFER_BIT : 0;
        // copy vertex data
        if (_draw_list.vertex.size > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vi_.vertex_buffer);
            // void* map = glMapBufferRange(
            // 	GL_ARRAY_BUFFER,
            // 	vi_.vertex_offset * sizeof(DrawVertex),
            // 	// 0,
            // 	_draw_list.vertex.size * sizeof(DrawVertex),
            // 	GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | inv
            // );
            // std::memcpy((DrawVertex*)map, _draw_list.vertex.data, _draw_list.vertex.size * sizeof(DrawVertex));
            // glUnmapBuffer(GL_ARRAY_BUFFER);
            glBufferSubData(GL_ARRAY_BUFFER, vi_.vertex_offset * sizeof(DrawVertex), _draw_list.vertex.size * sizeof(DrawVertex), _draw_list.vertex.data);
        }
        // copy index data
        if (_draw_list.index.size > 0)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vi_.index_buffer);
            // void* map = glMapBufferRange(
            // 	GL_ELEMENT_ARRAY_BUFFER,
            // 	vi_.index_offset * sizeof(DrawIndex),
            // 	_draw_list.index.size * sizeof(DrawIndex),
            // 	GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | inv
            // );
            // std::memcpy((DrawIndex*)map, _draw_list.index.data, _draw_list.index.size * sizeof(DrawIndex));
            // glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, vi_.index_offset * sizeof(DrawIndex), _draw_list.index.size * sizeof(DrawIndex), _draw_list.index.data);
        }
        
        return true;
    }
    void Renderer_OpenGL::clearDrawList()
    {
        for (size_t j_ = 0; j_ < _draw_list.command.size; j_ += 1)
        {
            _draw_list.command.data[j_].texture.reset();
        }
        _draw_list.vertex.size = 0;
        _draw_list.index.size = 0;
        _draw_list.command.size = 0;
    }

    bool Renderer_OpenGL::createBuffers()
    {
        glGenVertexArrays(1, &_vao);
        if (_vao == 0) return false;

        glGenBuffers(1, &_fx_vbuffer);
        if (_fx_vbuffer == 0) return false;

        DrawIndex const idx_[6] = { 0, 1, 2, 0, 2, 3 };
        glGenBuffers(1, &_fx_ibuffer);
        if (_fx_ibuffer == 0) return false;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _fx_ibuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx_), &idx_, GL_STATIC_DRAW);

        for (auto& vi_ : _vi_buffer)
        {
            glGenBuffers(1, &vi_.vertex_buffer);
            if (vi_.vertex_buffer == 0) return false;
            glBindBuffer(GL_ARRAY_BUFFER, vi_.vertex_buffer);
            // glBufferStorage(GL_ARRAY_BUFFER, _draw_list.vertex.capacity * sizeof(DrawVertex), 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            glBufferData(GL_ARRAY_BUFFER, _draw_list.vertex.capacity * sizeof(DrawVertex), 0, GL_DYNAMIC_DRAW);

            glGenBuffers(1, &vi_.index_buffer);
            if (vi_.index_buffer == 0) return false;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vi_.index_buffer);
            // glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, _draw_list.index.capacity * sizeof(DrawIndex), 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _draw_list.index.capacity * sizeof(DrawIndex), 0, GL_DYNAMIC_DRAW);
        }

        glGenBuffers(1, &_vp_matrix_buffer);
        if (_vp_matrix_buffer == 0) return false;

        glGenBuffers(1, &_world_matrix_buffer);
        if (_world_matrix_buffer == 0) return false;

        glGenBuffers(1, &_camera_pos_buffer);
        if (_camera_pos_buffer == 0) return false;

        glGenBuffers(1, &_fog_data_buffer);
        if (_fog_data_buffer == 0) return false;

        glGenBuffers(1, &_user_float_buffer);
        if (_user_float_buffer == 0) return false;

        return true;
    }
    bool Renderer_OpenGL::createStates()
    {
        {
            // Graphics::SamplerState sampler_state;

            // point

            _sampler_state[IDX(SamplerState::PointWrap)].filter = Filter(FilterMode::Nearest, FilterMode::Nearest);
            _sampler_state[IDX(SamplerState::PointWrap)].address_u = TextureAddressMode::Wrap;
            _sampler_state[IDX(SamplerState::PointWrap)].address_v = TextureAddressMode::Wrap;

            _sampler_state[IDX(SamplerState::PointClamp)].filter = Filter(FilterMode::Nearest, FilterMode::Nearest);
            _sampler_state[IDX(SamplerState::PointClamp)].address_u = TextureAddressMode::Clamp;
            _sampler_state[IDX(SamplerState::PointClamp)].address_v = TextureAddressMode::Clamp;

            _sampler_state[IDX(SamplerState::PointBorderBlack)].filter = Filter(FilterMode::Nearest, FilterMode::Nearest);
            _sampler_state[IDX(SamplerState::PointBorderBlack)].address_u = TextureAddressMode::Border;
            _sampler_state[IDX(SamplerState::PointBorderBlack)].address_v = TextureAddressMode::Border;
            _sampler_state[IDX(SamplerState::PointBorderBlack)].border_color = BorderColor::Black;

            _sampler_state[IDX(SamplerState::PointBorderWhite)].filter = Filter(FilterMode::Nearest, FilterMode::Nearest);
            _sampler_state[IDX(SamplerState::PointBorderWhite)].address_u = TextureAddressMode::Border;
            _sampler_state[IDX(SamplerState::PointBorderWhite)].address_v = TextureAddressMode::Border;
            _sampler_state[IDX(SamplerState::PointBorderWhite)].border_color = BorderColor::White;

            // linear

            _sampler_state[IDX(SamplerState::LinearWrap)].filter = Filter(FilterMode::Linear, FilterMode::Linear);
            _sampler_state[IDX(SamplerState::LinearWrap)].address_u = TextureAddressMode::Wrap;
            _sampler_state[IDX(SamplerState::LinearWrap)].address_v = TextureAddressMode::Wrap;

            _sampler_state[IDX(SamplerState::LinearClamp)].filter = Filter(FilterMode::Linear, FilterMode::Linear);
            _sampler_state[IDX(SamplerState::LinearClamp)].address_u = TextureAddressMode::Clamp;
            _sampler_state[IDX(SamplerState::LinearClamp)].address_v = TextureAddressMode::Clamp;

            _sampler_state[IDX(SamplerState::LinearBorderBlack)].filter = Filter(FilterMode::Linear, FilterMode::Linear);
            _sampler_state[IDX(SamplerState::LinearBorderBlack)].address_u = TextureAddressMode::Border;
            _sampler_state[IDX(SamplerState::LinearBorderBlack)].address_v = TextureAddressMode::Border;
            _sampler_state[IDX(SamplerState::LinearBorderBlack)].border_color = BorderColor::Black;

            _sampler_state[IDX(SamplerState::LinearBorderWhite)].filter = Filter(FilterMode::Linear, FilterMode::Linear);
            _sampler_state[IDX(SamplerState::LinearBorderWhite)].address_u = TextureAddressMode::Border;
            _sampler_state[IDX(SamplerState::LinearBorderWhite)].address_v = TextureAddressMode::Border;
            _sampler_state[IDX(SamplerState::LinearBorderWhite)].border_color = BorderColor::White;
        }

        return true;
    }
    void Renderer_OpenGL::initState()
    {
        _state_dirty = true;

        if (!_camera_state_set.is_3D)
        {
            setOrtho(_camera_state_set.ortho);
        }
        else
        {
            setPerspective(_camera_state_set.eye, _camera_state_set.lookat, _camera_state_set.headup, _camera_state_set.fov, _camera_state_set.aspect, _camera_state_set.znear, _camera_state_set.zfar);
        }

        setViewport(_state_set.viewport);
        setScissorRect(_state_set.scissor_rect);

        setVertexColorBlendState(_state_set.vertex_color_blend_state);
        setTexture(_state_texture.get());
        setSamplerState(_state_set.sampler_state, 0);
        setFogState(_state_set.fog_state, _state_set.fog_color, _state_set.fog_near_or_density, _state_set.fog_far);
        setDepthState(_state_set.depth_state);
        setBlendState(_state_set.blend_state);
        bindTextureAlphaType(_state_texture.get());
        
        _state_dirty = false;
    }
    void Renderer_OpenGL::setSamplerState(IRenderer::SamplerState state, GLuint index)
    {
        // GLuint opengl_sampler = static_cast<SamplerState_OpenGL*>(_sampler_state[IDX(state)].get())->GetState();
        // glBindSampler(GL_TEXTURE0 + index, opengl_sampler);
        // glTexParameteri(GL_TEXTURE_2D, );
        // 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        setSamplerState(_sampler_state[IDX(state)], index);
    }
    void Renderer_OpenGL::setSamplerState(Graphics::SamplerState state, GLuint index)
    {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, state.max_anisotropy);
        switch (state.filter.min)
        {
        case FilterMode::Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            break;
        case FilterMode::NearestMipNearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            break;
        case FilterMode::NearestMipLinear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            break;
        case FilterMode::Linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            break;
        case FilterMode::LinearMipNearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            break;
        case FilterMode::LinearMipLinear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            break;
        }
        switch (state.filter.mag)
        {
        default:
            assert(false);
            break;
        case FilterMode::Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case FilterMode::Linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        }

        switch (state.address_u)
        {
        case TextureAddressMode::Wrap:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            break;
        case TextureAddressMode::Mirror:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            break;
        case TextureAddressMode::Clamp:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            break;
        case TextureAddressMode::Border:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            break;
        }
        switch (state.address_v)
        {
        case TextureAddressMode::Wrap:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case TextureAddressMode::Mirror:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            break;
        case TextureAddressMode::Clamp:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case TextureAddressMode::Border:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            break;
        }

        if (state.address_u == TextureAddressMode::Border || state.address_v == TextureAddressMode::Border)
        {
            float borderColor[4];
        #define makeColor(r, g, b, a) \
            borderColor[0] = r;\
            borderColor[1] = g;\
            borderColor[2] = b;\
            borderColor[3] = a;

            switch (state.border_color)
            {
            case BorderColor::Black:
                makeColor(0.0f, 0.0f, 0.0f, 0.0f);
                break;
            case BorderColor::OpaqueBlack:
                makeColor(0.0f, 0.0f, 0.0f, 1.0f);
                break;
            case BorderColor::TransparentWhite:
                makeColor(1.0f, 1.0f, 1.0f, 0.0f);
                break;
            case BorderColor::White:
                makeColor(1.0f, 1.0f, 1.0f, 1.0f);
                break;
            }

        #undef makeColor
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
        }

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, state.mip_lod_bias);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_NEVER);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, state.min_lod);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, state.max_lod);
    }
    bool Renderer_OpenGL::uploadVertexIndexBufferFromDrawList()
    {
        // upload data
        if ((_draw_list.vertex.capacity - _vi_buffer[_vi_buffer_index].vertex_offset) < _draw_list.vertex.size
            || (_draw_list.index.capacity - _vi_buffer[_vi_buffer_index].index_offset) < _draw_list.index.size)
        {
            // next buffer
            _vi_buffer_index = (_vi_buffer_index + 1) % _vi_buffer_count;
            _vi_buffer[_vi_buffer_index].vertex_offset = 0;
            _vi_buffer[_vi_buffer_index].index_offset = 0;
            // discard and copy
            if (!uploadVertexIndexBuffer(true))
            {
                clearDrawList();
                return false;
            }
            // bind buffer
            if (_vi_buffer_count > 1)
            {
                setVertexIndexBuffer(); // need to switch v/i buffers
            }
        }
        else
        {
            // copy no overwrite
            if (!uploadVertexIndexBuffer(false))
            {
                clearDrawList();
                return false;
            }
        }
        return true;
    }
    void Renderer_OpenGL::bindTextureSamplerState(ITexture2D* texture)
    {
        std::optional<Graphics::SamplerState> sampler_from_texture = texture ? texture->getSamplerState() : std::optional<Graphics::SamplerState>();
        Graphics::SamplerState sampler = sampler_from_texture.value_or(_sampler_state[IDX(_state_set.sampler_state)]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D_OpenGL*>(texture)->GetResource());
        setSamplerState(sampler, 0);
        // glBindSampler(0, static_cast<SamplerState_OpenGL*>(sampler)->GetState());
    }
    void Renderer_OpenGL::bindTextureAlphaType(ITexture2D* texture)
    {
        IRenderer::TextureAlphaType const state = (texture ? texture->isPremultipliedAlpha() : false)
            ? IRenderer::TextureAlphaType::PremulAlpha
            : IRenderer::TextureAlphaType::Normal;
        if (_state_dirty || _state_set.texture_alpha_type != state)
        {
            _state_set.texture_alpha_type = state;
        }
        //GLuint subroutines[2] = { (GLuint)(IDX(_state_set.vertex_color_blend_state) * 2 + IDX(state)), (GLuint)(IDX(_state_set.fog_state) + 8) };
        //GLuint subroutines[2] = { (GLuint)(IDX(_state_set.fog_state) + 8), (GLuint)(IDX(_state_set.vertex_color_blend_state) * 2 + IDX(state)) };

        // GLuint subroutines[2];
        // subroutines[idx_blend_uniform] = (GLuint)(IDX(_state_set.vertex_color_blend_state) * 2 + IDX(state));
        // subroutines[idx_fog_uniform] = (GLuint)(IDX(_state_set.fog_state) + 8);

        // glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);
    }
    bool Renderer_OpenGL::batchFlush(bool discard)
    {
        ZoneScoped;
        if (!discard)
        {
            TracyGpuZone("BatchFlush");
            // upload data
            if (!uploadVertexIndexBufferFromDrawList()) return false;
            // draw
            if (_draw_list.command.size > 0)
            {
                VertexIndexBuffer& vi_ = _vi_buffer[_vi_buffer_index];
                for (size_t j_ = 0; j_ < _draw_list.command.size; j_ += 1)
                {
                    DrawCommand& cmd_ = _draw_list.command.data[j_];
                    if (cmd_.vertex_count > 0 && cmd_.index_count > 0)
                    {
                        bindTextureAlphaType(cmd_.texture.get());
                        bindTextureSamplerState(cmd_.texture.get());
                        glUseProgram(_programs[IDX(_state_set.vertex_color_blend_state)][IDX(_state_set.fog_state)][IDX(_state_set.texture_alpha_type)]);
                        // glDrawElementsBaseVertex(GL_TRIANGLES, cmd_.index_count, GL_UNSIGNED_SHORT, 0, vi_.index_offset);
                        glDrawElementsBaseVertex(GL_TRIANGLES, cmd_.index_count, GL_UNSIGNED_SHORT, (void*)(vi_.index_offset * sizeof(DrawIndex)), vi_.vertex_offset);
                    }
                    vi_.vertex_offset += cmd_.vertex_count;
                    vi_.index_offset += cmd_.index_count;
                }
            }
        }
        // clear
        clearDrawList();
        setTexture(_state_texture.get());
        return true;
    }

    bool Renderer_OpenGL::createResources()
    {
        spdlog::info("[core] Starting Renderer Initialization");
        
        if (!createBuffers())
        {
            spdlog::error("[core] Unable to create buffers");
            return false;
        }
        if (!createStates())
        {
            spdlog::error("[core] Unable to create render states");
            return false;
        }
        if (!createShaders())
        {
            spdlog::error("[core] Unable to create shaders");
            return false;
        }

        spdlog::info("[core] Renderer Initialized");

        return true;
    }
    void Renderer_OpenGL::onDeviceCreate()
    {
        createResources();
    }
    void Renderer_OpenGL::onDeviceDestroy()
    {
        batchFlush(true);

        _state_texture.reset();

        glDeleteBuffers(1, &_fx_vbuffer);
        glDeleteBuffers(1, &_fx_ibuffer);
        for (auto& v : _vi_buffer)
        {
            glDeleteBuffers(1, &v.vertex_buffer);
            glDeleteBuffers(1, &v.index_buffer);
            v.vertex_offset = 0;
            v.index_offset = 0;
        }
        _vi_buffer_index = 0;

        glDeleteBuffers(1, &_vp_matrix_buffer);
        glDeleteBuffers(1, &_world_matrix_buffer);
        glDeleteBuffers(1, &_camera_pos_buffer);
        glDeleteBuffers(1, &_fog_data_buffer);
        glDeleteBuffers(1, &_user_float_buffer);


        for (int i = 0; i < IDX(VertexColorBlendState::MAX_COUNT); i++)
        for (int j = 0; j < IDX(FogState::MAX_COUNT); j++)
        for (int k = 0; k < IDX(TextureAlphaType::MAX_COUNT); k++)
            glDeleteProgram(_programs[i][j][k]);

        spdlog::info("[core] Renderer Destroyed");
    }

    bool Renderer_OpenGL::beginBatch()
    {
        setVertexIndexBuffer();

        // GLuint bufs[4] = {
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _vp_matrix_buffer);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, _world_matrix_buffer);
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, _camera_pos_buffer);
        glBindBufferBase(GL_UNIFORM_BUFFER, 3, _fog_data_buffer);
        // };
        // glBindBuffersBase(GL_UNIFORM_BUFFER, 0, 4, bufs);

        initState();

        _batch_scope = true;
        return true;
    }
    bool Renderer_OpenGL::endBatch()
    {
        _batch_scope = false;
        if (!batchFlush())
            return false;
        _state_texture.reset();
        return true;
    }
    bool Renderer_OpenGL::flush()
    {
        return batchFlush();
    }

    void Renderer_OpenGL::clearRenderTarget(Color4B const& color)
    {
        batchFlush();
        glClearColor(
            (float)color.r / 255.0f,
            (float)color.g / 255.0f,
            (float)color.b / 255.0f,
            (float)color.a / 255.0f
        );
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void Renderer_OpenGL::clearDepthBuffer(float zvalue)
    {
        batchFlush();
        glClearDepth(zvalue);

        glClear(GL_DEPTH_BUFFER_BIT);
    }
    void Renderer_OpenGL::setRenderAttachment(IRenderTarget* p_rt)
    {
        batchFlush();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<RenderTarget_OpenGL*>(p_rt)->GetFramebuffer());
    }

    void Renderer_OpenGL::setOrtho(BoxF const& box)
    {
        if (_state_dirty || !_camera_state_set.isEqual(box))
        {
            batchFlush();
            _camera_state_set.ortho = box;
            _camera_state_set.is_3D = false;
            glm::mat4 m4 = glm::orthoLH_ZO(box.a.x, box.b.x, box.a.y, box.b.y, box.a.z, box.b.z);
            // spdlog::info("[core] setOrtho: {} {} {} {}", box.a.x, box.b.x, box.b.y, box.a.y);
            /* upload vp matrix */ {
                glBindBuffer(GL_UNIFORM_BUFFER, _vp_matrix_buffer);
                glBufferData(GL_UNIFORM_BUFFER, sizeof(m4), &m4, GL_STATIC_DRAW);
            }
        }
    }
    void Renderer_OpenGL::setPerspective(Vector3F const& eye, Vector3F const& lookat, Vector3F const& headup, float fov, float aspect, float znear, float zfar)
    {
        if (_state_dirty || !_camera_state_set.isEqual(eye, lookat, headup, fov, aspect, znear, zfar))
        {
            batchFlush();
            _camera_state_set.eye = eye;
            _camera_state_set.lookat = lookat;
            _camera_state_set.headup = headup;
            _camera_state_set.fov = fov;
            _camera_state_set.aspect = aspect;
            _camera_state_set.znear = znear;
            _camera_state_set.zfar = zfar;
            _camera_state_set.is_3D = true;
            glm::vec3 const eyef3(eye.x, -eye.y, eye.z);
            glm::vec3 const lookatf3(lookat.x, -lookat.y, lookat.z);
            glm::vec3 const headupf3(headup.x, headup.y, headup.z);
            glm::mat4 m4 = glm::scale(glm::perspectiveLH_ZO(fov, aspect, znear, zfar) * glm::lookAtLH(eyef3, lookatf3, headupf3), glm::vec3(1, -1, 1));
            float const camera_pos[8] = {
                eye.x, eye.y, eye.z, 0.0f,
                lookatf3.x - eyef3.x, lookatf3.y - eyef3.y, lookatf3.z - eyef3.z, 0.0f,
            };
            // auto* ctx = m_device->GetD3D11DeviceContext();
            // assert(ctx);
            /* upload vp matrix */ {
                glBindBuffer(GL_UNIFORM_BUFFER, _vp_matrix_buffer);
                glBufferData(GL_UNIFORM_BUFFER, sizeof(m4), &m4, GL_STATIC_DRAW);
            }
            /* upload camera pos */ {
                glBindBuffer(GL_UNIFORM_BUFFER, _camera_pos_buffer);
                glBufferData(GL_UNIFORM_BUFFER, sizeof(camera_pos), &camera_pos, GL_STATIC_DRAW);
            }
        }
    }

    void Renderer_OpenGL::setViewport(BoxF const& box)
    {
        if (_state_dirty || _state_set.viewport != box)
        {
            batchFlush();
            _state_set.viewport = box;
            glViewport((GLint)box.a.x, (GLint)box.a.y, (GLint)box.b.x - (GLint)box.a.x, (GLint)box.b.y - (GLint)box.a.y);
        }
    }
    void Renderer_OpenGL::setScissorRect(RectF const& rect)
    {
        if (_state_dirty || _state_set.scissor_rect != rect)
        {
            batchFlush();
            _state_set.scissor_rect = rect;
            glScissor((GLint)rect.a.x, (GLint)rect.a.y, (GLint)rect.width(), (GLint)rect.height());
        }
    }
    void Renderer_OpenGL::setViewportAndScissorRect()
    {
        _state_dirty = true;
        setViewport(_state_set.viewport);
        setScissorRect(_state_set.scissor_rect);
        _state_dirty = false;
    }

    void Renderer_OpenGL::setVertexColorBlendState(VertexColorBlendState state)
    {
        if (_state_dirty || _state_set.vertex_color_blend_state != state)
        {
            batchFlush();
            _state_set.vertex_color_blend_state = state;
            //GLuint subroutines[2] = { (GLuint)(IDX(state) * 2 + IDX(_state_set.texture_alpha_type)), (GLuint)(IDX(_state_set.fog_state) + 8) };
            //GLuint subroutines[2] = { (GLuint)(IDX(_state_set.fog_state) + 8), (GLuint)(IDX(state) * 2 + IDX(_state_set.texture_alpha_type)) };
            // GLuint subroutines[2];
            // subroutines[idx_blend_uniform] = (GLuint)(IDX(state) * 2 + IDX(_state_set.texture_alpha_type));
            // subroutines[idx_fog_uniform] = (GLuint)(IDX(_state_set.fog_state) + 8);

            // glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);
        }
    }
    void Renderer_OpenGL::setFogState(FogState state, Color4B const& color, float density_or_znear, float zfar)
    {
        if (_state_dirty || _state_set.fog_state != state || _state_set.fog_color != color || _state_set.fog_near_or_density != density_or_znear || _state_set.fog_far != zfar)
        {
            batchFlush();
            _state_set.fog_state = state;
            _state_set.fog_color = color;
            _state_set.fog_near_or_density = density_or_znear;
            _state_set.fog_far = zfar;
            float const fog_color_and_range[8] = {
                (float)color.r / 255.0f,
                (float)color.g / 255.0f,
                (float)color.b / 255.0f,
                (float)color.a / 255.0f,
                density_or_znear, zfar, 0.0f, zfar - density_or_znear,
            };
            /* upload */ {
                glBindBuffer(GL_UNIFORM_BUFFER, _fog_data_buffer);
                glBufferData(GL_UNIFORM_BUFFER, sizeof(fog_color_and_range), &fog_color_and_range, GL_STATIC_DRAW);
            }

            //GLuint subroutines[2] = { (GLuint)(IDX(_state_set.vertex_color_blend_state) * 2 + IDX(_state_set.texture_alpha_type)), (GLuint)(IDX(state) + 8) };
            //GLuint subroutines[2] = { (GLuint)(IDX(state) + 8), (GLuint)(IDX(_state_set.vertex_color_blend_state) * 2 + IDX(_state_set.texture_alpha_type)) };

            // GLuint subroutines[2];
            // subroutines[idx_blend_uniform] = (GLuint)(IDX(_state_set.vertex_color_blend_state) * 2 + IDX(_state_set.texture_alpha_type));
            // subroutines[idx_fog_uniform] = (GLuint)(IDX(state) + 8);

            // glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);
        }
    }
    void Renderer_OpenGL::setDepthState(DepthState state)
    {
        if (_state_dirty || _state_set.depth_state != state)
        {
            batchFlush();
            _state_set.depth_state = state;
            if (state == DepthState::Enable)
                glEnable(GL_DEPTH_TEST);
            else
                glDisable(GL_DEPTH_TEST);
        }
    }
    void Renderer_OpenGL::setBlendState(BlendState state)
    {
        if (_state_dirty || _state_set.blend_state != state)
        {
            batchFlush();
            _state_set.blend_state = state;
            switch (state) {
            default: assert(false); break;
            case BlendState::Disable:
                glDisable(GL_BLEND);
                break;
            case BlendState::Alpha:
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                break;
            case BlendState::One:
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                break;
            case BlendState::Min:
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
                glBlendEquationSeparate(GL_MIN, GL_MIN);
                break;
            case BlendState::Max:
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
                glBlendEquationSeparate(GL_MAX, GL_MAX);
                break;
            case BlendState::Mul:
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                break;
            case BlendState::Screen:
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                break;
            case BlendState::Add:
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                break;
            case BlendState::Sub:
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_ADD);
                break;
            case BlendState::RevSub:
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
                break;
            case BlendState::Inv:
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE);
                glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
                break;
            }
        }
    }

    inline bool is_same(Texture2D_OpenGL* a, ITexture2D* b)
    {
        if (a && b)
            return a->GetView() == static_cast<Texture2D_OpenGL*>(b)->GetView();
        else if (!a && !b)
            return true;
        else
            return false;
    }
    inline bool is_same(ScopeObject<Texture2D_OpenGL>& a, ITexture2D* b)
    {
        return is_same(*a, b);
    }

    void Renderer_OpenGL::setTexture(ITexture2D* texture)
    {
        if (!texture) return;
        if (_draw_list.command.size > 0 && is_same(_draw_list.command.data[_draw_list.command.size - 1].texture, texture))
        {
            // Can merge
        }
        else
        {
            // New render command
            if ((_draw_list.command.capacity - _draw_list.command.size) < 1)
            {
                batchFlush(); // Free up space
            }
            _draw_list.command.size += 1;
            DrawCommand& cmd_ = _draw_list.command.data[_draw_list.command.size - 1];
            cmd_.texture = static_cast<Texture2D_OpenGL*>(texture);
            cmd_.vertex_count = 0;
            cmd_.index_count = 0;
        }
        // Update texture of current state
        if (!is_same(_state_texture, texture))
        {
            _state_texture = static_cast<Texture2D_OpenGL*>(texture);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D_OpenGL*>(texture)->GetResource());
    }

    bool Renderer_OpenGL::drawTriangle(DrawVertex const& v1, DrawVertex const& v2, DrawVertex const& v3)
    {
        if ((_draw_list.vertex.capacity - _draw_list.vertex.size) < 3 || (_draw_list.index.capacity - _draw_list.index.size) < 3)
        {
            if (!batchFlush()) return false;
        }
        assert(_draw_list.command.size > 0);
        DrawCommand& cmd_ = _draw_list.command.data[_draw_list.command.size - 1];
        IRenderer::DrawVertex* vbuf_ = _draw_list.vertex.data + _draw_list.vertex.size;
        vbuf_[0] = v1;
        vbuf_[1] = v2;
        vbuf_[2] = v3;
        _draw_list.vertex.size += 3;
        DrawIndex* ibuf_ = _draw_list.index.data + _draw_list.index.size;
        ibuf_[0] = cmd_.vertex_count;
        ibuf_[1] = cmd_.vertex_count + 1;
        ibuf_[2] = cmd_.vertex_count + 2;
        _draw_list.index.size += 3;
        cmd_.vertex_count += 3;
        cmd_.index_count += 3;
        return true;
    }
    bool Renderer_OpenGL::drawTriangle(IRenderer::DrawVertex const* pvert)
    {
        return drawTriangle(pvert[0], pvert[1], pvert[2]);
    }
    bool Renderer_OpenGL::drawQuad(IRenderer::DrawVertex const& v1, IRenderer::DrawVertex const& v2, IRenderer::DrawVertex const& v3, IRenderer::DrawVertex const& v4)
    {
        if ((_draw_list.vertex.capacity - _draw_list.vertex.size) < 4 || (_draw_list.index.capacity - _draw_list.index.size) < 6)
        {
            if (!batchFlush()) return false;
        }
        assert(_draw_list.command.size > 0);
        DrawCommand& cmd_ = _draw_list.command.data[_draw_list.command.size - 1];
        IRenderer::DrawVertex* vbuf_ = _draw_list.vertex.data + _draw_list.vertex.size;
        vbuf_[0] = v1;
        vbuf_[1] = v2;
        vbuf_[2] = v3;
        vbuf_[3] = v4;
        _draw_list.vertex.size += 4;
        DrawIndex* ibuf_ = _draw_list.index.data + _draw_list.index.size;
        ibuf_[0] = cmd_.vertex_count;
        ibuf_[1] = cmd_.vertex_count + 1;
        ibuf_[2] = cmd_.vertex_count + 2;
        ibuf_[3] = cmd_.vertex_count;
        ibuf_[4] = cmd_.vertex_count + 2;
        ibuf_[5] = cmd_.vertex_count + 3;
        _draw_list.index.size += 6;
        cmd_.vertex_count += 4;
        cmd_.index_count += 6;
        return true;
    }
    bool Renderer_OpenGL::drawQuad(IRenderer::DrawVertex const* pvert)
    {
        return drawQuad(pvert[0], pvert[1], pvert[2], pvert[3]);
    }
    bool Renderer_OpenGL::drawRaw(IRenderer::DrawVertex const* pvert, uint16_t nvert, DrawIndex const* pidx, uint16_t nidx)
    {
        if (nvert > _draw_list.vertex.capacity || nidx > _draw_list.index.capacity)
        {
            assert(false); return false;
        }

        if ((_draw_list.vertex.capacity - _draw_list.vertex.size) < nvert || (_draw_list.index.capacity - _draw_list.index.size) < nidx)
        {
            if (!batchFlush()) return false;
        }

        assert(_draw_list.command.size > 0);
        DrawCommand& cmd_ = _draw_list.command.data[_draw_list.command.size - 1];

        IRenderer::DrawVertex* vbuf_ = _draw_list.vertex.data + _draw_list.vertex.size;
        std::memcpy(vbuf_, pvert, nvert * sizeof(IRenderer::DrawVertex));
        _draw_list.vertex.size += nvert;

        DrawIndex* ibuf_ = _draw_list.index.data + _draw_list.index.size;
        for (size_t idx_ = 0; idx_ < nidx; idx_ += 1)
        {
            ibuf_[idx_] = cmd_.vertex_count + pidx[idx_];
        }
        _draw_list.index.size += nidx;

        cmd_.vertex_count += nvert;
        cmd_.index_count += nidx;

        return true;
    }
    bool Renderer_OpenGL::drawRequest(uint16_t nvert, uint16_t nidx, IRenderer::DrawVertex** ppvert, DrawIndex** ppidx, uint16_t* idxoffset)
    {
        if (nvert > _draw_list.vertex.capacity || nidx > _draw_list.index.capacity)
        {
            assert(false); return false;
        }

        if ((_draw_list.vertex.capacity - _draw_list.vertex.size) < nvert || (_draw_list.index.capacity - _draw_list.index.size) < nidx)
        {
            if (!batchFlush()) return false;
        }

        // assert(_draw_list.command.size > 0);
        DrawCommand& cmd_ = _draw_list.command.data[_draw_list.command.size - 1];

        *ppvert = _draw_list.vertex.data + _draw_list.vertex.size;
        _draw_list.vertex.size += nvert;

        *ppidx = _draw_list.index.data + _draw_list.index.size;
        _draw_list.index.size += nidx;

        *idxoffset = cmd_.vertex_count; // Output vertex offset
        cmd_.vertex_count += nvert;
        cmd_.index_count += nidx;

        return true;
    }

    bool Renderer_OpenGL::createPostEffectShader(StringView path, IPostEffectShader** pp_effect)
    {
        try
        {
            *pp_effect = new PostEffectShader_OpenGL(m_device.get(), path, true);
            return true;
        }
        catch (...)
        {
            *pp_effect = nullptr;
            return false;
        }
    }
    bool Renderer_OpenGL::drawPostEffect(
        IPostEffectShader* p_effect,
        IRenderer::BlendState blend,
        ITexture2D* p_tex, SamplerState rtsv,
        Vector4F const* cv, size_t cv_n,
        ITexture2D* const* p_tex_arr, SamplerState const* sv, size_t tv_sv_n)
    {
        assert(p_effect);
        assert((cv_n == 0) || (cv_n > 0 && cv));
        assert((tv_sv_n == 0) || (tv_sv_n > 0 && p_tex_arr && sv));

        if (!endBatch()) return false;
        
        // PREPARE

        GLint w, h;
        /* get current rendertarget size */ {
            GLint rt = 0;
            glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &rt);
            if (rt == 0)
            {
                spdlog::error("[core] glGetFrameBufferAttachmentParameteriv failed");
                return false;
            }

            glBindTexture(GL_TEXTURE_2D, rt);

            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
        }
        
        // if (w < 1 || h < 1)
        // {
        // 	spdlog::warn("[core] LuaSTG::Core::Renderer::postEffect exiting early, as no render target is bound!");
        // 	return false;
        // }

        glViewport(0, 0, w, h);
        glScissor(0, 0, w, h);

        glUseProgram(static_cast<PostEffectShader_OpenGL*>(p_effect)->GetShader());

        /* upload vertex data */ {
            DrawVertex const vertex_data[4] = {
                DrawVertex(0.f,      0.f,      0.0f, 0.0f),
                DrawVertex((float)w, 0.f,      1.0f, 0.0f),
                DrawVertex((float)w, (float)h, 1.0f, 1.0f),
                DrawVertex(0.f,      (float)h, 0.0f, 1.0f),
            };
            glBindBuffer(GL_ARRAY_BUFFER, _fx_vbuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), &vertex_data, GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _fx_ibuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (const GLvoid *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (const GLvoid *)offsetof(DrawVertex, u));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DrawVertex), (const GLvoid *)offsetof(DrawVertex, color));
        glEnableVertexAttribArray(2);

        /* upload vp matrix */ {
            glm::mat4 mat4 = glm::orthoLH_ZO(0.0f, (float)w, 0.0f, (float)h, 0.0f, 1.0f);
            glBindBuffer(GL_UNIFORM_BUFFER, _vp_matrix_buffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4), &mat4, GL_STATIC_DRAW);
        }

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _vp_matrix_buffer);

        /* upload built-in value */ if (cv_n > 0) {
            glBindBuffer(GL_UNIFORM_BUFFER, _user_float_buffer);
            glBufferData(GL_UNIFORM_BUFFER, std::min<GLuint>((GLuint)cv_n, 8) * sizeof(Vector4F), cv, GL_STATIC_DRAW);
        }
        /* upload built-in value */ {
            float ps_cbdata[8] = {
                (float)w, (float)h, 0.0f, 0.0f,
                _state_set.viewport.a.x, _state_set.viewport.a.y, _state_set.viewport.b.x, _state_set.viewport.b.y,
            };
            glBindBuffer(GL_UNIFORM_BUFFER, _fog_data_buffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(ps_cbdata), &ps_cbdata, GL_STATIC_DRAW);
        }
        // GLuint frag_bufs[2] = { _fog_data_buffer, _user_float_buffer };
        // glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 2, frag_bufs);
        static_cast<PostEffectShader_OpenGL*>(p_effect)->bind(_fog_data_buffer, _user_float_buffer);

        for (int stage = 0; stage < std::min<int>((int)tv_sv_n, 4); stage++)
        {
            glActiveTexture(GL_TEXTURE1 + stage);
            glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D_OpenGL*>(p_tex_arr[stage])->GetResource());
            setSamplerState(sv[stage], stage);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D_OpenGL*>(p_tex)->GetResource());
        setSamplerState(rtsv, 4);

        glDisable(GL_DEPTH_TEST);
        switch (blend) {
        default: assert(false); break;
        case BlendState::Disable:
            glDisable(GL_BLEND);
            break;
        case BlendState::Alpha:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        case BlendState::One:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        case BlendState::Min:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
            glBlendEquationSeparate(GL_MIN, GL_MIN);
            break;
        case BlendState::Max:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
            glBlendEquationSeparate(GL_MAX, GL_MAX);
            break;
        case BlendState::Mul:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        case BlendState::Screen:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        case BlendState::Add:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        case BlendState::Sub:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_ADD);
            break;
        case BlendState::RevSub:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
            break;
        case BlendState::Inv:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE);
            glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
            break;
        }

        // DRAW

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

        return beginBatch();
    }
    bool Renderer_OpenGL::drawPostEffect(IPostEffectShader* p_effect, BlendState blend)
    {
        assert(p_effect);

        if (!endBatch()) return false;

        // PREPARE

        GLint w, h;
        /* get current rendertarget size */ {
            GLint rt = 0;
            glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &rt);
            if (rt == 0)
            {
                spdlog::error("[core] glGetFrameBufferAttachmentParameteriv failed");
                return false;
            }

            glBindTexture(GL_TEXTURE_2D, rt);

            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
        }
        
        // if (w < 1 || h < 1)
        // {
        // 	spdlog::warn("[core] LuaSTG::Core::Renderer::postEffect exiting early, as no render target is bound!");
        // 	return false;
        // }

        glViewport(0, 0, w, h);
        glScissor(0, 0, w, h);

        glUseProgram(static_cast<PostEffectShader_OpenGL*>(p_effect)->GetShader());

        /* upload vertex data */ {
            DrawVertex const vertex_data[4] = {
                DrawVertex(0.f,      0.f,      0.0f, 0.0f),
                DrawVertex((float)w, 0.f,      1.0f, 0.0f),
                DrawVertex((float)w, (float)h, 1.0f, 1.0f),
                DrawVertex(0.f,      (float)h, 0.0f, 1.0f),
            };
            glBindBuffer(GL_ARRAY_BUFFER, _fx_vbuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), &vertex_data, GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _fx_ibuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (const GLvoid *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (const GLvoid *)offsetof(DrawVertex, u));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DrawVertex), (const GLvoid *)offsetof(DrawVertex, color));
        glEnableVertexAttribArray(2);

        if (!p_effect->apply(this))
        {
            spdlog::error("[core] Cannot apply PostEffectShader variables");
            return false;
        }
        
        /* upload vp matrix */ {
            glm::mat4 mat4 = glm::orthoLH_ZO(0.0f, (float)w, 0.0f, (float)h, 0.0f, 1.0f);
            glBindBuffer(GL_UNIFORM_BUFFER, _vp_matrix_buffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4), &mat4, GL_STATIC_DRAW);
        }

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _vp_matrix_buffer);

        /* upload built-in value */ {
            float ps_cbdata[8] = {
                (float)w, (float)h, 0.0f, 0.0f,
                _state_set.viewport.a.x, _state_set.viewport.a.y, _state_set.viewport.b.x, _state_set.viewport.b.y,
            };
            glBindBuffer(GL_UNIFORM_BUFFER, _fog_data_buffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(ps_cbdata), &ps_cbdata, GL_STATIC_DRAW);
        }
        glBindBufferBase(GL_UNIFORM_BUFFER, 3, _fog_data_buffer);

        glDisable(GL_DEPTH_TEST);
        switch (blend) {
        default: assert(false); break;
        case BlendState::Disable:
            glDisable(GL_BLEND);
            break;
        case BlendState::Alpha:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        case BlendState::One:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        case BlendState::Min:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
            glBlendEquationSeparate(GL_MIN, GL_MIN);
            break;
        case BlendState::Max:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
            glBlendEquationSeparate(GL_MAX, GL_MAX);
            break;
        case BlendState::Mul:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        case BlendState::Screen:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        case BlendState::Add:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        case BlendState::Sub:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_ADD);
            break;
        case BlendState::RevSub:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
            break;
        case BlendState::Inv:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE);
            glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
            break;
        }

        // DRAW

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

        return beginBatch();
    }

    bool Renderer_OpenGL::createModel(StringView path, IModel** pp_model)
    {
        if (!m_model_shared)
        {
            spdlog::info("[core] Creating ModelSharedComponent");
            try
            {
                *(~m_model_shared) = new ModelSharedComponent_OpenGL(m_device.get());
                spdlog::info("[core] ModelSharedComponent created successfully");
            }
            catch (...)
            {
                spdlog::error("[core] Create ModelSharedComponent failed");
                return false;
            }
        }

        try
        {
            *pp_model = new Model_OpenGL(m_device.get(), m_model_shared.get(), path);
            return true;
        }
        catch (const std::exception&)
        {
            *pp_model = nullptr;
            spdlog::error("[core] LuaSTG::Core::Renderer::createModel failed");
            return false;
        }
    }
    bool Renderer_OpenGL::drawModel(IModel* p_model)
    {
        if (!p_model)
        {
            assert(false);
            return false;
        }

        if (!endBatch())
        {
            return false;
        }

        static_cast<Model_OpenGL*>(p_model)->draw(_state_set.fog_state);

        if (!beginBatch())
        {
            return false;
        }

        return true;
    }

    Graphics::SamplerState Renderer_OpenGL::getKnownSamplerState(SamplerState state)
    {
        return _sampler_state[IDX(state)];
    }

    Renderer_OpenGL::Renderer_OpenGL(Device_OpenGL* p_device)
        : m_device(p_device)
    {
        if (!createResources())
            throw std::runtime_error("Renderer_OpenGL::Renderer_OpenGL");
        m_device->addEventListener(this);
    }
    Renderer_OpenGL::~Renderer_OpenGL()
    {
        m_device->removeEventListener(this);
    }

    bool Renderer_OpenGL::create(Device_OpenGL* p_device, Renderer_OpenGL** pp_renderer)
    {
        try
        {
            *pp_renderer = new Renderer_OpenGL(p_device);
            return true;
        }
        catch (...)
        {
            *pp_renderer = nullptr;
            return false;
        }
    }

    bool IRenderer::create(IDevice* p_device, IRenderer** pp_renderer)
    {
        try
        {
            *pp_renderer = new Renderer_OpenGL(static_cast<Device_OpenGL*>(p_device));
            return true;
        }
        catch (...)
        {
            *pp_renderer = nullptr;
            return false;
        }
    }
}
