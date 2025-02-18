#pragma once
#include "Core/Audio/Decoder.hpp"
#include "Core/Object.hpp"
#include "Core/Audio/Device.hpp"
#include "SDL.h"
#include "miniaudio.h"
#include <cstdint>
#include <vector>

namespace Core::Audio
{
    struct IAudioDeviceEventListener
    {
        virtual void onAudioDeviceCreate() = 0;
        virtual void onAudioDeviceDestroy() = 0;
    };

    class Shared_SDL : public Object<IObject>
    {
    public:
        ma_engine engine;
        ma_sound_group grp_sfx;
        ma_sound_group grp_bgm;
    public:
        Shared_SDL();
        ~Shared_SDL();
    };

    class Device_SDL : public Object<IAudioDevice>
    {
    private:
        std::unordered_set<IAudioDeviceEventListener*> m_listener;
        bool m_dispatch_event{};
        void dispatchEventAudioDeviceCreate();
        void dispatchEventAudioDeviceDestroy();
    public:
        void addEventListener(IAudioDeviceEventListener* p_m_listener);
        void removeEventListener(IAudioDeviceEventListener* p_m_listener);

    private:
        std::vector<std::string> m_audio_device_list;
        std::string m_target_audio_device_name;
        std::string m_current_audio_device_name;
        bool refreshAudioDeviceList();
    public:
        uint32_t getAudioDeviceCount(bool refresh);
        std::string_view getAudioDeviceName(uint32_t index) const noexcept;
        bool setTargetAudioDevice(std::string_view const audio_device_name);
        std::string_view getCurrentAudioDeviceName() const noexcept { return m_current_audio_device_name; }

    private:
        ScopeObject<Shared_SDL> m_shared;
        SDL_AudioDeviceID m_dev = 0;
        float m_volume_direct = 1.0f;
        float m_volume_sound_effect = 1.0f;
        float m_volume_music = 1.0f;

    public:
        Shared_SDL* getShared() { return m_shared.get(); }
        bool createResources();
        void destroyResources();

    public:
        void setVolume(float v);
        float getVolume();
        void setMixChannelVolume(MixChannel ch, float v);
        float getMixChannelVolume(MixChannel ch);

        bool createAudioPlayer(IDecoder* p_decoder, IAudioPlayer** pp_player);
        bool createLoopAudioPlayer(IDecoder* p_decoder, IAudioPlayer** pp_player);
        bool createStreamAudioPlayer(IDecoder* p_decoder, IAudioPlayer** pp_player);

    public:
        Device_SDL();
        ~Device_SDL();

    public:
        static bool create(Device_SDL** pp_audio);
    };

    class AudioPlayer_SDL
        : public Object<IAudioPlayer>
        , public IAudioDeviceEventListener
    {
    private:
        ScopeObject<Device_SDL> m_device;
        ScopeObject<Shared_SDL> m_shared;
        ScopeObject<IDecoder> m_decoder;
        ma_sound m_sound;
        std::vector<int8_t> m_pcm_data;
        float m_volume = 1.0f;
        float m_output_balance = 0.0f;
        float m_speed = 1.0f;
        bool m_is_playing{};
    public:
        void onAudioDeviceCreate();
        void onAudioDeviceDestroy();
    private:
        bool createResources();
        void destoryResources();

    public:
        bool start();
        bool stop();
        bool reset();

        bool isPlaying();

        double getTotalTime();
        double getTime();
        bool setTime(double time);
        bool getLoop();
        void getLoopRange(double& start_pos, double& length);
        bool setLoop(bool enable);
        bool setLoopRange(double start_pos, double length);

        float getVolume();
        bool setVolume(float v);
        float getBalance();
        bool setBalance(float v);
        float getSpeed();
        bool setSpeed(float v);

        void updateFFT();
        uint32_t getFFTSize();
        float* getFFT();

        IDecoder* getDecoder();

    public:
        AudioPlayer_SDL(Device_SDL* p_device, IDecoder* p_decoder);
        ~AudioPlayer_SDL();
    };

    class LoopAudioPlayer_SDL
        : public Object<IAudioPlayer>
        , public IAudioDeviceEventListener
    {
    private:
        ScopeObject<Device_SDL> m_device;
        ScopeObject<Shared_SDL> m_shared;
        ScopeObject<IDecoder> m_decoder;
        ma_sound m_sound;
        std::vector<int8_t> m_pcm_data;
        float m_volume = 1.0f;
        float m_output_balance = 0.0f;
        float m_speed = 1.0f;
        bool m_is_playing{};
        double m_start_time{};
        bool m_is_loop{};
        double m_loop_start{};
        double m_loop_length{};
        uint32_t m_total_frame{};
        uint32_t m_sample_rate{};
        uint16_t m_frame_size{};
    public:
        void onAudioDeviceCreate();
        void onAudioDeviceDestroy();
    private:
        bool createResources();
        void destoryResources();

    public:
        bool start();
        bool stop();
        bool reset();

        bool isPlaying();

        double getTotalTime();
        double getTime();
        bool setTime(double t);
        bool getLoop();
        void getLoopRange(double& start_pos, double& length);
        bool setLoop(bool enable);
        bool setLoopRange(double start_pos, double length);

        float getVolume();
        bool setVolume(float v);
        float getBalance();
        bool setBalance(float v);
        float getSpeed();
        bool setSpeed(float v);

        void updateFFT();
        uint32_t getFFTSize();
        float* getFFT();

        IDecoder* getDecoder();

    public:
        LoopAudioPlayer_SDL(Device_SDL* p_device, IDecoder* p_decoder);
        ~LoopAudioPlayer_SDL();
    };

    class StreamAudioPlayer_SDL
        : public Object<IAudioPlayer>
        , public IAudioDeviceEventListener
    {
    private:
        using Duration = std::chrono::duration<double>;
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;
    private:
        struct AudioPeekNode
        {
            ma_node_base base;
            std::vector<float> raw_buffer;
            uint16_t frame_offset = 0;
            uint16_t frame_offset_edge = 0;
            int16_t frame_offset_old = -1;
            int16_t frame_offset_old_old = -1;

            static void processPcmFrames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut);
            static constexpr ma_node_vtable vtable =
            {
                processPcmFrames,
                NULL,
                1,
                1,
                MA_NODE_FLAG_PASSTHROUGH
            };
        };
        ma_result initAudioPeekNode(AudioPeekNode* node);
    private:
        ScopeObject<Device_SDL> m_device;
        ScopeObject<Shared_SDL> m_shared;
        ScopeObject<IDecoder> m_decoder;
        ma_sound m_sound;
        AudioPeekNode m_node;
        bool m_fft_enable = true;
        float m_volume = 1.0f;
        float m_output_balance = 0.0f;
        float m_speed = 1.0f;
        bool m_is_playing{};
        double m_start_time{};
        bool m_is_loop{};
        double m_loop_start{};
        double m_loop_length{};
        uint32_t m_total_frame{};
        uint32_t m_sample_rate{};
        uint16_t m_frame_size{};
        double start_time = 0.0;
        double total_time = 0.0;
        double current_time = 0.0;
        TimePoint last_fft;
        uint16_t fft_offset;
        std::vector<float> fft_wave_data;
        std::vector<float> fft_window;
        std::vector<float> fft_data;
        std::vector<float> fft_complex_output;
        std::vector<float> fft_output;
    public:
        void onAudioDeviceCreate();
        void onAudioDeviceDestroy();
    private:
        bool createResources();
        void destoryResources();

    public:

        bool start();
        bool stop();
        bool reset();

        bool isPlaying();

        double getTotalTime();
        double getTime();
        bool setTime(double t);
        bool getLoop();
        void getLoopRange(double& start_pos, double& length);
        bool setLoop(bool enable);
        bool setLoopRange(double start_pos, double length);

        float getVolume();
        bool setVolume(float v);
        float getBalance();
        bool setBalance(float v);
        float getSpeed();
        bool setSpeed(float v);

        void updateFFT();
        uint32_t getFFTSize();
        float* getFFT();

        IDecoder* getDecoder();

    public:
        StreamAudioPlayer_SDL(Device_SDL* p_device, IDecoder* p_decoder);
        ~StreamAudioPlayer_SDL();
    };
}
