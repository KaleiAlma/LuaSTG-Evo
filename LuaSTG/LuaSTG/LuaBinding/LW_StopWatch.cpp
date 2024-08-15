#include "LuaBinding/LuaWrapper.hpp"
#include <chrono>
#include <ratio>

using Duration = std::chrono::duration<double, std::micro>;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock, Duration>;

////////////////////////////////////////////////////////////////////////////////
/// @brief High-precision stopwatch
////////////////////////////////////////////////////////////////////////////////
class fcyStopWatch
{
private:
	TimePoint m_cLast{};     // Last time
	TimePoint m_cFixStart{}; // Time fix parameter on pause
	Duration m_cFixAll{};   // Time fix parameter on pause
public:
	void Pause();
	void Resume();
	void Reset();
	double GetElapsed();
	// in seconds
public:
	fcyStopWatch();
	~fcyStopWatch();
};

fcyStopWatch::fcyStopWatch(void)
{
	Reset();
}

fcyStopWatch::~fcyStopWatch(void)
{
}

void fcyStopWatch::Pause()
{
	m_cFixStart = Clock::now();
}

void fcyStopWatch::Resume()
{
	m_cFixAll += Clock::now() - m_cFixStart;
}

void fcyStopWatch::Reset()
{
	m_cLast = Clock::now();
	m_cFixAll = Duration(0);
}

double fcyStopWatch::GetElapsed()
{
	return std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - m_cLast - m_cFixAll).count();
}

namespace LuaSTGPlus::LuaWrapper
{
	void StopWatchWrapper::Register(lua_State* L)noexcept
	{
		struct Function
		{
		#define GETUDATA(p, i) fcyStopWatch* (p) = static_cast<fcyStopWatch*>(luaL_checkudata(L, (i), LUASTG_LUA_TYPENAME_STOPWATCH));
			static int Reset(lua_State* L)
			{
				GETUDATA(p, 1);
				p->Reset();
				return 1;
			}
			static int Pause(lua_State* L)
			{
				GETUDATA(p, 1);
				p->Pause();
				return 1;
			}
			static int Resume(lua_State* L)
			{
				GETUDATA(p, 1);
				p->Resume();
				return 1;
			}
			static int GetElapsed(lua_State* L)
			{
				GETUDATA(p, 1);
				lua_pushnumber(L, (lua_Number)p->GetElapsed());
				return 1;
			}
			static int Meta_ToString(lua_State* L)noexcept
			{
				::lua_pushfstring(L, LUASTG_LUA_TYPENAME_STOPWATCH);
				return 1;
			}
		#undef GETUDATA
		};

		luaL_Reg tMethods[] =
		{
			{ "Reset", &Function::Reset },
			{ "Pause", &Function::Pause },
			{ "Resume", &Function::Resume },
			{ "GetElapsed", &Function::GetElapsed },
			{ NULL, NULL }
		};
		luaL_Reg tMetaTable[] =
		{
			{ "__tostring", &Function::Meta_ToString },
			{ NULL, NULL }
		};

		RegisterClassIntoTable(L, ".StopWatch", tMethods, LUASTG_LUA_TYPENAME_STOPWATCH, tMetaTable);
	}

	void StopWatchWrapper::CreateAndPush(lua_State* L)
	{
		fcyStopWatch* p = static_cast<fcyStopWatch*>(lua_newuserdata(L, sizeof(fcyStopWatch))); // udata
		new(p) fcyStopWatch();
		luaL_getmetatable(L, LUASTG_LUA_TYPENAME_STOPWATCH); // udata mt
		lua_setmetatable(L, -2); // udata
	}
}
