#include "Particle/Particle3D.h"
#include "AppFrame.h"


namespace LuaSTGPlus::Particle
{
    void ParticlePool3D::Update()
    {
        for (Particle& p : plist)
        {
            p.vel += p.accel;
            p.pos += p.vel;
            p.rot += p.omiga;
            p.timer += 1;
        }
    }

    void ParticlePool3D::Render()
    {
        Core::Color4B color[4];
        img->GetSprite()->getColor(color);

        LAPP.updateGraph2DBlendMode(blend);

        for (Particle& p : plist)
        {
            img->GetSprite()->setColor(p.color);
            img->GetSprite()->draw(p.pos, p.rot, p.scale);
        }

        img->GetSprite()->setColor(color);
    }
}