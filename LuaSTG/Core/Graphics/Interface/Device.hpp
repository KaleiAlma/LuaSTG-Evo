#pragma once
#include "Core/Type.hpp"
#include "Core/Graphics/Interface/Texture2D.hpp"
#include "Core/Graphics/Interface/RenderTarget.hpp"
#include "Core/Graphics/Interface/DepthStencilBuffer.hpp"

namespace Core::Graphics {
    struct IDeviceEventListener {
        virtual void onDeviceCreate() = 0;
        virtual void onDeviceDestroy() = 0;
    };

    struct IDevice : public IObject {
        virtual void addEventListener(IDeviceEventListener* e) = 0;
        virtual void removeEventListener(IDeviceEventListener* e) = 0;

        virtual bool recreate() = 0;
        virtual void setPreferenceGpu(StringView preferred_gpu) = 0;
        virtual int32_t getGpuCount() = 0;
        virtual StringView getGpuName(int32_t index) = 0;
        virtual StringView getCurrentGpuName() const noexcept = 0;

        virtual void* getNativeHandle() = 0;
        // virtual void* getNativeRendererHandle() = 0;

        virtual bool createTextureFromFile(StringView path, bool mipmap, ITexture2D** pp_texutre) = 0;
        virtual bool createTextureFromMemory(void const* data, size_t size, bool mipmap, ITexture2D** pp_texutre) = 0;
        virtual bool createTexture(Vector2U size, ITexture2D** pp_texutre) = 0;

        virtual bool createRenderTarget(Vector2U size, IRenderTarget** pp_rt) = 0;
        virtual bool createDepthStencilBuffer(Vector2U size, IDepthStencilBuffer** pp_ds) = 0;

        static bool create(IDevice** p_device);
    };
}
