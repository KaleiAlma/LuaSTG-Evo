#pragma once
#include "Core/Type.hpp"
#include "Core/Audio/Decoder.hpp"

namespace Core::Audio
{
	enum class MixChannel
	{
		Direct,      // Output directly to the device, cannot be used for IDevice::setMixChannelVolume
		SoundEffect, // Belongs to the SFX channel
		Music,       // Belongs to the BGM channel
		Video,       // TODO: for video

		_COUNT,
	};

	struct IAudioPlayer : public IObject
	{
		virtual bool start() = 0;
		virtual bool stop() = 0;
		virtual bool reset() = 0;

		virtual bool isPlaying() = 0;

		virtual double getTotalTime() = 0;
		virtual double getTime() = 0;
		virtual bool setTime(double t) = 0;
		virtual bool getLoop() = 0;
		virtual void getLoopRange(double& start_pos, double& length) = 0;
		virtual bool setLoop(bool enable) = 0;
		virtual bool setLoopRange(double start_pos, double length) = 0;
		
		virtual float getVolume() = 0;
		virtual bool setVolume(float v) = 0;
		virtual float getBalance() = 0;
		virtual bool setBalance(float v) = 0;
		virtual float getSpeed() = 0;
		virtual bool setSpeed(float v) = 0;

		virtual void updateFFT() = 0;
		virtual uint32_t getFFTSize() = 0;
		virtual float* getFFT() = 0;

		// virtual IDecoder* getDecoder() = 0;
	};

	struct IAudioDevice : public IObject
	{
		virtual uint32_t getAudioDeviceCount(bool refresh) = 0;
		virtual std::string_view getAudioDeviceName(uint32_t index) const noexcept = 0;
		virtual bool setTargetAudioDevice(std::string_view const audio_device_name) = 0;
		virtual std::string_view getCurrentAudioDeviceName() const noexcept = 0;

		virtual void setVolume(float v) = 0;
		virtual float getVolume() = 0;
		virtual void setMixChannelVolume(MixChannel ch, float v) = 0;
		virtual float getMixChannelVolume(MixChannel ch) = 0;

		virtual bool createAudioPlayer(IDecoder* p_decoder, IAudioPlayer** pp_player) = 0; // Decode all into memory
		virtual bool createLoopAudioPlayer(IDecoder* p_decoder, IAudioPlayer** pp_player) = 0; // Decode all into memory
		virtual bool createStreamAudioPlayer(IDecoder* p_decoder, IAudioPlayer** pp_player) = 0; // Decode during playback
	};
}
