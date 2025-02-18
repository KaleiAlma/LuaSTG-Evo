#include "GameResource/Implement/ResourceMusicImpl.hpp"

namespace LuaSTGPlus
{
    void ResourceMusicImpl::Play(float vol, double position)
    {
        m_player->reset();

        m_player->setVolume(vol);
        m_player->setTime(position);

        m_player->start();
        m_status = 2;
    }
    void ResourceMusicImpl::Stop()
    {
        m_player->reset();
        m_status = 0;
    }
    void ResourceMusicImpl::Pause()
    {
        m_player->stop();
        m_status = 1;
    }
    void ResourceMusicImpl::Resume()
    {
        m_player->start();
        m_status = 2;
    }
    bool ResourceMusicImpl::IsPlaying() { return m_player->isPlaying(); }
    bool ResourceMusicImpl::IsPaused() { return m_status == 1; }
    bool ResourceMusicImpl::IsStopped() { return !IsPlaying() && m_player->getTotalTime() == 0.0; }
    void ResourceMusicImpl::SetVolume(float v) { m_player->setVolume(v); }
    float ResourceMusicImpl::GetVolume() { return m_player->getVolume(); }
    bool ResourceMusicImpl::SetSpeed(float speed) { return m_player->setSpeed(speed); }
    float ResourceMusicImpl::GetSpeed() { return m_player->getSpeed(); }
    void ResourceMusicImpl::SetLoop(bool v) { m_player->setLoop(v); }
    bool ResourceMusicImpl::SetLoopRange(double a, double b) { return m_player->setLoopRange(a, b); }
    bool ResourceMusicImpl::GetLoop() { return m_player->getLoop(); }
    void ResourceMusicImpl::GetLoopRange(double& a, double& b) { m_player->getLoopRange(a, b); }
    bool ResourceMusicImpl::SetTime(double position) { return m_player->setTime(position); }
    double ResourceMusicImpl::GetTime() { return m_player->getTime(); }
    double ResourceMusicImpl::GetTotalTime() { return m_player->getTotalTime(); }

    ResourceMusicImpl::ResourceMusicImpl(const char* name, Core::Audio::IAudioPlayer* p_player, double start, double end)
        : ResourceBaseImpl(ResourceType::Music, name)
        , m_player(p_player)
        , m_start(start)
        , m_end(end)
    {
    }
}
