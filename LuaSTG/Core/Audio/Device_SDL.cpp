#include "Core/Audio/Device_SDL.hpp"
#include "Core/Audio/Decoder.hpp"
#include "Core/Audio/Decoder_ma.hpp"
#include "Core/Audio/Device.hpp"
#include "SDL_audio.h"
#include "SDL_error.h"
#include "spdlog/spdlog.h"
// #include <cstddef>
// #include <cstdint>
#include <string>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "minivorbis.h"


static std::array<float, 1> s_empty_fft_data{};

namespace Core::Audio
{
	void data_callback(void* p_userdata, uint8_t* p_buffer, int bufferSizeInBytes)
	{
		Shared_SDL* p_shared = reinterpret_cast<Shared_SDL*>(p_userdata);
		ma_engine engine = p_shared->engine;
		uint32_t bpf = ma_get_bytes_per_frame(ma_format_f32, ma_engine_get_channels(&engine));
		bpf = bpf > 0 ? bpf : 1;
		uint32_t bufferSizeInFrames = (uint32_t)bufferSizeInBytes / bpf;
		ma_engine_read_pcm_frames(&engine, p_buffer, bufferSizeInFrames, NULL);
	}

	Shared_SDL::Shared_SDL() = default;
	Shared_SDL::~Shared_SDL() = default;

	void Device_SDL::addEventListener(IAudioDeviceEventListener* p_m_listener)
	{
		assert(!m_dispatch_event);
		if (!m_listener.contains(p_m_listener))
		{
			m_listener.insert(p_m_listener);
		}
	}
	void Device_SDL::removeEventListener(IAudioDeviceEventListener* p_m_listener)
	{
		assert(!m_dispatch_event);
		if (m_listener.contains(p_m_listener))
		{
			m_listener.erase(p_m_listener);
		}
	}
	void Device_SDL::dispatchEventAudioDeviceCreate()
	{
		m_dispatch_event = true;
		for (auto& v : m_listener)
		{
			v->onAudioDeviceCreate();
		}
		m_dispatch_event = false;
	}
	void Device_SDL::dispatchEventAudioDeviceDestroy()
	{
		m_dispatch_event = true;
		for (auto& v : m_listener)
		{
			v->onAudioDeviceDestroy();
		}
		m_dispatch_event = false;
	}

	bool Device_SDL::refreshAudioDeviceList()
	{
		m_audio_device_list.clear();

		int32_t device_count = SDL_GetNumAudioDevices(0);

		if (device_count == -1)
			return false;

		for (int32_t index = 0; index < device_count; index += 1)
		{
			m_audio_device_list.emplace_back(SDL_GetAudioDeviceName(index, 0));
		}

		return true;
	}
	uint32_t Device_SDL::getAudioDeviceCount(bool refresh)
	{
		if (refresh)
		{
			refreshAudioDeviceList();
		}
		return static_cast<uint32_t>(m_audio_device_list.size());
	}
	std::string_view Device_SDL::getAudioDeviceName(uint32_t index) const noexcept
	{
		if (!m_audio_device_list.empty() && index < m_audio_device_list.size())
		{
			return m_audio_device_list[index];
		}
		return "";
	}
	bool Device_SDL::setTargetAudioDevice(std::string_view const audio_device_name)
	{
		m_target_audio_device_name = audio_device_name;
		destroyResources();
		return createResources();
	}

	bool Device_SDL::createResources()
	{
		m_shared.attach(new Shared_SDL);

		// output

		std::string_view device_name;
		if (refreshAudioDeviceList())
		{
			for (auto const& v : m_audio_device_list)
			{
				if (v == m_target_audio_device_name)
				{
					device_name = v;
					break;
				}
			}
		}


		SDL_AudioSpec want, have;
		SDL_AudioDeviceID dev;

		SDL_zero(want);
		want.freq = 48000;
		want.format = AUDIO_F32;
		want.channels = 2;
		want.samples = 4096;
		want.userdata = m_shared.get();
		want.callback = data_callback;
		if (device_name.empty())
		{
			dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
		}
		else
		{
			dev = SDL_OpenAudioDevice(std::string(device_name).c_str(), 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
		}

		if (!dev)
		{
			spdlog::error("[core] Failed to initialize audio device. {}", SDL_GetError());
			return false;
		}

		ma_engine_config cfg = ma_engine_config_init();
		cfg.noDevice = MA_TRUE;
		cfg.channels = 2;
		cfg.sampleRate = 48000;
		if (ma_engine_init(&cfg, &m_shared->engine) != MA_SUCCESS)
		{
			spdlog::error("[core] Failed to initialize audio engine.");
			return false;
		}

		if (ma_sound_group_init(&m_shared->engine, 0, NULL, &m_shared->grp_sfx) != MA_SUCCESS)
		{
			spdlog::error("[core] Failed to initialize sound group.");
			return false;
		}
		if (ma_sound_group_init(&m_shared->engine, 0, NULL, &m_shared->grp_bgm) != MA_SUCCESS)
		{
			spdlog::error("[core] Failed to initialize sound group.");
			return false;
		}

		// build graph

		if (ma_engine_set_volume(&m_shared->engine, std::clamp(m_volume_direct, 0.0f, 1.0f)) != MA_SUCCESS)
		{
			spdlog::error("[core] Failed to set master volume.");
			return false;
		}
		ma_sound_set_volume(&m_shared->grp_sfx, std::clamp(m_volume_sound_effect, 0.0f, 1.0f));
		ma_sound_set_volume(&m_shared->grp_bgm, std::clamp(m_volume_music, 0.0f, 1.0f));
		
		m_current_audio_device_name = device_name;
		dispatchEventAudioDeviceCreate();
		SDL_PauseAudioDevice(dev, 0);

		return true;
	}
	void Device_SDL::destroyResources()
	{
		dispatchEventAudioDeviceDestroy();
		m_current_audio_device_name.clear();
		m_shared.reset();
	}

	void Device_SDL::setVolume(float v)
	{
		setMixChannelVolume(MixChannel::Direct, v);
	}
	float Device_SDL::getVolume()
	{
		return getMixChannelVolume(MixChannel::Direct);
	}
	void Device_SDL::setMixChannelVolume(MixChannel ch, float v)
	{
		switch (ch)
		{
		case MixChannel::Direct:
			m_volume_direct = v;
			break;
		case MixChannel::SoundEffect:
			m_volume_sound_effect = v;
			break;
		case MixChannel::Music:
			m_volume_music = v;
			break;
		default:
			assert(false);
			break;
		}

		if (!m_shared) return;

		switch (ch)
		{
		case MixChannel::Direct:
			if (ma_engine_set_volume(&m_shared->engine, std::clamp(v, 0.0f, 1.0f)) != MA_SUCCESS)
			{
				spdlog::error("[core] Failed to set master volume.");
			}
			break;
		case MixChannel::SoundEffect:
			ma_sound_set_volume(&m_shared->grp_sfx, std::clamp(v, 0.0f, 1.0f));
			break;
		case MixChannel::Music:
			ma_sound_set_volume(&m_shared->grp_bgm, std::clamp(v, 0.0f, 1.0f));
			break;
		default:
			assert(false);
			break;
		}
	}
	float Device_SDL::getMixChannelVolume(MixChannel ch)
	{
		switch (ch)
		{
		case MixChannel::Direct: return m_volume_direct;
		case MixChannel::SoundEffect: return m_volume_sound_effect;
		case MixChannel::Music: return m_volume_music;
		default: assert(false); return 1.0f;
		}
	}

	bool Device_SDL::createAudioPlayer(IDecoder* p_decoder, IAudioPlayer** pp_player)
	{
		try
		{
			*pp_player = new AudioPlayer_SDL(this, p_decoder);
			return true;
		}
		catch (std::exception const& e)
		{
			spdlog::error("[core] {}", e.what());
			*pp_player = nullptr;
			return false;
		}
	}
	bool Device_SDL::createLoopAudioPlayer(IDecoder* p_decoder, IAudioPlayer** pp_player)
	{
		try
		{
			*pp_player = new LoopAudioPlayer_SDL(this, p_decoder);
			return true;
		}
		catch (std::exception const& e)
		{
			spdlog::error("[core] {}", e.what());
			*pp_player = nullptr;
			return false;
		}
	}
	bool Device_SDL::createStreamAudioPlayer(IDecoder* p_decoder, IAudioPlayer** pp_player)
	{
		try
		{
			*pp_player = new StreamAudioPlayer_SDL(this, p_decoder);
			return true;
		}
		catch (std::exception const& e)
		{
			spdlog::error("[core] {}", e.what());
			*pp_player = nullptr;
			return false;
		}
	}

	Device_SDL::Device_SDL()
	{
		if (!createResources())
		{
			spdlog::error("[core] Audio device creation failed.");
		}
	}
	Device_SDL::~Device_SDL()
	{
	}

	bool Device_SDL::create(Device_SDL** pp_audio)
	{
		try
		{
			*pp_audio = new Device_SDL;
			return true;
		}
		catch (std::exception const& e)
		{
			spdlog::error("[core] {}", e.what());
			*pp_audio = nullptr;
			return false;
		}
	}
}

namespace Core::Audio
{
	// void WINAPI AudioPlayer_SDL::OnVoiceError(void*, HRESULT Error) noexcept
	// {
	// 	gHR = Error;
	// 	spdlog::error("[core] @IXAudio2VoiceCallback::OnVoiceError");
	// }

	void AudioPlayer_SDL::onAudioDeviceCreate()
	{
		createResources();
	}
	void AudioPlayer_SDL::onAudioDeviceDestroy()
	{
		destoryResources();
	}

	bool AudioPlayer_SDL::createResources()
	{
		if (!m_device->getShared()) return false;

		m_shared = m_device->getShared();

		ma_result r;

		r = ma_sound_init_from_data_source(&m_shared->engine, static_cast<Decoder_ma*>(m_decoder.get())->getRaw(), 0, &m_shared->grp_sfx, &m_sound);
		if (r != MA_SUCCESS)
		{
			spdlog::error("[core] Couldn't init audio player");
			return false;
		}

		ma_sound_set_volume(&m_sound, std::clamp(m_volume, 0.0f, 1.0f));
		ma_sound_set_pan(&m_sound, std::clamp(m_output_balance, -1.0f, 1.0f));
		ma_sound_set_pitch(&m_sound, m_speed);

		return true;
	}
	void AudioPlayer_SDL::destoryResources()
	{
		ma_sound_uninit(&m_sound);
		m_shared.reset();
	}

	bool AudioPlayer_SDL::start()
	{
		m_is_playing = true;
		return MA_SUCCESS == ma_sound_start(&m_sound);
	}
	bool AudioPlayer_SDL::stop()
	{
		m_is_playing = false;
		return MA_SUCCESS == ma_sound_stop(&m_sound);
	}
	bool AudioPlayer_SDL::reset()
	{
		m_is_playing = false;

		if (MA_SUCCESS != ma_sound_stop(&m_sound))
			return false;
		if (MA_SUCCESS != ma_sound_seek_to_pcm_frame(&m_sound, 0))
			return false;

		return true;
	}

	bool AudioPlayer_SDL::isPlaying()
	{
		return m_is_playing;
	}

	double AudioPlayer_SDL::getTotalTime() { assert(false); return 0.0; }
	double AudioPlayer_SDL::getTime() { assert(false); return 0.0; }
	bool AudioPlayer_SDL::setTime(double) { assert(false); return true; }
	bool AudioPlayer_SDL::setLoop(bool, double, double) { assert(false); return true; }

	float AudioPlayer_SDL::getVolume()
	{
		return m_volume;
	}
	bool AudioPlayer_SDL::setVolume(float v)
	{
		m_volume = v;
		ma_sound_set_volume(&m_sound, std::clamp(m_volume, 0.0f, 1.0f));
		return true;
	}
	float AudioPlayer_SDL::getBalance()
	{
		return m_output_balance;
	}
	bool AudioPlayer_SDL::setBalance(float v)
	{
		m_output_balance = v;
		ma_sound_set_pan(&m_sound, std::clamp(m_output_balance, -1.0f, 1.0f));
		return true;
	}
	float AudioPlayer_SDL::getSpeed()
	{
		return m_speed;
	}
	bool AudioPlayer_SDL::setSpeed(float v)
	{
		m_speed = v;
		ma_sound_set_pitch(&m_sound, m_speed);
		return true;
	}

	void AudioPlayer_SDL::updateFFT() { assert(false); }
	uint32_t AudioPlayer_SDL::getFFTSize() { assert(false); return 0; }
	float* AudioPlayer_SDL::getFFT() { assert(false); return s_empty_fft_data.data(); }

	AudioPlayer_SDL::AudioPlayer_SDL(Device_SDL* p_device, IDecoder* p_decoder)
		: m_device(p_device)
		, m_decoder(p_decoder)
	{
		// decoding

		m_pcm_data.resize(p_decoder->getFrameCount() * (uint32_t)p_decoder->getFrameSize());
		uint64_t frames_read = 0;
		if (!p_decoder->read(p_decoder->getFrameCount(), m_pcm_data.data(), &frames_read))
		{
			spdlog::error("[core] (IDecoder::read) Failed to read audio");
			throw std::runtime_error("AudioPlayer_SDL::AudioPlayer_SDL (4)");
		}

		// create audio

		if (createResources())
		{
			spdlog::info("[core] (AudioPlayer_SDL) Initialized audio player");
			// nothing wrong
		}

		// register

		m_device->addEventListener(this);
	}
	AudioPlayer_SDL::~AudioPlayer_SDL()
	{
		m_device->removeEventListener(this);
		destoryResources();
	}
}

namespace Core::Audio
{
	// void WINAPI LoopAudioPlayer_SDL::OnVoiceError(void*, HRESULT Error) noexcept
	// {
	// 	gHR = Error;
	// 	spdlog::error("[core] @IXAudio2VoiceCallback::OnVoiceError");
	// }

	void LoopAudioPlayer_SDL::onAudioDeviceCreate()
	{
		createResources();
	}
	void LoopAudioPlayer_SDL::onAudioDeviceDestroy()
	{
		destoryResources();
	}

	bool LoopAudioPlayer_SDL::createResources()
	{
		if (!m_device->getShared()) return false;

		m_shared = m_device->getShared();

		ma_result r;

		r = ma_sound_init_from_data_source(&m_shared->engine, static_cast<Decoder_ma*>(m_decoder.get())->getRaw(), 0, &m_shared->grp_bgm, &m_sound);
		if (r != MA_SUCCESS)
		{
			spdlog::error("[core] Couldn't init audio player");
			return false;
		}

		ma_sound_set_volume(&m_sound, std::clamp(m_volume, 0.0f, 1.0f));
		ma_sound_set_pan(&m_sound, std::clamp(m_output_balance, -1.0f, 1.0f));
		ma_sound_set_pitch(&m_sound, m_speed);

		return true;
	}
	void LoopAudioPlayer_SDL::destoryResources()
	{
		ma_sound_uninit(&m_sound);
		m_shared.reset();
	}

	bool LoopAudioPlayer_SDL::start()
	{
		m_is_playing = true;
		return MA_SUCCESS == ma_sound_start(&m_sound);
	}
	bool LoopAudioPlayer_SDL::stop()
	{
		m_is_playing = false;
		return MA_SUCCESS == ma_sound_stop(&m_sound);
	}
	bool LoopAudioPlayer_SDL::reset()
	{
		m_is_playing = false;

		if (MA_SUCCESS != ma_sound_stop(&m_sound))
			return false;
		if (MA_SUCCESS != ma_sound_seek_to_pcm_frame(&m_sound, 0))
			return false;

		return true;
	}

	bool LoopAudioPlayer_SDL::isPlaying()
	{
		return m_is_playing;
	}

	double LoopAudioPlayer_SDL::getTotalTime() { assert(false); return 0.0; }
	double LoopAudioPlayer_SDL::getTime() { assert(false); return 0.0; }
	bool LoopAudioPlayer_SDL::setTime(double t)
	{
		m_start_time = t;
		uint32_t const start_sample = (uint32_t)((double)m_sample_rate * m_start_time);
		assert(start_sample <= m_total_frame);
		return start_sample <= m_total_frame;
	}
	bool LoopAudioPlayer_SDL::setLoop(bool enable, double start_pos, double length)
	{
		m_is_loop = enable;
		m_loop_start = start_pos;
		m_loop_length = length;
		uint32_t const loop_start_sample = (uint32_t)((double)m_sample_rate * m_loop_start);
		uint32_t const loop_range_sample_count = (uint32_t)((double)m_sample_rate * m_loop_length);
		ma_sound_set_looping(&m_sound, enable);
		assert((loop_start_sample + loop_range_sample_count) <= m_total_frame);
		ma_result res = ma_data_source_set_loop_point_in_pcm_frames(m_sound.pDataSource, loop_start_sample, loop_start_sample + loop_range_sample_count);
		if (res != MA_SUCCESS)
			spdlog::error("[core] setLoop failed: {}", res);
		return (loop_start_sample + loop_range_sample_count) <= m_total_frame;
	}

	float LoopAudioPlayer_SDL::getVolume()
	{
		return m_volume;
	}
	bool LoopAudioPlayer_SDL::setVolume(float v)
	{
		m_volume = v;
		ma_sound_set_volume(&m_sound, std::clamp(m_volume, 0.0f, 1.0f));
		return true;
	}
	float LoopAudioPlayer_SDL::getBalance()
	{
		return m_output_balance;
	}
	bool LoopAudioPlayer_SDL::setBalance(float v)
	{
		m_output_balance = v;
		ma_sound_set_pan(&m_sound, std::clamp(m_output_balance, -1.0f, 1.0f));
		return true;
	}
	float LoopAudioPlayer_SDL::getSpeed()
	{
		return m_speed;
	}
	bool LoopAudioPlayer_SDL::setSpeed(float v)
	{
		m_speed = v;
		ma_sound_set_pitch(&m_sound, m_speed);
		return true;
	}

	void LoopAudioPlayer_SDL::updateFFT() { assert(false); }
	uint32_t LoopAudioPlayer_SDL::getFFTSize() { assert(false); return 0; }
	float* LoopAudioPlayer_SDL::getFFT() { assert(false); return s_empty_fft_data.data(); }

	LoopAudioPlayer_SDL::LoopAudioPlayer_SDL(Device_SDL* p_device, IDecoder* p_decoder)
		: m_device(p_device)
	#ifndef NDEBUG
		, m_decoder(p_decoder)
	#endif
	{
		// decoding

		m_pcm_data.resize(p_decoder->getFrameCount() * (uint32_t)p_decoder->getFrameSize());
		uint64_t frames_read = 0;
		if (!p_decoder->read(p_decoder->getFrameCount(), m_pcm_data.data(), &frames_read))
		{
			spdlog::error("[core] (IDecoder::read) Failed to read audio");
			throw std::runtime_error("AudioPlayer_SDL::AudioPlayer_SDL (4)");
		}

		m_total_frame = frames_read;
		m_frame_size = p_decoder->getFrameSize();
		m_sample_rate = p_decoder->getSampleRate();

		// create audio

		if (createResources())
		{
			spdlog::info("[core] (LoopAudioPlayer_SDL) Initialized audio player");
			// nothing wrong
		}

		// register

		m_device->addEventListener(this);
	}
	LoopAudioPlayer_SDL::~LoopAudioPlayer_SDL()
	{
		m_device->removeEventListener(this);
		destoryResources();
	}
}

namespace Core::Audio
{
	void StreamAudioPlayer_SDL::onAudioDeviceCreate()
	{
		createResources();
	}
	void StreamAudioPlayer_SDL::onAudioDeviceDestroy()
	{
		destoryResources();
	}

	bool StreamAudioPlayer_SDL::createResources()
	{
		if (!m_device->getShared()) return false;

		m_shared = m_device->getShared();

		ma_result r;

		r = ma_sound_init_from_data_source(&m_shared->engine, static_cast<Decoder_ma*>(m_decoder.get())->getRaw(), MA_SOUND_FLAG_STREAM, &m_shared->grp_bgm, &m_sound);
		if (r != MA_SUCCESS)
		{
			spdlog::error("[core] Couldn't init audio player");
			return false;
		}

		ma_sound_set_volume(&m_sound, std::clamp(m_volume, 0.0f, 1.0f));
		ma_sound_set_pan(&m_sound, std::clamp(m_output_balance, -1.0f, 1.0f));
		ma_sound_set_pitch(&m_sound, m_speed);

		return true;
	}
	void StreamAudioPlayer_SDL::destoryResources()
	{
		ma_sound_uninit(&m_sound);
		m_shared.reset();
	}

	bool StreamAudioPlayer_SDL::start()
	{
		m_is_playing = true;
		return MA_SUCCESS == ma_sound_start(&m_sound);
	}
	bool StreamAudioPlayer_SDL::stop()
	{
		m_is_playing = false;
		return MA_SUCCESS == ma_sound_stop(&m_sound);
	}
	bool StreamAudioPlayer_SDL::reset()
	{
		m_is_playing = false;

		if (MA_SUCCESS != ma_sound_stop(&m_sound))
			return false;
		if (MA_SUCCESS != ma_sound_seek_to_pcm_frame(&m_sound, 0))
			return false;

		return true;
	}

	bool StreamAudioPlayer_SDL::isPlaying()
	{
		return m_is_playing;
	}

	double StreamAudioPlayer_SDL::getTotalTime() { assert(false); return 0.0; }
	double StreamAudioPlayer_SDL::getTime() { assert(false); return 0.0; }
	bool StreamAudioPlayer_SDL::setTime(double t)
	{
		m_start_time = t;
		uint32_t const start_sample = (uint32_t)((double)m_sample_rate * m_start_time);
		assert(start_sample <= m_total_frame);
		return start_sample <= m_total_frame;
	}
	bool StreamAudioPlayer_SDL::setLoop(bool enable, double start_pos, double length)
	{
		m_is_loop = enable;
		m_loop_start = start_pos;
		m_loop_length = length;
		uint32_t const loop_start_sample = (uint32_t)((double)m_sample_rate * m_loop_start);
		uint32_t const loop_range_sample_count = (uint32_t)((double)m_sample_rate * m_loop_length);
		ma_sound_set_looping(&m_sound, enable);
		assert((loop_start_sample + loop_range_sample_count) <= m_total_frame);
		ma_result res = ma_data_source_set_loop_point_in_pcm_frames(m_sound.pDataSource, loop_start_sample, loop_start_sample + loop_range_sample_count);
		if (res != MA_SUCCESS)
			spdlog::error("[core] setLoop failed: {}", res);
		return (loop_start_sample + loop_range_sample_count) <= m_total_frame;
	}

	float StreamAudioPlayer_SDL::getVolume()
	{
		return m_volume;
	}
	bool StreamAudioPlayer_SDL::setVolume(float v)
	{
		m_volume = v;
		ma_sound_set_volume(&m_sound, std::clamp(m_volume, 0.0f, 1.0f));
		return true;
	}
	float StreamAudioPlayer_SDL::getBalance()
	{
		return m_output_balance;
	}
	bool StreamAudioPlayer_SDL::setBalance(float v)
	{
		m_output_balance = v;
		ma_sound_set_pan(&m_sound, std::clamp(m_output_balance, -1.0f, 1.0f));
		return true;
	}
	float StreamAudioPlayer_SDL::getSpeed()
	{
		return m_speed;
	}
	bool StreamAudioPlayer_SDL::setSpeed(float v)
	{
		m_speed = v;
		ma_sound_set_pitch(&m_sound, m_speed);
		return true;
	}

	void StreamAudioPlayer_SDL::updateFFT() { assert(false); }
	uint32_t StreamAudioPlayer_SDL::getFFTSize() { assert(false); return 0; }
	float* StreamAudioPlayer_SDL::getFFT() { assert(false); return s_empty_fft_data.data(); }

	StreamAudioPlayer_SDL::StreamAudioPlayer_SDL(Device_SDL* p_device, IDecoder* p_decoder)
		: m_device(p_device)
	// #ifndef NDEBUG
		, m_decoder(p_decoder)
	// #endif
	{
		// decoding

		m_pcm_data.resize(p_decoder->getFrameCount() * (uint32_t)p_decoder->getFrameSize());
		uint64_t frames_read = 0;
		
		if (!p_decoder->read(p_decoder->getFrameCount(), m_pcm_data.data(), &frames_read))
		{
			spdlog::error("[core] (IDecoder::read) Failed to read audio");
			throw std::runtime_error("StreamAudioPlayer_SDL::StreamAudioPlayer_SDL (4)");
		}

		m_total_frame = frames_read;
		m_frame_size = p_decoder->getFrameSize();
		m_sample_rate = p_decoder->getSampleRate();

		// create audio

		if (createResources())
		{
			spdlog::info("[core] (StreamAudioPlayer_SDL) Initialized audio player");
			// nothing wrong
		}

		// register

		m_device->addEventListener(this);
	}
	StreamAudioPlayer_SDL::~StreamAudioPlayer_SDL()
	{
		m_device->removeEventListener(this);
		destoryResources();
	}
}

