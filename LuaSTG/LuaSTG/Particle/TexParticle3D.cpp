#include "Particle/TexParticle3D.h"
#include "AppFrame.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"

#define LRDR LAPP.GetRenderer2D()

namespace LuaSTGPlus::Particle
{
    void TexParticlePool3D::Update()
    {
        for (TexParticlePool3D::Particle& p : plist)
        {
            p.vel += p.accel;
            p.pos += p.vel;
            p.rot += p.omiga;
            p.timer += 1;
        }
    }

    void TexParticlePool3D::Render()
    {
        LAPP.updateGraph2DBlendMode(blend);
        LRDR->setTexture(tex->GetTexture());

        for (TexParticlePool3D::Particle& p : plist)
        {

            float const w_2 = p.uv.width()  / 2.f;
            float const h_2 = p.uv.height() / 2.f;

            Core::RectF const rect = Core::RectF(
                (p.uv.a.x - w_2) * p.scale.x,
                (p.uv.a.y - h_2) * p.scale.y,
                (p.uv.b.x - w_2) * p.scale.x,
                (p.uv.b.y - h_2) * p.scale.y
            );

            Core::Graphics::IRenderer::DrawVertex vert[4] = {
                Core::Graphics::IRenderer::DrawVertex(rect.a.x, rect.a.y, 0.0f, p.uv.a.x, p.uv.a.y, p.color.color()),
                Core::Graphics::IRenderer::DrawVertex(rect.b.x, rect.a.y, 0.0f, p.uv.b.x, p.uv.a.y, p.color.color()),
                Core::Graphics::IRenderer::DrawVertex(rect.b.x, rect.b.y, 0.0f, p.uv.b.x, p.uv.b.y, p.color.color()),
                Core::Graphics::IRenderer::DrawVertex(rect.a.x, rect.b.y, 0.0f, p.uv.a.x, p.uv.b.y, p.color.color()),
            };

            glm::mat4 transform = glm::identity<glm::mat4>();

            transform = rotate(transform, p.rot.x, glm::vec3(1, 0, 0));
            transform = rotate(transform, p.rot.y, glm::vec3(0, 1, 0));
            transform = rotate(transform, p.rot.z, glm::vec3(0, 0, 1));

#define transform_xyz(UNIT) \
            {\
                glm::vec4 tvec = glm::vec4(vert[UNIT].x, vert[UNIT].y, vert[UNIT].z, 1.0f);\
                tvec = tvec * transform;\
                vert[UNIT].x = tvec.x;\
                vert[UNIT].y = tvec.y;\
                vert[UNIT].z = tvec.z;\
            }

            transform_xyz(0);
            transform_xyz(1);
            transform_xyz(2);
            transform_xyz(3);
#undef transform_xyz

            vert[0].x += p.pos.x; vert[0].y += p.pos.y; vert[0].z += p.pos.z;
            vert[1].x += p.pos.x; vert[1].y += p.pos.y; vert[1].z += p.pos.z;
            vert[2].x += p.pos.x; vert[2].y += p.pos.y; vert[2].z += p.pos.z;
            vert[3].x += p.pos.x; vert[3].y += p.pos.y; vert[3].z += p.pos.z;

            LRDR->drawQuad(vert);
        }
    }

    void TexParticlePool3D::Apply(std::function<bool(TexParticlePool3D::Particle* const)> fn)
    {
        plist.foreach(fn);
    }
}

#undef LRDR
