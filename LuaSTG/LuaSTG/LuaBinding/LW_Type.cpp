#include "Core/Type.hpp"
#include "LMathConstant.hpp"
#include "LuaBinding/LuaWrapper.hpp"
#include "LuaBinding/lua_luastg_hash.hpp"
#include "lauxlib.h"
#include "lua.h"
#include "spdlog/spdlog.h"


namespace LuaSTGPlus::LuaWrapper // Rect
{
	std::string_view const RectWrapper::ClassID = "lstg.Rect";

	Core::RectF* RectWrapper::Cast(lua_State* L, int idx)
	{
		return static_cast<Core::RectF*>(luaL_checkudata(L, idx, ClassID.data()));
	}

	void RectWrapper::Register(lua_State* L) noexcept
	{
		struct Function
		{
#define GETUDATA(p, i) Core::RectF* (p) = Cast(L, i);
#define GETVECDATA(p, i) Core::Vector2F* (p) = Vector2Wrapper::Cast(L, i);

			static int GetPointInside(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				GETVECDATA(v, 2);

				Vector2Wrapper::CreateAndPush(L, Core::Vector2F(
					(float)std::clamp(v->x, p->a.x, p->b.x),
					(float)std::clamp(v->y, p->a.y, p->b.y)
				));
				return 1;
			}

			static int IsPointInside(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				GETVECDATA(v, 2);

				if (v->x >= p->a.x && v->x <= p->b.x && v->y >= p->a.y && v->y <= p->b.y) {
					lua_pushboolean(L, 1);
				}
				else {
					lua_pushboolean(L, 0);
				}
				
				return 1;
			}

			static int Meta_Index(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 4)
						return luaL_error(L, "Rect index out of bounds: %d", idx);
					lua_pushnumber(L, (lua_Number)(*p)[idx - 1]);
					return 1;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapRectMember(key))
				{
				case LuaSTG::RectMember::m_l:
					lua_pushnumber(L, (lua_Number)p->a.x);
					break;
				case LuaSTG::RectMember::m_r:
					lua_pushnumber(L, (lua_Number)p->b.x);
					break;
				case LuaSTG::RectMember::m_b:
					lua_pushnumber(L, (lua_Number)p->b.y);
					break;
				case LuaSTG::RectMember::m_t:
					lua_pushnumber(L, (lua_Number)p->a.y);
					break;
				case LuaSTG::RectMember::m_lt:
					Vector2Wrapper::CreateAndPush(L, Core::Vector2F(
						p->a.x,
						p->a.y
					));
					break;
				case LuaSTG::RectMember::m_lb:
					Vector2Wrapper::CreateAndPush(L, Core::Vector2F(
						p->a.x,
						p->b.y
					));
					break;
				case LuaSTG::RectMember::m_rb:
					Vector2Wrapper::CreateAndPush(L, Core::Vector2F(
						p->b.x,
						p->b.y
					));
					break;
				case LuaSTG::RectMember::m_rt:
					Vector2Wrapper::CreateAndPush(L, Core::Vector2F(
						p->b.x,
						p->a.y
					));
					break;

				case LuaSTG::RectMember::m_ratio:
					lua_pushnumber(L, p->ratio());
					break;
				case LuaSTG::RectMember::m_width:
					lua_pushnumber(L, p->width());
					break;
				case LuaSTG::RectMember::m_height:
					lua_pushnumber(L, p->height());
					break;
				case LuaSTG::RectMember::m_center:
					Vector2Wrapper::CreateAndPush(L, Core::Vector2F(
						(p->a.x + p->b.x) / 2,
						(p->a.y + p->b.y) / 2
					));
					break;
				case LuaSTG::RectMember::m_dimension:
					Vector2Wrapper::CreateAndPush(L, p->dim());
					break;
				case LuaSTG::RectMember::f_GetPointInside:
					lua_pushcfunction(L, GetPointInside);
					break;
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 1;
			}
			static int Meta_NewIndex(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 4)
						return luaL_error(L, "Rect index out of bounds: %d", idx);
					(*p)[idx - 1] = (float)luaL_checknumber(L, 3);
					return 0;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapRectMember(key))
				{
				case LuaSTG::RectMember::m_l:
					p->a.x = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::RectMember::m_r:
					p->b.x = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::RectMember::m_b:
					p->b.y = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::RectMember::m_t:
					p->a.y = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::RectMember::m_lt:
					{
						GETVECDATA(v, 3);
						p->a.x = v->x;
						p->a.y = v->y;
					}
					break;
				case LuaSTG::RectMember::m_lb:
					{
						GETVECDATA(v, 3);
						p->a.x = v->x;
						p->b.y = v->y;
					}
					break;
				case LuaSTG::RectMember::m_rb:
					{
						GETVECDATA(v, 3);
						p->b.x = v->x;
						p->b.y = v->y;
					}
					break;
				case LuaSTG::RectMember::m_rt:
					{
						GETVECDATA(v, 3);
						p->b.x = v->x;
						p->a.y = v->y;
					}
					break;
				case LuaSTG::RectMember::m_width:
					{
						const float w = (float)luaL_checknumber(L, 3);
						const float cx = (p->a.x + p->b.x) / 2;
						p->a.x = cx - w / 2;
						p->b.x = cx + w / 2;
					}
					break;
				case LuaSTG::RectMember::m_height:
					{
						const float h = (float)luaL_checknumber(L, 3);
						const float cy = (p->a.y + p->b.y) / 2;
						p->a.y = cy - h / 2;
						p->b.y = cy + h / 2;
					}
					break;
				case LuaSTG::RectMember::m_center:
					{
						GETVECDATA(v, 3);
						const float w = p->width();
						const float h = p->height();
						p->a.x = v->x - w / 2;
						p->b.x = v->x + w / 2;
						p->a.y = v->y - h / 2;
						p->b.y = v->y + h / 2;
					}
					break;
				case LuaSTG::RectMember::m_dimension:
					{
						GETVECDATA(v, 3);
						const float cx = (p->a.x + p->b.x) / 2;
						const float cy = (p->a.y + p->b.y) / 2;
						const float w = v->x;
						const float h = v->y;
						p->a.x = cx - w / 2;
						p->b.x = cx + w / 2;
						p->a.y = cy - h / 2;
						p->b.y = cy + h / 2;
						break;
					}
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 0;
			}
			static int Meta_Eq(lua_State* L) noexcept
			{
				GETUDATA(pA, 1);
				GETUDATA(pB, 2);
				lua_pushboolean(L, *pA == *pB);
				return 1;
			}
			static int Meta_Add(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					return luaL_error(L, "Rect cannot add with numbers.");
				}
				else
				{
					GETUDATA(pA, 1);
					GETVECDATA(pB, 2);
					RectWrapper::CreateAndPush(L, *pA + *pB);
				}
				return 1;
			}
			static int Meta_Sub(lua_State* L) noexcept
			{

				if (lua_isnumber(L, 1))
				{
					return luaL_error(L, "Rect cannot subtract with numbers.");
				}
				else
				{
					GETUDATA(pA, 1);
					GETVECDATA(pB, 2);
					RectWrapper::CreateAndPush(L, *pA + *pB);
				}
				return 1;
			}
			static int Meta_ToString(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushfstring(L, "lstg.Rect(%f, %f, %f, %f)", p->a.x, p->a.y,p->b.x,p->b.y);
				return 1;
			}
			static int Rect(lua_State* L) noexcept
			{
				CreateAndPush(L, Core::RectF(
					(float)luaL_checknumber(L, 1),
					(float)luaL_checknumber(L, 2),
					(float)luaL_checknumber(L, 3),
					(float)luaL_checknumber(L, 4)
				));
				return 1;
			}

#undef GETUDATA
#undef GETVECDATA
		};

		luaL_Reg tMethods[] = {
			{ "GetPointInside", &Function::GetPointInside },
			{ "IsPointInside", &Function::IsPointInside },
			{ NULL, NULL }
		};

		luaL_Reg tMetaTable[] = {
			{ "__index", &Function::Meta_Index },
			{ "__newindex", &Function::Meta_NewIndex },
			{ "__eq", &Function::Meta_Eq },
			{ "__add", &Function::Meta_Add },
			{ "__sub", &Function::Meta_Sub },
			{ "__tostring", &Function::Meta_ToString },
			{ NULL, NULL }
		};

		luaL_Reg lib[] = {
			{ "Rect", &Function::Rect },
			{ NULL, NULL }
		};

		luaL_register(L, LUASTG_LUA_LIBNAME, lib);
		RegisterClassIntoTable2(L, ".Rect", tMethods, ClassID.data(), tMetaTable);
		lua_pop(L, 1);
	}

	void RectWrapper::CreateAndPush(lua_State* L, Core::RectF const& v)
	{
		Core::RectF* p = static_cast<Core::RectF*>(lua_newuserdata(L, sizeof(Core::RectF))); // udata
		p->a.x = v.a.x;
		p->a.y = v.a.y;
		p->b.x = v.b.x;
		p->b.y = v.b.y;
		luaL_getmetatable(L, ClassID.data()); // udata mt
		lua_setmetatable(L, -2); // udata
	}
} // Rect
namespace LuaSTGPlus::LuaWrapper // Vector2
{
	std::string_view const Vector2Wrapper::ClassID = "lstg.Vector2";

	Core::Vector2F* Vector2Wrapper::Cast(lua_State* L, int idx)
	{
		return static_cast<Core::Vector2F*>(luaL_checkudata(L, idx, ClassID.data()));
	}

	void Vector2Wrapper::Register(lua_State* L) noexcept
	{
		struct Function
		{
		#define GETUDATA(p, i) Core::Vector2F* (p) = Cast(L, i);
		#define GETMATDATA(p, i) Core::Matrix2F* (p) = Matrix2Wrapper::Cast(L, i);

			static int Angle(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushnumber(L, (lua_Number)p->angle() * L_RAD_TO_DEG);
				return 1;
			}

			static int Dot(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				GETUDATA(v, 2);
				lua_pushnumber(L, (lua_Number)p->dot(*v));
				return 1;
			}

			static int Length(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushnumber(L, (lua_Number)p->length());
				return 1;
			}

			static int Normalize(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->normalize());
				return 1;
			}

			static int Normalized(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->normalized());
				return 1;
			}

			static int Meta_Index(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 2)
						return luaL_error(L, "Vector2 index out of bounds: %d", idx);
					lua_pushnumber(L, (lua_Number)(*p)[idx - 1]);
					return 1;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapVector2Member(key))
				{
				case LuaSTG::Vector2Member::m_x:
					lua_pushnumber(L, (lua_Number)p->x);
					break;
				case LuaSTG::Vector2Member::m_y:
					lua_pushnumber(L, (lua_Number)p->y);
					break;
				case LuaSTG::Vector2Member::f_Angle:
					lua_pushcfunction(L, Angle);
					break;
				case LuaSTG::Vector2Member::f_Dot:
					lua_pushcfunction(L, Dot);
					break;
				case LuaSTG::Vector2Member::f_Length:
					lua_pushcfunction(L, Length);
					break;
				case LuaSTG::Vector2Member::f_Normalize:
					lua_pushcfunction(L, Normalize);
					break;
				case LuaSTG::Vector2Member::f_Normalized:
					lua_pushcfunction(L, Normalized);
					break;
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 1;
			}
			static int Meta_NewIndex(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 2)
						return luaL_error(L, "Vector2 index out of bounds: %d", idx);
					(*p)[idx - 1] = (float)luaL_checknumber(L, 3);
					return 0;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapVector2Member(key))
				{
				case LuaSTG::Vector2Member::m_x:
					p->x = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::Vector2Member::m_y:
					p->y = (float)luaL_checknumber(L, 3);
					break;
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 0;
			}
			static int Meta_Eq(lua_State* L) noexcept
			{
				GETUDATA(pA, 1);
				GETUDATA(pB, 2);
				lua_pushboolean(L, *pA == *pB);
				return 1;
			}
			static int Meta_Add(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector2Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector2Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Vector2Wrapper::CreateAndPush(L, *pA + *pB);
				}
				return 1;
			}
			static int Meta_Sub(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector2Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector2Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Vector2Wrapper::CreateAndPush(L, *pA - *pB);
				}
				return 1;
			}
			static int Meta_Mul(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector2Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector2Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Matrix2Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETMATDATA(pB, 2);
						Vector2Wrapper::CreateAndPush(L, *pA * *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Vector2Wrapper::CreateAndPush(L, *pA * *pB);
					}
				}
				return 1;
			}
			static int Meta_Div(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector2Wrapper::CreateAndPush(L, *p / (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector2Wrapper::CreateAndPush(L, (float)v / *p);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Matrix2Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETMATDATA(pB, 2);
						Vector2Wrapper::CreateAndPush(L, *pA / *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Vector2Wrapper::CreateAndPush(L, *pA / *pB);
					}
				}
				return 1;
			}
			static int Meta_ToString(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushfstring(L, "lstg.Vector2(%f, %f)", p->x, p->y);
				return 1;
			}

			static int Vector2(lua_State* L) noexcept
			{
				CreateAndPush(L, Core::Vector2F(
					(float)luaL_checknumber(L, 1),
					(float)luaL_checknumber(L, 2)
				));
				return 1;
			}

		#undef GETUDATA
		#undef GETMATDATA
		};

		luaL_Reg tMethods[] = {
			{ "Angle", &Function::Angle },
			{ "Dot", &Function::Dot },
			{ "Length", &Function::Length },
			{ "Normalize", &Function::Normalize },
			{ "Normalized", &Function::Normalized },
			{ NULL, NULL }
		};

		luaL_Reg tMetaTable[] = {
			{ "__index", &Function::Meta_Index },
			{ "__newindex", &Function::Meta_NewIndex },
			{ "__eq", &Function::Meta_Eq },
			{ "__add", &Function::Meta_Add },
			{ "__sub", &Function::Meta_Sub },
			{ "__mul", &Function::Meta_Mul },
			{ "__div", &Function::Meta_Div },
			{ "__tostring", &Function::Meta_ToString },
			{ NULL, NULL }
		};

		luaL_Reg lib[] = {
			{ "Vector2", &Function::Vector2 },
			{ NULL, NULL }
		};

		luaL_register(L, LUASTG_LUA_LIBNAME, lib);
		RegisterClassIntoTable2(L, ".Vector2", tMethods, ClassID.data(), tMetaTable);
		lua_pop(L, 1);
	}

	void Vector2Wrapper::CreateAndPush(lua_State* L, Core::Vector2F const& v)
	{
		Core::Vector2F* p = static_cast<Core::Vector2F*>(lua_newuserdata(L, sizeof(Core::Vector2F))); // udata
		p->x = v.x;
		p->y = v.y;
		luaL_getmetatable(L, ClassID.data()); // udata mt
		lua_setmetatable(L, -2); // udata
	}
} // Vector2

namespace LuaSTGPlus::LuaWrapper // Vector3
{
	std::string_view const Vector3Wrapper::ClassID = "lstg.Vector3";

	Core::Vector3F* Vector3Wrapper::Cast(lua_State* L, int idx)
	{
		return static_cast<Core::Vector3F*>(luaL_checkudata(L, idx, ClassID.data()));
	}

	void Vector3Wrapper::Register(lua_State* L) noexcept
	{
		struct Function
		{
		#define GETUDATA(p, i) Core::Vector3F* (p) = Cast(L, i);
		#define GETMATDATA(p, i) Core::Matrix3F* (p) = Matrix3Wrapper::Cast(L, i);

			static int Dot(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				GETUDATA(v, 2);
				lua_pushnumber(L, (lua_Number)p->dot(*v));
				return 1;
			}

			static int Length(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushnumber(L, (lua_Number)p->length());
				return 1;
			}

			static int Normalize(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->normalize());
				return 1;
			}

			static int Normalized(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->normalized());
				return 1;
			}

			static int Meta_Index(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 3)
						return luaL_error(L, "Vector3 index out of bounds: %d", idx);
					lua_pushnumber(L, (lua_Number)(*p)[idx - 1]);
					return 1;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapVector3Member(key))
				{
				case LuaSTG::Vector3Member::m_x:
					lua_pushnumber(L, (lua_Number)p->x);
					break;
				case LuaSTG::Vector3Member::m_y:
					lua_pushnumber(L, (lua_Number)p->y);
					break;
				case LuaSTG::Vector3Member::m_z:
					lua_pushnumber(L, (lua_Number)p->z);
					break;
				case LuaSTG::Vector3Member::f_Dot:
					lua_pushcfunction(L, Dot);
					break;
				case LuaSTG::Vector3Member::f_Length:
					lua_pushcfunction(L, Length);
					break;
				case LuaSTG::Vector3Member::f_Normalize:
					lua_pushcfunction(L, Normalize);
					break;
				case LuaSTG::Vector3Member::f_Normalized:
					lua_pushcfunction(L, Normalized);
					break;
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 1;
			}
			static int Meta_NewIndex(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 3)
						return luaL_error(L, "Vector3 index out of bounds: %d", idx);
					(*p)[idx - 1] = (float)luaL_checknumber(L, 3);
					return 0;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapVector3Member(key))
				{
				case LuaSTG::Vector3Member::m_x:
					p->x = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::Vector3Member::m_y:
					p->y = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::Vector3Member::m_z:
					p->z = (float)luaL_checknumber(L, 3);
					break;
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 0;
			}
			static int Meta_Eq(lua_State* L) noexcept
			{
				GETUDATA(pA, 1);
				GETUDATA(pB, 2);
				lua_pushboolean(L, *pA == *pB);
				return 1;
			}
			static int Meta_Add(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector3Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector3Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Vector3Wrapper::CreateAndPush(L, *pA + *pB);
				}
				return 1;
			}
			static int Meta_Sub(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector3Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector3Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Vector3Wrapper::CreateAndPush(L, *pA - *pB);
				}
				return 1;
			}
			static int Meta_Mul(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector3Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector3Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Matrix3Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETMATDATA(pB, 2);
						Vector3Wrapper::CreateAndPush(L, *pA * *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Vector3Wrapper::CreateAndPush(L, *pA * *pB);
					}
				}
				return 1;
			}
			static int Meta_Div(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector3Wrapper::CreateAndPush(L, *p / (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector3Wrapper::CreateAndPush(L, (float)v / *p);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Matrix3Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETMATDATA(pB, 2);
						Vector3Wrapper::CreateAndPush(L, *pA / *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Vector3Wrapper::CreateAndPush(L, *pA / *pB);
					}
				}
				return 1;
			}
			static int Meta_ToString(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushfstring(L, "lstg.Vector3(%f, %f, %f)", p->x, p->y, p->z);
				return 1;
			}

			static int Vector3(lua_State* L) noexcept
			{
				CreateAndPush(L, Core::Vector3F(
					(float)luaL_checknumber(L, 1),
					(float)luaL_checknumber(L, 2),
					(float)luaL_checknumber(L, 3)
				));
				return 1;
			}

		#undef GETUDATA
		#undef GETMATDATA
		};

		luaL_Reg tMethods[] = {
			{ "Dot", &Function::Dot },
			{ "Length", &Function::Length },
			{ "Normalize", &Function::Normalize },
			{ "Normalized", &Function::Normalized },
			{ NULL, NULL }
		};

		luaL_Reg tMetaTable[] = {
			{ "__index", &Function::Meta_Index },
			{ "__newindex", &Function::Meta_NewIndex },
			{ "__eq", &Function::Meta_Eq },
			{ "__add", &Function::Meta_Add },
			{ "__sub", &Function::Meta_Sub },
			{ "__mul", &Function::Meta_Mul },
			{ "__div", &Function::Meta_Div },
			{ "__tostring", &Function::Meta_ToString },
			{ NULL, NULL }
		};

		luaL_Reg lib[] = {
			{ "Vector3", &Function::Vector3 },
			{ NULL, NULL }
		};

		luaL_register(L, LUASTG_LUA_LIBNAME, lib);
		RegisterClassIntoTable2(L, ".Vector3", tMethods, ClassID.data(), tMetaTable);
		lua_pop(L, 1);
	}

	void Vector3Wrapper::CreateAndPush(lua_State* L, Core::Vector3F const& v)
	{
		Core::Vector3F* p = static_cast<Core::Vector3F*>(lua_newuserdata(L, sizeof(Core::Vector3F))); // udata
		p->x = v.x;
		p->y = v.y;
		p->z = v.z;
		luaL_getmetatable(L, ClassID.data()); // udata mt
		lua_setmetatable(L, -2); // udata
	}
} // Vector3

namespace LuaSTGPlus::LuaWrapper // Vector4
{
	std::string_view const Vector4Wrapper::ClassID = "lstg.Vector4";

	Core::Vector4F* Vector4Wrapper::Cast(lua_State* L, int idx)
	{
		return static_cast<Core::Vector4F*>(luaL_checkudata(L, idx, ClassID.data()));
	}

	void Vector4Wrapper::Register(lua_State* L) noexcept
	{
		struct Function
		{
		#define GETUDATA(p, i) Core::Vector4F* (p) = Cast(L, i);
		#define GETMATDATA(p, i) Core::Matrix4F* (p) = Matrix4Wrapper::Cast(L, i);

			static int Dot(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				GETUDATA(v, 2);
				lua_pushnumber(L, (lua_Number)p->dot(*v));
				return 1;
			}

			static int Length(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushnumber(L, (lua_Number)p->length());
				return 1;
			}

			static int Normalize(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->normalize());
				return 1;
			}

			static int Normalized(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->normalized());
				return 1;
			}

			static int Meta_Index(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 4)
						return luaL_error(L, "Vector4 index out of bounds: %d", idx);
					lua_pushnumber(L, (lua_Number)(*p)[idx - 1]);
					return 0;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapVector4Member(key))
				{
				case LuaSTG::Vector4Member::m_x:
					lua_pushnumber(L, (lua_Number)p->x);
					break;
				case LuaSTG::Vector4Member::m_y:
					lua_pushnumber(L, (lua_Number)p->y);
					break;
				case LuaSTG::Vector4Member::m_z:
					lua_pushnumber(L, (lua_Number)p->z);
					break;
				case LuaSTG::Vector4Member::m_w:
					lua_pushnumber(L, (lua_Number)p->w);
					break;
				case LuaSTG::Vector4Member::f_Dot:
					lua_pushcfunction(L, Dot);
					break;
				case LuaSTG::Vector4Member::f_Length:
					lua_pushcfunction(L, Length);
					break;
				case LuaSTG::Vector4Member::f_Normalize:
					lua_pushcfunction(L, Normalize);
					break;
				case LuaSTG::Vector4Member::f_Normalized:
					lua_pushcfunction(L, Normalized);
					break;
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 1;
			}
			static int Meta_NewIndex(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 4)
						return luaL_error(L, "Vector4 index out of bounds: %d", idx);
					(*p)[idx - 1] = (float)luaL_checknumber(L, 3);
					return 1;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapVector4Member(key))
				{
				case LuaSTG::Vector4Member::m_x:
					p->x = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::Vector4Member::m_y:
					p->y = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::Vector4Member::m_z:
					p->z = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::Vector4Member::m_w:
					p->w = (float)luaL_checknumber(L, 3);
					break;
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 0;
			}
			static int Meta_Eq(lua_State* L) noexcept
			{
				GETUDATA(pA, 1);
				GETUDATA(pB, 2);
				lua_pushboolean(L, *pA == *pB);
				return 1;
			}
			static int Meta_Add(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector4Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector4Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Vector4Wrapper::CreateAndPush(L, *pA + *pB);
				}
				return 1;
			}
			static int Meta_Sub(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector4Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector4Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Vector4Wrapper::CreateAndPush(L, *pA - *pB);
				}
				return 1;
			}
			static int Meta_Mul(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector4Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector4Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Matrix4Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETMATDATA(pB, 2);
						Vector4Wrapper::CreateAndPush(L, *pA * *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Vector4Wrapper::CreateAndPush(L, *pA * *pB);
					}
				}
				return 1;
			}
			static int Meta_Div(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Vector4Wrapper::CreateAndPush(L, *p / (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Vector4Wrapper::CreateAndPush(L, (float)v / *p);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Matrix4Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETMATDATA(pB, 2);
						Vector4Wrapper::CreateAndPush(L, *pA / *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Vector4Wrapper::CreateAndPush(L, *pA / *pB);
					}
				}
				return 1;
			}
			static int Meta_ToString(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushfstring(L, "lstg.Vector4(%f, %f, %f, %f)", p->x, p->y, p->z, p->w);
				return 1;
			}

			static int Vector4(lua_State* L) noexcept
			{
				CreateAndPush(L, Core::Vector4F(
					(float)luaL_checknumber(L, 1),
					(float)luaL_checknumber(L, 2),
					(float)luaL_checknumber(L, 3),
					(float)luaL_checknumber(L, 4)
				));
				return 1;
			}

		#undef GETUDATA
		#undef GETMATDATA
		};

		luaL_Reg tMethods[] = {
			{ "Dot", &Function::Dot },
			{ "Length", &Function::Length },
			{ "Normalize", &Function::Normalize },
			{ "Normalized", &Function::Normalized },
			{ NULL, NULL }
		};

		luaL_Reg tMetaTable[] = {
			{ "__index", &Function::Meta_Index },
			{ "__newindex", &Function::Meta_NewIndex },
			{ "__eq", &Function::Meta_Eq },
			{ "__add", &Function::Meta_Add },
			{ "__sub", &Function::Meta_Sub },
			{ "__mul", &Function::Meta_Mul },
			{ "__div", &Function::Meta_Div },
			{ "__tostring", &Function::Meta_ToString },
			{ NULL, NULL }
		};

		luaL_Reg lib[] = {
			{ "Vector4", &Function::Vector4 },
			{ NULL, NULL }
		};

		luaL_register(L, LUASTG_LUA_LIBNAME, lib);
		RegisterClassIntoTable2(L, ".Vector4", tMethods, ClassID.data(), tMetaTable);
		lua_pop(L, 1);
	}

	void Vector4Wrapper::CreateAndPush(lua_State* L, Core::Vector4F const& v)
	{
		Core::Vector4F* p = static_cast<Core::Vector4F*>(lua_newuserdata(L, sizeof(Core::Vector4F))); // udata
		p->x = v.x;
		p->y = v.y;
		p->z = v.z;
		p->w = v.w;
		luaL_getmetatable(L, ClassID.data()); // udata mt
		lua_setmetatable(L, -2); // udata
	}
} // Vector4

namespace LuaSTGPlus::LuaWrapper // Matrix2
{
	std::string_view const Matrix2Wrapper::ClassID = "lstg.Matrix2";

	Core::Matrix2F* Matrix2Wrapper::Cast(lua_State* L, int idx)
	{
		return static_cast<Core::Matrix2F*>(luaL_checkudata(L, idx, ClassID.data()));
	}

	void Matrix2Wrapper::Register(lua_State* L) noexcept
	{
		struct Function
		{
		#define GETUDATA(p, i) Core::Matrix2F* (p) = Cast(L, i);
		#define GETVECDATA(p, i) Core::Vector2F* (p) = Vector2Wrapper::Cast(L, i);

			static int Determinant(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushnumber(L, (lua_Number)p->determinant());
				return 1;
			}

			static int Inverse(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->inverse());
				return 1;
			}

			static int Transpose(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->transpose());
				return 1;
			}

			static int Meta_Index(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 2)
						return luaL_error(L, "Matrix2 index out of bounds: %d", idx);
					Vector2Wrapper::CreateAndPush(L, (*p)[idx - 1]);
					return 1;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapMatrix2Member(key))
				{
				case LuaSTG::Matrix2Member::f_Determinant:
					lua_pushcfunction(L, Determinant);
					break;
				case LuaSTG::Matrix2Member::f_Inverse:
					lua_pushcfunction(L, Inverse);
					break;
				case LuaSTG::Matrix2Member::f_Transpose:
					lua_pushcfunction(L, Transpose);
					break;
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 1;
			}
			static int Meta_NewIndex(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				GETVECDATA(v, 3);

				const lua_Integer idx = luaL_checkinteger(L, 2);
				if (idx < 1 || idx > 2)
					return luaL_error(L, "Matrix2 index out of bounds: %d", idx);
				(*p)[idx - 1] = *v;
				
				return 0;
			}
			static int Meta_Eq(lua_State* L) noexcept
			{
				GETUDATA(pA, 1);
				GETUDATA(pB, 2);
				lua_pushboolean(L, *pA == *pB);
				return 1;
			}
			static int Meta_Add(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix2Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix2Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Matrix2Wrapper::CreateAndPush(L, *pA + *pB);
				}
				return 1;
			}
			static int Meta_Sub(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix2Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix2Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Matrix2Wrapper::CreateAndPush(L, *pA - *pB);
				}
				return 1;
			}
			static int Meta_Mul(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix2Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix2Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Vector2Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETVECDATA(pB, 2);
						Vector2Wrapper::CreateAndPush(L, *pA * *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Matrix2Wrapper::CreateAndPush(L, *pA * *pB);
					}
				}
				return 1;
			}
			static int Meta_Div(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix2Wrapper::CreateAndPush(L, (float)v / *p);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix2Wrapper::CreateAndPush(L, *p / (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Vector2Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETVECDATA(pB, 2);
						Vector2Wrapper::CreateAndPush(L, *pA / *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Matrix2Wrapper::CreateAndPush(L, *pA / *pB);
					}
				}
				return 1;
			}
			static int Meta_ToString(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushfstring(L, "lstg.Matrix2(%f,%f|%f,%f)", (*p)[0][0], (*p)[0][1], (*p)[1][0], (*p)[1][1]);
				return 1;
			}

			static int Matrix2(lua_State* L) noexcept
			{
				if (lua_gettop(L) == 2) {
					CreateAndPush(L, Core::Matrix2F(
						*Vector2Wrapper::Cast(L, 1),
						*Vector2Wrapper::Cast(L, 2)
					));
				} else {
					CreateAndPush(L, Core::Matrix2F(
						(float)luaL_checknumber(L, 1),
						(float)luaL_checknumber(L, 2),
						(float)luaL_checknumber(L, 3),
						(float)luaL_checknumber(L, 4)
					));
				}
				return 1;
			}

		#undef GETUDATA
		#undef GETVECDATA
		};

		luaL_Reg tMethods[] = {
			{ "Determinant", &Function::Determinant },
			{ "Inverse", &Function::Inverse },
			{ "Transpose", &Function::Transpose },
			{ NULL, NULL }
		};

		luaL_Reg tMetaTable[] = {
			{ "__index", &Function::Meta_Index },
			{ "__newindex", &Function::Meta_NewIndex },
			{ "__eq", &Function::Meta_Eq },
			{ "__add", &Function::Meta_Add },
			{ "__sub", &Function::Meta_Sub },
			{ "__mul", &Function::Meta_Mul },
			{ "__div", &Function::Meta_Div },
			{ "__tostring", &Function::Meta_ToString },
			{ NULL, NULL }
		};

		luaL_Reg lib[] = {
			{ "Matrix2", &Function::Matrix2 },
			{ NULL, NULL }
		};

		luaL_register(L, LUASTG_LUA_LIBNAME, lib);
		RegisterClassIntoTable2(L, ".Matrix2", tMethods, ClassID.data(), tMetaTable);
		lua_pop(L, 1);
	}

	void Matrix2Wrapper::CreateAndPush(lua_State* L, Core::Matrix2F const& v)
	{
		Core::Matrix2F* p = static_cast<Core::Matrix2F*>(lua_newuserdata(L, sizeof(Core::Matrix2F))); // udata
		(*p)[0] = v[0];
		(*p)[1] = v[1];
		luaL_getmetatable(L, ClassID.data()); // udata mt
		lua_setmetatable(L, -2); // udata
	}
} // Matrix2

namespace LuaSTGPlus::LuaWrapper // Matrix3
{
	std::string_view const Matrix3Wrapper::ClassID = "lstg.Matrix3";

	Core::Matrix3F* Matrix3Wrapper::Cast(lua_State* L, int idx)
	{
		return static_cast<Core::Matrix3F*>(luaL_checkudata(L, idx, ClassID.data()));
	}

	void Matrix3Wrapper::Register(lua_State* L) noexcept
	{
		struct Function
		{
		#define GETUDATA(p, i) Core::Matrix3F* (p) = Cast(L, i);
		#define GETVECDATA(p, i) Core::Vector3F* (p) = Vector3Wrapper::Cast(L, i);

			static int Determinant(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushnumber(L, (lua_Number)p->determinant());
				return 1;
			}

			static int Inverse(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->inverse());
				return 1;
			}

			static int Transpose(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->transpose());
				return 1;
			}

			static int Meta_Index(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 3)
						return luaL_error(L, "Matrix3 index out of bounds: %d", idx);
					Vector3Wrapper::CreateAndPush(L, (*p)[idx - 1]);
					return 1;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapMatrix3Member(key))
				{
				case LuaSTG::Matrix3Member::f_Determinant:
					lua_pushcfunction(L, Determinant);
					break;
				case LuaSTG::Matrix3Member::f_Inverse:
					lua_pushcfunction(L, Inverse);
					break;
				case LuaSTG::Matrix3Member::f_Transpose:
					lua_pushcfunction(L, Transpose);
					break;
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 1;
			}
			static int Meta_NewIndex(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				GETVECDATA(v, 3);

				const lua_Integer idx = luaL_checkinteger(L, 2);
				if (idx < 1 || idx > 3)
					return luaL_error(L, "Matrix3 index out of bounds: %d", idx);
				(*p)[idx - 1] = *v;
				
				return 0;
			}
			static int Meta_Eq(lua_State* L) noexcept
			{
				GETUDATA(pA, 1);
				GETUDATA(pB, 2);
				lua_pushboolean(L, *pA == *pB);
				return 1;
			}
			static int Meta_Add(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix3Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix3Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Matrix3Wrapper::CreateAndPush(L, *pA + *pB);
				}
				return 1;
			}
			static int Meta_Sub(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix3Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix3Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Matrix3Wrapper::CreateAndPush(L, *pA - *pB);
				}
				return 1;
			}
			static int Meta_Mul(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix3Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix3Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Vector3Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETVECDATA(pB, 2);
						Vector3Wrapper::CreateAndPush(L, *pA * *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Matrix3Wrapper::CreateAndPush(L, *pA * *pB);
					}
				}
				return 1;
			}
			static int Meta_Div(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix3Wrapper::CreateAndPush(L, (float)v / *p);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix3Wrapper::CreateAndPush(L, *p / (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Vector3Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETVECDATA(pB, 2);
						Vector3Wrapper::CreateAndPush(L, *pA / *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Matrix3Wrapper::CreateAndPush(L, *pA / *pB);
					}
				}
				return 1;
			}
			static int Meta_ToString(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushfstring(L, "lstg.Matrix3(%f,%f,%f|%f,%f,%f|%f,%f,%f)",
					(*p)[0][0], (*p)[0][1], (*p)[0][2],
					(*p)[1][0], (*p)[1][1], (*p)[1][2],
					(*p)[2][0], (*p)[2][1], (*p)[2][2]
				);
				return 1;
			}

			static int Matrix3(lua_State* L) noexcept
			{
				if (lua_gettop(L) == 3) {
					CreateAndPush(L, Core::Matrix3F(
						*Vector3Wrapper::Cast(L, 1),
						*Vector3Wrapper::Cast(L, 2),
						*Vector3Wrapper::Cast(L, 3)
					));
				} else {
					CreateAndPush(L, Core::Matrix3F(
						(float)luaL_checknumber(L, 1),
						(float)luaL_checknumber(L, 2),
						(float)luaL_checknumber(L, 3),
						(float)luaL_checknumber(L, 4),
						(float)luaL_checknumber(L, 5),
						(float)luaL_checknumber(L, 6),
						(float)luaL_checknumber(L, 7),
						(float)luaL_checknumber(L, 8),
						(float)luaL_checknumber(L, 9)
					));
				}
				return 1;
			}

		#undef GETUDATA
		#undef GETVECDATA
		};

		luaL_Reg tMethods[] = {
			{ "Determinant", &Function::Determinant },
			{ "Inverse", &Function::Inverse },
			{ "Transpose", &Function::Transpose },
			{ NULL, NULL }
		};

		luaL_Reg tMetaTable[] = {
			{ "__index", &Function::Meta_Index },
			{ "__newindex", &Function::Meta_NewIndex },
			{ "__eq", &Function::Meta_Eq },
			{ "__add", &Function::Meta_Add },
			{ "__sub", &Function::Meta_Sub },
			{ "__mul", &Function::Meta_Mul },
			{ "__div", &Function::Meta_Div },
			{ "__tostring", &Function::Meta_ToString },
			{ NULL, NULL }
		};

		luaL_Reg lib[] = {
			{ "Matrix3", &Function::Matrix3 },
			{ NULL, NULL }
		};

		luaL_register(L, LUASTG_LUA_LIBNAME, lib);
		RegisterClassIntoTable2(L, ".Matrix3", tMethods, ClassID.data(), tMetaTable);
		lua_pop(L, 1);
	}

	void Matrix3Wrapper::CreateAndPush(lua_State* L, Core::Matrix3F const& v)
	{
		Core::Matrix3F* p = static_cast<Core::Matrix3F*>(lua_newuserdata(L, sizeof(Core::Matrix3F))); // udata
		(*p)[0] = v[0];
		(*p)[1] = v[1];
		(*p)[2] = v[2];
		luaL_getmetatable(L, ClassID.data()); // udata mt
		lua_setmetatable(L, -2); // udata
	}
} // Matrix3

namespace LuaSTGPlus::LuaWrapper // Matrix4
{
	std::string_view const Matrix4Wrapper::ClassID = "lstg.Matrix4";

	Core::Matrix4F* Matrix4Wrapper::Cast(lua_State* L, int idx)
	{
		return static_cast<Core::Matrix4F*>(luaL_checkudata(L, idx, ClassID.data()));
	}

	void Matrix4Wrapper::Register(lua_State* L) noexcept
	{
		struct Function
		{
		#define GETUDATA(p, i) Core::Matrix4F* (p) = Cast(L, i);
		#define GETVECDATA(p, i) Core::Vector4F* (p) = Vector4Wrapper::Cast(L, i);

			static int Determinant(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushnumber(L, (lua_Number)p->determinant());
				return 1;
			}

			static int Inverse(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->inverse());
				return 1;
			}

			static int Transpose(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				CreateAndPush(L, p->transpose());
				return 1;
			}

			static int Meta_Index(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				if (lua_isnumber(L, 2)) {
					const lua_Integer idx = luaL_checkinteger(L, 2);
					if (idx < 1 || idx > 4)
						return luaL_error(L, "Matrix4 index out of bounds: %d", idx);
					Vector4Wrapper::CreateAndPush(L, (*p)[idx - 1]);
					return 1;
				}
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapMatrix4Member(key))
				{
				case LuaSTG::Matrix4Member::f_Determinant:
					lua_pushcfunction(L, Determinant);
					break;
				case LuaSTG::Matrix4Member::f_Inverse:
					lua_pushcfunction(L, Inverse);
					break;
				case LuaSTG::Matrix4Member::f_Transpose:
					lua_pushcfunction(L, Transpose);
					break;
				default:
					return luaL_error(L, "Invalid index key.");
				}
				return 1;
			}
			static int Meta_NewIndex(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				GETVECDATA(v, 3);

				const lua_Integer idx = luaL_checkinteger(L, 2);
				if (idx < 1 || idx > 4)
					return luaL_error(L, "Matrix4 index out of bounds: %d", idx);
				(*p)[idx - 1] = *v;
				
				return 0;
			}
			static int Meta_Eq(lua_State* L) noexcept
			{
				GETUDATA(pA, 1);
				GETUDATA(pB, 2);
				lua_pushboolean(L, *pA == *pB);
				return 1;
			}
			static int Meta_Add(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix4Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix4Wrapper::CreateAndPush(L, *p + (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Matrix4Wrapper::CreateAndPush(L, *pA + *pB);
				}
				return 1;
			}
			static int Meta_Sub(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix4Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix4Wrapper::CreateAndPush(L, *p - (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Matrix4Wrapper::CreateAndPush(L, *pA - *pB);
				}
				return 1;
			}
			static int Meta_Mul(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix4Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix4Wrapper::CreateAndPush(L, *p * (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Vector4Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETVECDATA(pB, 2);
						Vector4Wrapper::CreateAndPush(L, *pA * *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Matrix4Wrapper::CreateAndPush(L, *pA * *pB);
					}
				}
				return 1;
			}
			static int Meta_Div(lua_State* L) noexcept
			{
				if (lua_isnumber(L, 1))
				{
					lua_Number const v = luaL_checknumber(L, 1);
					GETUDATA(p, 2);
					Matrix4Wrapper::CreateAndPush(L, (float)v / *p);
				}
				else if (lua_isnumber(L, 2))
				{
					lua_Number const v = luaL_checknumber(L, 2);
					GETUDATA(p, 1);
					Matrix4Wrapper::CreateAndPush(L, *p / (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					lua_getmetatable(L, 2);
					luaL_getmetatable(L, Vector4Wrapper::ClassID.data());
					if (lua_equal(L, -1, -2)) {
						lua_pop(L, 2);
						GETVECDATA(pB, 2);
						Vector4Wrapper::CreateAndPush(L, *pA / *pB);
					} else {
						lua_pop(L, 2);
						GETUDATA(pB, 2);
						Matrix4Wrapper::CreateAndPush(L, *pA / *pB);
					}
				}
				return 1;
			}
			static int Meta_ToString(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushfstring(L, "lstg.Matrix4(%f,%f,%f,%f|%f,%f,%f,%f|%f,%f,%f,%f|%f,%f,%f,%f)",
					(*p)[0][0], (*p)[0][1], (*p)[0][2], (*p)[0][3],
					(*p)[1][0], (*p)[1][1], (*p)[1][2], (*p)[1][3],
					(*p)[2][0], (*p)[2][1], (*p)[2][2], (*p)[2][3],
					(*p)[3][0], (*p)[3][1], (*p)[3][2], (*p)[3][3]
				);
				return 1;
			}

			static int Matrix4(lua_State* L) noexcept
			{
				if (lua_gettop(L) == 4) {
					CreateAndPush(L, Core::Matrix4F(
						*Vector4Wrapper::Cast(L, 1),
						*Vector4Wrapper::Cast(L, 2),
						*Vector4Wrapper::Cast(L, 3),
						*Vector4Wrapper::Cast(L, 4)
					));
				} else {
					CreateAndPush(L, Core::Matrix4F(
						(float)luaL_checknumber(L, 1),
						(float)luaL_checknumber(L, 2),
						(float)luaL_checknumber(L, 3),
						(float)luaL_checknumber(L, 4),
						(float)luaL_checknumber(L, 5),
						(float)luaL_checknumber(L, 6),
						(float)luaL_checknumber(L, 7),
						(float)luaL_checknumber(L, 8),
						(float)luaL_checknumber(L, 9),
						(float)luaL_checknumber(L, 10),
						(float)luaL_checknumber(L, 11),
						(float)luaL_checknumber(L, 12),
						(float)luaL_checknumber(L, 13),
						(float)luaL_checknumber(L, 14),
						(float)luaL_checknumber(L, 15),
						(float)luaL_checknumber(L, 16)
					));
				}
				return 1;
			}

		#undef GETUDATA
		#undef GETVECDATA
		};

		luaL_Reg tMethods[] = {
			{ "Determinant", &Function::Determinant },
			{ "Inverse", &Function::Inverse },
			{ "Transpose", &Function::Transpose },
			{ NULL, NULL }
		};

		luaL_Reg tMetaTable[] = {
			{ "__index", &Function::Meta_Index },
			{ "__newindex", &Function::Meta_NewIndex },
			{ "__eq", &Function::Meta_Eq },
			{ "__add", &Function::Meta_Add },
			{ "__sub", &Function::Meta_Sub },
			{ "__mul", &Function::Meta_Mul },
			{ "__div", &Function::Meta_Div },
			{ "__tostring", &Function::Meta_ToString },
			{ NULL, NULL }
		};

		luaL_Reg lib[] = {
			{ "Matrix4", &Function::Matrix4 },
			{ NULL, NULL }
		};

		luaL_register(L, LUASTG_LUA_LIBNAME, lib);
		RegisterClassIntoTable2(L, ".Matrix4", tMethods, ClassID.data(), tMetaTable);
		lua_pop(L, 1);
	}

	void Matrix4Wrapper::CreateAndPush(lua_State* L, Core::Matrix4F const& v)
	{
		Core::Matrix4F* p = static_cast<Core::Matrix4F*>(lua_newuserdata(L, sizeof(Core::Matrix4F))); // udata
		(*p)[0] = v[0];
		(*p)[1] = v[1];
		(*p)[2] = v[2];
		(*p)[3] = v[3];
		luaL_getmetatable(L, ClassID.data()); // udata mt
		lua_setmetatable(L, -2); // udata
	}
} // Matrix4
