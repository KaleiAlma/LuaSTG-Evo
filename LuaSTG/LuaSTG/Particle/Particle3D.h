#pragma once
#include "Core/Graphics/Sprite.hpp"
#include "GameResource/ResourceBase.hpp"
#include "GameResource/ResourceSprite.hpp"
#include "Particle/ParticleList.h"

namespace LuaSTGPlus::Particle
{
	class ParticlePool3D
	{
	public:
		ParticlePool3D(int32_t size, Core::ScopeObject<LuaSTGPlus::IResourceSprite> img, BlendMode blend) : plist(size), img(img), blend(blend) {}
		void Update();
		void Render();
	public:
		struct Particle
		{
			Core::Vector3F pos;
			Core::Vector3F vel;
			Core::Vector3F accel;
			Core::Vector3F rot;
			Core::Vector3F omiga;
			Core::Vector2F scale;
			Core::Color4B color;
			uint32_t timer;
			float extra1;
			float extra2;
			float extra3;
			float extra4;
		};

		ParticlePool3D::Particle* AddParticle(ParticlePool3D::Particle p) { plist.insert(p); return plist.GetFront(); }
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

