#include "Core/Type.hpp"
#include "LuaBinding/LuaWrapper.hpp"
#include "LuaBinding/lua_luastg_hash.hpp"
#include "lua.h"
#include "spdlog/spdlog.h"

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

			static int Angle(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 1)
				{
					lua_pushnumber(L, (lua_Number)p->angle());
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Dot(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 2)
				{
					GETUDATA(v, 2);
					lua_pushnumber(L, (lua_Number)p->dot(*v));
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Length(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 1)
				{
					lua_pushnumber(L, (lua_Number)p->length());
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Normalize(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 1)
				{
					CreateAndPush(L, p->normalize());
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Normalized(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 1)
				{
					CreateAndPush(L, p->normalized());
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Meta_Index(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
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
					GETUDATA(pB, 2);
					Vector2Wrapper::CreateAndPush(L, *pA * *pB);
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
					Vector2Wrapper::CreateAndPush(L, *p / (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Vector2Wrapper::CreateAndPush(L, *pA / *pB);
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

			static int Dot(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 2)
				{
					GETUDATA(v, 2);
					lua_pushnumber(L, (lua_Number)p->dot(*v));
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Length(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 1)
				{
					lua_pushnumber(L, (lua_Number)p->length());
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Normalize(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 1)
				{
					CreateAndPush(L, p->normalize());
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Normalized(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 1)
				{
					CreateAndPush(L, p->normalized());
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Meta_Index(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapVector3Member(key))
				{
				case LuaSTG::Vector3Member::m_x:
					lua_pushnumber(L, (lua_Number)p->x);
					break;
				case LuaSTG::Vector3Member::m_y:
					lua_pushnumber(L, (lua_Number)p->y);
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
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapVector3Member(key))
				{
				case LuaSTG::Vector3Member::m_x:
					p->x = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::Vector3Member::m_y:
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
					GETUDATA(pB, 2);
					Vector3Wrapper::CreateAndPush(L, *pA * *pB);
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
					Vector3Wrapper::CreateAndPush(L, *p / (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Vector3Wrapper::CreateAndPush(L, *pA / *pB);
				}
				return 1;
			}
			static int Meta_ToString(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushfstring(L, "lstg.Vector3(%f, %f)", p->x, p->y);
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

			static int Dot(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 2)
				{
					GETUDATA(v, 2);
					lua_pushnumber(L, (lua_Number)p->dot(*v));
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Length(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 1)
				{
					lua_pushnumber(L, (lua_Number)p->length());
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Normalize(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 1)
				{
					CreateAndPush(L, p->normalize());
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Normalized(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				int const argc = lua_gettop(L);
				if (argc == 1)
				{
					CreateAndPush(L, p->normalized());
					return 1;
				}
				else
				{
					return luaL_error(L, "Invalid args.");
				}
			}

			static int Meta_Index(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapVector4Member(key))
				{
				case LuaSTG::Vector4Member::m_x:
					lua_pushnumber(L, (lua_Number)p->x);
					break;
				case LuaSTG::Vector4Member::m_y:
					lua_pushnumber(L, (lua_Number)p->y);
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
				const char* key = luaL_checkstring(L, 2);
				switch (LuaSTG::MapVector4Member(key))
				{
				case LuaSTG::Vector4Member::m_x:
					p->x = (float)luaL_checknumber(L, 3);
					break;
				case LuaSTG::Vector4Member::m_y:
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
					GETUDATA(pB, 2);
					Vector4Wrapper::CreateAndPush(L, *pA * *pB);
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
					Vector4Wrapper::CreateAndPush(L, *p / (float)v);
				}
				else
				{
					GETUDATA(pA, 1);
					GETUDATA(pB, 2);
					Vector4Wrapper::CreateAndPush(L, *pA / *pB);
				}
				return 1;
			}
			static int Meta_ToString(lua_State* L) noexcept
			{
				GETUDATA(p, 1);
				lua_pushfstring(L, "lstg.Vector4(%f, %f)", p->x, p->y);
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
		p->z = v.w;
		luaL_getmetatable(L, ClassID.data()); // udata mt
		lua_setmetatable(L, -2); // udata
	}
} // Vector4
