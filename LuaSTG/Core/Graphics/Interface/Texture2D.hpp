#pragma once
#include "Core/Type.hpp"
#include <limits>
#include <optional>

namespace Core::Graphics {
    namespace {
        using namespace std;
    }

    enum class FilterMode {
        Nearest,
        NearestMipNearest,
        NearestMipLinear,
        Linear,
        LinearMipNearest,
        LinearMipLinear,
    };

    struct Filter {
        FilterMode min;
        FilterMode mag;

        Filter(FilterMode min, FilterMode mag)
        : min(min)
        , mag(mag) {}
    };

    enum class TextureAddressMode {
        Wrap,
        Mirror,
        Clamp,
        Border,
    };

    enum class BorderColor {
        Black,
        OpaqueBlack,
        TransparentWhite,
        White,
    };

    struct SamplerState {
        Filter filter;
        TextureAddressMode address_u;
        TextureAddressMode address_v;
        float mip_lod_bias;
        uint32_t max_anisotropy;
        float min_lod;
        float max_lod;
        BorderColor border_color;
        SamplerState()
            : filter(Filter(FilterMode::Linear, FilterMode::Linear))
            , address_u(TextureAddressMode::Clamp)
            , address_v(TextureAddressMode::Clamp)
            , mip_lod_bias(0.0f)
            , max_anisotropy(1u)
            , min_lod(-std::numeric_limits<float>::max())
            , max_lod(std::numeric_limits<float>::max())
            , border_color(BorderColor::Black) {}
        SamplerState(Filter filter_, TextureAddressMode address_)
            : filter(filter_)
            , address_u(address_)
            , address_v(address_)
            , mip_lod_bias(0.0f)
            , max_anisotropy(1u)
            , min_lod(-std::numeric_limits<float>::max())
            , max_lod(std::numeric_limits<float>::max())
            , border_color(BorderColor::Black) {}
    };

    struct ITexture2D : public IObject {
        virtual void* getNativeHandle() = 0;

        virtual bool isDynamic() = 0;
        virtual bool isPremultipliedAlpha() = 0;
        virtual void setPremultipliedAlpha(bool v) = 0;
        virtual Vector2U getSize() = 0;
        virtual bool setSize(Vector2U size) = 0;

        virtual bool uploadPixelData(RectU rc, void const* data, uint32_t pitch) = 0;
        // virtual void setPixelData(IData* p_data) = 0;

        virtual bool saveToFile(StringView path) = 0;

        virtual void setSamplerState(SamplerState sampler) = 0;
        virtual optional<SamplerState> getSamplerState() = 0;
    };

    template<class T>
    concept CTexture2D = requires(T a) {
        { a.getNativeHandle() } -> convertible_to<void*>;

        { a.isDynamic() } -> convertible_to<bool>;
        a.setPremultipliedAlpha(a.isPreMulipliedAlpha());
        { a.setSize(a.getSize()) } -> convertible_to<bool>;

        { a.uploadPixelData() } -> convertible_to<bool>;
        
        { a.savePixelData("") } -> convertible_to<bool>;

        a.setSamplerState(declval<SamplerState>());
        { a.getSamplerState() } -> same_as<optional<SamplerState>>;
    };
}
