#include "Core/Graphics/Model_OpenGL.hpp"
// #include "Platform/RuntimeLoader/Direct3DCompiler.hpp"

// static std::string_view const built_in_shader(R"(
// // pipeline data flow

// struct VS_INPUT
// {
//     float3 pos  : POSITION;
//     float3 norm : NORMAL;
//     float2 uv   : TEXCOORD;
// };
// struct VS_P3F_N3F_C4F_T2F
// {
//     float3 pos  : POSITION;
//     float3 norm : NORMAL;
//     float4 col  : COLOR;
//     float2 uv   : TEXCOORD;
// };
// struct PS_INPUT
// {
//     float4 pos  : SV_POSITION;
//     float4 wpos : POSITION;
//     float4 norm : NORMAL;
//     float2 uv   : TEXCOORD;
// };
// struct PS_S4F_P4F_N4F_C4F_T2F
// {
//     float4 pos  : SV_POSITION;
//     float4 wpos : POSITION;
//     float4 norm : NORMAL;
//     float4 col  : COLOR;
//     float2 uv   : TEXCOORD;
// };
// struct OM_INPUT
// {
//     float4 col : SV_Target;
// };

// // vertex stage

// cbuffer constantBuffer0 : register(b0)
// {
//     float4x4 ProjectionMatrix;
// };
// cbuffer constantBuffer1 : register(b1)
// {
//     float4x4 LocalWorldMatrix;
//     float4x4 NormalLocalWorldMatrix;
// };

// PS_INPUT VS_Main(VS_INPUT input)
// {
//     float4 wpos = mul(LocalWorldMatrix, float4(input.pos, 1.0f));
//     PS_INPUT output;
//     output.pos = mul(ProjectionMatrix, wpos);
//     output.wpos = wpos;
//     output.norm = mul(NormalLocalWorldMatrix, float4(input.norm, 0.0f)); // no move
//     output.uv = input.uv;
//     return output;
// };

// PS_S4F_P4F_N4F_C4F_T2F VS_Main_VertexColor(VS_P3F_N3F_C4F_T2F input)
// {
//     float4 wpos = mul(LocalWorldMatrix, float4(input.pos, 1.0f));
//     PS_S4F_P4F_N4F_C4F_T2F output;
//     output.pos = mul(ProjectionMatrix, wpos);
//     output.wpos = wpos;
//     output.norm = mul(NormalLocalWorldMatrix, float4(input.norm, 0.0f)); // no move
//     output.col = input.col;
//     output.uv = input.uv;
//     return output;
// };

// // pixel stage

// cbuffer cameraInfo : register(b0)
// {
//     float4 CameraPos;
//     float4 CameraLookTo;
// };
// cbuffer fogInfo : register(b1)
// {
//     float4 fog_color;
//     float4 fog_range;
// };
// cbuffer alphaCull : register(b2)
// {
//     float4 base_color;
//     float4 alpha;
// };
// cbuffer lightInfo : register(b3)
// {
//     float4 ambient;
//     float4 sunshine_pos;
//     float4 sunshine_dir;
//     float4 sunshine_color;
// };

// SamplerState sampler0 : register(s0);
// Texture2D texture0 : register(t0);

// float4 ApplySimpleLight(float4 norm, float4 wpos, float4 solid_color)
// {
//     float3 v_normal = normalize(norm.xyz);
//     float light_factor = max(0.0f, dot(v_normal, -sunshine_dir.xyz));
//     //float3 pixel_to_eye = normalize(CameraPos.xyz - wpos.xyz);
//     //float reflact_factor = pow(max(0.0f, dot(reflect(sunshine_dir, v_normal), pixel_to_eye)), 10.0f);
//     return float4((ambient.rgb * ambient.a + sunshine_color.rgb * sunshine_color.a * light_factor) * solid_color.rgb, solid_color.a);
// }

// float4 ApplyFog(float4 wpos, float4 solid_color)
// {
//     #if defined(FOG_ENABLE)
//         // camera_pos.xyz 和 input.pos.xyz 都是在世界坐标系下的坐标，求得的距离也是基于世界坐标系的
//         float dist = distance(CameraPos.xyz, wpos.xyz);
//         #if defined(FOG_EXP)
//             // 指数雾，fog_range.x 是雾密度
//             float k = clamp(1.0f - exp(-(dist * fog_range.x)), 0.0f, 1.0f);
//         #elif defined(FOG_EXP2)
//             // 二次指数雾，fog_range.x 是雾密度
//             float k = clamp(1.0f - exp(-pow(dist * fog_range.x, 2.0f)), 0.0f, 1.0f);
//         #else // FOG_LINEAR
//             // 线性雾，fog_range.x 是雾起始距离，fog_range.y 是雾浓度最大处的距离，fog_range.w 是雾范围（fog_range.y - fog_range.x）
//             float k = clamp((dist - fog_range.x) / fog_range.w, 0.0f, 1.0f);
//         #endif
//         return float4(lerp(solid_color.rgb, fog_color.rgb, k), solid_color.a);
//     #else
//         return solid_color;
//     #endif
// }

// OM_INPUT PS_Main(PS_INPUT input)
// {
//     float4 tex_color = texture0.Sample(sampler0, input.uv);
//     float4 solid_color = base_color * float4(pow(tex_color.rgb, 2.2f), tex_color.a);
//     solid_color = ApplySimpleLight(input.norm, input.wpos, solid_color);
//     solid_color = ApplyFog(input.wpos, solid_color);
//     OM_INPUT output;
//     output.col = pow(solid_color, 1.0f / 2.2f);
//     return output; 
// }

// OM_INPUT PS_Main_AlphaMask(PS_INPUT input)
// {
//     float4 tex_color = texture0.Sample(sampler0, input.uv);
//     float4 solid_color = base_color * float4(pow(tex_color.rgb, 2.2f), tex_color.a);
//     if (solid_color.a < alpha.x)
//     {
//         discard;
//     }
//     solid_color = ApplySimpleLight(input.norm, input.wpos, solid_color);
//     solid_color = ApplyFog(input.wpos, solid_color);
//     OM_INPUT output;
//     output.col = pow(solid_color, 1.0f / 2.2f);
//     return output; 
// }

// OM_INPUT PS_Main_NoBaseTexture(PS_INPUT input)
// {
//     float4 solid_color = base_color;
//     solid_color = ApplySimpleLight(input.norm, input.wpos, solid_color);
//     solid_color = ApplyFog(input.wpos, solid_color);
//     OM_INPUT output;
//     output.col = pow(solid_color, 1.0f / 2.2f);
//     return output; 
// }

// OM_INPUT PS_Main_NoBaseTexture_AlphaMask(PS_INPUT input)
// {
//     float4 solid_color = base_color;
//     if (solid_color.a < alpha.x)
//     {
//         discard;
//     }
//     solid_color = ApplySimpleLight(input.norm, input.wpos, solid_color);
//     solid_color = ApplyFog(input.wpos, solid_color);
//     OM_INPUT output;
//     output.col = pow(solid_color, 1.0f / 2.2f);
//     return output; 
// }

// // 2

// OM_INPUT PS_Main_VertexColor(PS_S4F_P4F_N4F_C4F_T2F input)
// {
//     float4 tex_color = texture0.Sample(sampler0, input.uv);
//     float4 solid_color = base_color * input.col * float4(pow(tex_color.rgb, 2.2f), tex_color.a);
//     solid_color = ApplySimpleLight(input.norm, input.wpos, solid_color);
//     solid_color = ApplyFog(input.wpos, solid_color);
//     OM_INPUT output;
//     output.col = pow(solid_color, 1.0f / 2.2f);
//     return output; 
// }

// OM_INPUT PS_Main_AlphaMask_VertexColor(PS_S4F_P4F_N4F_C4F_T2F input)
// {
//     float4 tex_color = texture0.Sample(sampler0, input.uv);
//     float4 solid_color = base_color * input.col * float4(pow(tex_color.rgb, 2.2f), tex_color.a);
//     if (solid_color.a < alpha.x)
//     {
//         discard;
//     }
//     solid_color = ApplySimpleLight(input.norm, input.wpos, solid_color);
//     solid_color = ApplyFog(input.wpos, solid_color);
//     OM_INPUT output;
//     output.col = pow(solid_color, 1.0f / 2.2f);
//     return output; 
// }

// OM_INPUT PS_Main_NoBaseTexture_VertexColor(PS_S4F_P4F_N4F_C4F_T2F input)
// {
//     float4 solid_color = base_color * input.col;
//     solid_color = ApplySimpleLight(input.norm, input.wpos, solid_color);
//     solid_color = ApplyFog(input.wpos, solid_color);
//     OM_INPUT output;
//     output.col = pow(solid_color, 1.0f / 2.2f);
//     return output; 
// }

// OM_INPUT PS_Main_NoBaseTexture_AlphaMask_VertexColor(PS_S4F_P4F_N4F_C4F_T2F input)
// {
//     float4 solid_color = base_color * input.col;
//     if (solid_color.a < alpha.x)
//     {
//         discard;
//     }
//     solid_color = ApplySimpleLight(input.norm, input.wpos, solid_color);
//     solid_color = ApplyFog(input.wpos, solid_color);
//     OM_INPUT output;
//     output.col = pow(solid_color, 1.0f / 2.2f);
//     return output; 
// }

// )");

// Default Fragment Shader
const GLchar default_fragment[]{R"(
#version 450 core

layout(binding = 2) uniform camera_data
{
    vec4 camera_pos;
    vec4 camera_at;
};
layout(binding = 3) uniform fog_data
{
    vec4 fog_color;
    vec4 fog_range;
};
layout(binding = 4) uniform alpha_cull
{
    vec4 base_color;
    vec4 alpha;
};
layout(binding = 5) uniform light_info
{
    vec4 ambient;
    vec4 sunshine_pos;
    vec4 sunshine_dir;
    vec4 sunshine_color;
};
layout(binding = 0) uniform sampler2D sampler0;

float channel_minimum = 1.0 / 255.0;

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 wpos;
layout(location = 2) in vec4 norm;
layout(location = 3) in vec4 col;
layout(location = 4) in vec2 uv;

layout(location = 0) out vec4 col_out;

subroutine vec4 Fog(vec4);
subroutine vec4 BaseTexture();
subroutine vec4 VertexColor();
subroutine bool AlphaMask();

layout(location = 0) subroutine uniform Fog fog_uniform;
layout(location = 1) subroutine uniform BaseTexture btex_uniform;
layout(location = 2) subroutine uniform VertexColor vc_uniform;
layout(location = 3) subroutine uniform AlphaMask amask_uniform;

layout(index = 0) subroutine(Fog) vec4 fog_none(vec4 color)
{
    return color; // pass through
}

layout(index = 1) subroutine(Fog) vec4 fog_linear(vec4 color)
{
    float dist = distance(camera_pos.xyz, wpos.xyz);
    float k = clamp((dist - fog_range.x) / fog_range.w, 0.0, 1.0);
    float k1 = 1.0 - k;
    float alpha = k1 * color.a + k * color.a * fog_color.a;
    float ka = k1 * (k1 + k * fog_color.a);
    float kb = k * alpha;
    color = float4(ka * color.rgb + kb * fog_color.rgb, alpha);
    return color;
}

layout(index = 2) subroutine(Fog) vec4 fog_exp(vec4 color)
{
    float dist = distance(camera_pos.xyz, wpos.xyz);
    float k = clamp(1.0 - exp(-(dist * fog_range.x)), 0.0, 1.0);
    float k1 = 1.0 - k;
    float alpha = k1 * color.a + k * color.a * fog_color.a;
    float ka = k1 * (k1 + k * fog_color.a);
    float kb = k * alpha;
    color = float4(ka * color.rgb + kb * fog_color.rgb, alpha);
    return color;
}

layout(index = 3) subroutine(Fog) vec4 fog_exp2(vec4 color)
{
    float dist = distance(camera_pos.xyz, wpos.xyz);
    float k = clamp(1.0 - exp(-pow(dist * fog_range.x, 2.0)), 0.0, 1.0);
    float k1 = 1.0 - k;
    float alpha = k1 * color.a + k * color.a * fog_color.a;
    float ka = k1 * (k1 + k * fog_color.a);
    float kb = k * alpha;
    color = float4(ka * color.rgb + kb * fog_color.rgb, alpha);
    return color;
}

layout(index = 4) subroutine(BaseTexture) vec4 no_base_texture()
{
    return vec4(1.0, 1.0, 1.0, 1.0);
}

layout(index = 5) subroutine(BaseTexture) vec4 base_texture()
{
    vec4 color = texture(sampler0, uv);
    return color * vec4(pow(tex_color.rgb, 2.2), tex_color.a);
}

layout(index = 6) subroutine(VertexColor) vec4 no_vertex_color()
{
    return vec4(1.0, 1.0, 1.0, 1.0);
}

layout(index = 7) subroutine(VertexColor) vec4 vertex_color()
{
    return col;
}

layout(index = 8) subroutine(AlphaMask) bool no_alpha_mask()
{
    return false;
}

layout(index = 9) subroutine(AlphaMask) bool alpha_mask()
{
    return solid_color.a < alpha.x;
}

vec4 ApplySimpleLight(vec4 solid_color)
{
    vec3 v_normal = normalize(norm.xyz);
    float light_factor = max(0.0, dot(v_normal, -sunshine_dir.xyz));
    //vec3 pixel_to_eye = normalize(CameraPos.xyz - wpos.xyz);
    //float reflact_factor = pow(max(0.0, dot(reflect(sunshine_dir, v_normal), pixel_to_eye)), 10.0);
    return vec4((ambient.rgb * ambient.a + sunshine_color.rgb * sunshine_color.a * light_factor) * solid_color.rgb, solid_color.a);
}

void main()
{
    vec4 color = base_color * btex_uniform() * vc_uniform();
    if (amask_uniform)
    {
        discard;
    }
    col_out = fog_uniform(ApplySimpleLight(color));
}
)"};

// Default Vertex Shader
const GLchar default_vertex[]{R"(
#version 450 core

layout(binding = 0) uniform view_proj_buffer
{
    mat4 view_proj;
};
layout(binding = 1) uniform world_buffer
{
    mat4 world;
    mat4 norm_world;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec4 col;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec4 pos_out;
layout(location = 1) out vec4 wpos_out;
layout(location = 2) out vec4 norm_out;
layout(location = 3) out vec4 col_out;
layout(location = 4) out vec2 uv_out;

void main()
{
    vec4 wpos = world * vec4(pos, 1.0));
    pos_out = view_proj * wpos;
    wpos_out = wpos;
    norm_out = norm_world * float4(norm, 0.0); // no move
    col_out = col;
    uv_out = uv;
};
)"};

#define IDX(x) (size_t)static_cast<uint8_t>(x)

// static Platform::RuntimeLoader::Direct3DCompiler g_d3dcompiler_loader;

namespace Core::Graphics
{
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

		GLuint frag, vert;
		compileFragmentShaderMacro(default_fragment, sizeof(default_fragment), frag);
		compileVertexShaderMacro(default_vertex, sizeof(default_vertex), vert);
		glAttachShader(shader_program, frag);
		glAttachShader(shader_program, vert);
		glLinkProgram(shader_program);

		glDeleteShader(frag);
		glDeleteShader(vert);

        return true;
    }
}
