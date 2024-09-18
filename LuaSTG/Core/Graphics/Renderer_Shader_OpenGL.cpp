#include "Core/Graphics/Renderer.hpp"
#include "Core/Graphics/Renderer_OpenGL.hpp"
#include "Core/FileManager.hpp"

#include "Core/Type.hpp"
#include "glad/gl.h"
#include "spdlog/spdlog.h"
#include <cassert>
#include <format>
#include <string>
#include <string_view>
#include <vector>

// Default Fragment Shader
const constexpr GLchar default_fragment[]{R"(
#version 410 core

#define {}
#define {}
#define {}

uniform camera_data
{{
    vec4 camera_pos;
}};
uniform fog_data
{{
    vec4 fog_color;
    vec4 fog_range;
}};
uniform sampler2D sampler0;

const float channel_minimum = 1.0 / 255.0;

layout(location = 0) in vec4 sxy;
layout(location = 1) in vec4 pos;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec4 col;

layout(location = 0) out vec4 col_out;


vec4 vb_zero()
{{
    vec4 color = texture(sampler0, uv);
    color.rgb *= color.a;
    return color;
}}

vec4 vb_zero_pmul()
{{
    return texture(sampler0, uv); // pass through
}}

vec4 vb_one()
{{
    vec4 color = col;
    color.rgb *= color.a;
    return color;
}}

vec4 vb_one_pmul()
{{
    return col; // pass through
}}

vec4 add_common(vec4 color)
{{
    color.rgb += col.rgb;
    color.r = min(color.r, 1.0);
    color.g = min(color.g, 1.0);
    color.b = min(color.b, 1.0);
    color.a *= col.a;
    color.rgb *= color.a;
    return color;
}}

vec4 vb_add()
{{
    vec4 color = texture(sampler0, uv);
    return add_common(color);
}}

vec4 vb_add_pmul()
{{
    vec4 color = texture(sampler0, uv);

    // cancel out alpha multiplication
    if (color.a < channel_minimum)
    {{
        discard; // avoid division by zero
    }}
    color.rgb /= color.a;

    return add_common(color);
}}

vec4 vb_mul()
{{
    vec4 color = texture(sampler0, uv) * col;
    color.rgb *= color.a;
    return color;
}}

vec4 vb_mul_pmul()
{{
    vec4 color = texture(sampler0, uv) * col;
    color.rgb *= col.a; // need to multiply with texture alpha
    return color;
}}


vec4 fog_none(vec4 color)
{{
    return color; // pass through
}}

vec4 fog_linear(vec4 color)
{{
    float dist = distance(camera_pos.xyz, pos.xyz);
    float k = clamp((dist - fog_range.x) / fog_range.w, 0.0, 1.0);
    float k1 = 1.0 - k;
    float alpha = k1 * color.a + k * color.a * fog_color.a;
    float ka = k1 * (k1 + k * fog_color.a);
    float kb = k * alpha;
    color = vec4(ka * color.rgb + kb * fog_color.rgb, alpha);
    return color;
}}

vec4 fog_exp(vec4 color)
{{
    float dist = distance(camera_pos.xyz, pos.xyz);
    float k = clamp(1.0 - exp(-(dist * fog_range.x)), 0.0, 1.0);
    float k1 = 1.0 - k;
    float alpha = k1 * color.a + k * color.a * fog_color.a;
    float ka = k1 * (k1 + k * fog_color.a);
    float kb = k * alpha;
    color = vec4(ka * color.rgb + kb * fog_color.rgb, alpha);
    return color;
}}

vec4 fog_exp2(vec4 color)
{{
    float dist = distance(camera_pos.xyz, pos.xyz);
    float k = clamp(1.0 - exp(-pow(dist * fog_range.x, 2.0)), 0.0, 1.0);
    float k1 = 1.0 - k;
    float alpha = k1 * color.a + k * color.a * fog_color.a;
    float ka = k1 * (k1 + k * fog_color.a);
    float kb = k * alpha;
    color = vec4(ka * color.rgb + kb * fog_color.rgb, alpha);
    return color;
}}

void main()
{{
#if defined(PREMUL_ALPHA)
    #if defined(VERTEX_ADD)
        col_out = vb_add_pmul();
    #elif defined(VERTEX_ONE)
        col_out = vb_one_pmul();
    #elif defined(VERTEX_ZERO)
        col_out = vb_zero_pmul();
    #else // VERTEX_MUL
        col_out = vb_mul_pmul();
    #endif
#else
    #if defined(VERTEX_ADD)
        col_out = vb_add();
    #elif defined(VERTEX_ONE)
        col_out = vb_one();
    #elif defined(VERTEX_ZERO)
        col_out = vb_zero();
    #else // VERTEX_MUL
        col_out = vb_mul();
    #endif
#endif
#if defined(FOG_LINEAR)
    col_out = fog_linear(col_out);
#elif defined(FOG_EXP)
    col_out = fog_exp(col_out);
#elif defined(FOG_EXP2)
    col_out = fog_exp2(col_out);
#endif
}}
)"};

const constexpr std::string_view dfrag_sv{default_fragment};

// Default Vertex Shader
const constexpr GLchar default_vertex[]{R"(
#version 410 core

uniform view_proj_buffer
{
    mat4 view_proj;
};
#if defined(WORLD_MATRIX)
uniform world_buffer
{
    mat4 world;
};
#endif

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec2 uv_in;
layout(location = 2) in vec4 col_in;

layout(location = 0) out vec4 sxy;
layout(location = 1) out vec4 pos;
layout(location = 2) out vec2 uv;
layout(location = 3) out vec4 col;

void main()
{
    vec4 pos_world = vec4(pos_in, 1.0);
#if defined(WORLD_MATRIX)
    pos_world = world * pos_world;
#endif

    gl_Position = view_proj * pos_world;
    sxy = view_proj * pos_world;
    pos = pos_world;
    uv = uv_in;
    col = col_in;
}
)"};

#define IDX(x) (size_t)static_cast<uint8_t>(x)

namespace Core::Graphics
{
    const constexpr char* vertex_blend_state[IDX(IRenderer::VertexColorBlendState::MAX_COUNT)]{
        "VERTEX_ZERO",
        "VERTEX_ONE",
        "VERTEX_ADD",
        "VERTEX_MUL",
    };
    const constexpr char* fog_state[IDX(IRenderer::FogState::MAX_COUNT)]{
        "FOG_DISABLE",
        "FOG_LINEAR",
        "FOG_EXP",
        "FOG_EXP2",
    };
    const constexpr char* pmul_alpha_state[IDX(IRenderer::TextureAlphaType::MAX_COUNT)]{
        "NO_PREMUL_ALPHA",
        "PREMUL_ALPHA",
    };

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

    bool PostEffectShader_OpenGL::createResources()
    {
        GLuint opengl_frag = 0;
        GLuint opengl_vert = 0;
        if (!opengl_frag)
        {
            if (is_path)
            {
                std::vector<uint8_t> src;
                if (!GFileManager().loadEx(source, src))
                    return false;
                if (!compileFragmentShaderMacro((const GLchar*)src.data(), src.size(), opengl_frag))
                    return false;
                if (!compileVertexShaderMacro(default_vertex, sizeof(default_vertex), opengl_vert))
                    return false;
            }
            else
            {
                if (!compileFragmentShaderMacro((const GLchar*)source.data(), source.size(), opengl_frag))
                    return false;
                if (!compileVertexShaderMacro(default_vertex, sizeof(default_vertex), opengl_vert))
                    return false;
            }
        }

        // Link Program

        opengl_prgm = glCreateProgram();
        glAttachShader(opengl_prgm, opengl_frag);
        glAttachShader(opengl_prgm, opengl_vert);
        glLinkProgram(opengl_prgm);

        GLint result;
        glGetProgramiv(opengl_prgm, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            GLchar log[1024];
            int32_t log_len;
            glGetProgramInfoLog(opengl_prgm, 1024, &log_len, log);
            spdlog::error("[core] Failed to link shader: {}", log);
            glDeleteShader(opengl_frag);
            glDeleteShader(opengl_vert);
            glDeleteProgram(opengl_prgm);
            return false;
        }

        glDeleteShader(opengl_frag);
        glDeleteShader(opengl_vert);

        // Uniform Blocks

        GLint amt_uniform_blocks = 0;
        glGetProgramiv(opengl_prgm, GL_ACTIVE_UNIFORM_BLOCKS, &amt_uniform_blocks);

        std::vector<GLchar> name_buffer(128);
        int block_offs = 1;

        for (int block = 0; block < amt_uniform_blocks; block++)
        {
            GLint binding, datasize, vars, namelen;
            glUniformBlockBinding(opengl_prgm, block, block + block_offs);
            glGetActiveUniformBlockiv(opengl_prgm, block, GL_UNIFORM_BLOCK_BINDING, &binding);
            glGetActiveUniformBlockiv(opengl_prgm, block, GL_UNIFORM_BLOCK_DATA_SIZE, &datasize);
            glGetActiveUniformBlockiv(opengl_prgm, block, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &vars);
            glGetActiveUniformBlockiv(opengl_prgm, block, GL_UNIFORM_BLOCK_NAME_LENGTH, &namelen);
            name_buffer.resize(namelen);
            glGetActiveUniformBlockName(opengl_prgm, block, name_buffer.size(), NULL, &name_buffer[0]);
            std::string name(name_buffer.data(), name_buffer.size() - 1);

            LocalConstantBuffer local_buffer;
            local_buffer.index = block;
            local_buffer.binding = block + block_offs;
            local_buffer.buffer.resize(datasize);
            local_buffer.variable.reserve(vars);
            glGenBuffers(1, &local_buffer.opengl_buffer);

            m_buffer_map.emplace(name, std::move(local_buffer));

            if (name == "view_proj_buffer")
            {
                glUniformBlockBinding(opengl_prgm, block, 0);
                m_buffer_map["view_proj_buffer"].binding = 0;
                block_offs -= 1;
            }
        }

        GLint amt_uniforms = 0;
        glGetProgramiv(opengl_prgm, GL_ACTIVE_UNIFORMS, &amt_uniforms);
        glUseProgram(opengl_prgm);

        GLuint tex_idx = 0;

        for (GLuint uniform = 0; uniform < amt_uniforms; uniform++)
        {
            GLint type, offset, arrsize, block_idx, namelen;
            glGetActiveUniformsiv(opengl_prgm, 1, &uniform, GL_UNIFORM_TYPE, &type);
            glGetActiveUniformsiv(opengl_prgm, 1, &uniform, GL_UNIFORM_OFFSET, &offset);
            glGetActiveUniformsiv(opengl_prgm, 1, &uniform, GL_UNIFORM_SIZE, &arrsize);
            glGetActiveUniformsiv(opengl_prgm, 1, &uniform, GL_UNIFORM_BLOCK_INDEX, &block_idx);
            glGetActiveUniformsiv(opengl_prgm, 1, &uniform, GL_UNIFORM_NAME_LENGTH, &namelen);
            name_buffer.resize(namelen);
            glGetActiveUniformName(opengl_prgm, uniform, name_buffer.size(), NULL, &name_buffer[0]);
            std::string name(name_buffer.data(), name_buffer.size() - 1);

            if (type == GL_SAMPLER_2D) // Handle Texture Uniforms
            {
                LocalTexture2D local_texture2d;
                local_texture2d.index = tex_idx;

                glUniform1i(glGetUniformLocation(opengl_prgm, name.c_str()), tex_idx);

                tex_idx++;
                m_texture2d_map.emplace(name, std::move(local_texture2d));
            }
            else // Handle Uniform Buffers
            {
                LocalVariable local_variable;
                local_variable.offset = offset;
                switch (type)
                {
                case GL_FLOAT:
                    local_variable.size = arrsize * sizeof(float);
                    break;
                case GL_FLOAT_VEC2:
                    local_variable.size = arrsize * sizeof(float) * 2;
                    break;
                case GL_FLOAT_VEC3:
                    local_variable.size = arrsize * sizeof(float) * 3;
                    break;
                case GL_FLOAT_VEC4:
                    local_variable.size = arrsize * sizeof(float) * 4;
                    break;

                case GL_FLOAT_MAT2:
                    local_variable.size = arrsize * sizeof(float) * 4;
                    break;
                case GL_FLOAT_MAT3:
                    local_variable.size = arrsize * sizeof(float) * 9;
                    break;
                case GL_FLOAT_MAT4:
                    local_variable.size = arrsize * sizeof(float) * 16;
                    break;

                default:
                    assert(false);
                }

                // data structures are different between DirectX and OpenGL, so take a performance hit
                for (auto& v : m_buffer_map)
                {
                    if (v.second.index == block_idx)
                    {
                        v.second.variable.emplace(name, std::move(local_variable));
                        break;
                    }
                }
            }
        }

        return true;
    }

    bool Renderer_OpenGL::createShaders()
    {
        GLuint vert = 0;
        compileVertexShaderMacro(default_vertex, sizeof(default_vertex), vert);

        for (int i = 0; i < IDX(VertexColorBlendState::MAX_COUNT); i++)
        for (int j = 0; j < IDX(FogState::MAX_COUNT); j++)
        for (int k = 0; k < IDX(TextureAlphaType::MAX_COUNT); k++)
        {
            GLuint frag = 0;
            std::string s_frag = std::format(dfrag_sv, vertex_blend_state[i], fog_state[j], pmul_alpha_state[k]);
            compileFragmentShaderMacro(s_frag.c_str(), s_frag.length(), frag);
            _programs[i][j][k] = glCreateProgram();
            glAttachShader(_programs[i][j][k], vert);
            glAttachShader(_programs[i][j][k], frag);
            glLinkProgram(_programs[i][j][k]);

            glDeleteShader(frag);

            GLuint idx_view_proj_buffer = glGetUniformBlockIndex(_programs[i][j][k], "view_proj_buffer");
            GLuint idx_camera_data = glGetUniformBlockIndex(_programs[i][j][k], "camera_data");
            GLuint idx_fog_data = glGetUniformBlockIndex(_programs[i][j][k], "fog_data");

            glUniformBlockBinding(_programs[i][j][k], idx_view_proj_buffer, 0);
            glUniformBlockBinding(_programs[i][j][k], idx_camera_data, 2);
            glUniformBlockBinding(_programs[i][j][k], idx_fog_data, 3);
        }

        glDeleteShader(vert);

        return true;
    }
}
