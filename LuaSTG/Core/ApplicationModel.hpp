#pragma once
#include "Core/Type.hpp"
#include "Core/Graphics/Window.hpp"
#include "Core/Graphics/Device.hpp"
#include "Core/Graphics/SwapChain.hpp"
#include "Core/Graphics/Renderer.hpp"
#include "Core/Audio/Device.hpp"

namespace Core
{
    struct IFrameRateController
    {
        virtual double update() = 0;
        virtual uint32_t getTargetFPS() = 0;
        virtual void setTargetFPS(uint32_t target_FPS) = 0;
        virtual double getFPS() = 0;
        virtual uint64_t getTotalFrame() = 0;
        virtual double getTotalTime() = 0;
        virtual double getAvgFPS() = 0;
        virtual double getMinFPS() = 0;
        virtual double getMaxFPS() = 0;
    };

    struct IApplicationEventListener
    {
        // [Work Thread]
        virtual bool onUpdate() { return true; }
        // [Work Thread]
        virtual bool onRender() { return true; }
    };

    struct FrameStatistics
    {
        double total_time{};
        double wait_time{};
        double update_time{};
        double render_time{};
        double present_time{};
    };

    struct FrameRenderStatistics
    {
        double render_time{};
    };

    struct IApplicationModel : public IObject
    {
        // [Work Thread]
        virtual IFrameRateController* getFrameRateController() = 0;
        // [Main thread | Work Thread]
        virtual Graphics::IWindow* getWindow() = 0;
        // [Work Thread]
        virtual Graphics::IDevice* getDevice() = 0;
        // [Work Thread]
        virtual Graphics::ISwapChain* getSwapChain() = 0;
        // [Work Thread]
        virtual Graphics::IRenderer* getRenderer() = 0;
        // [Work Thread]
        virtual Audio::IAudioDevice* getAudioDevice() = 0;
        // [Work Thread]
        virtual FrameStatistics getFrameStatistics() = 0;
        // [Work Thread]
        virtual FrameRenderStatistics getFrameRenderStatistics() = 0;

        // [Main thread | Work Thread]
        virtual void requestExit() = 0;
        // [Main thread ]
        virtual bool run() = 0;

        static bool create(IApplicationEventListener* p_app, IApplicationModel** pp_model);
    };
}
