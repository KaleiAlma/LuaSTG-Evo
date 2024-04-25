#include "Core/Audio/Decoder_ma.hpp"
#include "Core/FileManager.hpp"
#include <cstdint>

namespace Core::Audio
{
    void Decoder_ma::destroyResources()
    {
        if (m_init)
        {
            m_init = false;
            ma_decoder_uninit(&m_decoder);
        }
        m_data.clear();
    }

    uint32_t Decoder_ma::getFrameCount()
    {
        ma_uint64 len;
        ma_decoder_get_length_in_pcm_frames(&m_decoder, &len);
        return len;
    }
    
    bool Decoder_ma::seek(uint64_t pcm_frame)
    {
        return MA_SUCCESS == ma_decoder_seek_to_pcm_frame(&m_decoder, pcm_frame);
    }
    
    bool Decoder_ma::seekByTime(double sec)
    {
        ma_uint64 cur;
        ma_result r = ma_decoder_get_cursor_in_pcm_frames(&m_decoder, &cur);
        if (r != MA_SUCCESS) return false;
        uint64_t pcm_frame = cur + sec * getSampleRate();
        return MA_SUCCESS == ma_decoder_seek_to_pcm_frame(&m_decoder, pcm_frame);
    }
    
    bool Decoder_ma::tell(uint64_t* pcm_frame)
    {
        return MA_SUCCESS == ma_decoder_get_cursor_in_pcm_frames(&m_decoder, (ma_uint64*)pcm_frame);
    }
    
    bool Decoder_ma::tellAsTime(double* sec)
    {
        ma_uint64 cur;
        ma_result r = ma_decoder_get_cursor_in_pcm_frames(&m_decoder, &cur);
        if (r != MA_SUCCESS) return false;
        *sec = (double) cur / (double) getSampleRate();
        return MA_SUCCESS == r;
    }

    bool Decoder_ma::read(uint64_t pcm_frame, void* buffer, uint64_t* read_pcm_frame)
    {
        return MA_SUCCESS == ma_decoder_read_pcm_frames(&m_decoder, buffer, pcm_frame, (ma_uint64*)read_pcm_frame);
    }

    Decoder_ma::Decoder_ma(StringView path)
        : m_init(false)
    {
        if (!GFileManager().loadEx(path, m_data))
        {
            destroyResources();
            throw std::runtime_error("Decoder_ma::Decoder_ma (1)");
        }

        ma_result r;

        ma_decoder_config cfg = ma_decoder_config_init(ma_format_f32, 2, 48000);
        r = ma_decoder_init_memory(m_data.data(), m_data.size(), &cfg, &m_decoder);

        if (r != MA_SUCCESS)
        {
            destroyResources();
            throw std::runtime_error("Decoder_ma::Decoder_ma (2)");
        }
        m_init = true; // Mark it as needing cleanup

        if (!seek(0))
        {
            destroyResources();
            throw std::runtime_error("Decoder_ma::Decoder_ma (3)");
        }
    }
    Decoder_ma::~Decoder_ma()
    {
        destroyResources();
    }
}