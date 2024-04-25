#include "Debugger/Logger.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/ostream_sink.h"
// #include "Platform/HResultChecker.hpp"
#include "Platform/CommandLineArguments.hpp"
#include "Core/InitializeConfigure.hpp"
// #include "utf8.hpp"

namespace LuaSTG::Debugger
{
    static bool enable_console = false;
    static bool open_console = false;
    static bool wait_console = false;
    static void openWin32Console();
    static void closeWin32Console();
    static std::string make_time_path();

    void Logger::create()
    {
        Core::InitializeConfigure config;
        config.loadFromFile("config.json");

    // #ifdef USING_CONSOLE_OUTPUT
    //     enable_console = Platform::CommandLineArguments::Get().IsOptionExist("--log-window");
    //     wait_console = Platform::CommandLineArguments::Get().IsOptionExist("--log-window-wait");
    //     // openWin32Console();
    // #endif

        std::vector<spdlog::sink_ptr> sinks;

        if (config.log_file_enable)
        {
            std::string parser_path;
            // if (config.log_file_path.empty())
            // {
                parser_path = "engine.log";
            // }
            // else
            // {
            //     Core::InitializeConfigure::parserFilePath(config.log_file_path, parser_path, true);
            // }
            auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(parser_path, true);
            sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%L] %v");
            sinks.emplace_back(sink);
        }
        
        // if (config.persistent_log_file_enable)
        // {
        //     std::filesystem::path path;
        //     if (config.persistent_log_file_directory.empty())
        //     {
        //         std::filesystem::path directory("logs/"); // TODO: Write here.
        //         path = directory / make_time_path();
        //     }
        //     else
        //     {
        //         std::string parser_path;
        //         Core::InitializeConfigure::parserDirectory(config.persistent_log_file_directory, parser_path, true);
        //         std::filesystem::path directory(parser_path);
        //         path = directory / make_time_path();
        //     }
        //     auto persistent_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.string(), true);
        //     persistent_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%L] %v");
        //     sinks.emplace_back(persistent_sink);
        // }
        
    #ifndef NDEBUG
        // auto sink_debugger = std::make_shared<spdlog::sinks::ostream_sink_mt>();
        // sink_debugger->set_pattern("[%Y-%m-%d %H:%M:%S] [%L] %v");
        // sinks.emplace_back(sink_debugger);
    #endif

    // #ifdef USING_CONSOLE_OUTPUT
        // if (open_console)
        // {
            auto sink_console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sink_console->set_pattern("%^[%Y-%m-%d %H:%M:%S] [%L]%$ %v");
            sinks.emplace_back(sink_console);
        // }
    // #endif

        auto logger = std::make_shared<spdlog::logger>("luastg", sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);
        //logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%L] %v");
        //logger->set_pattern("%^[%Y-%m-%d %H:%M:%S] [%L]%$ %v");
        //logger->set_pattern("%^[%Y-%m-%d %H:%M:%S] [%L] [%n]%$ %v");
        logger->flush_on(spdlog::level::info);

        spdlog::set_default_logger(logger);

        struct HResultCheckerCallback
        {
            static void WriteError(std::string_view const message)
            {
                spdlog::error("[luastg] {}", message);
            }
        };

        // Platform::HResultChecker::SetPrintCallback(&HResultCheckerCallback::WriteError);
    }
    void Logger::destroy()
    {
        if (auto logger = spdlog::get("luastg"))
        {
            logger->flush();
        }
        
        // Platform::HResultChecker::SetPrintCallback();

        spdlog::drop_all();
        spdlog::shutdown();

    // #ifdef USING_CONSOLE_OUTPUT
    //     closeWin32Console();
    // #endif

        // 清理 log 文件

        Core::InitializeConfigure config;
        config.loadFromFile("config.json");

        if (config.persistent_log_file_enable)
        {
            std::filesystem::path path;
            if (config.persistent_log_file_directory.empty())
            {
                std::filesystem::path directory("logs/"); // TODO: Write here.
                path = directory;
            }
            else
            {
                std::string parser_path;
                Core::InitializeConfigure::parserDirectory(config.persistent_log_file_directory, parser_path, false);
                std::filesystem::path directory(parser_path);
                path = directory;
            }
            // 下面就全都用宽字符了吧，省心
            std::error_code ec;
            std::vector<std::filesystem::path> logs;
            for (auto& it : std::filesystem::directory_iterator(path, ec)) {
                if (it.is_regular_file(ec) && it.path().extension() == L".log") {
                    logs.push_back(it.path());
                }
            }
            if (logs.size() > static_cast<size_t>(config.persistent_log_file_max_count)) {
                std::sort(logs.begin(), logs.end(), [](std::filesystem::path const& a, std::filesystem::path const& b) -> bool
                    {
                        return a.filename().generic_wstring() < b.filename().generic_wstring();
                    });
                size_t const remove_count = logs.size() - static_cast<size_t>(config.persistent_log_file_max_count);
                for (size_t idx = 0; idx < remove_count; idx += 1) {
                    std::filesystem::remove(logs[idx], ec);
                }
            }
        }
    }
};
