#pragma once
#include "Core/Type.hpp"
#include "GameResource/ResourceBase.hpp"
#include "GameResource/ResourceTexture.hpp"
#include "Particle/ParticleList.h"

namespace LuaSTGPlus::Particle
{
	class TexParticlePool2D
	{
	public:
		TexParticlePool2D(int32_t size, Core::ScopeObject<LuaSTGPlus::IResourceTexture> tex, BlendMode blend) : plist(size), tex(tex), blend(blend) {}
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
			Core::RectI uv;
			float extra1;
			float extra2;
			float extra3;
		};

		TexParticlePool2D::Particle* AddParticle(TexParticlePool2D::Particle p) { plist.insert(p); return plist.GetFront(); }
	public:
		void Apply(std::function<bool(Particle*)> fn);
	public:
		int32_t GetSize() { return plist.GetSize(); }
	private:
		ParticleList<Particle> plist;
		Core::ScopeObject<LuaSTGPlus::IResourceTexture> tex;
		BlendMode blend;
	};
}

