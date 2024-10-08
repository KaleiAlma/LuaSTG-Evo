#include "Particle/TexParticle2D.h"
#include "AppFrame.h"

#define LRDR LAPP.GetRenderer2D()

namespace LuaSTGPlus::Particle
{
    void TexParticlePool2D::Update()
    {
        for (TexParticlePool2D::Particle& p : plist)
        {
            p.vel += p.accel;
            p.pos += p.vel;
            p.rot += p.omiga;
            p.timer += 1;
        }
    }

    void TexParticlePool2D::Render()
    {
        LAPP.updateGraph2DBlendMode(blend);
        LRDR->setTexture(tex->GetTexture());

        for (TexParticlePool2D::Particle& p : plist)
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

            float const sinv = sinf(p.rot);
            float const cosv = cosf(p.rot);

#define rotate_xy(UNIT) \
            {\
                float const tx = vert[UNIT].x * cosv - vert[UNIT].y * sinv;\
                float const ty = vert[UNIT].x * sinv + vert[UNIT].y * cosv;\
                vert[UNIT].x = tx;\
                vert[UNIT].y = ty;\
            }

            rotate_xy(0);
            rotate_xy(1);
            rotate_xy(2);
            rotate_xy(3);
#undef rotate_xy

            vert[0].x += p.pos.x; vert[0].y += p.pos.y;
            vert[1].x += p.pos.x; vert[1].y += p.pos.y;
            vert[2].x += p.pos.x; vert[2].y += p.pos.y;
            vert[3].x += p.pos.x; vert[3].y += p.pos.y;
            
            LRDR->drawQuad(vert);
        }
    }

    void TexParticlePool2D::Apply(std::function<bool(TexParticlePool2D::Particle* const)> fn)
    {
        plist.foreach(fn);
    }
}

#undef LRDR