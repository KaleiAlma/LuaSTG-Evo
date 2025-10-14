#pragma once
#include "Core/Graphics/Interface/Device.hpp"
#include "Core/Type.hpp"

namespace Core::Graphics {
    struct IRenderMode : public IObject {
        virtual void* getNativeHandle() = 0;

        static bool create(IDevice* p_device, IRenderMode** pp_rdrmode);
    };
}
