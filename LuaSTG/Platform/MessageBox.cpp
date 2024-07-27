#include "MessageBox.hpp"
#include <cstdint>
#include <sys/types.h>
#include "spdlog/spdlog.h"
#include "SDL_messagebox.h"

namespace Platform
{
    bool MessageBox::Show(int type, std::string_view title, std::string_view message, SDL_Window* window)
    {
        std::string str_title(std::move(title));
        std::string str_message(std::move(message));
        SDL_MessageBoxData msg_box;
        msg_box.flags = SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
        msg_box.window = window;
        msg_box.title = str_title.c_str();
        msg_box.message = str_message.c_str();
        SDL_MessageBoxButtonData* btn;

        switch (type)
        {
        case 1:
            msg_box.flags |= SDL_MESSAGEBOX_WARNING;
            // msg_box.flags |= MB_OKCANCEL;
            msg_box.numbuttons = 2;
            btn = new SDL_MessageBoxButtonData[2];
            btn[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            btn[0].text = "OK";
            btn[0].buttonid = 0;
            btn[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            btn[1].text = "Cancel";
            btn[1].buttonid = 1;
            msg_box.buttons = btn;
            break;
        case 2:
            msg_box.flags |= SDL_MESSAGEBOX_ERROR;
            // msg_box.flags |= MB_OK;
            msg_box.numbuttons = 1;
            btn = new SDL_MessageBoxButtonData[1];
            btn[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            btn[0].text = "OK";
            btn[0].buttonid = 0;
            msg_box.buttons = btn;
            break;
        default:
            assert(false);
            break;
        }

        int32_t b_id;
        if (SDL_ShowMessageBox(&msg_box, &b_id))
        {
            spdlog::error("[luastg] (GetError = {}) SDL_ShowMessageBox failed", SDL_GetError());
            // assert(false);
        }
        return 0 == b_id;
        // return IDOK == MessageBoxW(window, wide_message.c_str(), wide_title.c_str(), flags);
        
    }
    bool MessageBox::Warning(std::string_view title, std::string_view message)
    {
        return Show(1, title, message);
    }
    void MessageBox::Error(std::string_view title, std::string_view message)
    {
        Show(2, title, message);
    }
    bool MessageBox::WarningFromWindow(std::string_view title, std::string_view message, void* window)
    {
        return Show(1, title, message, (SDL_Window*)window);
    }
    void MessageBox::ErrorFromWindow(std::string_view title, std::string_view message, void* window)
    {
        Show(2, title, message, (SDL_Window*)window);
    }
}
