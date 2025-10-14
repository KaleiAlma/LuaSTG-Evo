#pragma once
#include "Core/Type.hpp"

namespace Core::Graphics {
    namespace {
        using namespace std;
    }

    struct IDepthStencilBuffer : public IObject {
        virtual void* getNativeHandle() = 0;

        virtual bool setSize(Vector2U size) = 0;
        virtual Vector2U getSize() = 0;
    };

    template<class T>
    concept CDepthStencilBuffer = requires(T a) {
        { a.getNativeHandle() } -> convertible_to<void*>;
        { a.setSize(a.getSize()) } -> convertible_to<bool>;
    };
}
