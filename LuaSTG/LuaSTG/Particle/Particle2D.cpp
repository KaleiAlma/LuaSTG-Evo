#include "Particle/Particle2D.h"
#include "AppFrame.h"

namespace LuaSTGPlus::Particle
{
    void ParticlePool2D::Update()
    {
        for (ParticlePool2D::Particle& p : plist)
        {
            p.vel += p.accel;
            p.pos += p.vel;
            p.rot += p.omiga;
            p.timer += 1;
        }
    }

    void ParticlePool2D::Render()
    {
        Core::Color4B color[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
        img->GetSprite()->getColor(color);

        LAPP.updateGraph2DBlendMode(blend);

        for (ParticlePool2D::Particle& p : plist)
        {
            img->GetSprite()->setColor(Core::Color4B(p.color));
            img->GetSprite()->draw(p.pos, p.scale, p.rot);
        }
        img->GetSprite()->setColor(color);
    }

    void ParticlePool2D::Apply(std::function<bool(ParticlePool2D::Particle* const)> fn)
    {
        plist.foreach(fn);
    }
}