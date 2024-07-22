#include "Core/Graphics/Model_OpenGL.hpp"
#include "Core/Graphics/Renderer.hpp"
#include "glad/gl.h"

// Default Fragment Shader
const constexpr GLchar default_fragment[]{R"(
#version 410 core

#define {}
#define {}
#define {}
#define {}

uniform camera_data
{{
    vec4 camera_pos;
    vec4 camera_at;
}};
uniform fog_data
{{
    vec4 fog_color;
    vec4 fog_range;
}};
uniform alpha_cull
{{
    vec4 base_color;
    vec4 alpha;
}};
uniform light_info
{{
    vec4 ambient;
    vec4 sunshine_pos;
    vec4 sunshine_dir;
    vec4 sunshine_color;
}};
uniform sampler2D sampler0;

float channel_minimum = 1.0 / 255.0;

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 wpos;
layout(location = 2) in vec4 norm;
layout(location = 3) in vec4 col;
layout(location = 4) in vec2 uv;

layout(location = 0) out vec4 col_out;

vec4 fog_none(vec4 color)
{{
    return color; // pass through
}}

vec4 fog_linear(vec4 color)
{{
    float dist = distance(camera_pos.xyz, wpos.xyz);
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
    float dist = distance(camera_pos.xyz, wpos.xyz);
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
    float dist = distance(camera_pos.xyz, wpos.xyz);
    float k = clamp(1.0 - exp(-pow(dist * fog_range.x, 2.0)), 0.0, 1.0);
    float k1 = 1.0 - k;
    float alpha = k1 * color.a + k * color.a * fog_color.a;
    float ka = k1 * (k1 + k * fog_color.a);
    float kb = k * alpha;
    color = vec4(ka * color.rgb + kb * fog_color.rgb, alpha);
    return color;
}}

vec4 ApplySimpleLight(vec4 solid_color)
{{
    vec3 v_normal = normalize(norm.xyz);
    float light_factor = max(0.0, dot(v_normal, -sunshine_dir.xyz));
    //vec3 pixel_to_eye = normalize(CameraPos.xyz - wpos.xyz);
    //float reflact_factor = pow(max(0.0, dot(reflect(sunshine_dir, v_normal), pixel_to_eye)), 10.0);
    return vec4((ambient.rgb * ambient.a + sunshine_color.rgb * sunshine_color.a * light_factor) * solid_color.rgb, solid_color.a);
}}

void main()
{{
    vec4 color = base_color;
#if defined(BASE_TEXTURE)
    color *= texture(sampler0, uv);
#endif
#if defined(VERTEX_COLOR)
    color *= col;
#endif
#if defined(ALPHA_MASK)
    if (color.a < alpha.x)
    {{
        discard;
    }}
#endif
    col_out = ApplySimpleLight(color);
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
uniform world_buffer
{
    mat4 world;
    mat4 norm_world;
};

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec2 uv_in;
layout(location = 2) in vec4 col_in;
layout(location = 3) in vec3 norm_in;

layout(location = 0) out vec4 pos;
layout(location = 1) out vec4 wpos;
layout(location = 2) out vec4 norm;
layout(location = 3) out vec4 col;
layout(location = 4) out vec2 uv;

void main()
{
    wpos = world * vec4(pos_in, 1.0);
    pos = view_proj * wpos;
    gl_Position = pos;
    norm = norm_world * vec4(norm_in, 0.0); // no move
    col = col_in;
    uv = uv_in;
}
)"};

#define IDX(x) (size_t)static_cast<uint8_t>(x)

// static Platform::RuntimeLoader::Direct3DCompiler g_d3dcompiler_loader;

namespace Core::Graphics
{
    const constexpr char* fog_state[IDX(IRenderer::FogState::MAX_COUNT)]{
        "FOG_DISABLE",
        "FOG_LINEAR",
        "FOG_EXP",
        "FOG_EXP2",
    };

    const constexpr char* amask[2]{
        "NO_ALPHA_MASK",
        "ALPHA_MASK",
    };

    const constexpr char* btex[2]{
        "NO_BASE_TEXTURE",
        "BASE_TEXTURE",
    };

    const constexpr char* vc[2]{
        "NO_VERTEX_COLOR",
        "VERTEX_COLOR",
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
    bool ModelSharedComponent_OpenGL::createShader()
    {
        // built-in: compile shader

        GLuint vert = 0;
        compileVertexShaderMacro(default_vertex, sizeof(default_vertex), vert);

        GLuint idx_view_proj_buffer;
        GLuint idx_world_buffer;
        GLuint idx_camera_data;
        GLuint idx_fog_data;
        GLuint idx_alpha_cull;
        GLuint idx_light_info;

        for (int i = 0; i < IDX(IRenderer::FogState::MAX_COUNT); i++)
        for (int j = 0; j < 2; j++)
        for (int k = 0; k < 2; k++)
        for (int l = 0; l < 2; l++)
        {
            GLuint frag = 0;
            std::string s_frag = std::format(dfrag_sv, fog_state[i], amask[j], btex[k], vc[l]);
            compileFragmentShaderMacro(s_frag.c_str(), s_frag.length(), frag);

            GLuint prgm = glCreateProgram();
            programs[i][j][k][l] = prgm;
            glAttachShader(prgm, vert);
            glAttachShader(prgm, frag);
            glLinkProgram(prgm);

            glDeleteShader(frag);

            idx_view_proj_buffer = glGetUniformBlockIndex(prgm, "view_proj_buffer");
            idx_world_buffer = glGetUniformBlockIndex(prgm, "world_buffer");
            idx_camera_data = glGetUniformBlockIndex(prgm, "camera_data");
            idx_fog_data = glGetUniformBlockIndex(prgm, "fog_data");
            idx_alpha_cull = glGetUniformBlockIndex(prgm, "alpha_cull");
            idx_light_info = glGetUniformBlockIndex(prgm, "light_info");

            glUniformBlockBinding(prgm, idx_view_proj_buffer, 0);
            glUniformBlockBinding(prgm, idx_world_buffer, 1);
            glUniformBlockBinding(prgm, idx_camera_data, 2);
            glUniformBlockBinding(prgm, idx_fog_data, 3);
            glUniformBlockBinding(prgm, idx_alpha_cull, 4);
            glUniformBlockBinding(prgm, idx_light_info, 5);
        }

        glDeleteShader(vert);

        // idx_fog_uniform = glGetSubroutineUniformLocation(shader_program, GL_FRAGMENT_SHADER, "fog_uniform");
        // idx_btex_uniform = glGetSubroutineUniformLocation(shader_program, GL_FRAGMENT_SHADER, "btex_uniform");
        // idx_vc_uniform = glGetSubroutineUniformLocation(shader_program, GL_FRAGMENT_SHADER, "vc_uniform");
        // idx_amask_uniform = glGetSubroutineUniformLocation(shader_program, GL_FRAGMENT_SHADER, "amask_uniform");

        return true;
    }
}
