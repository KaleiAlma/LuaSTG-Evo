#pragma once
#include "Core/Graphics/Interface/Device.hpp"
#include "Core/Type.hpp"

namespace Core::Graphics {
    namespace {
        using namespace std;
    }

    enum class ShaderStage {
        Vertex,
        Fragment,
        Compute,
    };

    template<ShaderStage S>
    struct IShader : public IObject {
        virtual void* getNativeHandle() = 0;

        static bool create(IDevice* p_device, IShader** pp_shader);
    };

    template<template<ShaderStage> class T, ShaderStage S>
    concept CShader = requires(T<S> a) {
        { a.getNativeHandle() } -> convertible_to<void*>;
    };
}
