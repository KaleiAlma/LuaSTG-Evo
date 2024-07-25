#pragma once
#include "Core/Type.hpp"
#include <cstdint>
#include <string_view>

namespace Core::Graphics
{
    struct IWindowEventListener
    {
        virtual void onWindowCreate() {};
        virtual void onWindowDestroy() {};

        virtual void onWindowActive() {};
        virtual void onWindowInactive() {};

        virtual void onWindowSize(Vector2I size) { (void)size; };

        virtual void onWindowClose() {};

        virtual void onDeviceChange() {};

        struct NativeWindowMessageResult
        {
            intptr_t result;
            bool should_return;

            NativeWindowMessageResult() : result(0), should_return(false) {}
            NativeWindowMessageResult(intptr_t v, bool b) : result(v), should_return(b) {}
        };

        virtual NativeWindowMessageResult onNativeWindowMessage(void*) { return {}; };
    };

    enum class WindowFrameStyle
    {
        None,
        Fixed,
        Normal,
    };

    enum class WindowLayer
    {
        Unknown,
        Invisible,
        Normal,
        Top,
    };

    enum class WindowCursor
    {
        None,

        Arrow,
        Hand,

        Cross,
        TextInput,

        Resize,
        ResizeEW,
        ResizeNS,
        ResizeNESW,
        ResizeNWSE,

        NotAllowed,
        Wait,
    };

    enum class FullscreenMode
    {
        Windowed,
        Borderless,
        Exclusive,
    };

    struct IWindow : public IObject
    {
        virtual void addEventListener(IWindowEventListener* e) = 0;
        virtual void removeEventListener(IWindowEventListener* e) = 0;

        virtual void* getNativeHandle() = 0;

        virtual void setTitleText(StringView str) = 0;
        virtual StringView getTitleText() = 0;

        virtual bool setFrameStyle(WindowFrameStyle style) = 0;
        virtual WindowFrameStyle getFrameStyle() = 0;

        virtual Vector2U getSize() = 0;
        virtual bool setSize(Vector2U v) = 0;

        virtual WindowLayer getLayer() = 0;
        virtual bool setLayer(WindowLayer layer) = 0;

        virtual void setWindowMode(Vector2U size) = 0;
        virtual void setExclusiveFullScreenMode() = 0;
        virtual void setBorderlessFullScreenMode() = 0;

        virtual uint32_t getMonitorCount() = 0;
        virtual RectI getMonitorRect(uint32_t index) = 0;
        virtual void setMonitorCentered(uint32_t index) = 0;
        virtual void setMonitorFullScreen(uint32_t index) = 0;

        virtual bool setCursor(WindowCursor type) = 0;
        virtual WindowCursor getCursor() = 0;

        virtual void setTextInputEnable(bool enable) = 0;
        virtual std::string getTextInput() = 0;
        virtual std::string getIMEComp() = 0;
        virtual void setTextInput(StringView text) = 0;
        virtual void clearTextInput() = 0;
        virtual uint32_t getTextInputLength() = 0;
        virtual uint32_t getTextCursorPos() = 0;
        virtual uint32_t getTextCursorPosRaw() = 0;
        virtual int32_t getIMECursorPos() = 0;
        virtual bool setTextCursorPos(uint32_t pos) = 0;
        virtual void insertInputTextAtCursor(StringView text, bool move_cursor) = 0;
        virtual bool insertInputText(StringView text, uint32_t pos) = 0;
        virtual uint32_t removeInputTextAtCursor(uint32_t length, bool after) = 0;
        virtual int32_t removeInputText(uint32_t length, uint32_t pos) = 0;
        virtual void setTextInputReturnEnable(bool enable) = 0;
        virtual void setTextInputRect(RectI rect) = 0;

        virtual std::string getClipboardText() = 0;
        virtual bool setClipboardText(StringView text) = 0;

        static bool create(IWindow** pp_window);
        static bool create(Vector2U size, StringView title_text, WindowFrameStyle style, bool show, IWindow** pp_window);
    };
}
