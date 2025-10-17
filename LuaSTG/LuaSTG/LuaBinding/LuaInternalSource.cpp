#include "LuaBinding/LuaInternalSource.hpp"

#if _WIN32
#pragma region x86 lib
static const std::string _InternalSource_x86 = R"(

package.cpath = ""
package.cpath = package.cpath .. ".\\x86\\?.dll;"
package.cpath = package.cpath .. ".\\?.dll;"

)";
#pragma endregion

#pragma region x64 lib
static const std::string _InternalSource_amd64 = R"(

package.cpath = ""
package.cpath = package.cpath .. ".\\amd64\\?.dll;"
package.cpath = package.cpath .. ".\\?.dll;"

)";
#pragma endregion

#pragma region arm64 lib
static const std::string _InternalSource_arm64 = R"(

package.cpath = ""
package.cpath = package.cpath .. ".\\arm64\\?.dll;"
package.cpath = package.cpath .. ".\\?.dll;"

)";
#pragma endregion
#else
#pragma region x86 lib
static const std::string _InternalSource_x86 = R"(

package.cpath = ""
package.cpath = package.cpath .. "./x86/?.so;"
package.cpath = package.cpath .. "./?.so;"

)";
#pragma endregion

#pragma region x64 lib
static const std::string _InternalSource_amd64 = R"(

package.cpath = ""
package.cpath = package.cpath .. "./amd64/?.so;"
package.cpath = package.cpath .. "./?.so;"

)";
#pragma endregion

#pragma region arm64 lib
static const std::string _InternalSource_arm64 = R"(

package.cpath = ""
package.cpath = package.cpath .. "./arm64/?.so;"
package.cpath = package.cpath .. "./?.so;"

)";
#pragma endregion
#endif

#pragma region main
static const std::string _InternalSource_Main = R"(

package.path = ""
package.path = package.path .. "./?.lua;"
package.path = package.path .. "./?/init.lua;"
package.path = package.path .. "./src/?.lua;"

function GameInit()
end
function FrameFunc()
    return false
end
function RenderFunc()
end
function GameExit()
end
function FocusLoseFunc()
end
function FocusGainFunc()
end
function EventFunc(event, ...)
end

)";
#pragma endregion

#pragma region api
static const std::string _InternalSource_API = R"(

local lstg = require("lstg")

function lstg.ShowSplashWindow()
end

function lstg.PostEffectCapture()
end

function lstg.PostEffectApply()
end

function lstg.SystemLog(text)
    lstg.Log(2, text)
end

function lstg.Print(...)
    local args = {...}
    local argc = select('#', ...)
    for i = 1, argc do args[i] = tostring(args[i]) end
    lstg.Log(2, table.concat(args, '\t'))
end

print = lstg.Print

local rad = math.rad
local deg = math.deg
local sin = math.sin
local cos = math.cos
local tan = math.tan
local asin = math.asin
local acos = math.acos
local atan = math.atan
local atan2 = math.atan2 or math.atan
function lstg.sin(x) return sin(rad(x)) end
function lstg.cos(x) return cos(rad(x)) end
function lstg.tan(x) return tan(rad(x)) end
function lstg.asin(x) return deg(asin(x)) end
function lstg.acos(x) return deg(acos(x)) end
function lstg.atan(...) return deg(atan(...)) end
function lstg.atan2(y, x) return deg(atan2(y, x)) end

)";
#pragma endregion

namespace LuaSTGPlus {
    std::string LuaInternalSource_1() {
        // if constexpr (sizeof(void*) >= 8) {
        // 	return _InternalSource_amd64 + _InternalSource_Main;
        // } else {
        // 	return _InternalSource_x86 + _InternalSource_Main;
        // }
        return
#if defined(__aarch64__) || defined(_M_ARM64)
            _InternalSource_arm64
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
            _InternalSource_amd64
#endif
            + _InternalSource_Main;
    }
    std::string LuaInternalSource_2() {
        return _InternalSource_API;
    }
}
