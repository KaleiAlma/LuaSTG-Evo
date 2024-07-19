#include "Platform/MessageBox.hpp"
#include "Platform/ApplicationSingleInstance.hpp"
#include "Platform/CommandLineArguments.hpp"
#include "Core/InitializeConfigure.hpp"
#include "Debugger/Logger.hpp"
#include "SteamAPI/SteamAPI.hpp"
#include "Utility/Utility.h"
#include "AppFrame.h"
#include "RuntimeCheck.hpp"

#include "SDL_main.h"

#ifdef __APPLE__
#include <filesystem>
#include "SDL_filesystem.h"
#endif

#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif // _WIN32

int main(int argc, char *argv[])
{
#ifdef _DEBUG
#ifdef _WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	// _CrtSetBreakAlloc(5351);
#endif
#endif
	Platform::CommandLineArguments::Get().Update(argc, argv);

	// [[maybe_unused]] Platform::ApplicationSingleInstance single_instance(LUASTG_INFO);
	// Core::InitializeConfigure cfg;
	// if (cfg.loadFromFile("config.json")) {
	// 	if (cfg.single_application_instance && !cfg.application_instance_id.empty()) {
	// 		single_instance.Initialize(cfg.application_instance_id);
	// 	}
	// }

	// if (!LuaSTG::CheckUserRuntime())
	// {
	// 	return EXIT_FAILURE;
	// }

	// LuaSTGPlus::CoInitializeScope com_runtime;
	// if (!com_runtime())
	// {
	// 	Platform::MessageBox::Error(LUASTG_INFO,
	// 		"Engine Initialization Failed!\n"
	// 		"Failed to initialize COM runtime, try relaunching.");
	// 	return EXIT_FAILURE;
	// }

#ifdef __APPLE__
	std::filesystem::create_directory(SDL_GetBasePath());
	std::filesystem::current_path(SDL_GetBasePath());
#endif

	LuaSTG::Debugger::Logger::create();

	int result = EXIT_SUCCESS;
	if (LuaSTG::SteamAPI::Init())
	{
		if (LAPP.Init())
		{
			LAPP.Run();
			result = EXIT_SUCCESS;
		}
		else
		{
			Platform::MessageBox::Error(LUASTG_INFO,
				"Engine Initialization Failed!\n"
				"See engine.log for details.\n"
				"Please try relaunching. If the problem persists, try contacting the developer.");
			result = EXIT_FAILURE;
		}
		LAPP.Shutdown();
		LuaSTG::SteamAPI::Shutdown();
	}
	else
	{
		result = EXIT_FAILURE;
	}

	LuaSTG::Debugger::Logger::destroy();

	return result;
}

// #include "Platform/CleanWindows.hpp"

// _Use_decl_annotations_ int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
// {
// 	return main();
// }
