#include "Core/Graphics/Model_OpenGL.hpp"
#include "Core/FileManager.hpp"
#include "glad/gl.h"
#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/matrix.hpp"
#include "spdlog/spdlog.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/ext/quaternion_float.hpp"
#include <cstdint>

#define IDX(x) (size_t)static_cast<uint8_t>(x)

namespace glm
{
    inline mat4 inverseTranspose(mat4 M)
    {
        // The transpose of the inverse of the world matrix is only for normal vectors,
        // and we don't need the translation component of the world matrix.

        // If we don't remove it, subsequent multiplication by observation
        // matrices and such will result in incorrect transformations.
        mat4 A = row(M, 3, vec4(0.f, 0.f, 0.f , 1.f));

        return transpose(inverse(A));
    }
    inline mat4 rotateRollPitchYaw(mat4 M, vec3 R)
    {
        M = rotate(M, R.x, vec3(1, 0, 0));
        M = rotate(M, R.y, vec3(0, 1, 0));
        M = rotate(M, R.z, vec3(0, 0, 1));
        return M;
    }
}

namespace Core::Graphics
{
    bool ModelSharedComponent_OpenGL::createImage()
    {
        // default: purple & black tile image

        uint32_t black = 0x000000FFu;
        uint32_t purple = 0xFF00FFFFu;
        std::vector<uint32_t> pixels(64 * 64);
        uint32_t* ptr = pixels.data();
        for (int i = 0; i < 32; i += 1)
        {
            for (int j = 0; j < 32; j += 1)
            {
                *ptr = black;
                ptr++;
            }
            for (int j = 0; j < 32; j += 1)
            {
                *ptr = purple;
                ptr++;
            }
        }
        for (int i = 0; i < 32; i += 1)
        {
            for (int j = 0; j < 32; j += 1)
            {
                *ptr = purple;
                ptr++;
            }
            for (int j = 0; j < 32; j += 1)
            {
                *ptr = black;
                ptr++;
            }
        }

        // default: create

        glGenTextures(1, &default_image);
        if (default_image == 0) {
            assert(false);
            return false;
        }
        glBindTexture(GL_TEXTURE_2D, default_image);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        return true;
    }
    bool ModelSharedComponent_OpenGL::createSampler()
    {
        default_sampler.minFilter = TINYGLTF_TEXTURE_FILTER_LINEAR;
        default_sampler.magFilter = TINYGLTF_TEXTURE_FILTER_LINEAR;
        default_sampler.wrapS = TINYGLTF_TEXTURE_WRAP_REPEAT;
        default_sampler.wrapT = TINYGLTF_TEXTURE_WRAP_REPEAT;

        return true;
    }
    bool ModelSharedComponent_OpenGL::createConstantBuffer()
    {
        // built-in: view-proj matrix

        glGenBuffers(1, &ubo_mvp);
        if (ubo_mvp == 0) {
            assert(false);
            return false;
        }

        // built-in: local-world matrix

        glGenBuffers(1, &ubo_mlw);
        if (ubo_mlw == 0) {
            assert(false);
            return false;
        }
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_mlw);
        glBufferData(GL_UNIFORM_BUFFER, 128, 0, GL_DYNAMIC_DRAW);


        // built-in: camera info

        glGenBuffers(1, &ubo_caminfo);
        if (ubo_caminfo == 0) {
            assert(false);
            return false;
        }

        // built-in: alpha mask

        glGenBuffers(1, &ubo_alpha);
        if (ubo_alpha == 0) {
            assert(false);
            return false;
        }

        // built-in: light

        glGenBuffers(1, &ubo_light);
        if (ubo_light == 0) {
            assert(false);
            return false;
        }

        return true;
    }
    bool ModelSharedComponent_OpenGL::createState()
    {
        // OpenGL doesn't do state in the same way as DirectX

        return true;
    }

    bool ModelSharedComponent_OpenGL::createResources()
    {
        // load image to shader resource

        if (!createImage()) return false;

        // create sampler state

        if (!createSampler()) return false;

        // create shader and input layout

        if (!createShader()) return false;

        // create constant buffer

        if (!createConstantBuffer()) return false;

        // create state

        if (!createState()) return false;

        return true;
    }
    void ModelSharedComponent_OpenGL::onDeviceCreate()
    {
        createResources();
    }
    void ModelSharedComponent_OpenGL::onDeviceDestroy()
    {
        glDeleteTextures(1, &default_image);

        glDeleteBuffers(1, &ubo_mvp);
        glDeleteBuffers(1, &ubo_mlw);
        glDeleteBuffers(1, &ubo_caminfo);
        glDeleteBuffers(1, &ubo_alpha);
        glDeleteBuffers(1, &ubo_light);
    }

    ModelSharedComponent_OpenGL::ModelSharedComponent_OpenGL(Device_OpenGL* p_device)
        : m_device(p_device)
    {
        if (!createResources())
            throw std::runtime_error("ModelSharedComponent_OpenGL::ModelSharedComponent_OpenGL");
        m_device->addEventListener(this);
    }
    ModelSharedComponent_OpenGL::~ModelSharedComponent_OpenGL()
    {
        m_device->removeEventListener(this);
    }
}

namespace Core::Graphics
{
    void map_sampler_to_opengl(tinygltf::Sampler& samp, GLuint tex)
    {
        glBindTexture(GL_TEXTURE_2D, tex);
    #define MAKE_FILTER(MIN_MIP, MAG) ((MAG << 16) | (MIN_MIP))
        switch (MAKE_FILTER(samp.minFilter, samp.magFilter))
        {
        default:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_NEAREST, TINYGLTF_TEXTURE_FILTER_NEAREST):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_LINEAR, TINYGLTF_TEXTURE_FILTER_NEAREST):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST, TINYGLTF_TEXTURE_FILTER_NEAREST):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST, TINYGLTF_TEXTURE_FILTER_NEAREST):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR, TINYGLTF_TEXTURE_FILTER_NEAREST):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR, TINYGLTF_TEXTURE_FILTER_NEAREST):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_NEAREST, TINYGLTF_TEXTURE_FILTER_LINEAR):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_LINEAR, TINYGLTF_TEXTURE_FILTER_LINEAR):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST, TINYGLTF_TEXTURE_FILTER_LINEAR):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST, TINYGLTF_TEXTURE_FILTER_LINEAR):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR, TINYGLTF_TEXTURE_FILTER_LINEAR):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case MAKE_FILTER(TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR, TINYGLTF_TEXTURE_FILTER_LINEAR):
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.f);
            break;
        }
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.6f);
    #undef MAKE_FILTER
        switch (samp.wrapS)
        {
        default:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            break;
        case TINYGLTF_TEXTURE_WRAP_REPEAT:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            break;
        case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            break;
        case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            break;
        }
        switch (samp.wrapT)
        {
        default:
        case -1:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case TINYGLTF_TEXTURE_WRAP_REPEAT:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            break;
        }
    }
    void map_primitive_topology_to_opengl(tinygltf::Primitive& prim, GLenum& topo)
    {
        switch (prim.mode)
        {
        default:
            topo = GL_TRIANGLES;
            break;
        case TINYGLTF_MODE_POINTS:
            topo = GL_POINTS;
            break;
        case TINYGLTF_MODE_LINE:
            topo = GL_LINES;
            break;
        case TINYGLTF_MODE_LINE_LOOP:
            topo = GL_LINE_LOOP;
            break;
        case TINYGLTF_MODE_LINE_STRIP:
            topo = GL_LINE_STRIP;
            break;
        case TINYGLTF_MODE_TRIANGLES:
            topo = GL_TRIANGLES;
            break;
        case TINYGLTF_MODE_TRIANGLE_STRIP:
            topo = GL_TRIANGLE_STRIP;
            break;
        case TINYGLTF_MODE_TRIANGLE_FAN:
            topo = GL_TRIANGLE_FAN;
            break;
        }
    }
    glm::mat4 get_local_transfrom_from_node(tinygltf::Node& node)
    {
        if (!node.matrix.empty())
        {
        #pragma warning(disable:4244)
            // [Potential Overflow]
            glm::mat4 mM(
                node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
                node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
                node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
                node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]);
        #pragma warning(default:4244)
            return mM;
        }
        else
        {
            glm::mat4 mS = glm::identity<glm::mat4>();
            glm::mat4 mR = glm::identity<glm::mat4>();
            glm::mat4 mT = glm::identity<glm::mat4>();
            if (!node.scale.empty())
            {
            #pragma warning(disable:4244)
                // [Potential Overflow]
                mS = glm::scale(mS, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
            #pragma warning(default:4244)
            }
            if (!node.rotation.empty())
            {
            #pragma warning(disable:4244)
                // [Potential Overflow]
                glm::quat quat = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
            #pragma warning(default:4244)
                mR = glm::toMat4(quat);
            }
            if (!node.translation.empty())
            {
            #pragma warning(disable:4244)
                // [Potential Overflow]
                mT = glm::translate(mT, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
            #pragma warning(default:4244)
            }
            return mT * mR * mS;
        }
    }

    void Model_OpenGL::setAmbient(Vector3F const& color, float brightness)
    {
        sunshine.ambient = glm::vec4(color.x, color.y, color.z, brightness);
    }
    void Model_OpenGL::setDirectionalLight(Vector3F const& direction, Vector3F const& color, float brightness)
    {
        sunshine.dir = glm::vec4(direction.x, direction.y, direction.z, 0.0f);
        sunshine.color = glm::vec4(color.x, color.y, color.z, brightness);
    }
    void Model_OpenGL::setScaling(Vector3F const& scale)
    {
        t_scale_ = glm::scale(glm::identity<glm::mat4>(), glm::vec3(scale.x, scale.y, scale.z));
    }
    void Model_OpenGL::setPosition(Vector3F const& pos)
    {
        t_trans_ = glm::translate(glm::identity<glm::mat4>(), glm::vec3(pos.x, pos.y, pos.z));
    }
    void Model_OpenGL::setRotationRollPitchYaw(float roll, float pitch, float yaw)
    {
        t_mbrot_ = glm::rotateRollPitchYaw(glm::identity<glm::mat4>(), glm::vec3(pitch, yaw, roll));
    }
    void Model_OpenGL::setRotationQuaternion(Vector4F const& quat)
    {
        glm::quat const xq(quat.w, quat.x, quat.y, quat.z);
        t_mbrot_ = glm::toMat4(xq);
    }

    bool Model_OpenGL::createImage(tinygltf::Model& model)
    {
        // gltf: create

        image.resize(model.images.size());
        glGenTextures(model.images.size(), image.data());
        for (size_t idx = 0; idx < model.images.size(); idx += 1)
        {
            if (image[idx] == 0)
            {
                assert(false);
                return false;
            }

            tinygltf::Image& img = model.images[idx];

            if (img.width <= 0 || img.height <= 0)
            {
                image[idx] = shared_->default_image; // That's a weird texture you got there, man
                spdlog::error("[core] Load model texture '{}' failed", img.name);
                continue;
            }

            glBindTexture(GL_TEXTURE_2D, image[idx]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.image.data());
            // glGenerateMipmap(GL_TEXTURE_2D);
        }

        return true;
    }
    bool Model_OpenGL::createSampler(tinygltf::Model& model)
    {
        sampler = model.samplers;

        return true;
    }
    bool Model_OpenGL::processNode(tinygltf::Model& model, tinygltf::Node& node)
    {
        glm::mat4 mTRS = get_local_transfrom_from_node(node);

        if (node.mesh >= 0)
        {
            tinygltf::Mesh& mesh = model.meshes[node.mesh];
            for (tinygltf::Primitive& prim : mesh.primitives)
            {
                ModelBlock mblock;
                glGenVertexArrays(1, &mblock.vao);
                if (mblock.vao == 0)
                {
                    assert(false);
                    return false;
                }
                glBindVertexArray(mblock.vao);
                glm::mat4 mTRSw = mTRS;
                for (auto it = mTRS_stack.crbegin(); it != mTRS_stack.crend(); it++)
                {
                    mTRSw = *it * mTRSw;
                }
                mTRSw = glm::scale(mTRSw, glm::vec3(1.0f, 1.0f, -1.0f)); // to left-hand
                mblock.local_matrix = mTRSw;
                mblock.local_matrix_normal = glm::inverseTranspose(mTRSw); // face normal
                if (prim.attributes.contains("POSITION"))
                {
                    tinygltf::Accessor& accessor = model.accessors[prim.attributes["POSITION"]];
                    tinygltf::BufferView& bufferview = model.bufferViews[accessor.bufferView];
                    tinygltf::Buffer& buffer = model.buffers[bufferview.buffer];

                    int32_t vertex_size = tinygltf::GetComponentSizeInBytes(accessor.componentType);
                    uint32_t bytewidth = vertex_size * tinygltf::GetNumComponentsInType(accessor.type) * accessor.count;
                    glGenBuffers(1, &mblock.vertex_buffer);
                    if (mblock.vertex_buffer == 0)
                    {
                        assert(false);
                        return false;
                    }
                    glBindBuffer(GL_ARRAY_BUFFER, mblock.vertex_buffer);
                    glBufferData(GL_ARRAY_BUFFER, bytewidth, buffer.data.data() + bufferview.byteOffset + accessor.byteOffset, GL_STATIC_DRAW);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

                    mblock.draw_count = accessor.count;
                }
                if (prim.attributes.contains("NORMAL"))
                {
                    tinygltf::Accessor& accessor = model.accessors[prim.attributes["NORMAL"]];
                    tinygltf::BufferView& bufferview = model.bufferViews[accessor.bufferView];
                    tinygltf::Buffer& buffer = model.buffers[bufferview.buffer];

                    int32_t normal_size = tinygltf::GetComponentSizeInBytes(accessor.componentType);
                    uint32_t bytewidth = normal_size * tinygltf::GetNumComponentsInType(accessor.type) * accessor.count;
                    glGenBuffers(1, &mblock.normal_buffer);
                    if (mblock.normal_buffer == 0)
                    {
                        assert(false);
                        return false;
                    }
                    glBindBuffer(GL_ARRAY_BUFFER, mblock.normal_buffer);
                    glBufferData(GL_ARRAY_BUFFER, bytewidth, buffer.data.data() + bufferview.byteOffset + accessor.byteOffset, GL_STATIC_DRAW);
                    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
                }
                if (prim.attributes.contains("COLOR_0"))
                {
                    tinygltf::Accessor& accessor = model.accessors[prim.attributes["COLOR_0"]];
                    tinygltf::BufferView& bufferview = model.bufferViews[accessor.bufferView];
                    tinygltf::Buffer& buffer = model.buffers[bufferview.buffer];

                    int32_t color_size = tinygltf::GetComponentSizeInBytes(accessor.componentType);
                    uint32_t bytewidth = color_size * tinygltf::GetNumComponentsInType(accessor.type) * accessor.count;
                    glGenBuffers(1, &mblock.color_buffer);
                    if (mblock.color_buffer == 0)
                    {
                        assert(false);
                        return false;
                    }
                    glBindBuffer(GL_ARRAY_BUFFER, mblock.color_buffer);
                    glBufferData(GL_ARRAY_BUFFER, bytewidth, buffer.data.data() + bufferview.byteOffset + accessor.byteOffset, GL_STATIC_DRAW);
                    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
                }
                if (prim.attributes.contains("TEXCOORD_0"))
                {
                    tinygltf::Accessor& accessor = model.accessors[prim.attributes["TEXCOORD_0"]];
                    tinygltf::BufferView& bufferview = model.bufferViews[accessor.bufferView];
                    tinygltf::Buffer& buffer = model.buffers[bufferview.buffer];

                    int32_t uv_size = tinygltf::GetComponentSizeInBytes(accessor.componentType);
                    uint32_t bytewidth = uv_size * tinygltf::GetNumComponentsInType(accessor.type) * accessor.count;
                    glGenBuffers(1, &mblock.uv_buffer);
                    if (mblock.uv_buffer == 0)
                    {
                        assert(false);
                        return false;
                    }
                    glBindBuffer(GL_ARRAY_BUFFER, mblock.uv_buffer);
                    glBufferData(GL_ARRAY_BUFFER, bytewidth, buffer.data.data() + bufferview.byteOffset + accessor.byteOffset, GL_STATIC_DRAW);
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
                }
                if (prim.indices >= 0)
                {
                    tinygltf::Accessor& accessor = model.accessors[prim.indices];
                    tinygltf::BufferView& bufferview = model.bufferViews[accessor.bufferView];
                    tinygltf::Buffer& buffer = model.buffers[bufferview.buffer];

                    int32_t index_size = tinygltf::GetComponentSizeInBytes(accessor.componentType);
                    uint32_t bytewidth = index_size * tinygltf::GetNumComponentsInType(accessor.type) * accessor.count;
                    void* p_sysmem = buffer.data.data() + bufferview.byteOffset + accessor.byteOffset;
                    std::vector<uint16_t> index_work;

                    if (index_size == 1)
                    {
                        index_work.resize(bytewidth);
                        uint8_t* ptr = (uint8_t*)p_sysmem;
                        for (size_t i = 0; i < bytewidth; i += 1)
                        {
                            index_work[i] = ptr[i];
                        }
                        index_size = 2;
                        bytewidth *= 2;
                        p_sysmem = index_work.data();
                    }
                    assert(index_size == 2 || index_size == 4);
                    mblock.index_format = index_size == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

                    glGenBuffers(1, &mblock.index_buffer);
                    if (mblock.index_buffer == 0)
                    {
                        assert(false);
                        return false;
                    }
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mblock.index_buffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytewidth, p_sysmem, GL_STATIC_DRAW);

                    mblock.draw_count = accessor.count;
                }
                if (prim.material >= 0)
                {
                    tinygltf::Material& material = model.materials[prim.material];
                    auto& bcc = material.pbrMetallicRoughness.baseColorFactor;
                #pragma warning(disable:4244)
                    // [Potential Overflow]
                    mblock.base_color = glm::vec4(bcc[0], bcc[1], bcc[2], bcc[3]);
                #pragma warning(default:4244)
                    tinygltf::TextureInfo& texture_info = material.pbrMetallicRoughness.baseColorTexture;
                    if (texture_info.index >= 0)
                    {
                        tinygltf::Texture& texture = model.textures[texture_info.index];
                        mblock.image = image[texture.source];
                        if (texture.sampler >= 0)
                        {
                            mblock.sampler = sampler[texture.sampler];
                        }
                        else
                        {
                            mblock.sampler = shared_->default_sampler;
                        }
                        map_sampler_to_opengl(mblock.sampler, mblock.image);
                        glGenerateMipmap(GL_TEXTURE_2D);
                    }
                    else
                    {
                        // mblock.image = shared_->default_image;
                        mblock.sampler = shared_->default_sampler;
                    }
                    if (material.alphaMode == "MASK")
                    {
                        mblock.alpha_cull = true;
                    }
                    else if (material.alphaMode == "BLEND")
                    {
                        mblock.alpha_blend = true;
                    }
                    // [Potential Overflow]
                #pragma warning(disable:4244)
                    mblock.alpha = material.alphaCutoff;
                #pragma warning(default:4244)
                    mblock.double_side = material.doubleSided;
                }
                map_primitive_topology_to_opengl(prim, mblock.primitive_topology);
                model_block.emplace_back(mblock);
            }
        }

        mTRS_stack.push_back(mTRS);
        if (!node.children.empty())
        {
            for (auto const& child_node_idx : node.children)
            {
                if (!processNode(model, model.nodes[child_node_idx]))
                {
                    return false;
                }
            }
        }
        mTRS_stack.pop_back();

        return true;
    };
    bool Model_OpenGL::createModelBlock(tinygltf::Model& model)
    {
        int default_scene = model.defaultScene;
        if (default_scene < 0) default_scene = 0;
        tinygltf::Scene& scene = model.scenes[default_scene];
        for (int const& node_idx : scene.nodes)
        {
            tinygltf::Node& node = model.nodes[node_idx];
            if (!processNode(model, node))
            {
                return false;
            }
        }
        return true;
    }

    bool Model_OpenGL::createResources()
    {
        struct FileSystemWrapper
        {
            static bool FileExists(const std::string& abs_filename, void*)
            {
                return GFileManager().containEx(abs_filename);
            }
            static bool ReadWholeFile(std::vector<unsigned char>* out, std::string* err, const std::string& filepath, void*)
            {
                if (!GFileManager().loadEx(filepath, *out))
                {
                    if (err)
                    {
                        (*err) += "File load error : " + filepath + "\n";
                    }
                }
                return true;
            }
        };
        tinygltf::FsCallbacks fs_cb = {
            .FileExists = &FileSystemWrapper::FileExists,
            .ExpandFilePath = &tinygltf::ExpandFilePath,
            .ReadWholeFile = &FileSystemWrapper::ReadWholeFile,
            .WriteWholeFile = &tinygltf::WriteWholeFile,
            .user_data = nullptr,
        };
        tinygltf::TinyGLTF gltf_ctx;
        gltf_ctx.SetStoreOriginalJSONForExtrasAndExtensions(true);
        gltf_ctx.SetFsCallbacks(fs_cb);

        tinygltf::Model model;
        std::string warn;
        std::string err;

        bool ret = false;
        if (gltf_path.ends_with(".gltf"))
        {
            ret = gltf_ctx.LoadASCIIFromFile(&model, &err, &warn, gltf_path.c_str());
        }
        else
        {
            ret = gltf_ctx.LoadBinaryFromFile(&model, &err, &warn, gltf_path.c_str());
        }
        if (!warn.empty())
        {
            spdlog::warn("[core] gltf model warning: {}", warn);
        }
        if (!err.empty())
        {
            spdlog::error("[core] gltf model error: {}", err);
        }
        if (!ret)
        {
            return false;
        }

        // create sampler state

        if (!createSampler(model)) return false;

        // load image to shader resource

        if (!createImage(model)) return false;

        // create model block

        if (!createModelBlock(model)) return false;

        return true;
    }
    void Model_OpenGL::onDeviceCreate()
    {
        createResources();
    }
    void Model_OpenGL::onDeviceDestroy()
    {
        image.clear();
        sampler.clear();

        model_block.clear();
    }

    void Model_OpenGL::draw(IRenderer::FogState fog)
    {
        // common data

        glBindBuffer(GL_UNIFORM_BUFFER, shared_->ubo_light);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(sunshine), &sunshine, GL_STATIC_DRAW);
        glm::mat4 const t_locwo_ = t_trans_ * t_mbrot_ * t_scale_;

        auto set_state_matrix_from_block = [&](ModelBlock& mblock)
        {
            glUseProgram(shared_->programs[IDX(fog)][mblock.alpha_cull ? 1 : 0][mblock.image ? 1 : 0][mblock.color_buffer ? 1 : 0]);
        };
        auto upload_local_world_matrix = [&](ModelBlock& mblock)
        {
            struct
            {
                glm::mat4 v1;
                glm::mat4 v2;
            } v{};
            glm::mat4 const t_total_ = t_locwo_ * mblock.local_matrix;
            v.v1 = t_total_;
            v.v2 = glm::inverseTranspose(t_total_);
            glBindBuffer(GL_UNIFORM_BUFFER, shared_->ubo_mlw);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(v), &v);
        };
        auto set_state_from_block = [&](ModelBlock& mblock)
        {
            glBindVertexArray(mblock.vao);
            if (mblock.vertex_buffer)
                glEnableVertexAttribArray(0);
            if (mblock.uv_buffer)
                glEnableVertexAttribArray(1);
            if (mblock.color_buffer)
                glEnableVertexAttribArray(2);
            if (mblock.normal_buffer)
                glEnableVertexAttribArray(3);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mblock.index_buffer);

            set_state_matrix_from_block(mblock);

            upload_local_world_matrix(mblock);
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, shared_->ubo_mlw);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mblock.image);
            map_sampler_to_opengl(mblock.sampler, mblock.image);
            if (!mblock.alpha_cull)
            {
                GLfloat const alpha[8] = {
                    mblock.base_color.x, mblock.base_color.y, mblock.base_color.z, mblock.base_color.w,
                    0.5f, 0.0f, 0.0f, 0.0f,
                };
                glBindBuffer(GL_UNIFORM_BUFFER, shared_->ubo_alpha);
                glBufferData(GL_UNIFORM_BUFFER, sizeof(alpha), &alpha, GL_STATIC_DRAW);
            }
            else
            {
                GLfloat const alpha[8] = {
                    mblock.base_color.x, mblock.base_color.y, mblock.base_color.z, mblock.base_color.w,
                    mblock.alpha, 0.0f, 0.0f, 0.0f,
                };
                glBindBuffer(GL_UNIFORM_BUFFER, shared_->ubo_alpha);
                glBufferData(GL_UNIFORM_BUFFER, sizeof(alpha), &alpha, GL_STATIC_DRAW);
            }

            glBindBufferBase(GL_UNIFORM_BUFFER, 4, shared_->ubo_alpha);
            glBindBufferBase(GL_UNIFORM_BUFFER, 5, shared_->ubo_light);

            // OM

            glEnable(GL_DEPTH_TEST);
            if (mblock.alpha_blend)
            {
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            }
            else
            {
                glDisable(GL_BLEND);
            }
        };
        auto draw_block = [&](ModelBlock& mblock)
        {
            glDrawElements(mblock.primitive_topology, mblock.draw_count, mblock.index_format, 0);
        };

        // pass 1 opaque object

        for (auto& mblock : model_block)
        {
            if (!mblock.alpha_cull && !mblock.alpha_blend)
            {
                set_state_from_block(mblock);
                draw_block(mblock);
            }
        }

        // pass 2 alpha mask object

        for (auto& mblock : model_block)
        {
            if (mblock.alpha_cull)
            {
                set_state_from_block(mblock);
                draw_block(mblock);
            }
        }

        // pass 3 alpha blend object

        for (auto& mblock : model_block)
        {
            if (mblock.alpha_blend)
            {
                set_state_from_block(mblock);
                draw_block(mblock);
            }
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        glDisable(GL_DEPTH_TEST);
    }

    Model_OpenGL::Model_OpenGL(Device_OpenGL* p_device, ModelSharedComponent_OpenGL* p_model_shared, StringView path)
        : m_device(p_device)
        , shared_(p_model_shared)
        , gltf_path(path)
    {
        t_scale_ = glm::identity<glm::mat4>();
        t_trans_ = glm::identity<glm::mat4>();
        t_mbrot_ = glm::identity<glm::mat4>();
        if (!createResources())
            throw std::runtime_error("Model_OpenGL::Model_OpenGL");
        m_device->addEventListener(this);
    }
    Model_OpenGL::~Model_OpenGL()
    {
        m_device->removeEventListener(this);
    }
}
