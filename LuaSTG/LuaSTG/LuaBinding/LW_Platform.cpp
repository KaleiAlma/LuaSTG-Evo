#include "LuaBinding/LuaWrapper.hpp"
#include "Platform/MessageBox.hpp"
#include "lua.h"
#include "lua_utility.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

void LuaSTGPlus::LuaWrapper::PlatformWrapper::Register(lua_State* L) noexcept
{
    struct Wrapper
    {
        static int Execute(lua_State* L) noexcept
        {
            struct Detail_
            {
                static bool Execute(const char* path, const char* args, const char* directory, bool bWait, bool bShow) noexcept
                {
                    try
                    {
#ifdef _WIN32

                        SHELLEXECUTEINFOA tShellExecuteInfo;
                        memset(&tShellExecuteInfo, 0, sizeof(SHELLEXECUTEINFO));

                        tShellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFO);
                        tShellExecuteInfo.fMask = bWait ? SEE_MASK_NOCLOSEPROCESS : 0;
                        tShellExecuteInfo.lpVerb = "open";
                        tShellExecuteInfo.lpFile = path;
                        tShellExecuteInfo.lpParameters = args;
                        tShellExecuteInfo.lpDirectory = directory;
                        tShellExecuteInfo.nShow = bShow ? SW_SHOWDEFAULT : SW_HIDE;
                        
                        if (FALSE == ShellExecuteExA(&tShellExecuteInfo))
                            return false;

                        if (bWait)
                        {
                            WaitForSingleObject(tShellExecuteInfo.hProcess, INFINITE);
                            CloseHandle(tShellExecuteInfo.hProcess);
                        }
#else

                        // execute
                        pid_t pid = fork();

                        if (pid == -1)
                        {
                            spdlog::error("[luastg] (Execute) call to fork() failed");
                            return false;
                        }
                        else if (pid == 0)
                        {
                            std::vector<const char*> args_vec;
                            std::vector<std::string> work;

                            if (directory)
                            {
                                chdir(directory);
                            }

                            // prepare args
                            intptr_t start_idx = 0;
                            args_vec.push_back(path);
                            const char* p_args = args;
                            while (*p_args != '\0')
                            {
                                if (*p_args == '\\')
                                    p_args++;
                                if (*p_args == '\0')
                                    break;
                                if (*p_args == ' ')
                                {
                                    work.push_back(std::string(args + start_idx, p_args - args + start_idx));
                                    args_vec.push_back(work.back().c_str());
                                    start_idx = p_args - args;
                                }
                                p_args++;
                            }
                            args_vec.push_back(NULL);

                            execvp(path, (char* const*)args_vec.data());
                            exit(1);
                        }
                        else if (bWait)
                        {
                            int status = 0;
                            waitpid(pid, &status, 0);
                        }
#endif
                        return true;
                    }
                    catch (const std::bad_alloc&)
                    {
                        return false;
                    }
                }
            };

            const char* path = luaL_checkstring(L, 1);
            const char* args = luaL_optstring(L, 2, "");
            const char* directory = luaL_optstring(L, 3, NULL);
            bool bWait = true;
            bool bShow = true;
            if (lua_gettop(L) >= 4)
                bWait = lua_toboolean(L, 4) == 0 ? false : true;
            if (lua_gettop(L) >= 5)
                bShow = lua_toboolean(L, 5) == 0 ? false : true;
            
            lua_pushboolean(L, Detail_::Execute(path, args, directory, bWait, bShow));
            return 1;
        }
        static int api_MessageBox(lua_State* L)
        {
            std::string_view title = luaL_check_string_view(L, 1);
            std::string_view text = luaL_check_string_view(L, 2);
            bool result = Platform::MessageBox::Show(luaL_checkinteger(L, 3), title, text);
            lua_pushboolean(L, result);
            return 1;
        }
    };

    luaL_Reg const lib[] = {
        { "Execute", &Wrapper::Execute },
        { "MessageBox", &Wrapper::api_MessageBox },
        { NULL, NULL },
    };

    luaL_Reg const lib_empty[] = {
        { NULL, NULL },
    };

    luaL_register(L, LUASTG_LUA_LIBNAME, lib);             // ??? lstg
    luaL_register(L, LUASTG_LUA_LIBNAME ".Platform", lib); // ??? lstg lstg.Platform
    lua_setfield(L, -1, "Platform");                       // ??? lstg
    lua_pop(L, 1);                                         // ???
}
