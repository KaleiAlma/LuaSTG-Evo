#pragma once
#include "Core/Graphics/Device.hpp"
#include "Core/Object.hpp"
#include "Core/Graphics/Renderer.hpp"
#include "Core/Graphics/Device_OpenGL.hpp"
#include "glad/gl.h"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "tiny_gltf.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <vector>

#define IDX(x) (size_t)static_cast<uint8_t>(x)

namespace Core::Graphics
{
    class ModelSharedComponent_OpenGL
        : public Object<IObject>
        , public IDeviceEventListener
    {
        friend class Model_OpenGL;
    private:
        ScopeObject<Device_OpenGL> m_device;

        // Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> default_image;
        // Microsoft::WRL::ComPtr<ID3D11SamplerState> default_sampler;
        GLuint default_image = 0;
        tinygltf::Sampler default_sampler;

        // Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
        // Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_vc;
        // Microsoft::WRL::ComPtr<ID3D11VertexShader> shader_vertex;
        // Microsoft::WRL::ComPtr<ID3D11VertexShader> shader_vertex_vc;
        // Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_pixel[IDX(IRenderer::FogState::MAX_COUNT)];
        // Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_pixel_alpha[IDX(IRenderer::FogState::MAX_COUNT)];
        // Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_pixel_nt[IDX(IRenderer::FogState::MAX_COUNT)];
        // Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_pixel_alpha_nt[IDX(IRenderer::FogState::MAX_COUNT)];
        // Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_pixel_vc[IDX(IRenderer::FogState::MAX_COUNT)];
        // Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_pixel_alpha_vc[IDX(IRenderer::FogState::MAX_COUNT)];
        // Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_pixel_nt_vc[IDX(IRenderer::FogState::MAX_COUNT)];
        // Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_pixel_alpha_nt_vc[IDX(IRenderer::FogState::MAX_COUNT)];
        GLuint shader_program;

        // Microsoft::WRL::ComPtr<ID3D11RasterizerState> state_rs_cull_none;
        // Microsoft::WRL::ComPtr<ID3D11RasterizerState> state_rs_cull_back;
        // Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state_ds;
        // Microsoft::WRL::ComPtr<ID3D11BlendState> state_blend;
        // Microsoft::WRL::ComPtr<ID3D11BlendState> state_blend_alpha;

        // Microsoft::WRL::ComPtr<ID3D11Buffer> cbo_mvp;
        // Microsoft::WRL::ComPtr<ID3D11Buffer> cbo_mlw;
        // Microsoft::WRL::ComPtr<ID3D11Buffer> cbo_caminfo;
        // Microsoft::WRL::ComPtr<ID3D11Buffer> cbo_alpha;
        // Microsoft::WRL::ComPtr<ID3D11Buffer> cbo_light;
        GLuint ubo_mvp = 0;
        GLuint ubo_mlw = 0;
        GLuint ubo_caminfo = 0;
        GLuint ubo_alpha = 0;
        GLuint ubo_light = 0;

    private:
        bool createImage();
        bool createSampler();
        bool createShader();
        bool createConstantBuffer();
        bool createState();

        bool createResources();
        void onDeviceCreate();
        void onDeviceDestroy();

    public:
        ModelSharedComponent_OpenGL(Device_OpenGL* p_device);
        ~ModelSharedComponent_OpenGL();
    };

    class Model_OpenGL
        : public Object<IModel>
        , public IDeviceEventListener
    {
    private:
        ScopeObject<Device_OpenGL> m_device;
        ScopeObject<ModelSharedComponent_OpenGL> shared_;

        glm::mat4 t_scale_;
        glm::mat4 t_trans_;
        glm::mat4 t_mbrot_;

        std::string gltf_path;

        struct ModelBlock
        {
            GLuint vao = 0;
            GLuint vertex_buffer = 0;
            GLuint uv_buffer = 0;
            GLuint normal_buffer = 0;
            GLuint color_buffer = 0;
            GLuint index_buffer = 0;
            tinygltf::Sampler sampler;
            GLuint image = 0;
            glm::mat4 local_matrix;
            glm::mat4 local_matrix_normal; // notice: pair with local_matrix
            glm::vec4 base_color;
            bool double_side = false;
            bool alpha_blend = false;
            bool alpha_cull = false;
            float alpha = 0.5f;
            uint32_t draw_count = 0;
            GLenum index_format = GL_UNSIGNED_SHORT;
            GLenum primitive_topology = GL_TRIANGLES;
            ModelBlock()
            : local_matrix(glm::identity<glm::mat4>())
            , local_matrix_normal(glm::identity<glm::mat4>())
            , base_color(glm::vec4(1.f, 1.f, 1.f, 1.f))
            {
            }
        };
        struct Sunshine
        {
            glm::vec4 ambient;
            glm::vec4 pos;
            glm::vec4 dir;
            glm::vec4 color;

            void setDir(float dir_deg, float upd_deg)
            {
                glm::vec3 slfrom(1.0f, 0.0f, 0.0f);
                auto rotate_vec2 = [](float& x, float& y, float r)
                {
                    float sin_v = sinf(r);
                    float cos_v = cosf(r);
                    float xx = x * cos_v - y * sin_v;
                    float yy = x * sin_v + y * cos_v;
                    x = xx;
                    y = yy;
                };
                // up and down
                rotate_vec2(slfrom.x, slfrom.y, glm::radians(upd_deg));
                // direction
                rotate_vec2(slfrom.x, slfrom.z, glm::radians(dir_deg));
                // inverse
                dir.x = -slfrom.x;
                dir.y = -slfrom.y;
                dir.z = -slfrom.z;
                dir.w = 0.0f;
            }

            Sunshine()
            {
                ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // full ambient light
                pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                dir = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
                color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f); // no directional light
            }
        };

        std::vector<GLuint> image;
        std::vector<tinygltf::Sampler> sampler;
        // std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> sampler;

        std::vector<ModelBlock> model_block;

        Sunshine sunshine;

        std::vector<glm::mat4> mTRS_stack;

        bool processNode(tinygltf::Model& model, tinygltf::Node& node);
        bool createImage(tinygltf::Model& model);
        bool createSampler(tinygltf::Model& model);
        bool createModelBlock(tinygltf::Model& model);

        bool createResources();
        void onDeviceCreate();
        void onDeviceDestroy();

    public:

        void setAmbient(Vector3F const& color, float brightness);
        void setDirectionalLight(Vector3F const& direction, Vector3F const& color, float brightness);
        void setScaling(Vector3F const& scale);
        void setPosition(Vector3F const& pos);
        void setRotationRollPitchYaw(float roll, float pitch, float yaw);
        void setRotationQuaternion(Vector4F const& quat);

        void draw(IRenderer::FogState fog);

    public:
        Model_OpenGL(Device_OpenGL* p_device, ModelSharedComponent_OpenGL* p_model_shared, StringView path);
        ~Model_OpenGL();
    };
}

#undef IDX
