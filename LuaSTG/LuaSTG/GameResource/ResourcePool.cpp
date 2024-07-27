#include "Core/Graphics/Sprite_OpenGL.hpp"
#include "GameResource/ResourceManager.h"
#include "GameResource/Implement/ResourceTextureImpl.hpp"
#include "GameResource/Implement/ResourceSpriteImpl.hpp"
#include "GameResource/Implement/ResourceAnimationImpl.hpp"
#include "GameResource/Implement/ResourceMusicImpl.hpp"
#include "GameResource/Implement/ResourceSoundEffectImpl.hpp"
#include "GameResource/Implement/ResourceParticleImpl.hpp"
#include "GameResource/Implement/ResourceFontImpl.hpp"
#include "GameResource/Implement/ResourcePostEffectShaderImpl.hpp"
#include "GameResource/Implement/ResourceModelImpl.hpp"
#include "Core/FileManager.hpp"
#include "AppFrame.h"
#include "LuaBinding/lua_utility.hpp"
#include <spdlog/spdlog.h>

namespace LuaSTGPlus
{
    // Overall management

    void ResourcePool::Clear() noexcept
    {
        m_TexturePool.clear();
        m_SpritePool.clear();
        m_AnimationPool.clear();
        m_MusicPool.clear();
        m_SoundSpritePool.clear();
        m_ParticlePool.clear();
        m_SpriteFontPool.clear();
        m_TTFFontPool.clear();
        m_FXPool.clear();
        m_ModelPool.clear();
        spdlog::info("[luastg] '{}' pools cleared", getResourcePoolTypeName());
    }

    template<typename T>
    inline void removeResource(T& pool, const char* name)
    {
        auto i = pool.find(std::string_view(name));
        if (i == pool.end())
        {
            spdlog::warn("[luastg] RemoveResource: Attempted to remove non-existent resource '{}'", name);
            return;
        }
        pool.erase(i);
        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] RemoveResource: Resource '{}' removed", name);
        }
    }

    const char* ResourcePool::getResourcePoolTypeName()
    {
        switch (m_iType) {
            case ResourcePoolType::Global:
                return "global";
            case ResourcePoolType::Stage:
                return "stage";
            default:
                return "none";
        }
    }

    void ResourcePool::RemoveResource(ResourceType t, const char* name) noexcept
    {
        switch (t)
        {
        case ResourceType::Texture:
            removeResource(m_TexturePool, name);
            break;
        case ResourceType::Sprite:
            removeResource(m_SpritePool, name);
            break;
        case ResourceType::Animation:
            removeResource(m_AnimationPool, name);
            break;
        case ResourceType::Music:
            removeResource(m_MusicPool, name);
            break;
        case ResourceType::SoundEffect:
            removeResource(m_SoundSpritePool, name);
            break;
        case ResourceType::Particle:
            removeResource(m_ParticlePool, name);
            break;
        case ResourceType::SpriteFont:
            removeResource(m_SpriteFontPool, name);
            break;
        case ResourceType::TrueTypeFont:
            removeResource(m_TTFFontPool, name);
            break;
        case ResourceType::FX:
            removeResource(m_FXPool, name);
            break;
        case ResourceType::Model:
            removeResource(m_ModelPool, name);
            break;
        default:
            spdlog::warn("[luastg] RemoveResource: Attempted to remove invalid resource type ({})", (int)t);
            return;
        }
    }

    bool ResourcePool::CheckResourceExists(ResourceType t, std::string_view name) const noexcept
    {
        switch (t)
        {
        case ResourceType::Texture:
            return m_TexturePool.find(name) != m_TexturePool.end();
        case ResourceType::Sprite:
            return m_SpritePool.find(name) != m_SpritePool.end();
        case ResourceType::Animation:
            return m_AnimationPool.find(name) != m_AnimationPool.end();
        case ResourceType::Music:
            return m_MusicPool.find(name) != m_MusicPool.end();
        case ResourceType::SoundEffect:
            return m_SoundSpritePool.find(name) != m_SoundSpritePool.end();
        case ResourceType::Particle:
            return m_ParticlePool.find(name) != m_ParticlePool.end();
        case ResourceType::SpriteFont:
            return m_SpriteFontPool.find(name) != m_SpriteFontPool.end();
        case ResourceType::TrueTypeFont:
            return m_TTFFontPool.find(name) != m_TTFFontPool.end();
        case ResourceType::FX:
            return m_FXPool.find(name) != m_FXPool.end();
        case ResourceType::Model:
            return m_ModelPool.find(name) != m_ModelPool.end();
        default:
            spdlog::warn("[luastg] CheckRes: Attempted to check invalid resource type ({})", (int)t);
            break;
        }
        return false;
    }

    template<typename T>
    inline void listResourceName(lua_State* L, T& resource_set)
    {
        lua::stack_t S(L);
        int index = 0;
        S.create_array(resource_set.size());
        for (auto& i : resource_set)
        {
            auto ptr = i.second;
            index += 1;
            S.set_array_value<std::string_view>(index, ptr->GetResName());
        }
    }

    int ResourcePool::ExportResourceList(lua_State* L, ResourceType t) const noexcept
    {
        lua::stack_t S(L);
        switch (t)
        {
        case ResourceType::Texture:
            listResourceName(L, m_TexturePool);
            break;
        case ResourceType::Sprite:
            listResourceName(L, m_SpritePool);
            break;
        case ResourceType::Animation:
            listResourceName(L, m_AnimationPool);
            break;
        case ResourceType::Music:
            listResourceName(L, m_MusicPool);
            break;
        case ResourceType::SoundEffect:
            listResourceName(L, m_SoundSpritePool);
            break;
        case ResourceType::Particle:
            listResourceName(L, m_ParticlePool);
            break;
        case ResourceType::SpriteFont:
            listResourceName(L, m_SpriteFontPool);
            break;
        case ResourceType::TrueTypeFont:
            listResourceName(L, m_TTFFontPool);
            break;
        case ResourceType::FX:
            listResourceName(L, m_FXPool);
            break;
        case ResourceType::Model:
            listResourceName(L, m_ModelPool);
            break;
        default:
            spdlog::warn("[luastg] EnumRes: Attempted to enumerate invalid resource type ({})", (int)t);
            S.create_array(0);
            break;
        }
        return 1;
    }

    // 加载纹理

    bool ResourcePool::LoadTexture(const char* name, const char* path, bool mipmaps) noexcept
    {
        if (m_TexturePool.find(std::string_view(name)) != m_TexturePool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] LoadTexture: Texture '{}' already exists, loading cancelled.", name);
            }
            return true;
        }
    
        Core::ScopeObject<Core::Graphics::ITexture2D> p_texture;
        // spdlog::debug("tex_ptr: {}", (size_t)&p_texture); // 140737488345752 140737488345752
        if (!LAPP.GetAppModel()->getDevice()->createTextureFromFile(path, mipmaps, ~p_texture))
        {
            spdlog::error("[luastg] Failed to create texture '{}' from '{}'", name, path);
            return false;
        }

        try
        {
            Core::ScopeObject<IResourceTexture> tRes;
            tRes.attach(new ResourceTextureImpl(name, p_texture.get()));
            m_TexturePool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] LoadTexture: Failed to load texture '{}' ({})", name, e.what());
            return false;
        }
    
        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadTexture: path '{}', name '{}' ({})", path, name, getResourcePoolTypeName());
        }
    
        return true;
    }

    bool ResourcePool::CreateTexture(const char* name, int width, int height) noexcept
    {
        if (m_TexturePool.find(std::string_view(name)) != m_TexturePool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] CreateTexture: Texture '{}' already exists, loading cancelled", name);
            }
            return true;
        }

        Core::ScopeObject<Core::Graphics::ITexture2D> p_texture;
        // spdlog::debug("tex_ptr: {}", (size_t)&p_texture); // 
        if (!LAPP.GetAppModel()->getDevice()->createTexture(Core::Vector2U((uint32_t)width, (uint32_t)height), ~p_texture))
        {
            spdlog::error("[luastg] Failed to create texture '{}' ({}x{})", name, width, height);
            return false;
        }

        try
        {
            Core::ScopeObject<IResourceTexture> tRes;
            tRes.attach(new ResourceTextureImpl(name, p_texture.get()));
            m_TexturePool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] CreateTexture: Failed to create texture ({})", e.what());
            return false;
        }

        if (ResourceMgr::GetResourceLoadingLog()) {
            spdlog::info("[luastg] CreateTexture: Created texture '{}' ({}x{}) ({})", name, width, height, getResourcePoolTypeName());
        }

        return true;
    }

    // Creating render targets

    bool ResourcePool::CreateRenderTarget(const char* name, int width, int height, bool depth_buffer) noexcept
    {
        if (m_TexturePool.find(std::string_view(name)) != m_TexturePool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] CreateRenderTarget: Render target '{}' already exists, aborting", name);
            }
            return true;
        }
    
        std::string_view ds_info(" with depth buffer");

        try
        {
            Core::ScopeObject<IResourceTexture> tRes;
            if (width <= 0 || height <= 0)
            {
                tRes.attach(new ResourceTextureImpl(name));
            }
            else
            {
                tRes.attach(new ResourceTextureImpl(name, width, height));
            }
            m_TexturePool.emplace(name, tRes);
        }
        catch (std::runtime_error const& e)
        {
            spdlog::error("[luastg] CreateRenderTarget: Failed to create render target '{}' ({})", name, e.what());
            return false;
        }
    
        if (ResourceMgr::GetResourceLoadingLog())
        {
            if (width <= 0 || height <= 0)
            {
                spdlog::info("[luastg] CreateRenderTarget: Render target created{} '{}' ({})", ds_info, name, getResourcePoolTypeName());
            }
            else
            {
                spdlog::info("[luastg] CreateRenderTarget: Render target created{} '{}' ({}x{}) ({})", ds_info, name, width, height, getResourcePoolTypeName());
            }
        }
    
        return true;
    }

    // Create an image sprite

    bool ResourcePool::CreateSprite(const char* name, const char* texname,
                                    double x, double y, double w, double h,
                                    double a, double b, bool rect) noexcept
    {
        if (m_SpritePool.find(std::string_view(name)) != m_SpritePool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] CreateSprite: Image sprite '{}' already exists, aborting", name);
            }
            return true;
        }
    
        Core::ScopeObject<IResourceTexture> pTex = m_pMgr->FindTexture(texname);
        if (!pTex)
        {
            spdlog::error("[luastg] CreateSprite: Unable to create sprite '{}', can't find texture '{}'", name, texname);
            return false;
        }
    
        Core::ScopeObject<Core::Graphics::ISprite> p_sprite;
        if (!Core::Graphics::ISprite::create(
            LAPP.GetAppModel()->getRenderer(),
            pTex->GetTexture(),
            ~p_sprite
        ))
        {
            spdlog::error("[luastg] Failed to create image sprite '{}' from texture '{}'", texname, name);
            return false;
        }
        p_sprite->setTextureRect(Core::RectF((float)x, (float)y, (float)(x + w), (float)(y + h)));
        p_sprite->setTextureCenter(Core::Vector2F((float)(x + w * 0.5), (float)(y + h * 0.5)));

        try
        {
            Core::ScopeObject<IResourceSprite> tRes;
            tRes.attach(new ResourceSpriteImpl(name, p_sprite.get(), a, b, rect));
            m_SpritePool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] CreateSprite: Failed to create sprite '{}' ({})", name, e.what());
            return false;
        }
    
        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] CreateSprite: texture '{}', image name '{}' ({})", texname, name, getResourcePoolTypeName());
        }
    
        return true;
    }

    // Create animated sprite

    bool ResourcePool::CreateAnimation(const char* name, const char* texname,
                                       double x, double y, double w, double h, int n, int m, int intv,
                                       double a, double b, bool rect) noexcept
    {
        if (m_AnimationPool.find(std::string_view(name)) != m_AnimationPool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] CreateAnimation: Animation '{}' already exists, aborting", name);
            }
            return true;
        }

        Core::ScopeObject<IResourceTexture> pTex = m_pMgr->FindTexture(texname);
        if (!pTex)
        {
            spdlog::error("[luastg] CreateAnimation: Unable to create animation '{}', can't find texture '{}'", name, texname);
            return false;
        }

        try {
            Core::ScopeObject<IResourceAnimation> tRes;
            tRes.attach(
                new ResourceAnimationImpl(name, pTex,
                    (float) x, (float) y,
                    (float) w, (float) h,
                    n, m, intv,
                    a, b, rect)
            );
            m_AnimationPool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] CreateAnimation: Failed to create animation '{}' ({})", name, e.what());
            return false;
        }

        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] CreateAnimation: texture '{}', animation name '{}' ({})", texname, name, getResourcePoolTypeName());
        }
    
        return true;
    }

    bool ResourcePool::CreateAnimation(const char* name,
        std::vector<Core::ScopeObject<IResourceSprite>> const& sprite_list,
        int intv,
        double a, double b, bool rect) noexcept
    {
        if (m_AnimationPool.find(std::string_view(name)) != m_AnimationPool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] CreateAnimation: Animation '{}' already exists, aborting", name);
            }
            return true;
        }

        try {
            Core::ScopeObject<IResourceAnimation> tRes;
            tRes.attach(
                new ResourceAnimationImpl(name, sprite_list, intv, a, b, rect)
            );
            m_AnimationPool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] CreateAnimation: Failed to create animation '{}' ({})", name, e.what());
            return false;
        }

        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] CreateAnimation: Created animation '{}' ({})", name, getResourcePoolTypeName());
        }

        return true;
    }

    // Load music

    bool ResourcePool::LoadMusic(const char* name, const char* path, double start, double end, bool once_decode) noexcept
    {
        if (m_MusicPool.find(std::string_view(name)) != m_MusicPool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] LoadMusic: Music '{}' already exists, loading cancelled", name);
            }
            return true;
        }
    
        using namespace Core;
        using namespace Core::Audio;

        // Create decoder
        ScopeObject<IDecoder> p_decoder;
        if (!IDecoder::create(path, ~p_decoder))
        {
            spdlog::error("[luastg] LoadMusic: Cannot decode file '{}', format must be WAV/OGG/MP3/FLAC", path);
            return false;
        }
        auto to_sample = [&p_decoder](double t) -> uint32_t
        {
            return (uint32_t)(t * (double)p_decoder->getSampleRate());
        };

        // Check loop section
        if (0 == to_sample(start) && to_sample(start) == to_sample(end))
        {
            end = (double)p_decoder->getFrameCount() / (double)p_decoder->getSampleRate();
            spdlog::info("[luastg] LoadMusic: Loop range set to entire track (start = {}, end = {})", start, end);
        }
        if (to_sample(start) >= to_sample(end))
        {
            spdlog::error("[luastg] LoadMusic: End position cannot be less than or equal to start position (start = {}, end = {})", start, end);
            return false;
        }
    
        // Configure loop decoder (don't worry about exeptions, they're handled above)
        // ScopeObject<Audio::LoopDecoder> p_loop_decoder;
        // p_loop_decoder.attach(new ResourceMusicImpl::LoopDecoder(p_decoder.get(), start, end));

        // Create a player
        ScopeObject<IAudioPlayer> p_player;
        if (!once_decode)
        {
            // Stream player
            if (!LAPP.GetAppModel()->getAudioDevice()->createStreamAudioPlayer(p_decoder.get(), ~p_player))
            {
                spdlog::error("[luastg] LoadMusic: Unable to create stream audio player");
                return false;
            }
            if (!p_player->setLoop(true, start, end - start))
                spdlog::error("[luastg] StreamAudioPlayer: invalid loop");
        }
        else
        {
            // One-time decoder
            if (!LAPP.GetAppModel()->getAudioDevice()->createLoopAudioPlayer(p_decoder.get(), ~p_player))
            {
                spdlog::error("[luastg] LoadMusic: Unable to create once-decode audio player");
                return false;
            }
            if (!p_player->setLoop(true, start, end - start))
                spdlog::error("[luastg] LoopAudioPlayer: invalid loop");
        }

        try
        {
            // Place into resource pool
            Core::ScopeObject<IResourceMusic> tRes;
            tRes.attach(new ResourceMusicImpl(name, p_player.get(), start, end));
            m_MusicPool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] LoadMusic: Failed to load music '{}' ({})", name, e.what());
            return false;
        }

        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadMusic: path '{}', name '{}'{} ({})", path, name, once_decode ? " (decode immediately)" : "", getResourcePoolTypeName());
        }

        return true;
    }

    // Load sound effects

    bool ResourcePool::LoadSoundEffect(const char* name, const char* path) noexcept
    {
        if (m_SoundSpritePool.find(std::string_view(name)) != m_SoundSpritePool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] LoadSoundEffect: Sound effect '{}' already exists, loading cancelled.", name);
            }
            return true;
        }

        using namespace Core;
        using namespace Core::Audio;

        // Create decoder
        ScopeObject<IDecoder> p_decoder;
        if (!IDecoder::create(path, ~p_decoder))
        {
            spdlog::error("[luastg] LoadSoundEffect: Cannot decode file '{}', format must be WAV/OGG/MP3/FLAC", path);
            return false;
        }

        // Create audio player
        ScopeObject<IAudioPlayer> p_player;
        if (!LAPP.GetAppModel()->getAudioDevice()->createAudioPlayer(p_decoder.get(), ~p_player))
        {
            spdlog::error("[luastg] LoadSoundEffect: Unable to create audiio player");
            return false;
        }

        try
        {
            Core::ScopeObject<IResourceSoundEffect> tRes;
            tRes.attach(new ResourceSoundEffectImpl(name, p_player.get()));
            m_SoundSpritePool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] LoadSoundEffect: Unable to load sound effect '{}' ({})", name, e.what());
            return false;
        }
    
        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadSoundEffect: path '{}', name '{}' ({})", path, name, getResourcePoolTypeName());
        }
    
        return true;
    }

    // Load particle effects (HGE)

    bool ResourcePool::LoadParticle(const char* name, const hgeParticleSystemInfo& info, const char* img_name,
                                    double a,double b, bool rect, bool _nolog) noexcept
    {
        if (m_ParticlePool.find(std::string_view(name)) != m_ParticlePool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] LoadParticle: Particle System '{}' already exists, loading cancelled.", name);
            }
            return true;
        }
    
        Core::ScopeObject<IResourceSprite> pSprite = m_pMgr->FindSprite(img_name);
        if (!pSprite)
        {
            spdlog::error("[luastg] LoadParticle: Unable to load particle system '{}', cannot find image sprite '{}'", name, img_name);
            return false;
        }
    
        Core::ScopeObject<Core::Graphics::ISprite> p_sprite;
        if (!pSprite->GetSprite()->clone(~p_sprite))
        {
            spdlog::error("[luastg] LoadParticle: Unable to load particle system '{}', unable to copy sprite '{}'", name, img_name);
            return false;
        }

        try
        {
            Core::ScopeObject<IResourceParticle> tRes;
            tRes.attach(new ResourceParticleImpl(name, info, p_sprite.get(), a, b, rect));
            m_ParticlePool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] LoadParticle: Unable to load particle system '{}' ({})", name, e.what());
            return false;
        }
    
        if (!_nolog && ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadParticle: name '{}' ({})", name, getResourcePoolTypeName());
        }
    
        return true;
    }

    bool ResourcePool::LoadParticle(const char* name, const char* path, const char* img_name,
                                    double a, double b,bool rect) noexcept
    {
        std::vector<uint8_t> src;
        if (!GFileManager().loadEx(path, src))
        {
            spdlog::error("[luastg] LoadParticle: Unable to load particle system '{}' from '{}', failed to read file", path, name);
            return false;
        }
    
        if (src.size() != sizeof(hgeParticleSystemInfo))
        {
            spdlog::error("[luastg] LoadParticle: Particle effect definition file '{}' is ill-formed", path);
            return false;
        }
        hgeParticleSystemInfo tInfo;
        std::memcpy(&tInfo, src.data(), sizeof(hgeParticleSystemInfo));
    
        if (!LoadParticle(name, tInfo, img_name, a, b, rect, /* _nolog */ true))
        {
            return false;
        }
    
        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadParticle: path '{}', name '{}' ({})", path, name, getResourcePoolTypeName());
        }
    
        return true;
    }

    // Load texture fonts（HGE）

    bool ResourcePool::LoadSpriteFont(const char* name, const char* path, bool mipmaps) noexcept
    {
        if (m_SpriteFontPool.find(std::string_view(name)) != m_SpriteFontPool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] LoadSpriteFont: Texture font '{}' already exists, loading cancelled.", name);
            }
            return true;
        }

        // Create definitions
        try
        {
            Core::ScopeObject<IResourceFont> tRes;
            tRes.attach(new ResourceFontImpl(name, path, mipmaps));
            m_SpriteFontPool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] LoadSpriteFont: Unable to load HGE texture font '{}' ({})", name, e.what());
            return false;
        }

        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadSpriteFont (HGE): path '{}', name '{}' ({})", path, name, getResourcePoolTypeName());
        }
    
        return true;
    }

    // Load texture fonts (fancy2d)

    bool ResourcePool::LoadSpriteFont(const char* name, const char* path, const char* tex_path, bool mipmaps) noexcept
    {
        if (m_SpriteFontPool.find(std::string_view(name)) != m_SpriteFontPool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] LoadSpriteFont: Texture font '{}' already exists, loading cancelled.", name);
            }
            return true;
        }
    
        // Create definitions
        try
        {
            Core::ScopeObject<IResourceFont> tRes;
            tRes.attach(new ResourceFontImpl(name, path, tex_path, mipmaps));
            m_SpriteFontPool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] LoadSpriteFont: Unable to load fancy2d texture font '{}' ({})", name, e.what());
            return false;
        }
    
        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadSpriteFont (fancy2d): path '{}' & '{}', name '{}' ({})", path, tex_path, name, getResourcePoolTypeName());
        }
    
        return true;
    }

    // Load TTFs

    bool ResourcePool::LoadTTFFont(const char* name, const char* path, float width, float height) noexcept
    {
        if (m_TTFFontPool.find(std::string_view(name)) != m_TTFFontPool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] LoadTTFFont: TTF '{}' already exists, loading cancelled", name);
            }
            return true;
        }
    
        Core::ScopeObject<Core::Graphics::IGlyphManager> p_glyphmgr;
        Core::Graphics::TrueTypeFontInfo create_info = {
            .source = path,
            .font_face = 0,
            .font_size = Core::Vector2F(width, height),
            .is_force_to_file = false,
            .is_buffer = false,
        };
        if (!Core::Graphics::IGlyphManager::create(LAPP.GetAppModel()->getDevice(), &create_info, 1, ~p_glyphmgr))
        {
            spdlog::error("[luastg] LoadTTFFont: Loading TTF '{}' failed", name);
            return false;
        }

        // Create definitions
        try
        {
            Core::ScopeObject<IResourceFont> tRes;
            tRes.attach(new ResourceFontImpl(name, p_glyphmgr.get()));
            m_TTFFontPool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] LoadTTFFont: Unable to load TTF '{}' ({})", name, e.what());
            return false;
        }
    
        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadTTFFont: path '{}', name '{}' ({})", path, name, getResourcePoolTypeName());
        }
    
        return true;
    }

    bool ResourcePool::LoadTrueTypeFont(const char* name, Core::Graphics::TrueTypeFontInfo* fonts, size_t count) noexcept
    {
        if (m_TTFFontPool.find(std::string_view(name)) != m_TTFFontPool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] LoadTrueTypeFont: TTF '{}' already exists, loading cancelled", name);
            }
            return true;
        }
    
        Core::ScopeObject<Core::Graphics::IGlyphManager> p_glyphmgr;
        if (!Core::Graphics::IGlyphManager::create(LAPP.GetAppModel()->getDevice(), fonts, count, ~p_glyphmgr))
        {
            spdlog::error("[luastg] LoadTrueTypeFont: Loading TTF '{}' failed", name);
            return false;
        }

        // Create definitions
        try
        {
            Core::ScopeObject<IResourceFont> tRes;
            tRes.attach(new ResourceFontImpl(name, p_glyphmgr.get()));
            m_TTFFontPool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] LoadTrueTypeFont: Unable to load TTF '{}' ({})", name, e.what());
            return false;
        }
    
        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadTrueTypeFont: Loaded TTF '{}' ({})", name, getResourcePoolTypeName());
        }
    
        return true;
    }

    // Load PostEffects

    bool ResourcePool::LoadFX(const char* name, const char* path) noexcept
    {
        if (m_FXPool.find(std::string_view(name)) != m_FXPool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] LoadFX: FX '{}' already exists, loading cancelled.", name);
            }
            return true;
        }
    
        try
        {
            Core::ScopeObject<IResourcePostEffectShader> tRes;
            tRes.attach(new ResourcePostEffectShaderImpl(name, path));
            if (!tRes->GetPostEffectShader())
            {
                spdlog::error("[luastg] LoadFX: Failed to load FX '{}' from '{}'", name, path);
                return false;
            }
            m_FXPool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] LoadFX: Failed to load FX '{}' ({})", name, e.what());
            return false;
        }

        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadFX: path '{}', name '{}' ({})", path, name, getResourcePoolTypeName());
        }
    
        return true;
    }

    // Load Models

    bool ResourcePool::LoadModel(const char* name, const char* path) noexcept
    {
        if (m_ModelPool.find(std::string_view(name)) != m_ModelPool.end())
        {
            if (ResourceMgr::GetResourceLoadingLog())
            {
                spdlog::warn("[luastg] LoadModel: Model '{}' already exists, loading cancelled.", name);
            }
            return true;
        }
    
        try
        {
            Core::ScopeObject<IResourceModel> tRes;
            tRes.attach(new ResourceModelImpl(name, path));
            m_ModelPool.emplace(name, tRes);
        }
        catch (std::exception const& e)
        {
            spdlog::error("[luastg] LoadModel: Unable to load model '{}' ({})", name, e.what());
            return false;
        }
    
        if (ResourceMgr::GetResourceLoadingLog())
        {
            spdlog::info("[luastg] LoadModel: path '{}', name '{}' ({})", path, name, getResourcePoolTypeName());
        }
    
        return true;
    }

    // Find and retrieve

    template<typename T>
    inline T::value_type::second_type findResource(T& resource_set, std::string_view name)
    {
        auto i = resource_set.find(name);
        if (i == resource_set.end())
            return nullptr;
        else
            return i->second;
    }

    Core::ScopeObject<IResourceTexture> ResourcePool::GetTexture(std::string_view name) noexcept
    {
        return findResource(m_TexturePool, name);
    }

    Core::ScopeObject<IResourceSprite> ResourcePool::GetSprite(std::string_view name) noexcept
    {
        return findResource(m_SpritePool, name);
    }

    Core::ScopeObject<IResourceAnimation> ResourcePool::GetAnimation(std::string_view name) noexcept
    {
        return findResource(m_AnimationPool, name);
    }

    Core::ScopeObject<IResourceMusic> ResourcePool::GetMusic(std::string_view name) noexcept
    {
        return findResource(m_MusicPool, name);
    }

    Core::ScopeObject<IResourceSoundEffect> ResourcePool::GetSound(std::string_view name) noexcept
    {
        return findResource(m_SoundSpritePool, name);
    }

    Core::ScopeObject<IResourceParticle> ResourcePool::GetParticle(std::string_view name) noexcept
    {
        return findResource(m_ParticlePool, name);
    }

    Core::ScopeObject<IResourceFont> ResourcePool::GetSpriteFont(std::string_view name) noexcept
    {
        return findResource(m_SpriteFontPool, name);
    }

    Core::ScopeObject<IResourceFont> ResourcePool::GetTTFFont(std::string_view name) noexcept
    {
        return findResource(m_TTFFontPool, name);
    }

    Core::ScopeObject<IResourcePostEffectShader> ResourcePool::GetFX(std::string_view name) noexcept
    {
        return findResource(m_FXPool, name);
    }

    Core::ScopeObject<IResourceModel> ResourcePool::GetModel(std::string_view name) noexcept
    {
        return findResource(m_ModelPool, name);
    }

    ResourcePool::ResourcePool(ResourceMgr* mgr, ResourcePoolType t)
        : m_pMgr(mgr)
        , m_iType(t)
        , m_TexturePool(&m_memory_resource)
        , m_SpritePool(&m_memory_resource)
        , m_AnimationPool(&m_memory_resource)
        , m_MusicPool(&m_memory_resource)
        , m_SoundSpritePool(&m_memory_resource)
        , m_ParticlePool(&m_memory_resource)
        , m_SpriteFontPool(&m_memory_resource)
        , m_TTFFontPool(&m_memory_resource)
        , m_FXPool(&m_memory_resource)
        , m_ModelPool(&m_memory_resource)
    {

    }
}
