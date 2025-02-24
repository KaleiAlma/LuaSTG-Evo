#include "Particle/Particle2D.h"
#include "AppFrame.h"

namespace LuaSTGPlus::Particle
{
    void ParticlePool2D::Update()
    {
        for (Particle& p : plist)
        {
            p.vel += p.accel;
            p.pos += p.vel;
            p.rot += p.omiga;
            p.timer += 1;
        }
    }

    void ParticlePool2D::Render()
    {
        Core::Color4B color[4];
        img->GetSprite()->getColor(color);

        LAPP.updateGraph2DBlendMode(blend);

        for (Particle& p : plist)
        {
            img->GetSprite()->setColor(p.color);
            img->GetSprite()->draw(p.pos, p.scale, p.rot);
        }

        img->GetSprite()->setColor(color);
    }
}