#pragma once
#include <string_view>
#include "SDL_video.h"

// FUCK MICROSOFT

#ifdef MessageBox
#undef MessageBox
#endif

namespace Platform
{
    class MessageBox
    {
    public:
        static bool Warning(std::string_view title, std::string_view message);
        static void Error(std::string_view title, std::string_view message);
        static bool WarningFromWindow(std::string_view title, std::string_view message, void* window);
        static void ErrorFromWindow(std::string_view title, std::string_view message, void* window);
        static bool Show(int type, std::string_view title, std::string_view message, SDL_Window* window = NULL);
    };
}
