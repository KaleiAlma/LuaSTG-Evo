#include "Core/Graphics/Renderer_OpenGL.hpp"
#include "Core/FileManager.hpp"

#include "Core/Type.hpp"
#include "glad/gl.h"
#include "spdlog/spdlog.h"
#include <vector>

// Default Fragment Shader
const GLchar default_fragment[]{R"(
#version 450 core

layout(binding = 2) uniform camera_data
{
    vec4 camera_pos;
};
layout(binding = 3) uniform fog_data
{
    vec4 fog_color;
    vec4 fog_range;
};
layout(binding = 0) uniform sampler2D sampler0;

float channel_minimum = 1.0 / 255.0;

layout(location = 0) in vec4 sxy;
layout(location = 1) in vec4 pos;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec4 col;

layout(location = 0) out vec4 col_out;

subroutine vec4 Blend();
subroutine vec4 Fog(vec4);

layout(location = 0) subroutine uniform Blend blend_uniform;
layout(location = 1) subroutine uniform Fog fog_uniform;

layout(index = 0) subroutine(Blend) vec4 vb_zero()
{
    vec4 color = texture(sampler0, uv);
    color.rgb *= color.a;
    return color;
}

layout(index = 1) subroutine(Blend) vec4 vb_zero_pmul()
{
    return texture(sampler0, uv); // pass through
}

layout(index = 2) subroutine(Blend) vec4 vb_one()
{
    vec4 color = col;
    color.rgb *= color.a;
    return color;
}

layout(index = 3) subroutine(Blend) vec4 vb_one_pmul()
{
    return col; // pass through
}

vec4 add_common(vec4 color) {
    color.rgb += col.rgb;
    color.r = min(color.r, 1.0);
    color.g = min(color.g, 1.0);
    color.b = min(color.b, 1.0);
    color.a *= col.a;
    color.rgb *= col.a;
    return color;
}

layout(index = 4) subroutine(Blend) vec4 vb_add()
{
    vec4 color = texture(sampler0, uv);
    return add_common(color);
}

layout(index = 5) subroutine(Blend) vec4 vb_add_pmul()
{
    vec4 color = texture(sampler0, uv);

    // cancel out alpha multiplication
    if (color.a < channel_minimum)
    {
        discard; // avoid division by zero
    }
    color.rgb /= color.a;

    return add_common(color);
}

layout(index = 6) subroutine(Blend) vec4 vb_mul()
{
    vec4 color = texture(sampler0, uv) * col;
    color.rgb *= color.a;
    return color;
}

layout(index = 7) subroutine(Blend) vec4 vb_mul_pmul()
{
    vec4 color = texture(sampler0, uv) * col;
    color.rgb *= col.a; // need to multiply with texture alpha
    return color;
}


layout(index = 8) subroutine(Fog) vec4 fog_none(vec4 color)
{
    return color; // pass through
}

layout(index = 9) subroutine(Fog) vec4 fog_linear(vec4 color)
{
    float dist = distance(camera_pos.xyz, pos.xyz);
    float k = clamp((dist - fog_range.x) / fog_range.w, 0.0, 1.0);
    float k1 = 1.0 - k;
    float alpha = k1 * color.a + k * color.a * fog_color.a;
    float ka = k1 * (k1 + k * fog_color.a);
    float kb = k * alpha;
    color = vec4(ka * color.rgb + kb * fog_color.rgb, alpha);
    return color;
}

layout(index = 10) subroutine(Fog) vec4 fog_exp(vec4 color)
{
    float dist = distance(camera_pos.xyz, pos.xyz);
    float k = clamp(1.0 - exp(-(dist * fog_range.x)), 0.0, 1.0);
    float k1 = 1.0 - k;
    float alpha = k1 * color.a + k * color.a * fog_color.a;
    float ka = k1 * (k1 + k * fog_color.a);
    float kb = k * alpha;
    color = vec4(ka * color.rgb + kb * fog_color.rgb, alpha);
    return color;
}

layout(index = 11) subroutine(Fog) vec4 fog_exp2(vec4 color)
{
    float dist = distance(camera_pos.xyz, pos.xyz);
    float k = clamp(1.0 - exp(-pow(dist * fog_range.x, 2.0)), 0.0, 1.0);
    float k1 = 1.0 - k;
    float alpha = k1 * color.a + k * color.a * fog_color.a;
    float ka = k1 * (k1 + k * fog_color.a);
    float kb = k * alpha;
    color = vec4(ka * color.rgb + kb * fog_color.rgb, alpha);
    return color;
}

void main()
{
    col_out = fog_uniform(blend_uniform());
}
)"};

// Default Vertex Shader
const GLchar default_vertex[]{R"(
#version 450 core

layout(binding = 0) uniform view_proj_buffer
{
    mat4 view_proj;
};
#if defined(WORLD_MATRIX)
layout(binding = 1) uniform world_buffer
{
    mat4 world;
};
#endif

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 col;

layout(location = 0) out vec4 sxy_out;
layout(location = 1) out vec4 pos_out;
layout(location = 2) out vec2 uv_out;
layout(location = 3) out vec4 col_out;

void main()
{
    vec4 pos_world = vec4(pos, 1.0);
#if defined(WORLD_MATRIX)
    pos_world = world * pos_world;
#endif

    sxy_out = view_proj * pos_world;
    pos_out = pos_world;
    uv_out = uv;
    col_out = col;
}
)"};

#define IDX(x) (size_t)static_cast<uint8_t>(x)

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

	bool PostEffectShader_OpenGL::createResources()
	{
		GLuint opengl_frag = 0;
		GLuint opengl_vert = 0;
		if (!opengl_frag)
		{
			if (is_path)
			{
				std::vector<uint8_t> src;
				if (!GFileManager().loadEx(source, (IData**)src.data()))
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
			glGetShaderInfoLog(opengl_prgm, 1024, &log_len, log);
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
		glGetProgramInterfaceiv(opengl_prgm, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &amt_uniform_blocks);

		const size_t block_properties_size = 4;
		GLenum block_properties[block_properties_size] = { GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE, GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH };
		GLint block_values[block_properties_size];

		std::vector<GLchar> name_buffer(128);

		for (int block = 0; block < amt_uniform_blocks; block++)
		{
			glGetProgramResourceiv(opengl_prgm, GL_UNIFORM_BLOCK, block, block_properties_size, block_properties, block_properties_size, NULL, block_values);
			name_buffer.resize(block_values[3]);
			glGetProgramResourceName(opengl_prgm, GL_PROGRAM_INPUT, block, name_buffer.size(), NULL, &name_buffer[0]);
			std::string name((char*)&name_buffer, name_buffer.size() - 1);

			LocalConstantBuffer local_buffer;
			local_buffer.index = block_values[0];
			local_buffer.buffer.resize(block_values[1]);
			local_buffer.variable.reserve(block_values[2]);

			m_buffer_map.emplace(name, std::move(local_buffer));
		}

		GLint amt_uniforms = 0;
		glGetProgramInterfaceiv(opengl_prgm, GL_UNIFORM, GL_ACTIVE_RESOURCES, &amt_uniforms);

		const size_t uniform_properties_size = 5;
		GLenum uniform_properties[uniform_properties_size] = { GL_TYPE, GL_NAME_LENGTH, GL_OFFSET, GL_ARRAY_SIZE, GL_BLOCK_INDEX };
		GLint uniform_values[uniform_properties_size];

		GLint amt_samplers = 0;
		GLuint tex_idx = 0;

		for (int uniform = 0; uniform < amt_uniforms; uniform++)
		{
			glGetProgramResourceiv(opengl_prgm, GL_UNIFORM, uniform, uniform_properties_size, uniform_properties, uniform_properties_size, NULL, uniform_values);
			name_buffer.resize(uniform_values[1]);
			glGetProgramResourceName(opengl_prgm, GL_PROGRAM_INPUT, uniform, name_buffer.size(), NULL, &name_buffer[0]);
			std::string name((char*)&name_buffer, name_buffer.size() - 1);

			if (uniform_values[0] == GL_SAMPLER_2D) // Handle Texture Uniforms
			{
				LocalTexture2D local_texture2d;
				local_texture2d.index = tex_idx;
				tex_idx++;

				m_texture2d_map.emplace(name, std::move(local_texture2d));
			}
			else // Handle Uniform Buffers
			{
				LocalVariable local_variable;
				local_variable.offset = uniform_values[2];
				local_variable.size = uniform_values[3];

				// data structures are different between DirectX and OpenGL, so take a performance hit
				for (auto& v : m_buffer_map)
				{
					if (v.second.index == uniform_values[4])
					{
						v.second.variable.emplace(name, std::move(local_variable));
						break;
					}
				}
			}
		}

		// Create buffers

		for (auto& v : m_buffer_map)
		{
			glGenBuffers(1, &v.second.opengl_buffer);
			if (v.second.opengl_buffer == 0)
				return false;
		}

		return true;
	}

	bool Renderer_OpenGL::createShaders()
	{
		GLuint frag, vert;
		compileFragmentShaderMacro(default_fragment, sizeof(default_fragment), frag);
		compileVertexShaderMacro(default_vertex, sizeof(default_vertex), vert);
		glAttachShader(_program, frag);
		glAttachShader(_program, vert);
		glLinkProgram(_program);

		glDeleteShader(frag);
		glDeleteShader(vert);

		return true;
	}
}
