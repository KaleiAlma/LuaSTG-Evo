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
		ParticlePool2D(int32_t size, Core::ScopeObject<LuaSTGPlus::IResourceSprite> img, BlendMode blend) : plist(size), img(img), blend(blend) {}
		void Update();
		void Render();
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

		ParticlePool2D::Particle* AddParticle(ParticlePool2D::Particle p) { plist.insert(p); return plist.GetFront(); }
	public:
		void Apply(std::function<bool(Particle*)> fn);
	public:
		int32_t GetSize() { return plist.GetSize(); }
	private:
		ParticleList<Particle> plist;
		Core::ScopeObject<LuaSTGPlus::IResourceSprite> img;
		BlendMode blend;
	};
}

