#include "AppFrame.h"
#include "Core/Type.hpp"
#include "SDL.h"
#include <cstddef>

// #include "Platform/Keyboard.hpp"

namespace LuaSTGPlus
{
    // static Platform::Keyboard g_Keyboard;
    // static Platform::Keyboard::State g_KeyboardState;
    static SDL_Keycode lastKeyDown;
    static SDL_Keycode lastKeyUp;
    static Core::Vector2I mouseWheelDelta;
}

static struct InputEventListener : public Core::Graphics::IWindowEventListener
{
    NativeWindowMessageResult onNativeWindowMessage(void* ev)
    {
        SDL_Event* e = reinterpret_cast<SDL_Event*>(ev);
        switch (e->type)
        {
        case SDL_KEYDOWN:
            LuaSTGPlus::lastKeyDown = e->key.keysym.sym;
            break;
        case SDL_KEYUP:
            LuaSTGPlus::lastKeyUp = e->key.keysym.sym;
            break;
        case SDL_MOUSEWHEEL:
            LuaSTGPlus::mouseWheelDelta = Core::Vector2I(e->wheel.x, e->wheel.y);
            break;
        }

        // switch (e->type)
        // {
        // case SDL_MOUSEMOTION:
        // case SDL_MOUSEBUTTONDOWN:
        // case SDL_MOUSEBUTTONUP:
        // case SDL_MOUSEWHEEL:
        // case SDL_WINDOWEVENT:
        //     LuaSTGPlus::g_Mouse.ProcessMessage(ev);
        //     break;
        // }

        return {};
    }
} g_InputEventListener;

namespace LuaSTGPlus
{
    // static std::unique_ptr<DirectX::Mouse> Mouse;
    // static DirectX::Mouse::State MouseState;

    void AppFrame::OpenInput()
    {
        // g_Keyboard.Reset();
        // Mouse = std::make_unique<DirectX::Mouse>();
        // ZeroMemory(&MouseState, sizeof(MouseState));
        m_pAppModel->getWindow()->addEventListener(&g_InputEventListener);
        // Mouse->SetWindow((HWND)m_pAppModel->getWindow()->getNativeHandle());
    }
    void AppFrame::CloseInput()
    {
        m_pAppModel->getWindow()->removeEventListener(&g_InputEventListener);
        // Mouse = nullptr;
    }
    void AppFrame::UpdateInput()
    {
        // g_Keyboard.GetState(g_KeyboardState, true);
        // if (Mouse)
        // {
        //     MouseState = Mouse->GetState();
        //     Mouse->ResetScrollWheelValue();
        // }
        // else
        // {
        //     ZeroMemory(&MouseState, sizeof(MouseState));
        // }
    }
    void AppFrame::ResetKeyboardInput()
    {
        lastKeyDown = -1;
        lastKeyUp = -1;
        // g_KeyboardState.Reset();
    }
    void AppFrame::ResetMouseInput()
    {
        // ZeroMemory(&MouseState, sizeof(MouseState));
    }

    bool AppFrame::GetKeyState(int VKCode)noexcept
    {
        return SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(VKCode)];
    }

    int AppFrame::GetLastKey()noexcept
    {
        return (int)lastKeyDown;
    }
    
    inline Core::Vector2F MapLetterBoxingPosition(Core::Vector2U isize, Core::Vector2I osize, Core::Vector2I pos)
    {
        float const hscale = (float)osize.x / (float)isize.x;
        float const vscale = (float)osize.y / (float)isize.y;
        float const scale = std::min(hscale, vscale);
        float const sizew = scale * (float)isize.x;
        float const sizeh = scale * (float)isize.y;
        float const dx = ((float)osize.x - sizew) * 0.5f;
        float const dy = ((float)osize.y - sizeh) * 0.5f;
        float const x1 = (float)pos.x - dx;
        float const y1 = (float)pos.y - dy;
        float const x2 = x1 / scale;
        float const y2 = y1 / scale;
        return Core::Vector2F(x2, y2);
    }

    // bool AppFrame::GetMouseState_legacy(int button)noexcept
    // {
    //     switch (button)
    //     {
    //     case 0:
    //         return MouseState.leftButton;
    //     case 1:
    //         return MouseState.middleButton;
    //     case 2:
    //         return MouseState.rightButton;
    //     case 3:
    //         return MouseState.xButton1;
    //     case 4:
    //         return MouseState.xButton2;
    //     default:
    //         return false;
    //     }
    // }
    bool AppFrame::GetMouseState(int button)noexcept
    {
        // switch (button)
        // {
        // case VK_LBUTTON:
        //     return MouseState.leftButton;
        // case VK_MBUTTON:
        //     return MouseState.middleButton;
        // case VK_RBUTTON:
        //     return MouseState.rightButton;
        // case VK_XBUTTON1:
        //     return MouseState.xButton1;
        // case VK_XBUTTON2:
        //     return MouseState.xButton2;
        // default:
        //     return false;
        // }
        return SDL_BUTTON(button) & SDL_GetMouseState(NULL, NULL);
    }
    Core::Vector2F AppFrame::GetMousePosition(bool no_flip)noexcept
    {
        if (m_sdl_window_size.x == 0 || m_sdl_window_size.y == 0)
        {
            SDL_GetWindowSizeInPixels(reinterpret_cast<SDL_Window*>(GetAppModel()->getWindow()->getNativeHandle()), &m_sdl_window_size.x, &m_sdl_window_size.y);
        }
        auto const w_size = m_sdl_window_size;
        auto const c_size = GetAppModel()->getSwapChain()->getCanvasSize();
        Core::Vector2I mouse_pos;
        SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        auto const m_p = MapLetterBoxingPosition(c_size, w_size, mouse_pos);
        if (no_flip)
        {
            return m_p;
        }
        else
        {
            return Core::Vector2F(m_p.x, (float)c_size.y - m_p.y);
        }
    }
    Core::Vector2F AppFrame::GetCurrentWindowSizeF()
    {
        if (m_sdl_window_size.x == 0 || m_sdl_window_size.y == 0)
        {
            SDL_GetWindowSizeInPixels(reinterpret_cast<SDL_Window*>(GetAppModel()->getWindow()->getNativeHandle()), &m_sdl_window_size.x, &m_sdl_window_size.y);
        }
        auto const w_size = m_sdl_window_size;
        return Core::Vector2F((float)w_size.x, (float)w_size.y);
    }
    Core::Vector4F AppFrame::GetMousePositionTransformF()
    {
        if (m_sdl_window_size.x == 0 || m_sdl_window_size.y == 0)
        {
            SDL_GetWindowSizeInPixels(reinterpret_cast<SDL_Window*>(GetAppModel()->getWindow()->getNativeHandle()), &m_sdl_window_size.x, &m_sdl_window_size.y);
        }
        auto const w_size = m_sdl_window_size;
        auto const c_size = GetAppModel()->getSwapChain()->getCanvasSize();

        float const hscale = (float)w_size.x / (float)c_size.x;
        float const vscale = (float)w_size.y / (float)c_size.y;
        float const scale = std::min(hscale, vscale);
        float const sizew = scale * (float)c_size.x;
        float const sizeh = scale * (float)c_size.y;
        float const dx = ((float)w_size.x - sizew) * 0.5f;
        float const dy = ((float)w_size.y - sizeh) * 0.5f;

        return Core::Vector4F(-dx, -dy, 1.0f / scale, 1.0f / scale);
    }
    int32_t AppFrame::GetMouseWheelDelta()noexcept
    {
        return mouseWheelDelta.y;
    }
};
