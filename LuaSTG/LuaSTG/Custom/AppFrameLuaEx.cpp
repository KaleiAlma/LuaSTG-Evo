#include "AppFrame.h"
#include "Config.h"
#include "Core/FileManager.hpp"
#include "Core/InitializeConfigure.hpp"

namespace LuaSTGPlus
{
    bool AppFrame::OnLoadLaunchScriptAndFiles()
    {
        bool is_launch_loaded = false;
        #ifdef USING_LAUNCH_FILE
        spdlog::info("[luastg] Loading launch script");
        std::vector<uint8_t> src;
        if (GFileManager().loadEx("launch", src))
        {
            if (SafeCallScript((char const*)src.data(), src.size(), "launch"))
            {
                is_launch_loaded = true;
                spdlog::info("[luastg] Loaded launch script");
            }
            else
            {
                spdlog::error("[luastg] Loading launch script failed");
            }
        }
        #endif
        if (!is_launch_loaded)
        {
            Core::InitializeConfigure config;
            if (config.loadFromFile("config.json"))
            {
                spdlog::info("[luastg] Engine config found: 'config.json'");
                LAPP.SetWindowed(!config.fullscreen_enable);
                LAPP.SetVsync(config.vsync_enable);
                LAPP.SetResolution(config.canvas_width, config.canvas_height);
                is_launch_loaded = true;
            }
        }
        #ifdef USING_LAUNCH_FILE
        if (!is_launch_loaded)
        {
            spdlog::error("[luastg] Can't find launch script ('./launch')");
        }
        #endif

        return true;
    };
    
    bool AppFrame::OnLoadMainScriptAndFiles()
    {
        spdlog::info("[luastg] Load main script");
        std::string_view entry_scripts[3] = {
            "core.lua",
            "main.lua",
            "src/main.lua",
        };
        std::vector<uint8_t> src;
        bool is_load = false;
        for (auto& v : entry_scripts)
        {
            if (GFileManager().loadEx(v, src))
            {
                if (SafeCallScript((char const*)src.data(), src.size(), v.data()))
                {
                    spdlog::info("[luastg] Loading main script '{}'", v);
                    is_load = true;
                    break;
                }
            }
        }
        if (!is_load)
        {
            spdlog::error("[luastg] No main script found (searched: '{}', '{}', '{}')", entry_scripts[0], entry_scripts[1], entry_scripts[2]);
        }
        return true;
    }
}
