#pragma once
#include "Core/Graphics/Sprite.hpp"
#include "GameResource/ResourceBase.hpp"
#include "GameResource/ResourceSprite.hpp"
#include "Particle/ParticleList.h"

namespace LuaSTGPlus::Particle
{
    class ParticlePool2D
    {
    public:
        struct Particle
        {
            Core::Vector2F pos;
            Core::Vector2F vel;
            Core::Vector2F accel;
            Core::Vector2F scale;
            float rot;
            float omiga;
            Core::Color4B color;
            uint32_t timer;
            float extra1;
            float extra2;
            float extra3;
        };

    public:
        Particle* AddParticle(Particle p) { plist.insert(p); return plist.GetFront(); }
        void Apply(std::function<bool(Particle*)> fn) { plist.foreach(fn); }
        void Clear() { plist.clear(); }
        Index GetSize() { return plist.GetSize(); }

        void Update();
        void Render();

    public:
        ParticlePool2D(Index size, Core::ScopeObject<IResourceSprite> img, BlendMode blend)
            : plist(size), img(img), blend(blend) {}

    private:
        ParticleList<Particle> plist;
        Core::ScopeObject<IResourceSprite> img;
        BlendMode blend;
    };
}

