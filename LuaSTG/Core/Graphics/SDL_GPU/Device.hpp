#pragma once
#include "Core/Object.hpp"
#include "Core/Graphics/Interface/Device.hpp"
#include "Core/Type.hpp"
#include <SDL3/SDL.h>
#include <cstddef>
#include <cstdint>
#include <optional>
// #include "Platform/RuntimeLoader/DXGI.hpp"
// #include "Platform/RuntimeLoader/Direct3D11.hpp"
// #include "Platform/RuntimeLoader/Direct2D1.hpp"
// #include "Platform/RuntimeLoader/DirectWrite.hpp"

namespace Core::Graphics::SDL_GPU {
    class Device : public Object<IDevice> {
    private:
        SDL_GPUDevice* device;
    private:
        enum class EventType {
            DeviceCreate,
            DeviceDestroy,
        };
        bool m_is_dispatch_event{ false };
        std::vector<IDeviceEventListener*> m_eventobj;
        std::vector<IDeviceEventListener*> m_eventobj_late;
    private:
        void dispatchEvent(EventType t);
    public:
        void addEventListener(IDeviceEventListener* e);
        void removeEventListener(IDeviceEventListener* e);

        bool recreate();
        void setPreferenceGpu(StringView preferred_gpu);
        int32_t getGpuCount();
        StringView getGpuName(int32_t index);
        StringView getCurrentGpuName() const noexcept;

        void* getNativeHandle() { return device; }
        // void* getNativeRendererHandle() { return SDL_GL_GetCurrentContext(); }

        bool createTextureFromFile(StringView path, bool mipmap, ITexture2D** pp_texutre);
        bool createTextureFromMemory(void const* data, size_t size, bool mipmap, ITexture2D** pp_texutre);
        bool createTexture(Vector2U size, ITexture2D** pp_texutre);

        bool createRenderTarget(Vector2U size, IRenderTarget** pp_rt);
        bool createDepthStencilBuffer(Vector2U size, IDepthStencilBuffer** pp_ds);

    public:
        Device();
        ~Device();

    public:
        static bool create(Device** p_device);
    };

    class Texture2D
        : public Object<ITexture2D>
        , public IDeviceEventListener {
    private:
        ScopeObject<Device> m_device;
        std::optional<SamplerState> m_sampler;
        ScopeObject<IData> m_data;
        std::string source_path;
        SDL_GPUTexture* texture{ nullptr };
        Vector2U m_size{};
        bool m_dynamic{ false };
        bool m_premul{ false };
        bool m_mipmap{ false };
        bool m_isrt{ false };

    public:
        void onDeviceCreate();
        void onDeviceDestroy();

        bool createResource();

    public:
        SDL_GPUTexture* GetResource() { return texture; }
        SDL_GPUTexture* GetView() { return texture; }

    public:
        void* getNativeHandle() { return texture; }

        bool isDynamic() { return m_dynamic; }
        bool isPremultipliedAlpha() { return m_premul; }
        void setPremultipliedAlpha(bool v) { m_premul = v; }
        Vector2U getSize() { return m_size; }
        bool setSize(Vector2U size);

        bool uploadPixelData(RectU rc, void const* data, uint32_t pitch);
        void setPixelData(IData* p_data) { m_data = p_data; }

        bool saveToFile(StringView path);

        void setSamplerState(SamplerState sampler) { m_sampler = sampler; }
        std::optional<SamplerState> getSamplerState() { return m_sampler; }

    public:
        Texture2D(Device* device, StringView path, bool mipmap);
        Texture2D(Device* device, void const* data, size_t size, bool mipmap);
        Texture2D(Device* device, Vector2U size, bool rendertarget); // if rendertarget, then hand over control to RenderTarget
        ~Texture2D();
    };

    class DepthStencilBuffer
        : public Object<IDepthStencilBuffer>
        , public IDeviceEventListener {
    private:
        ScopeObject<Device> m_device;
        SDL_GPUTexture* m_texture = 0;
        Vector2U m_size{};

    public:
        void onDeviceCreate();
        void onDeviceDestroy();

        bool createResource();

    public:
        SDL_GPUTexture* GetResource() { return m_texture; }
        SDL_GPUTexture* GetView() { return m_texture; }

    public:
        void* getNativeHandle() { return (void*)(intptr_t)m_texture; }

        bool setSize(Vector2U size);
        Vector2U getSize() { return m_size; }

    public:
        DepthStencilBuffer(Device* device, Vector2U size);
        ~DepthStencilBuffer();
    };

    class RenderTarget
        : public Object<IRenderTarget>
        , public IDeviceEventListener {
    private:
        ScopeObject<Device> m_device;
        ScopeObject<Texture2D> m_texture;
        ScopeObject<DepthStencilBuffer> m_depthstencilbuffer;
        bool m_depthstencilbuffer_enable = false;

    public:
        void onDeviceCreate();
        void onDeviceDestroy();

        bool createResource();

    public:
        bool depthStencilBufferEnabled() { return m_depthstencilbuffer_enable; }

    public:
        bool setSize(Vector2U size);
        ITexture2D* getTexture() { return *m_texture; }

    public:
        RenderTarget(Device* device, Vector2U size);
        ~RenderTarget();
    };
}
