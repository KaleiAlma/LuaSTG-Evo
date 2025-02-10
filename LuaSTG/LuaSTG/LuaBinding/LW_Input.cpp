#include "LuaBinding/LuaWrapper.hpp"
#include "AppFrame.h"
#include "SDL.h"
// #include "Platform/Keyboard.hpp"
// #include "Platform/Mouse.hpp"


static int register_keyboard(lua_State* L)
{
	struct Wrapper
	{
		static int GetKeyState(lua_State* L)
		{
			lua_pushboolean(L, LAPP.GetKeyState(luaL_checkinteger(L, -1)));
			return 1;
		}
	};

	luaL_Reg lib_empty[] = {
		{NULL, NULL},
	};
	luaL_Reg lib_keyboard[] = {
		{ "GetKeyState", &Wrapper::GetKeyState },
		{NULL, NULL},
	};

	luaL_register(L, LUASTG_LUA_LIBNAME ".Input", lib_empty);             // ??? lstg.Input
	luaL_register(L, LUASTG_LUA_LIBNAME ".Input.Keyboard", lib_keyboard); // ??? lstg.Input lstg.Input.Keyboard

	struct { char const* id; char const* name; lua_Integer value; } kcode[] = {
		{"None", "None", SDLK_UNKNOWN},

		{"Back", "Back", SDLK_BACKSPACE},
		{"Tab", "Tab", SDLK_TAB},
		{ "Clear", "Clear", SDLK_CLEAR },
		{"Enter", "Enter", SDLK_RETURN},
		// { "Shift", "Shift", 0x10 },
		// { "Control", "Control", /* VK_CONTROL */ 0x11 },
		// { "Alt", "Alt", /* VK_MENU */ 0x12 },
		{"Pause", "Pause", SDLK_PAUSE},
		{"CapsLock", "CapsLock", SDLK_CAPSLOCK},

		// { "ImeHangul", "ImeHangul", 0x15 },
		// { "ImeKana", "ImeKana", 0x15 },
		// {"ImeOn", "ImeOn", SDLK_},
		// { "ImeJunja", "ImeJunja", 0x17 },
		// { "ImeFinal", "ImeFinal", /* VK_FINAL */ 0x18 },
		// { "ImeKanji", "ImeKanji", 0x19 },
		// { "ImeHanja", "ImeHanja", 0x19 },
		// {"ImeOff", "ImeOff", SDLK_},

		{"Escape", "Escape", SDLK_ESCAPE},

		// {"ImeConvert", "ImeConvert", SDLK_},
		// {"ImeNoConvert", "ImeNoConvert", SDLK_},
		// { "ImeAccept", "ImeAccept", /* VK_ACCEPT */ 0x1E },
		// { "ImeModeChangeRequest", "ImeModeChangeRequest", /* VK_MODECHANGE */ 0x1F },

		{"Space", "Space", SDLK_SPACE},
		{"PageUp", "PageUp", SDLK_PAGEUP},
		{"PageDown", "PageDown", SDLK_PAGEDOWN},
		{"End", "End", SDLK_END},
		{"Home", "Home", SDLK_HOME},
		{"Left", "Left", SDLK_LEFT},
		{"Up", "Up", SDLK_UP},
		{"Right", "Right", SDLK_RIGHT},
		{"Down", "Down", SDLK_DOWN},
		{"Select", "Select", SDLK_SELECT},
		// {"Print", "Print", SDLK_P},
		{"Execute", "Execute", SDLK_EXECUTE},
		{"PrintScreen", "PrintScreen", SDLK_PRINTSCREEN},
		{"Insert", "Insert", SDLK_INSERT},
		{"Delete", "Delete", SDLK_DELETE},
		{"Help", "Help", SDLK_HELP},
		{"D0", "D0", SDLK_0},
		{"D1", "D1", SDLK_1},
		{"D2", "D2", SDLK_2},
		{"D3", "D3", SDLK_3},
		{"D4", "D4", SDLK_4},
		{"D5", "D5", SDLK_5},
		{"D6", "D6", SDLK_6},
		{"D7", "D7", SDLK_7},
		{"D8", "D8", SDLK_8},
		{"D9", "D9", SDLK_9},

		{"A", "A", SDLK_a},
		{"B", "B", SDLK_b},
		{"C", "C", SDLK_c},
		{"D", "D", SDLK_d},
		{"E", "E", SDLK_e},
		{"F", "F", SDLK_f},
		{"G", "G", SDLK_g},
		{"H", "H", SDLK_h},
		{"I", "I", SDLK_i},
		{"J", "J", SDLK_j},
		{"K", "K", SDLK_k},
		{"L", "L", SDLK_l},
		{"M", "M", SDLK_m},
		{"N", "N", SDLK_n},
		{"O", "O", SDLK_o},
		{"P", "P", SDLK_p},
		{"Q", "Q", SDLK_q},
		{"R", "R", SDLK_r},
		{"S", "S", SDLK_s},
		{"T", "T", SDLK_t},
		{"U", "U", SDLK_u},
		{"V", "V", SDLK_v},
		{"W", "W", SDLK_w},
		{"X", "X", SDLK_x},
		{"Y", "Y", SDLK_y},
		{"Z", "Z", SDLK_z},
		{"LeftWindows", "LeftWindows", SDLK_LGUI},
		{"RightWindows", "RightWindows", SDLK_RGUI},
		{"Apps", "Apps", SDLK_APPLICATION},

		{"Sleep", "Sleep", SDLK_SLEEP},
		{"NumPad0", "NumPad0", SDLK_KP_0},
		{"NumPad1", "NumPad1", SDLK_KP_1},
		{"NumPad2", "NumPad2", SDLK_KP_2},
		{"NumPad3", "NumPad3", SDLK_KP_3},
		{"NumPad4", "NumPad4", SDLK_KP_4},
		{"NumPad5", "NumPad5", SDLK_KP_5},
		{"NumPad6", "NumPad6", SDLK_KP_6},
		{"NumPad7", "NumPad7", SDLK_KP_7},
		{"NumPad8", "NumPad8", SDLK_KP_8},
		{"NumPad9", "NumPad9", SDLK_KP_9},
		{"Multiply", "Multiply", SDLK_KP_MULTIPLY},
		{"Add", "Add", SDLK_KP_PLUS},
		{"Separator", "Separator", SDLK_SEPARATOR},
		{"Subtract", "Subtract", SDLK_KP_MINUS},
		{"Decimal", "Decimal", SDLK_KP_DECIMAL},
		{"Divide", "Divide", SDLK_KP_DIVIDE},

		{"F1", "F1", SDLK_F1},
		{"F2", "F2", SDLK_F2},
		{"F3", "F3", SDLK_F3},
		{"F4", "F4", SDLK_F4},
		{"F5", "F5", SDLK_F5},
		{"F6", "F6", SDLK_F6},
		{"F7", "F7", SDLK_F7},
		{"F8", "F8", SDLK_F8},
		{"F9", "F9", SDLK_F9},
		{"F10", "F10", SDLK_F10},
		{"F11", "F11", SDLK_F11},
		{"F12", "F12", SDLK_F12},
		{"F13", "F13", SDLK_F13},
		{"F14", "F14", SDLK_F14},
		{"F15", "F15", SDLK_F15},
		{"F16", "F16", SDLK_F16},
		{"F17", "F17", SDLK_F17},
		{"F18", "F18", SDLK_F18},
		{"F19", "F19", SDLK_F19},
		{"F20", "F20", SDLK_F20},
		{"F21", "F21", SDLK_F21},
		{"F22", "F22", SDLK_F22},
		{"F23", "F23", SDLK_F23},
		{"F24", "F24", SDLK_F24},

		// {"NumLock", "NumLock", SDLK_},
		{"Scroll", "Scroll", SDLK_SCROLLLOCK},
		{"LeftShift", "LeftShift", SDLK_LSHIFT},
		{"RightShift", "RightShift", SDLK_RSHIFT},
		{"LeftControl", "LeftControl", SDLK_LCTRL},
		{"RightControl", "RightControl", SDLK_RCTRL},
		{"LeftAlt", "LeftAlt", SDLK_LALT},
		{"RightAlt", "RightAlt", SDLK_RALT},

		// {"BrowserBack", "BrowserBack", SDLK_},
		// {"BrowserForward", "BrowserForward", SDLK_},
		// {"BrowserRefresh", "BrowserRefresh", SDLK_},
		// {"BrowserStop", "BrowserStop", SDLK_},
		// {"BrowserSearch", "BrowserSearch", SDLK_},
		// {"BrowserFavorites", "BrowserFavorites", SDLK_},
		// {"BrowserHome", "BrowserHome", SDLK_},
		{"VolumeMute", "VolumeMute", SDLK_MUTE},
		{"VolumeDown", "VolumeDown", SDLK_VOLUMEDOWN},
		{"VolumeUp", "VolumeUp", SDLK_VOLUMEUP},
		{"MediaNextTrack", "MediaNextTrack", SDLK_AUDIONEXT},
		{"MediaPreviousTrack", "MediaPreviousTrack", SDLK_AUDIOPREV},
		{"MediaStop", "MediaStop", SDLK_AUDIOSTOP},
		{"MediaPlayPause", "MediaPlayPause", SDLK_AUDIOPLAY},
		{"LaunchMail", "LaunchMail", SDLK_MAIL},
		{"SelectMedia", "SelectMedia", SDLK_MEDIASELECT},
		{"LaunchApplication1", "LaunchApplication1", SDLK_APP1},
		{"LaunchApplication2", "LaunchApplication2", SDLK_APP2},

		{"Semicolon", "Semicolon", SDLK_SEMICOLON},
		{"Plus", "Plus", SDLK_PLUS},
		{"Comma", "Comma", SDLK_COMMA},
		{"Minus", "Minus", SDLK_MINUS},
		{"Period", "Period", SDLK_PERIOD},
		{"Question", "Question", SDLK_QUESTION},
		{"Tilde", "Tilde", SDLK_BACKQUOTE},

		{"OpenBrackets", "OpenBrackets", SDLK_LEFTBRACKET},
		{"Pipe", "Pipe", SDLK_BACKSLASH},
		{"CloseBrackets", "CloseBrackets", SDLK_RIGHTBRACKET},
		{"Quotes", "Quotes", SDLK_QUOTE},
		// {"Oem8", "Oem8", SDLK_},

		// KNV("Oem102", OemBackslash),

		// {"ProcessKey", "ProcessKey", SDLK_},

		{"NumPadEnter", "NumPadEnter", SDLK_KP_ENTER},

		{"OemCopy", "OemCopy", SDLK_COPY},
		// {"OemAuto", "OemAuto", SDLK_},
		// {"OemEnlW", "OemEnlW", SDLK_},

		// {"Attn", "Attn", SDLK_},
		{"Crsel", "Crsel", SDLK_CRSEL},
		{"Exsel", "Exsel", SDLK_EXSEL},
		// {"EraseEof", "EraseEof", SDLK_},

		// {"Play", "Play", SDLK_},
		// {"Zoom", "Zoom", SDLK_},

		// {"Pa1", "Pa1", SDLK_},
		// {"OemClear", "OemClear", SDLK_},
	};
	for (auto const& v : kcode)
	{
		lua_pushstring(L, v.id);     // ??? lstg.Input lstg.Keyboard "name"
		lua_pushinteger(L, v.value); // ??? lstg.Input lstg.Keyboard "name" code
		lua_settable(L, -3);         // ??? lstg.Input lstg.Keyboard
	}

	lua_setfield(L, -1, "Keyboard"); // ??? lstg.Input
	lua_pop(L, 1);                   // ???
	
	return 0;
}
static int register_mouse(lua_State* L)
{
	struct Wrapper
	{
		static int GetKeyState(lua_State* L)
		{
			lua_pushboolean(L, LAPP.GetMouseState(luaL_checkinteger(L, 1)));
			return 1;
		}
		static int GetPosition(lua_State* L) noexcept
		{
			Core::Vector2F tPos = LAPP.GetMousePosition(lua_toboolean(L, 1));
			lua_pushnumber(L, tPos.x);
			lua_pushnumber(L, tPos.y);
			return 2;
		}
		static int GetWheelDelta(lua_State* L) noexcept
		{
			lua_pushnumber(L, (lua_Number)LAPP.GetMouseWheelDelta() / /* WHEEL_DELTA */ 120.0);
			return 1;
		}
	};

	luaL_Reg lib_empty[] = {
		{NULL, NULL},
	};
	luaL_Reg lib_mouse[] = {
		{ "GetKeyState", &Wrapper::GetKeyState },
		{ "GetPosition", &Wrapper::GetPosition },
		{ "GetWheelDelta", &Wrapper::GetWheelDelta },
		{NULL, NULL},
	};

	luaL_register(L, LUASTG_LUA_LIBNAME ".Input", lib_empty);       // ??? lstg.Input
	luaL_register(L, LUASTG_LUA_LIBNAME ".Input.Mouse", lib_mouse); // ??? lstg.Input lstg.Input.Mouse

	struct { char const* id; lua_Integer value; } mcode[] = {
		{ "None", 0 },

		{ "Primary", /* VK_LBUTTON */ SDL_BUTTON_LEFT },
		{ "Left", /* VK_LBUTTON */ SDL_BUTTON_LEFT },
		{ "Middle", /* VK_MBUTTON */ SDL_BUTTON_MIDDLE },
		{ "Secondary", /* VK_RBUTTON */ SDL_BUTTON_RIGHT },
		{ "Right", /* VK_RBUTTON */ SDL_BUTTON_RIGHT },

		{ "X1", /* VK_XBUTTON1 */ SDL_BUTTON_X1 },
		{ "XButton1", /* VK_XBUTTON1 */ SDL_BUTTON_X1 },
		{ "X2", /* VK_XBUTTON2 */ SDL_BUTTON_X2 },
		{ "XButton2", /* VK_XBUTTON2 */ SDL_BUTTON_X2 },
	};
	for (auto const& v : mcode)
	{
		lua_pushstring(L, v.id);     // ??? lstg.Input lstg.Input.Mouse "name"
		lua_pushinteger(L, v.value); // ??? lstg.Input lstg.Input.Mouse "name" code
		lua_settable(L, -3);         // ??? lstg.Input lstg.Input.Mouse
	}

	lua_setfield(L, -1, "Mouse"); // ??? lstg.Input
	lua_pop(L, 1);                // ???

	return 0;
}

void LuaSTGPlus::LuaWrapper::InputWrapper::Register(lua_State* L) noexcept
{
	struct Wrapper
	{
		static int GetKeyState(lua_State* L)
		{
			lua_pushboolean(L, LAPP.GetKeyState(luaL_checkinteger(L, -1)));
			return 1;
		}
		static int GetMouseState(lua_State* L)
		{
			lua_pushboolean(L, LAPP.GetMouseState(luaL_checkinteger(L, 1) + 1));
			return 1;
		}
		static int GetMousePosition(lua_State* L) noexcept
		{
			Core::Vector2F tPos = LAPP.GetMousePosition(lua_toboolean(L, 1));
			lua_pushnumber(L, tPos.x);
			lua_pushnumber(L, tPos.y);
			return 2;
		}
		static int GetMouseWheelDelta(lua_State* L) noexcept
		{
			lua_pushinteger(L, (lua_Integer)LAPP.GetMouseWheelDelta());
			return 1;
		}
		// 应该废弃的方法
		static int GetLastKey(lua_State* L) noexcept
		{
			lua_pushinteger(L, LAPP.GetLastKey());
			return 1;
		}
	};

	luaL_Reg lib_empty[] = {
		{NULL, NULL},
	};
	luaL_Reg lib_compat[] = {
		{ "GetKeyState", &Wrapper::GetKeyState },
		{ "GetMouseState", &Wrapper::GetMouseState },
		{ "GetMousePosition", &Wrapper::GetMousePosition },
		{ "GetMouseWheelDelta", &Wrapper::GetMouseWheelDelta },
		// 应该废弃的方法
		{ "GetLastKey", &Wrapper::GetLastKey },
		{NULL, NULL},
	};

	luaL_register(L, LUASTG_LUA_LIBNAME, lib_compat);         // ??? lstg
	luaL_register(L, LUASTG_LUA_LIBNAME ".Input", lib_empty); // ??? lstg lstg.Input
	lua_setfield(L, -1, "Input");                             // ??? lstg
	lua_pop(L, 1);                                            // ???

	register_keyboard(L);
	register_mouse(L);
}
