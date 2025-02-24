#include "lua_particle.hpp"
#include "Core/Type.hpp"
#include "GameResource/ResourceTexture.hpp"
#include "LMathConstant.hpp"
#include "Particle/Particle2D.h"
#include "Particle/Particle3D.h"
#include "Particle/TexParticle2D.h"
#include "Particle/TexParticle3D.h"
#include "Core/Graphics/Sprite.hpp"
#include "AppFrame.h"
#include "LuaWrapper.hpp"
#include "LuaWrapperMisc.hpp"
#include "lua.h"
#include "lua_particle_hash.hpp"

using namespace LuaSTGPlus;
using namespace LuaSTGPlus::Particle;

std::string_view const ID_Pool2D("particle.ParticlePool2D");
std::string_view const ID_Particle2D("particle.Particle2D");
std::string_view const ID_Pool3D("particle.ParticlePool3D");
std::string_view const ID_Particle3D("particle.Particle3D");
std::string_view const ID_TexPool2D("particle.TexParticlePool2D");
std::string_view const ID_TexParticle2D("particle.TexParticle2D");
std::string_view const ID_TexPool3D("particle.TexParticlePool3D");
std::string_view const ID_TexParticle3D("particle.TexParticle3D");

int lua_NewPool2D(lua_State* L)
{
    int const argc = lua_gettop(L);
    int32_t poolsize = 512;
    BlendMode blend = BlendMode::MulAlpha;
    Core::ScopeObject<LuaSTGPlus::IResourceSprite> img;

    if (argc < 1)
        return luaL_error(L, "must provide image resource for particle system");

    img = LAPP.GetResourceMgr().FindSprite(luaL_checkstring(L, 1));
    if (!img)
        return luaL_error(L, "can't find sprite '%s'", luaL_checkstring(L, 1));

    if (argc >= 2)
        blend = TranslateBlendMode(L, 2);
    if (argc >= 3)
        poolsize = luaL_checkint(L, 3);

    ParticlePool2D** pool = static_cast<ParticlePool2D**>(lua_newuserdata(L, sizeof(**pool)));
    luaL_setmetatable(L, ID_Pool2D.data());
    *pool = new ParticlePool2D(poolsize, img, blend);

    return 1;
}

int lua_pool2d_AddParticle(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));
    int const argc = lua_gettop(L);

    ParticlePool2D::Particle p{
        .pos = Core::Vector2F(luaL_checknumber(L, 2), luaL_checknumber(L, 3)),
        .vel = Core::Vector2F((float)luaL_optnumber(L, 5, 0), (float)luaL_optnumber(L, 6, 0)),
        .scale = Core::Vector2F(1, 1),
        .rot = (float)(L_DEG_TO_RAD * luaL_optnumber(L, 4, 0)),
        .color = Core::Color4B(0xFFFFFFFF),
    };

    if (argc == 7)
    {
        float s = luaL_checknumber(L, 7);
        p.scale = Core::Vector2F(s, s);
    }
    else if (argc > 7)
    {
        p.scale = Core::Vector2F(luaL_checknumber(L, 7), luaL_checknumber(L, 8));
    }

    ParticlePool2D::Particle** ud = static_cast<ParticlePool2D::Particle**>(lua_newuserdata(L, sizeof(**ud)));
    luaL_setmetatable(L, ID_Particle2D.data());
    new (ud) ParticlePool2D::Particle* { (*self)->AddParticle(std::move(p)) };

    return 1;
}

int lua_pool2d_Update(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));
    (*self)->Update();
    return 0;
}

int lua_pool2d_Render(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));
    (*self)->Render();
    return 0;
}

int lua_pool2d_Apply(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));

    int r_fn = luaL_ref(L, LUA_REGISTRYINDEX);

    (*self)->Apply([&](auto p) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, r_fn);

        ParticlePool2D::Particle** ud = static_cast<ParticlePool2D::Particle**>(lua_newuserdata(L, sizeof(**ud)));
        *ud = p;
        luaL_getmetatable(L, ID_Particle2D.data());
        lua_setmetatable(L, -2);


        lua_call(L, 1, 1);
        bool ret = lua_toboolean(L, -1);
        lua_pop(L, 1);

        return ret;
    });

    luaL_unref(L, LUA_REGISTRYINDEX, r_fn);
    return 0;
}

int lua_pool2d_Clear(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));
    (*self)->Clear();
    return 0;
}

int lua_pool2d_GetSize(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));

    lua_pushnumber(L, (*self)->GetSize());

    return 1;
}

int lua_pool2d_mtToString(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));

    lua_pushfstring(L, "ParticlePool2D(size = %d)", (*self)->GetSize());

    return 1;
}

int lua_pool2d_mtGC(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));

    delete *self;

    return 0;
}

int lua_particle2d_mtIndex(lua_State* L)
{
    ParticlePool2D::Particle** self = static_cast<ParticlePool2D::Particle**>(luaL_checkudata(L, 1, ID_Particle2D.data()));
    const char* k = luaL_checkstring(L, 2);

    switch (MapParticle2DMember(k)) {
    case Particle2DMember::x:
        lua_pushnumber(L, (*self)->pos.x);
        break;
    case Particle2DMember::y:
        lua_pushnumber(L, (*self)->pos.y);
        break;
    case Particle2DMember::ax:
        lua_pushnumber(L, (*self)->accel.x);
        break;
    case Particle2DMember::ay:
        lua_pushnumber(L, (*self)->accel.y);
        break;
    case Particle2DMember::sx:
        lua_pushnumber(L, (*self)->scale.x);
        break;
    case Particle2DMember::sy:
        lua_pushnumber(L, (*self)->scale.y);
        break;
    case Particle2DMember::vx:
        lua_pushnumber(L, (*self)->vel.x);
        break;
    case Particle2DMember::vy:
        lua_pushnumber(L, (*self)->vel.y);
        break;
    case Particle2DMember::a:
        lua_pushnumber(L, (*self)->color.a);
        break;
    case Particle2DMember::r:
        lua_pushnumber(L, (*self)->color.r);
        break;
    case Particle2DMember::g:
        lua_pushnumber(L, (*self)->color.g);
        break;
    case Particle2DMember::b:
        lua_pushnumber(L, (*self)->color.b);
        break;
    case Particle2DMember::color:
        LuaWrapper::ColorWrapper::CreateAndPush(L, (*self)->color);
        break;
    case Particle2DMember::rot:
        lua_pushnumber(L, (*self)->rot * L_RAD_TO_DEG);
        break;
    case Particle2DMember::omiga:
        lua_pushnumber(L, (*self)->omiga * L_RAD_TO_DEG);
        break;
    case Particle2DMember::timer:
        lua_pushinteger(L, (*self)->timer);
        break;
    case Particle2DMember::extra1:
        lua_pushnumber(L, (*self)->extra1);
        break;
    case Particle2DMember::extra2:
        lua_pushnumber(L, (*self)->extra2);
        break;
    case Particle2DMember::extra3:
        lua_pushnumber(L, (*self)->extra3);
        break;
    case Particle2DMember::pos:
        LuaWrapper::Vector2Wrapper::CreateAndPush(L, (*self)->pos);
        break;
    case Particle2DMember::vel:
        LuaWrapper::Vector2Wrapper::CreateAndPush(L, (*self)->vel);
        break;
    case Particle2DMember::accel:
        LuaWrapper::Vector2Wrapper::CreateAndPush(L, (*self)->accel);
        break;
    case Particle2DMember::scale:
        LuaWrapper::Vector2Wrapper::CreateAndPush(L, (*self)->scale);
        break;
    case Particle2DMember::speed:
        lua_pushnumber(L, (*self)->vel.length());
        break;
    case Particle2DMember::angle:
        lua_pushnumber(L, (*self)->vel.angle());
        break;
    default:
        lua_pushnil(L);
        break;
    }

    return 1;
}

int lua_particle2d_mtNewIndex(lua_State* L)
{
    ParticlePool2D::Particle** self = static_cast<ParticlePool2D::Particle**>(luaL_checkudata(L, 1, ID_Particle2D.data()));
    const char* k = luaL_checkstring(L, 2);


    switch (MapParticle2DMember(k)) {
    case Particle2DMember::x:
        (*self)->pos.x = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::y:
        (*self)->pos.y = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::ax:
        (*self)->accel.x = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::ay:
        (*self)->accel.y = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::sx:
        (*self)->scale.x = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::sy:
        (*self)->scale.y = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::vx:
        (*self)->vel.x = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::vy:
        (*self)->vel.y = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::a:
        (*self)->color.a = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::r:
        (*self)->color.r = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::g:
        (*self)->color.g = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::b:
        (*self)->color.b = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::color:
        (*self)->color = *LuaWrapper::ColorWrapper::Cast(L, 3);
        break;
    case Particle2DMember::rot:
        (*self)->rot = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case Particle2DMember::omiga:
        (*self)->omiga = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case Particle2DMember::timer:
        (*self)->timer = luaL_checkinteger(L, 3);
        break;
    case Particle2DMember::extra1:
        (*self)->extra1 = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::extra2:
        (*self)->extra2 = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::extra3:
        (*self)->extra3 = luaL_checknumber(L, 3);
        break;
    case Particle2DMember::pos:
        (*self)->pos = *LuaWrapper::Vector2Wrapper::Cast(L, 3);
        break;
    case Particle2DMember::vel:
        (*self)->vel = *LuaWrapper::Vector2Wrapper::Cast(L, 3);
        break;
    case Particle2DMember::accel:
        (*self)->accel = *LuaWrapper::Vector2Wrapper::Cast(L, 3);
        break;
    case Particle2DMember::scale:
        (*self)->scale = *LuaWrapper::Vector2Wrapper::Cast(L, 3);
        break;
    case Particle2DMember::speed:
        (*self)->vel = (*self)->vel.normalize() * luaL_checknumber(L, 3);
        break;
    case Particle2DMember::angle:
        (*self)->vel = Core::Vector2F(std::cos(luaL_checknumber(L, 3)), std::sin(luaL_checknumber(L, 3))) 
                       * (*self)->vel.normalize();
        break;
    default:
        return luaL_error(L, "Attempted to set an invalid particle index.");
        break;
    }

    return 0;
}

int lua_particle2d_mtToString(lua_State* L)
{
    ParticlePool2D::Particle** self = static_cast<ParticlePool2D::Particle**>(luaL_checkudata(L, 1, ID_Particle2D.data()));

    lua_pushfstring(L, "Particle2D(pos = %d, %d)", (*self)->pos.x, (*self)->pos.y);

    return 1;
}

int lua_NewPool3D(lua_State* L)
{
    int const argc = lua_gettop(L);
    int32_t poolsize = 512;
    BlendMode blend = BlendMode::MulAlpha;
    //Core::ScopeObject<Core::Graphics::ISprite> img;
    Core::ScopeObject<LuaSTGPlus::IResourceSprite> img;

    if (argc < 1)
        return luaL_error(L, "must provide image resource for particle system");

    img = LAPP.GetResourceMgr().FindSprite(luaL_checkstring(L, 1));
    if (!img)
        return luaL_error(L, "can't find sprite '%s'", luaL_checkstring(L, 1));

    if (argc >= 2)
        blend = TranslateBlendMode(L, 2);
    if (argc >= 3)
        poolsize = luaL_checkint(L, 3);

    ParticlePool3D** pool = static_cast<ParticlePool3D**>(lua_newuserdata(L, sizeof(**pool)));
    luaL_setmetatable(L, ID_Pool3D.data());
    *pool = new ParticlePool3D(poolsize, img, blend);

    return 1;
}

int lua_pool3d_AddParticle(lua_State* L)
{
    ParticlePool3D** self = static_cast<ParticlePool3D**>(luaL_checkudata(L, 1, ID_Pool3D.data()));
    int const argc = lua_gettop(L);

    ParticlePool3D::Particle p{
        .pos = Core::Vector3F(luaL_checknumber(L, 2), luaL_checknumber(L, 3),luaL_checknumber(L, 4)),
        .vel = Core::Vector3F(luaL_optnumber(L, 5, 0), luaL_optnumber(L, 6, 0), luaL_optnumber(L, 7, 0)),
        .rot = Core::Vector3F(luaL_optnumber(L, 8, 0), luaL_optnumber(L, 9, 0), luaL_optnumber(L, 10, 0)),
        .scale = Core::Vector2F(luaL_optnumber(L, 11, 1), luaL_optnumber(L, 12, luaL_optnumber(L, 11, 1))),
        .color = Core::Color4B(0xFFFFFFFF),
    };

    ParticlePool3D::Particle** ud = static_cast<ParticlePool3D::Particle**>(lua_newuserdata(L, sizeof(**ud)));
    luaL_setmetatable(L, ID_Particle3D.data());
    new (ud) ParticlePool3D::Particle* { (*self)->AddParticle(std::move(p)) };

    return 1;
}

int lua_pool3d_Update(lua_State* L)
{
    ParticlePool3D** self = static_cast<ParticlePool3D**>(luaL_checkudata(L, 1, ID_Pool3D.data()));
    (*self)->Update();
    return 0;
}

int lua_pool3d_Render(lua_State* L)
{
    ParticlePool3D** self = static_cast<ParticlePool3D**>(luaL_checkudata(L, 1, ID_Pool3D.data()));
    (*self)->Render();
    return 0;
}

int lua_pool3d_Apply(lua_State* L)
{
    ParticlePool3D** self = static_cast<ParticlePool3D**>(luaL_checkudata(L, 1, ID_Pool3D.data()));

    int r_fn = luaL_ref(L, LUA_REGISTRYINDEX);

    (*self)->Apply([&](auto p) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, r_fn);

        ParticlePool3D::Particle** ud = static_cast<ParticlePool3D::Particle**>(lua_newuserdata(L, sizeof(**ud)));
        *ud = p;
        luaL_getmetatable(L, ID_Particle3D.data());
        lua_setmetatable(L, -2);


        lua_call(L, 1, 1);
        bool ret = lua_toboolean(L, -1);
        lua_pop(L, 1);

        return ret;
    });

    luaL_unref(L, LUA_REGISTRYINDEX, r_fn);
    return 0;
}

int lua_pool3d_Clear(lua_State* L)
{
    ParticlePool3D** self = static_cast<ParticlePool3D**>(luaL_checkudata(L, 1, ID_Pool3D.data()));
    (*self)->Clear();
    return 0;
}

int lua_pool3d_GetSize(lua_State* L)
{
    ParticlePool3D** self = static_cast<ParticlePool3D**>(luaL_checkudata(L, 1, ID_Pool3D.data()));

    lua_pushnumber(L, (*self)->GetSize());

    return 1;
}

int lua_pool3d_mtToString(lua_State* L)
{
    ParticlePool3D** self = static_cast<ParticlePool3D**>(luaL_checkudata(L, 1, ID_Pool3D.data()));

    lua_pushfstring(L, "ParticlePool3D(size = %d)", (*self)->GetSize());

    return 1;
}

int lua_pool3d_mtGC(lua_State* L)
{
    ParticlePool3D** self = static_cast<ParticlePool3D**>(luaL_checkudata(L, 1, ID_Pool3D.data()));

    delete *self;

    return 0;
}

int lua_particle3d_mtIndex(lua_State* L)
{
    ParticlePool3D::Particle** self = static_cast<ParticlePool3D::Particle**>(luaL_checkudata(L, 1, ID_Particle3D.data()));
    const char* k = luaL_checkstring(L, 2);

    switch (MapParticle3DMember(k)) {
    case Particle3DMember::x:
        lua_pushnumber(L, (*self)->pos.x);
        break;
    case Particle3DMember::y:
        lua_pushnumber(L, (*self)->pos.y);
        break;
    case Particle3DMember::z:
        lua_pushnumber(L, (*self)->pos.z);
        break;
    case Particle3DMember::ax:
        lua_pushnumber(L, (*self)->accel.x);
        break;
    case Particle3DMember::ay:
        lua_pushnumber(L, (*self)->accel.y);
        break;
    case Particle3DMember::az:
        lua_pushnumber(L, (*self)->accel.z);
        break;
    case Particle3DMember::ox:
        lua_pushnumber(L, (*self)->omiga.x * L_RAD_TO_DEG);
        break;
    case Particle3DMember::oy:
        lua_pushnumber(L, (*self)->omiga.y * L_RAD_TO_DEG);
        break;
    case Particle3DMember::oz:
        lua_pushnumber(L, (*self)->omiga.z * L_RAD_TO_DEG);
        break;
    case Particle3DMember::rx:
        lua_pushnumber(L, (*self)->rot.x * L_RAD_TO_DEG);
        break;
    case Particle3DMember::ry:
        lua_pushnumber(L, (*self)->rot.y * L_RAD_TO_DEG);
        break;
    case Particle3DMember::rz:
        lua_pushnumber(L, (*self)->rot.z * L_RAD_TO_DEG);
        break;
    case Particle3DMember::sx:
        lua_pushnumber(L, (*self)->scale.x);
        break;
    case Particle3DMember::sy:
        lua_pushnumber(L, (*self)->scale.y);
        break;
    case Particle3DMember::vx:
        lua_pushnumber(L, (*self)->vel.x);
        break;
    case Particle3DMember::vy:
        lua_pushnumber(L, (*self)->vel.y);
        break;
    case Particle3DMember::vz:
        lua_pushnumber(L, (*self)->vel.z);
        break;
    case Particle3DMember::a:
        lua_pushnumber(L, (*self)->color.a);
        break;
    case Particle3DMember::r:
        lua_pushnumber(L, (*self)->color.r);
        break;
    case Particle3DMember::g:
        lua_pushnumber(L, (*self)->color.g);
        break;
    case Particle3DMember::b:
        lua_pushnumber(L, (*self)->color.b);
        break;
    case Particle3DMember::color:
        LuaWrapper::ColorWrapper::CreateAndPush(L, (*self)->color);
        break;
    case Particle3DMember::timer:
        lua_pushinteger(L, (*self)->timer);
        break;
    case Particle3DMember::extra1:
        lua_pushnumber(L, (*self)->extra1);
        break;
    case Particle3DMember::extra2:
        lua_pushnumber(L, (*self)->extra2);
        break;
    case Particle3DMember::extra3:
        lua_pushnumber(L, (*self)->extra3);
        break;
    case Particle3DMember::extra4:
        lua_pushnumber(L, (*self)->extra4);
        break;
    case Particle3DMember::pos:
        LuaWrapper::Vector3Wrapper::CreateAndPush(L, (*self)->pos);
        break;
    case Particle3DMember::vel:
        LuaWrapper::Vector3Wrapper::CreateAndPush(L, (*self)->vel);
        break;
    case Particle3DMember::accel:
        LuaWrapper::Vector3Wrapper::CreateAndPush(L, (*self)->accel);
        break;
    case Particle3DMember::scale:
        LuaWrapper::Vector2Wrapper::CreateAndPush(L, (*self)->scale);
        break;
    case Particle3DMember::speed:
        lua_pushnumber(L, (*self)->vel.length());
        break;
    default:
        lua_pushnil(L);
        break;
    }

    return 1;
}

int lua_particle3d_mtNewIndex(lua_State* L)
{
    ParticlePool3D::Particle** self = static_cast<ParticlePool3D::Particle**>(luaL_checkudata(L, 1, ID_Particle3D.data()));
    const char* k = luaL_checkstring(L, 2);


    switch (MapParticle3DMember(k)) {
    case Particle3DMember::x:
        (*self)->pos.x = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::y:
        (*self)->pos.y = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::z:
        (*self)->pos.z = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::ax:
        (*self)->accel.x = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::ay:
        (*self)->accel.y = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::az:
        (*self)->accel.z = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::ox:
        (*self)->omiga.x = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case Particle3DMember::oy:
        (*self)->omiga.y = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case Particle3DMember::oz:
        (*self)->omiga.z = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case Particle3DMember::rx:
        (*self)->rot.x = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case Particle3DMember::ry:
        (*self)->rot.y = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case Particle3DMember::rz:
        (*self)->rot.z = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case Particle3DMember::sx:
        (*self)->scale.x = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::sy:
        (*self)->scale.y = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::vx:
        (*self)->vel.x = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::vy:
        (*self)->vel.y = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::vz:
        (*self)->vel.z = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::a:
        (*self)->color.a = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::r:
        (*self)->color.r = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::g:
        (*self)->color.g = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::b:
        (*self)->color.b = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::color:
        (*self)->color = *LuaWrapper::ColorWrapper::Cast(L, 3);
        break;
    case Particle3DMember::timer:
        (*self)->timer = luaL_checkinteger(L, 3);
        break;
    case Particle3DMember::extra1:
        (*self)->extra1 = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::extra2:
        (*self)->extra2 = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::extra3:
        (*self)->extra3 = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::extra4:
        (*self)->extra4 = luaL_checknumber(L, 3);
        break;
    case Particle3DMember::pos:
        (*self)->pos = *LuaWrapper::Vector3Wrapper::Cast(L, 3);
        break;
    case Particle3DMember::vel:
        (*self)->vel = *LuaWrapper::Vector3Wrapper::Cast(L, 3);
        break;
    case Particle3DMember::accel:
        (*self)->accel = *LuaWrapper::Vector3Wrapper::Cast(L, 3);
        break;
    case Particle3DMember::scale:
        (*self)->scale = *LuaWrapper::Vector2Wrapper::Cast(L, 3);
        break;
    case Particle3DMember::speed:
        (*self)->vel = (*self)->vel.normalize() * luaL_checknumber(L, 3);
        break;
    default:
        return luaL_error(L, "Attempted to set an invalid particle index.");
    }

    return 0;
}

int lua_particle3d_mtToString(lua_State* L)
{
    ParticlePool3D::Particle** self = static_cast<ParticlePool3D::Particle**>(luaL_checkudata(L, 1, ID_Particle3D.data()));

    lua_pushfstring(L, "Particle3D(pos = %d, %d, %d)", (*self)->pos.x, (*self)->pos.y, (*self)->pos.z);

    return 1;
}

int lua_NewTexPool2D(lua_State* L)
{
    int const argc = lua_gettop(L);
    int32_t poolsize = 512;
    BlendMode blend = BlendMode::MulAlpha;
    Core::ScopeObject<LuaSTGPlus::IResourceTexture> tex;

    if (argc < 1)
        return luaL_error(L, "must provide texture resource for particle system");

    tex = LAPP.GetResourceMgr().FindTexture(luaL_checkstring(L, 1));
    if (!tex)
        return luaL_error(L, "can't find texture '%s'", luaL_checkstring(L, 1));

    if (argc >= 2)
        blend = TranslateBlendMode(L, 2);
    if (argc >= 3)
        poolsize = luaL_checkint(L, 3);

    TexParticlePool2D** pool = static_cast<TexParticlePool2D**>(lua_newuserdata(L, sizeof(**pool)));
    luaL_setmetatable(L, ID_TexPool2D.data());
    *pool = new TexParticlePool2D(poolsize, tex, blend);

    return 1;
}

int lua_texpool2d_AddParticle(lua_State* L)
{
    TexParticlePool2D** self = static_cast<TexParticlePool2D**>(luaL_checkudata(L, 1, ID_TexPool2D.data()));
    int const argc = lua_gettop(L);

    TexParticlePool2D::Particle p{
        .pos = Core::Vector2F(luaL_checknumber(L, 6), luaL_checknumber(L, 7)),
        .vel = Core::Vector2F((float)luaL_optnumber(L, 9, 0), (float)luaL_optnumber(L, 10, 0)),
        .scale = Core::Vector2F(1, 1),
        .rot = (float)(L_DEG_TO_RAD * luaL_optnumber(L, 8, 0)),
        .color = Core::Color4B(0xFFFFFFFF),
        .uv = Core::RectI(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5)),
    };

    if (argc == 7)
    {
        float s = luaL_checknumber(L, 7);
        p.scale = Core::Vector2F(s, s);
    }
    else if (argc > 7)
    {
        p.scale = Core::Vector2F(luaL_checknumber(L, 7), luaL_checknumber(L, 8));
    }

    TexParticlePool2D::Particle** ud = static_cast<TexParticlePool2D::Particle**>(lua_newuserdata(L, sizeof(**ud)));
    luaL_setmetatable(L, ID_TexParticle2D.data());
    new (ud) TexParticlePool2D::Particle* { (*self)->AddParticle(std::move(p)) };

    return 1;
}

int lua_texpool2d_Update(lua_State* L)
{
    TexParticlePool2D** self = static_cast<TexParticlePool2D**>(luaL_checkudata(L, 1, ID_TexPool2D.data()));
    (*self)->Update();
    return 0;
}

int lua_texpool2d_Render(lua_State* L)
{
    TexParticlePool2D** self = static_cast<TexParticlePool2D**>(luaL_checkudata(L, 1, ID_TexPool2D.data()));
    (*self)->Render();
    return 0;
}

int lua_texpool2d_Apply(lua_State* L)
{
    TexParticlePool2D** self = static_cast<TexParticlePool2D**>(luaL_checkudata(L, 1, ID_TexPool2D.data()));

    int r_fn = luaL_ref(L, LUA_REGISTRYINDEX);

    (*self)->Apply([&](auto p) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, r_fn);

        TexParticlePool2D::Particle** ud = static_cast<TexParticlePool2D::Particle**>(lua_newuserdata(L, sizeof(**ud)));
        *ud = p;
        luaL_getmetatable(L, ID_TexParticle2D.data());
        lua_setmetatable(L, -2);


        lua_call(L, 1, 1);
        bool ret = lua_toboolean(L, -1);
        lua_pop(L, 1);

        return ret;
    });

    luaL_unref(L, LUA_REGISTRYINDEX, r_fn);
    return 0;
}

int lua_texpool2d_Clear(lua_State* L)
{
    TexParticlePool2D** self = static_cast<TexParticlePool2D**>(luaL_checkudata(L, 1, ID_TexPool2D.data()));
    (*self)->Clear();
    return 0;
}

int lua_texpool2d_GetSize(lua_State* L)
{
    TexParticlePool2D** self = static_cast<TexParticlePool2D**>(luaL_checkudata(L, 1, ID_TexPool2D.data()));

    lua_pushnumber(L, (*self)->GetSize());

    return 1;
}

int lua_texpool2d_mtToString(lua_State* L)
{
    TexParticlePool2D** self = static_cast<TexParticlePool2D**>(luaL_checkudata(L, 1, ID_TexPool2D.data()));

    lua_pushfstring(L, "TexParticlePool2D(size = %d)", (*self)->GetSize());

    return 1;
}

int lua_texpool2d_mtGC(lua_State* L)
{
    TexParticlePool2D** self = static_cast<TexParticlePool2D**>(luaL_checkudata(L, 1, ID_TexPool2D.data()));

    delete *self;

    return 0;
}

int lua_texparticle2d_mtIndex(lua_State* L)
{
    TexParticlePool2D::Particle** self = static_cast<TexParticlePool2D::Particle**>(luaL_checkudata(L, 1, ID_TexParticle2D.data()));
    const char* k = luaL_checkstring(L, 2);

    switch (MapTexParticle2DMember(k)) {
    case TexParticle2DMember::x:
        lua_pushnumber(L, (*self)->pos.x);
        break;
    case TexParticle2DMember::y:
        lua_pushnumber(L, (*self)->pos.y);
        break;
    case TexParticle2DMember::ax:
        lua_pushnumber(L, (*self)->accel.x);
        break;
    case TexParticle2DMember::ay:
        lua_pushnumber(L, (*self)->accel.y);
        break;
    case TexParticle2DMember::sx:
        lua_pushnumber(L, (*self)->scale.x);
        break;
    case TexParticle2DMember::sy:
        lua_pushnumber(L, (*self)->scale.y);
        break;
    case TexParticle2DMember::vx:
        lua_pushnumber(L, (*self)->vel.x);
        break;
    case TexParticle2DMember::vy:
        lua_pushnumber(L, (*self)->vel.y);
        break;
    case TexParticle2DMember::a:
        lua_pushnumber(L, (*self)->color.a);
        break;
    case TexParticle2DMember::r:
        lua_pushnumber(L, (*self)->color.r);
        break;
    case TexParticle2DMember::g:
        lua_pushnumber(L, (*self)->color.g);
        break;
    case TexParticle2DMember::b:
        lua_pushnumber(L, (*self)->color.b);
        break;
    case TexParticle2DMember::color:
        LuaWrapper::ColorWrapper::CreateAndPush(L, (*self)->color);
        break;
    case TexParticle2DMember::rot:
        lua_pushnumber(L, (*self)->rot * L_RAD_TO_DEG);
        break;
    case TexParticle2DMember::omiga:
        lua_pushnumber(L, (*self)->omiga * L_RAD_TO_DEG);
        break;
    case TexParticle2DMember::timer:
        lua_pushinteger(L, (*self)->timer);
        break;
    case TexParticle2DMember::extra1:
        lua_pushnumber(L, (*self)->extra1);
        break;
    case TexParticle2DMember::extra2:
        lua_pushnumber(L, (*self)->extra2);
        break;
    case TexParticle2DMember::extra3:
        lua_pushnumber(L, (*self)->extra3);
        break;
    case TexParticle2DMember::u:
        lua_pushinteger(L, (*self)->uv.a.x);
        break;
    case TexParticle2DMember::v:
        lua_pushinteger(L, (*self)->uv.a.y);
        break;
    case TexParticle2DMember::w:
        lua_pushinteger(L, (*self)->uv.width());
        break;
    case TexParticle2DMember::h:
        lua_pushinteger(L, (*self)->uv.height());
        break;
    case TexParticle2DMember::pos:
        LuaWrapper::Vector2Wrapper::CreateAndPush(L, (*self)->pos);
        break;
    case TexParticle2DMember::vel:
        LuaWrapper::Vector2Wrapper::CreateAndPush(L, (*self)->vel);
        break;
    case TexParticle2DMember::accel:
        LuaWrapper::Vector2Wrapper::CreateAndPush(L, (*self)->accel);
        break;
    case TexParticle2DMember::scale:
        LuaWrapper::Vector2Wrapper::CreateAndPush(L, (*self)->scale);
        break;
    case TexParticle2DMember::speed:
        lua_pushnumber(L, (*self)->vel.length());
        break;
    case TexParticle2DMember::angle:
        lua_pushnumber(L, (*self)->vel.angle());
        break;
    default:
        lua_pushnil(L);
        break;
    }

    return 1;
}

int lua_texparticle2d_mtNewIndex(lua_State* L)
{
    TexParticlePool2D::Particle** self = static_cast<TexParticlePool2D::Particle**>(luaL_checkudata(L, 1, ID_TexParticle2D.data()));
    const char* k = luaL_checkstring(L, 2);


    switch (MapTexParticle2DMember(k)) {
    case TexParticle2DMember::x:
        (*self)->pos.x = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::y:
        (*self)->pos.y = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::ax:
        (*self)->accel.x = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::ay:
        (*self)->accel.y = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::sx:
        (*self)->scale.x = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::sy:
        (*self)->scale.y = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::vx:
        (*self)->vel.x = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::vy:
        (*self)->vel.y = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::a:
        (*self)->color.a = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::r:
        (*self)->color.r = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::g:
        (*self)->color.g = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::b:
        (*self)->color.b = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::color:
        (*self)->color = *LuaWrapper::ColorWrapper::Cast(L, 3);
        break;
    case TexParticle2DMember::rot:
        (*self)->rot = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case TexParticle2DMember::omiga:
        (*self)->omiga = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case TexParticle2DMember::timer:
        (*self)->timer = luaL_checkinteger(L, 3);
        break;
    case TexParticle2DMember::extra1:
        (*self)->extra1 = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::extra2:
        (*self)->extra2 = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::extra3:
        (*self)->extra3 = luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::u:
        (*self)->uv.a.x = luaL_checkinteger(L, 3);
        break;
    case TexParticle2DMember::v:
        (*self)->uv.a.y = luaL_checkinteger(L, 3);
        break;
    case TexParticle2DMember::w:
        (*self)->uv.b.x = (*self)->uv.a.x + luaL_checkinteger(L, 3);
        break;
    case TexParticle2DMember::h:
        (*self)->uv.b.y = (*self)->uv.a.y + luaL_checkinteger(L, 3);
        break;
    case TexParticle2DMember::pos:
        (*self)->pos = *LuaWrapper::Vector2Wrapper::Cast(L, 3);
        break;
    case TexParticle2DMember::vel:
        (*self)->vel = *LuaWrapper::Vector2Wrapper::Cast(L, 3);
        break;
    case TexParticle2DMember::accel:
        (*self)->accel = *LuaWrapper::Vector2Wrapper::Cast(L, 3);
        break;
    case TexParticle2DMember::scale:
        (*self)->scale = *LuaWrapper::Vector2Wrapper::Cast(L, 3);
        break;
    case TexParticle2DMember::speed:
        (*self)->vel = (*self)->vel.normalize() * luaL_checknumber(L, 3);
        break;
    case TexParticle2DMember::angle:
        (*self)->vel = Core::Vector2F(std::cos(luaL_checknumber(L, 3)), std::sin(luaL_checknumber(L, 3))) 
                       * (*self)->vel.normalize();
        break;
    default:
        return luaL_error(L, "Attempted to set an invalid particle index.");
        break;
    }

    return 0;
}

int lua_texparticle2d_mtToString(lua_State* L)
{
    TexParticlePool2D::Particle** self = static_cast<TexParticlePool2D::Particle**>(luaL_checkudata(L, 1, ID_TexParticle2D.data()));

    lua_pushfstring(L, "TexParticle2D(pos = %d, %d)", (*self)->pos.x, (*self)->pos.y);

    return 1;
}

int lua_NewTexPool3D(lua_State* L)
{
    int const argc = lua_gettop(L);
    int32_t poolsize = 512;
    BlendMode blend = BlendMode::MulAlpha;
    Core::ScopeObject<LuaSTGPlus::IResourceTexture> tex;

    if (argc < 1)
        return luaL_error(L, "must provide texture resource for particle system");

    tex = LAPP.GetResourceMgr().FindTexture(luaL_checkstring(L, 1));
    if (!tex)
        return luaL_error(L, "can't find texture '%s'", luaL_checkstring(L, 1));

    if (argc >= 2)
        blend = TranslateBlendMode(L, 2);
    if (argc >= 3)
        poolsize = luaL_checkint(L, 3);

    TexParticlePool3D** pool = static_cast<TexParticlePool3D**>(lua_newuserdata(L, sizeof(**pool)));
    luaL_setmetatable(L, ID_TexPool3D.data());
    *pool = new TexParticlePool3D(poolsize, tex, blend);

    return 1;
}

int lua_texpool3d_AddParticle(lua_State* L)
{
    TexParticlePool3D** self = static_cast<TexParticlePool3D**>(luaL_checkudata(L, 1, ID_TexPool3D.data()));
    int const argc = lua_gettop(L);

    TexParticlePool3D::Particle p{
        .pos = Core::Vector3F(luaL_checknumber(L, 6), luaL_checknumber(L, 7),luaL_checknumber(L, 8)),
        .vel = Core::Vector3F(luaL_optnumber(L, 9, 0), luaL_optnumber(L, 10, 0), luaL_optnumber(L, 11, 0)),
        .rot = Core::Vector3F(luaL_optnumber(L, 12, 0), luaL_optnumber(L, 13, 0), luaL_optnumber(L, 14, 0)),
        .scale = Core::Vector2F(0.01f, 0.01f),
        .color = Core::Color4B(0xFFFFFFFF),
        .uv = Core::RectI(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5)),
    };

    TexParticlePool3D::Particle** ud = static_cast<TexParticlePool3D::Particle**>(lua_newuserdata(L, sizeof(**ud)));
    luaL_setmetatable(L, ID_TexParticle3D.data());
    new (ud) TexParticlePool3D::Particle* { (*self)->AddParticle(std::move(p)) };

    return 1;
}

int lua_texpool3d_Update(lua_State* L)
{
    TexParticlePool3D** self = static_cast<TexParticlePool3D**>(luaL_checkudata(L, 1, ID_TexPool3D.data()));
    (*self)->Update();
    return 0;
}

int lua_texpool3d_Render(lua_State* L)
{
    TexParticlePool3D** self = static_cast<TexParticlePool3D**>(luaL_checkudata(L, 1, ID_TexPool3D.data()));
    (*self)->Render();
    return 0;
}

int lua_texpool3d_Apply(lua_State* L)
{
    TexParticlePool3D** self = static_cast<TexParticlePool3D**>(luaL_checkudata(L, 1, ID_TexPool3D.data()));

    int r_fn = luaL_ref(L, LUA_REGISTRYINDEX);

    (*self)->Apply([&](auto p) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, r_fn);

        TexParticlePool3D::Particle** ud = static_cast<TexParticlePool3D::Particle**>(lua_newuserdata(L, sizeof(**ud)));
        *ud = p;
        luaL_getmetatable(L, ID_TexParticle3D.data());
        lua_setmetatable(L, -2);


        lua_call(L, 1, 1);
        bool ret = lua_toboolean(L, -1);
        lua_pop(L, 1);

        return ret;
    });

    luaL_unref(L, LUA_REGISTRYINDEX, r_fn);
    return 0;
}

int lua_texpool3d_Clear(lua_State* L)
{
    TexParticlePool3D** self = static_cast<TexParticlePool3D**>(luaL_checkudata(L, 1, ID_TexPool3D.data()));
    (*self)->Clear();
    return 0;
}

int lua_texpool3d_GetSize(lua_State* L)
{
    TexParticlePool3D** self = static_cast<TexParticlePool3D**>(luaL_checkudata(L, 1, ID_TexPool3D.data()));

    lua_pushnumber(L, (*self)->GetSize());

    return 1;
}

int lua_texpool3d_mtToString(lua_State* L)
{
    TexParticlePool3D** self = static_cast<TexParticlePool3D**>(luaL_checkudata(L, 1, ID_TexPool3D.data()));

    lua_pushfstring(L, "TexParticlePool3D(size = %d)", (*self)->GetSize());

    return 1;
}

int lua_texpool3d_mtGC(lua_State* L)
{
    TexParticlePool3D** self = static_cast<TexParticlePool3D**>(luaL_checkudata(L, 1, ID_TexPool3D.data()));

    delete *self;

    return 0;
}

int lua_texparticle3d_mtIndex(lua_State* L)
{
    TexParticlePool3D::Particle** self = static_cast<TexParticlePool3D::Particle**>(luaL_checkudata(L, 1, ID_TexParticle3D.data()));
    const char* k = luaL_checkstring(L, 2);

    switch (MapTexParticle3DMember(k)) {
    case TexParticle3DMember::x:
        lua_pushnumber(L, (*self)->pos.x);
        break;
    case TexParticle3DMember::y:
        lua_pushnumber(L, (*self)->pos.y);
        break;
    case TexParticle3DMember::z:
        lua_pushnumber(L, (*self)->pos.z);
        break;
    case TexParticle3DMember::ax:
        lua_pushnumber(L, (*self)->accel.x);
        break;
    case TexParticle3DMember::ay:
        lua_pushnumber(L, (*self)->accel.y);
        break;
    case TexParticle3DMember::az:
        lua_pushnumber(L, (*self)->accel.z);
        break;
    case TexParticle3DMember::ox:
        lua_pushnumber(L, (*self)->omiga.x * L_RAD_TO_DEG);
        break;
    case TexParticle3DMember::oy:
        lua_pushnumber(L, (*self)->omiga.y * L_RAD_TO_DEG);
        break;
    case TexParticle3DMember::oz:
        lua_pushnumber(L, (*self)->omiga.z * L_RAD_TO_DEG);
        break;
    case TexParticle3DMember::rx:
        lua_pushnumber(L, (*self)->rot.x * L_RAD_TO_DEG);
        break;
    case TexParticle3DMember::ry:
        lua_pushnumber(L, (*self)->rot.y * L_RAD_TO_DEG);
        break;
    case TexParticle3DMember::rz:
        lua_pushnumber(L, (*self)->rot.z * L_RAD_TO_DEG);
        break;
    case TexParticle3DMember::sx:
        lua_pushnumber(L, (*self)->scale.x);
        break;
    case TexParticle3DMember::sy:
        lua_pushnumber(L, (*self)->scale.y);
        break;
    case TexParticle3DMember::vx:
        lua_pushnumber(L, (*self)->vel.x);
        break;
    case TexParticle3DMember::vy:
        lua_pushnumber(L, (*self)->vel.y);
        break;
    case TexParticle3DMember::vz:
        lua_pushnumber(L, (*self)->vel.z);
        break;
    case TexParticle3DMember::a:
        lua_pushnumber(L, (*self)->color.a);
        break;
    case TexParticle3DMember::r:
        lua_pushnumber(L, (*self)->color.r);
        break;
    case TexParticle3DMember::g:
        lua_pushnumber(L, (*self)->color.g);
        break;
    case TexParticle3DMember::b:
        lua_pushnumber(L, (*self)->color.b);
        break;
    case TexParticle3DMember::color:
        LuaWrapper::ColorWrapper::CreateAndPush(L, (*self)->color);
        break;
    case TexParticle3DMember::timer:
        lua_pushinteger(L, (*self)->timer);
        break;
    case TexParticle3DMember::extra1:
        lua_pushnumber(L, (*self)->extra1);
        break;
    case TexParticle3DMember::extra2:
        lua_pushnumber(L, (*self)->extra2);
        break;
    case TexParticle3DMember::extra3:
        lua_pushnumber(L, (*self)->extra3);
        break;
    case TexParticle3DMember::extra4:
        lua_pushnumber(L, (*self)->extra4);
        break;
    case TexParticle3DMember::u:
        lua_pushinteger(L, (*self)->uv.a.x);
        break;
    case TexParticle3DMember::v:
        lua_pushinteger(L, (*self)->uv.a.y);
        break;
    case TexParticle3DMember::w:
        lua_pushinteger(L, (*self)->uv.width());
        break;
    case TexParticle3DMember::h:
        lua_pushinteger(L, (*self)->uv.height());
        break;
    case TexParticle3DMember::pos:
        LuaWrapper::Vector3Wrapper::CreateAndPush(L, (*self)->pos);
        return 1;
    case TexParticle3DMember::vel:
        LuaWrapper::Vector3Wrapper::CreateAndPush(L, (*self)->vel);
        return 1;
    case TexParticle3DMember::accel:
        LuaWrapper::Vector3Wrapper::CreateAndPush(L, (*self)->accel);
        return 1;
    case TexParticle3DMember::scale:
        LuaWrapper::Vector2Wrapper::CreateAndPush(L, (*self)->scale);
        return 1;
    case TexParticle3DMember::speed:
        lua_pushnumber(L, (*self)->vel.length());
        break;
    default:
        lua_pushnil(L);
        break;
    }

    return 1;
}

int lua_texparticle3d_mtNewIndex(lua_State* L)
{
    TexParticlePool3D::Particle** self = static_cast<TexParticlePool3D::Particle**>(luaL_checkudata(L, 1, ID_TexParticle3D.data()));
    const char* k = luaL_checkstring(L, 2);


    switch (MapTexParticle3DMember(k)) {
    case TexParticle3DMember::x:
        (*self)->pos.x = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::y:
        (*self)->pos.y = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::z:
        (*self)->pos.z = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::ax:
        (*self)->accel.x = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::ay:
        (*self)->accel.y = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::az:
        (*self)->accel.z = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::ox:
        (*self)->omiga.x = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case TexParticle3DMember::oy:
        (*self)->omiga.y = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case TexParticle3DMember::oz:
        (*self)->omiga.z = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case TexParticle3DMember::rx:
        (*self)->rot.x = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case TexParticle3DMember::ry:
        (*self)->rot.y = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case TexParticle3DMember::rz:
        (*self)->rot.z = luaL_checknumber(L, 3) * L_DEG_TO_RAD;
        break;
    case TexParticle3DMember::sx:
        (*self)->scale.x = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::sy:
        (*self)->scale.y = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::vx:
        (*self)->vel.x = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::vy:
        (*self)->vel.y = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::vz:
        (*self)->vel.z = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::a:
        (*self)->color.a = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::r:
        (*self)->color.r = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::g:
        (*self)->color.g = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::b:
        (*self)->color.b = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::color:
        (*self)->color = *LuaWrapper::ColorWrapper::Cast(L, 3);
        break;
    case TexParticle3DMember::timer:
        (*self)->timer = luaL_checkinteger(L, 3);
        break;
    case TexParticle3DMember::extra1:
        (*self)->extra1 = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::extra2:
        (*self)->extra2 = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::extra3:
        (*self)->extra3 = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::extra4:
        (*self)->extra4 = luaL_checknumber(L, 3);
        break;
    case TexParticle3DMember::u:
        (*self)->uv.a.x = luaL_checkinteger(L, 3);
        break;
    case TexParticle3DMember::v:
        (*self)->uv.a.y = luaL_checkinteger(L, 3);
        break;
    case TexParticle3DMember::w:
        (*self)->uv.b.x = (*self)->uv.a.x + luaL_checkinteger(L, 3);
        break;
    case TexParticle3DMember::h:
        (*self)->uv.b.y = (*self)->uv.a.y + luaL_checkinteger(L, 3);
        break;
    case TexParticle3DMember::pos:
        (*self)->pos = *LuaWrapper::Vector3Wrapper::Cast(L, 3);
        break;
    case TexParticle3DMember::vel:
        (*self)->vel = *LuaWrapper::Vector3Wrapper::Cast(L, 3);
        break;
    case TexParticle3DMember::accel:
        (*self)->accel = *LuaWrapper::Vector3Wrapper::Cast(L, 3);
        break;
    case TexParticle3DMember::scale:
        (*self)->scale = *LuaWrapper::Vector2Wrapper::Cast(L, 3);
        break;
    case TexParticle3DMember::speed:
        (*self)->vel = (*self)->vel.normalize() * luaL_checknumber(L, 3);
        break;
    default:
        return luaL_error(L, "Attempted to set an invalid particle index.");
    }

    return 0;
}

int lua_texparticle3d_mtToString(lua_State* L)
{
    TexParticlePool3D::Particle** self = static_cast<TexParticlePool3D::Particle**>(luaL_checkudata(L, 1, ID_TexParticle3D.data()));

    lua_pushfstring(L, "TexParticle3D(pos = %d, %d, %d)", (*self)->pos.x, (*self)->pos.y, (*self)->pos.z);

    return 1;
}


luaL_Reg const pool2d_methods[] = {
    { "AddParticle", &lua_pool2d_AddParticle },
    { "Update", &lua_pool2d_Update },
    { "Render", &lua_pool2d_Render },
    { "Apply", &lua_pool2d_Apply },
    { "Clear", &lua_pool2d_Clear },
    { "GetSize", &lua_pool2d_GetSize },

    { NULL, NULL }
};

luaL_Reg const pool2d_mt[] = {
    { "__tostring", &lua_pool2d_mtToString },
    { "__gc", &lua_pool2d_mtGC },

    { NULL, NULL }
};

luaL_Reg particle2d_mt[] = {
    { "__index", &lua_particle2d_mtIndex },
    { "__newindex", &lua_particle2d_mtNewIndex },
    { "__tostring", &lua_particle2d_mtToString },

    { NULL, NULL }
};

luaL_Reg const pool3d_methods[] = {
    { "AddParticle", &lua_pool3d_AddParticle },
    { "Update", &lua_pool3d_Update },
    { "Render", &lua_pool3d_Render },
    { "Apply", &lua_pool3d_Apply },
    { "Clear", &lua_pool3d_Clear },
    { "GetSize", &lua_pool3d_GetSize },

    { NULL, NULL }
};

luaL_Reg const pool3d_mt[] = {
    { "__tostring", &lua_pool3d_mtToString },
    { "__gc", &lua_pool3d_mtGC },

    { NULL, NULL }
};

luaL_Reg particle3d_mt[] = {
    { "__index", &lua_particle3d_mtIndex },
    { "__newindex", &lua_particle3d_mtNewIndex },
    { "__tostring", &lua_particle3d_mtToString },

    { NULL, NULL }
};

luaL_Reg const texpool2d_methods[] = {
    { "AddParticle", &lua_texpool2d_AddParticle },
    { "Update", &lua_texpool2d_Update },
    { "Render", &lua_texpool2d_Render },
    { "Apply", &lua_texpool2d_Apply },
    { "Clear", &lua_texpool2d_Clear },
    { "GetSize", &lua_texpool2d_GetSize },

    { NULL, NULL }
};

luaL_Reg const texpool2d_mt[] = {
    { "__tostring", &lua_texpool2d_mtToString },
    { "__gc", &lua_texpool2d_mtGC },

    { NULL, NULL }
};

luaL_Reg texparticle2d_mt[] = {
    { "__index", &lua_texparticle2d_mtIndex },
    { "__newindex", &lua_texparticle2d_mtNewIndex },
    { "__tostring", &lua_texparticle2d_mtToString },

    { NULL, NULL }
};

luaL_Reg const texpool3d_methods[] = {
    { "AddParticle", &lua_texpool3d_AddParticle },
    { "Update", &lua_texpool3d_Update },
    { "Render", &lua_texpool3d_Render },
    { "Apply", &lua_texpool3d_Apply },
    { "Clear", &lua_texpool3d_Clear },
    { "GetSize", &lua_texpool3d_GetSize },

    { NULL, NULL }
};

luaL_Reg const texpool3d_mt[] = {
    { "__tostring", &lua_texpool3d_mtToString },
    { "__gc", &lua_texpool3d_mtGC },

    { NULL, NULL }
};

luaL_Reg texparticle3d_mt[] = {
    { "__index", &lua_texparticle3d_mtIndex },
    { "__newindex", &lua_texparticle3d_mtNewIndex },
    { "__tostring", &lua_texparticle3d_mtToString },

    { NULL, NULL }
};

luaL_Reg const funcs[] = {
    { "NewPool2D", &lua_NewPool2D },
    { "NewPool3D", &lua_NewPool3D },
    { "NewTexPool2D", &lua_NewTexPool2D },
    { "NewTexPool3D", &lua_NewTexPool3D },

    { NULL, NULL }
};

luaL_Reg nofuncs[] = {
    { NULL, NULL }
};

int luaopen_particle(lua_State* L)
{
    luaL_register(L, "particle", funcs);
    RegisterClassIntoTable2(L, ".Particle2D", nofuncs, ID_Particle2D.data(), particle2d_mt);
    RegisterClassIntoTable(L, ".ParticlePool2D", pool2d_methods, ID_Pool2D.data(), pool2d_mt);
    RegisterClassIntoTable2(L, ".Particle3D", nofuncs, ID_Particle3D.data(), particle3d_mt);
    RegisterClassIntoTable(L, ".ParticlePool3D", pool3d_methods, ID_Pool3D.data(), pool3d_mt);
    RegisterClassIntoTable2(L, ".TexParticle2D", nofuncs, ID_TexParticle2D.data(), texparticle2d_mt);
    RegisterClassIntoTable(L, ".TexParticlePool2D", texpool2d_methods, ID_TexPool2D.data(), texpool2d_mt);
    RegisterClassIntoTable2(L, ".TexParticle3D", nofuncs, ID_TexParticle3D.data(), texparticle3d_mt);
    RegisterClassIntoTable(L, ".TexParticlePool3D", texpool3d_methods, ID_TexPool3D.data(), texpool3d_mt);
    lua_pop(L, 1);

    return 1;
}
