#include "AppFrame.h"
#include "GameResource/ResourcePassword.hpp"
#include "LConfig.h"
#include "LuaBinding/LuaAppFrame.hpp"
#include "LuaBinding/LuaCustomLoader.hpp"
#include "LuaBinding/LuaInternalSource.hpp"
#include "LuaBinding/LuaWrapper.hpp"
extern "C" {
#include "lua_cjson.h"
#include "lfs.h"
extern int luaopen_utf8(lua_State* L);
extern int luaopen_string_pack(lua_State* L);
extern int luaopen_mime_core(lua_State* L);
extern int luaopen_socket_core(lua_State* L);
extern int luaopen_toml(lua_State * L);
}
#include "lua_steam.h"
#include "LuaBinding/lua_random.hpp"
#include "LuaBinding/lua_particle.hpp"
#include "LuaBinding/Resource.hpp"

#include "Core/FileManager.hpp"
#include "Platform/CommandLineArguments.hpp"
#include "Platform/MessageBox.hpp"

namespace LuaSTGPlus
{
    static int StackTraceback(lua_State *L) noexcept
    {
        // errmsg
        int ret = 0;
        
        lua_getfield(L, LUA_GLOBALSINDEX, "debug");            // ??? errmsg t
        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1);                                     // ??? errmsg
            return 1;
        }
        lua_getfield(L, -1, "traceback");                      // ??? errmsg t f
        if (!lua_isfunction(L, -1) && !lua_iscfunction(L, -1))
        {
            lua_pop(L, 2);                                     // ??? errmsg
            return 1;
        }
        
        lua_pushvalue(L, 1);         // ??? errmsg t f errmsg
        lua_pushinteger(L, 2);       // ??? errmsg t f errmsg 2
        ret = lua_pcall(L, 2, 1, 0); // ??? errmsg t msg
        if (0 != ret)
        {
            char const* errmsg = lua_tostring(L, -1);
            if (errmsg == nullptr) {
                errmsg = "(error object is a nil value)";
            }
            spdlog::error("[luajit] StackTraceback时发生错误：{}", errmsg); // ??? errmsg t errmsg
            lua_pop(L, 2);                                                // ??? errmsg
            return 1;
        }
        
        return 1;
    }
    
    bool AppFrame::SafeCallScript(const char* source, size_t len, const char* desc) noexcept
    {
        lua_pushcfunction(L, &StackTraceback);          // ??? f
        int tStacktraceIndex = lua_gettop(L);
        if (0 != luaL_loadbuffer(L, source, len, desc)) // ??? f f/s
        {
            try
            {
                spdlog::error("[luajit] 编译'{}'失败：{}", desc, lua_tostring(L, -1));
                // MessageBoxW(
                //     m_pAppModel ? (HWND)m_pAppModel->getWindow()->getNativeHandle() : NULL,
                //     utf8::to_wstring(
                //         fmt::format("编译'{}'失败：{}", desc, lua_tostring(L, -1))
                //     ).c_str(),
                //     L"" LUASTG_INFO,
                //     MB_ICONERROR | MB_OK
                // );
            }
            catch (const std::bad_alloc&)
            {
                spdlog::error("[luastg] Error when logging");
            }
            lua_pop(L, 2);
            return false;
        }
        if (0 != lua_pcall(L, 0, 0, tStacktraceIndex)) // ??? f _/e
        {
            try
            {
                char const* errmsg = lua_tostring(L, -1);
                if (errmsg == nullptr) {
                    errmsg = "(error object is a nil value)";
                }
                spdlog::error("[luajit] Error when running '{}':{}", desc, errmsg);
                Platform::MessageBox::ErrorFromWindow(LUASTG_INFO, fmt::format("Error when running '{}':\n{}", desc, errmsg), m_pAppModel->getWindow()->getNativeHandle());
            }
            catch (const std::bad_alloc&)
            {
                spdlog::error("[luastg] Error when logging");
            }
            lua_pop(L, 2);
            return false;
        }
        lua_pop(L, 1);
        return true;
    }
    
    bool AppFrame::UnsafeCallGlobalFunction(const char* name, int retc) noexcept
    {
        lua_getglobal(L, name); // ... f
        if (lua_isfunction(L, -1) || lua_iscfunction(L, -1))
        {
            lua_call(L, 0, retc);
            return true;
        }
        return false;
    }
    
    bool AppFrame::SafeCallGlobalFunction(const char* name, int retc) noexcept
    {
        lua_pushcfunction(L, &StackTraceback); // ... f
        int tStacktraceIndex = lua_gettop(L);
        lua_getglobal(L, name);                // ... f f
        if (0 != lua_pcall(L, 0, retc, tStacktraceIndex))
        {
            try
            {
                char const* errmsg = lua_tostring(L, -1);
                if (errmsg == nullptr) {
                    errmsg = "(error object is a nil value)";
                }
                spdlog::error("[luajit] Error when calling global function '{}':{}", name, errmsg);
                Platform::MessageBox::ErrorFromWindow(LUASTG_INFO, fmt::format("Error when calling global function '{}':\n{}", name, errmsg), m_pAppModel->getWindow()->getNativeHandle());
            }
            catch (const std::bad_alloc&)
            {
                spdlog::error("[luastg] Error when logging");
            }
            lua_pop(L, 2);
            return false;
        }
        lua_remove(L, tStacktraceIndex);
        return true;
    }
    
    // TODO: 废弃
    bool AppFrame::SafeCallGlobalFunctionB(const char* name, int argc, int retc) noexcept
    {
        const int base_stack = lua_gettop(L) - argc;
        //																// ? ...
        lua_pushcfunction(L, &StackTraceback);							// ? ... trace
        lua_getglobal(L, name);											// ? ... trace func
        if (lua_type(L, lua_gettop(L)) != LUA_TFUNCTION)
        {
            //															// ? ... trace nil
        #ifdef _DEBUG
            try
            {
                spdlog::error("[luajit] Error calling global function '{}': global function '{}' does not exist", name, name);
                // Platform::MessageBox::ErrorFromWindow(LUASTG_INFO, fmt::format("[luajit] Error calling global function '{}': global function '{}' does not exist", name, name), m_pAppModel->getWindow()->getNativeHandle());
            }
            catch (const std::bad_alloc&)
            {
                spdlog::error("[luastg] Error when logging");
            }
        #endif
            lua_pop(L, argc + 2); 										// ?
            return false;
        }
        if (argc > 0)
        {
            lua_insert(L, base_stack + 1);								// ? func ... trace
            lua_insert(L, base_stack + 1);								// ? trace func ...
        }
        if (0 != lua_pcall(L, argc, retc, base_stack + 1))
        {
            //															// ? trace errmsg
            try
            {
                spdlog::error("[luajit] Error when calling global function '{}':{}", name, lua_tostring(L, -1));
                Platform::MessageBox::ErrorFromWindow(LUASTG_INFO, fmt::format("Error when calling global function '{}':\n{}", name, lua_tostring(L, -1)), m_pAppModel->getWindow()->getNativeHandle());
            }
            catch (const std::bad_alloc&)
            {
                spdlog::error("[luastg] Error when logging");
            }
            lua_pop(L, 2);												// ?
            return false;
        }
        else
        {
            if (retc > 0)
            {
                //														// ? trace ...
                lua_remove(L, base_stack + 1);							// ? ...
            }
            else
            {
                //														// ? trace
                lua_pop(L, 1);											// ?
            }
            return true;
        }
    }
    
    void AppFrame::LoadScript(lua_State* SL, const char* path, const char* packname)
    {
#define L (fuck) // 这里不能使用全局的 lua_State，必须使用传入的
        if (ResourceMgr::GetResourceLoadingLog())
        {
            if (packname)
                spdlog::info("[luastg] Loading script '{}' from package '{}'", packname, path);
            else
                spdlog::info("[luastg] Loading script '{}'", path);
        }
        bool loaded = false;
        std::vector<uint8_t> src;
        if (packname)
        {
            auto& arc = GFileManager().getFileArchive(packname);
            if (!arc.empty())
            {
                loaded = arc.load(path, src);
            }
        }
        else
        {
            loaded = GFileManager().loadEx(path, src);
        }
        if (!loaded)
        {
            spdlog::error("[luastg] Unable to load file '{}'", path);
            luaL_error(SL, "can't load file '%s'", path);
            return;
        }
        if (0 != luaL_loadbuffer(SL, (char const*)src.data(), (size_t)src.size(), luaL_checkstring(SL, 1)))
        {
            const char* tDetail = lua_tostring(SL, -1);
            spdlog::error("[luajit] Failed to compile '{}':{}", path, tDetail);
            luaL_error(SL, "failed to compile '%s': %s", path, tDetail);
            return;
        }
        lua_call(SL, 0, LUA_MULTRET);// This would normally only be called in lua code, which already has pcall in the outer layer
#undef L
    }
    
    bool AppFrame::OnOpenLuaEngine()
    {
        // Loading Lua VM
        spdlog::info("[luajit] {}", LUAJIT_VERSION);
        L = luaL_newstate();
        if (!L)
        {
            spdlog::error("[luajit] Unable to create luajit engine");
            return false;
        }
        if (0 == luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON))
        {
            spdlog::error("[luajit] Unble to start jit mode");
        }
        lua_gc(L, LUA_GCSTOP, 0);  // Turn off GC on init
        {
            spdlog::info("[luajit] Registering libraries and packages");
            luaL_openlibs(L);  // lua built-ins
            lua_register_custom_loader(L); // enhanced package loader (require)
            
            if (!SafeCallScript(LuaInternalSource_1().c_str(), LuaInternalSource_1().length(), "internal.main")) {
                spdlog::error("[luajit] Error in built-in script 'internal.main'");
                return false;
            }
            
            luaopen_cjson(L);
            luaopen_lfs(L);
            //lua_xlsx_open(L);
            //lua_csv_open(L);
            lua_steam_open(L);
            // lua_xinput_open(L);
            // luaopen_dwrite(L);
            luaopen_particle(L);
            luaopen_random(L);
            // luaopen_utf8(L);
            // luaopen_string_pack(L);
            {
                lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED"); // ... _LOADED
                {
                    luaopen_socket_core(L);        // ... _LOADED socket
                    lua_setfield(L, -2, "socket.core"); // ... _LOADED
                    luaopen_mime_core(L);          // ... _LOADED mime
                    lua_setfield(L, -2, "mime.core");   // ... _LOADED
                    luaopen_toml(L);               // ... _LOADED toml
                    lua_setfield(L, -2, "toml");   // ... _LOADED
                }
                lua_pop(L, 1); // ...
            }
            lua_settop(L, 0);
            
            RegistBuiltInClassWrapper(L);  // register built-in classes (luastg lib)
            luaopen_LuaSTG_Sub(L);
            lua_settop(L, 0);
            
            // set command line parameters
            spdlog::info("[luajit] Storing command line arguments");
            std::vector<std::string_view> args;
            Platform::CommandLineArguments::Get().GetArguments(args);
            if (!args.empty())
            {
                // print command line arguments, hide unwanted command line arguments
                std::vector<std::string_view> args_lua;
                for (size_t idx = 0; idx < args.size(); idx += 1)
                {
                    spdlog::info("[luajit] [{}] {}", idx, args[idx]);
                    if (args[idx] != "--log-window"
                        && args[idx] != "--log-window-wait")
                    {
                        args_lua.emplace_back(args[idx]);
                    }
                }
                // save
                lua_getglobal(L, "lstg");                       // ? t
                lua_createtable(L, (int)args_lua.size(), 0);    // ? t t
                for (int idx = 0; idx < (int)args_lua.size(); idx += 1)
                {
                    lua_pushstring(L, args_lua[idx].data());    // ? t t s
                    lua_rawseti(L, -2, idx + 1);                // ? t t
                }
                lua_setfield(L, -2, "args");                    // ? t
                lua_pop(L, 1);                                  // ?
            }
            
            if (!SafeCallScript(LuaInternalSource_2().c_str(), LuaInternalSource_2().length(), "internal.api")) {
                spdlog::error("[luajit] Error in built-in script 'internal.api'");
                return false;
            }
        }
        lua_gc(L, LUA_GCRESTART, -1);  // Restart GC
        
        return true;
    }
};
