#pragma once
#include "Core/Object.hpp"
#include "Core/Audio/Decoder.hpp"
#include "miniaudio.h"

namespace Core::Audio
{
    class Decoder_ma : public Object<IDecoder>
    {
    private:
        std::vector<uint8_t> m_data;
        ma_decoder m_decoder;
        bool m_init;

    private:
        void destroyResources();

    public:
        ma_decoder* getRaw() { return &m_decoder; }

    public:
        uint16_t getSampleSize() { return 4; }
        uint16_t getChannelCount() { return 2; }
        uint16_t getFrameSize() { return getChannelCount() * getSampleSize(); }
        uint32_t getSampleRate() { return 48000; }
        uint32_t getByteRate() { return getSampleRate() * (uint32_t)getFrameSize(); }
        uint32_t getFrameCount();
        
        bool seek(uint64_t pcm_frame);
        bool seekByTime(double sec);
        bool tell(uint64_t* pcm_frame);
        bool tellAsTime(double* sec);
        bool read(uint64_t pcm_frame, void* buffer, uint64_t* read_pcm_frame);

    public:
        Decoder_ma(StringView path);
        ~Decoder_ma();
    };
}