#include "Core/Audio/Decoder_ma.hpp"
#include "Core/FileManager.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>

static ma_result ma_decoding_backend_init__libvorbis(void* pUserData, ma_read_proc onRead, ma_seek_proc onSeek, ma_tell_proc onTell, void* pReadSeekTellUserData, const ma_decoding_backend_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_data_source** ppBackend)
{
    ma_result result;
    ma_libvorbis* pVorbis;

    (void)pUserData;

    pVorbis = (ma_libvorbis*)ma_malloc(sizeof(*pVorbis), pAllocationCallbacks);
    if (pVorbis == NULL) {
        return MA_OUT_OF_MEMORY;
    }

    result = ma_libvorbis_init(onRead, onSeek, onTell, pReadSeekTellUserData, pConfig, pAllocationCallbacks, pVorbis);
    if (result != MA_SUCCESS) {
        ma_free(pVorbis, pAllocationCallbacks);
        return result;
    }

    *ppBackend = pVorbis;

    return MA_SUCCESS;
}

static ma_result ma_decoding_backend_init_file__libvorbis(void* pUserData, const char* pFilePath, const ma_decoding_backend_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_data_source** ppBackend)
{
    ma_result result;
    ma_libvorbis* pVorbis;

    (void)pUserData;

    pVorbis = (ma_libvorbis*)ma_malloc(sizeof(*pVorbis), pAllocationCallbacks);
    if (pVorbis == NULL) {
        return MA_OUT_OF_MEMORY;
    }

    result = ma_libvorbis_init_file(pFilePath, pConfig, pAllocationCallbacks, pVorbis);
    if (result != MA_SUCCESS) {
        ma_free(pVorbis, pAllocationCallbacks);
        return result;
    }

    *ppBackend = pVorbis;

    return MA_SUCCESS;
}

static void ma_decoding_backend_uninit__libvorbis(void* pUserData, ma_data_source* pBackend, const ma_allocation_callbacks* pAllocationCallbacks)
{
    ma_libvorbis* pVorbis = (ma_libvorbis*)pBackend;

    (void)pUserData;

    ma_libvorbis_uninit(pVorbis, pAllocationCallbacks);
    ma_free(pVorbis, pAllocationCallbacks);
}

static ma_result ma_decoding_backend_get_channel_map__libvorbis(void* pUserData, ma_data_source* pBackend, ma_channel* pChannelMap, size_t channelMapCap)
{
    ma_libvorbis* pVorbis = (ma_libvorbis*)pBackend;

    (void)pUserData;

    return ma_libvorbis_get_data_format(pVorbis, NULL, NULL, NULL, pChannelMap, channelMapCap);
}

static ma_decoding_backend_vtable g_ma_decoding_backend_vtable_libvorbis =
{
    ma_decoding_backend_init__libvorbis,
    ma_decoding_backend_init_file__libvorbis,
    NULL, /* onInitFileW() */
    NULL, /* onInitMemory() */
    ma_decoding_backend_uninit__libvorbis
};

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

        ma_decoding_backend_vtable* pCustomBackendVTables[] =
        {
            &g_ma_decoding_backend_vtable_libvorbis,
        };
        cfg.pCustomBackendUserData = NULL;
        cfg.ppCustomBackendVTables = pCustomBackendVTables;
        cfg.customBackendCount = sizeof(pCustomBackendVTables) / sizeof(pCustomBackendVTables[0]);
        r = ma_decoder_init_memory(m_data.data(), m_data.size(), &cfg, &m_decoder);

        if (r != MA_SUCCESS)
        {
            destroyResources();
            spdlog::error("r = {}", r);
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