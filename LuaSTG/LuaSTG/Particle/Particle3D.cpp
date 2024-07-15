#include "Particle/Particle3D.h"
#include "AppFrame.h"


namespace LuaSTGPlus::Particle
{
    void ParticlePool3D::Update()
    {
        for (ParticlePool3D::Particle& p : plist)
        {
            p.vel += p.accel;
            p.pos += p.vel;
            p.rot += p.omiga;
            p.timer += 1;
        }
    }

    void ParticlePool3D::Render()
    {
        Core::Color4B color[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
        img->GetSprite()->getColor(color);

        LAPP.updateGraph2DBlendMode(blend);

        for (ParticlePool3D::Particle& p : plist)
        {
            img->GetSprite()->setColor(Core::Color4B(p.color));
            img->GetSprite()->draw(p.pos, p.rot, p.scale);
        }
        img->GetSprite()->setColor(color);
    }

    void ParticlePool3D::Apply(std::function<bool(ParticlePool3D::Particle* const)> fn)
    {
        plist.foreach(fn);
    }
}