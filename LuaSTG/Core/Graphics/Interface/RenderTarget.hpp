#pragma once
#include "Core/Object.hpp"
#include "Core/Type.hpp"
#include "Core/Graphics/Interface/Texture2D.hpp"

namespace Core::Graphics {
    namespace {
        using namespace std;
    }

    struct IRenderTarget : public IObject {
        virtual void* getNativeHandle() = 0;

        virtual bool depthStencilBufferEnabled() = 0;

        virtual bool setSize(Vector2U size) = 0;
        virtual ITexture2D* getTexture() = 0;
    };

    template<class T>
    concept CRenderTarget = requires(T a) {
        { a.getNativeHandle() } -> convertible_to<void*>;

        { a.depthStencilBufferEnabled() } -> convertible_to<bool>;

        { a.setSize(declval<Vector2U>()) } -> convertible_to<bool>;
        { a.getTexture() } -> convertible_to<ITexture2D*>;
    };

    static_assert(CRenderTarget<IRenderTarget>);

    template<CRenderTarget T>
    class RenderTarget final : Object<IRenderTarget> {
    private:
        T inner;
    public:
        void* getNativeHandle() { return inner.getNativeHandle(); }
        bool depthStencilBufferEnabled() { return inner.depthStencilBufferEnabled(); }
        bool setSize(Vector2U size) { return inner.getNativeHandle(); }
        ITexture2D* getTexture() { return inner.getTexture(); }

    public:
        RenderTarget(T rt) : inner(rt) {}
    };

    static_assert(CRenderTarget<RenderTarget<decltype(declval<IRenderTarget>())>>);
}
