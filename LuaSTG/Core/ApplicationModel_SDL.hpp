#pragma once
#include "Core/Object.hpp"
#include "Core/ApplicationModel.hpp"
#include "Core/Graphics/Window_SDL.hpp"
#include "Core/Graphics/Device_OpenGL.hpp"
#include "Core/Graphics/SwapChain_OpenGL.hpp"
#include "Core/Graphics/Renderer_OpenGL.hpp"
#include "Core/Audio/Device_SDL.hpp"
#include <chrono>

namespace Core
{

	class FrameRateController : public IFrameRateController
	{
	private:
		using Duration = std::chrono::duration<double>;
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = std::chrono::time_point<Clock>;
	private:
		// int64_t freq_{};
		TimePoint last_{};
		double wait_{};
		// int64_t _2ms_{};
		uint64_t total_frame_{};
		double total_time_{};
		double target_fps_{ 60.0 };
		double fps_[256]{};
		double fps_avg_{};
		double fps_min_{};
		double fps_max_{};
		size_t fps_index_{};
	private:
		double indexFPS(size_t idx);
	public:
		double udateData(TimePoint curr);
		bool arrive();
		double update();
	public:
		uint32_t getTargetFPS();
		void setTargetFPS(uint32_t target_FPS);
		double getFPS();
		uint64_t getTotalFrame();
		double getTotalTime();
		double getAvgFPS();
		double getMinFPS();
		double getMaxFPS();
	public:
		FrameRateController(uint32_t target_FPS = 60);
		~FrameRateController();
	};

	class ApplicationModel_SDL : public Object<IApplicationModel>
	{
	private:
		// Shared between threads

		ScopeObject<Graphics::Window_SDL> m_window;
		// Microsoft::WRL::Wrappers::Event win32_event_exit;
		bool m_exit_flag{};

		// Work thread exclusive

		ScopeObject<Graphics::Device_OpenGL> m_device;
		ScopeObject<Graphics::SwapChain_OpenGL> m_swapchain;
		ScopeObject<Graphics::Renderer_OpenGL> m_renderer;
		ScopeObject<Audio::Device_SDL> m_audiosys;
		FrameRateController m_frame_rate_controller;
		IApplicationEventListener* m_listener{ nullptr };
		size_t m_framestate_index{ 0 };
		FrameStatistics m_framestate[2]{};

		bool runSingleThread();

	public:
		// Internal Public

		void runFrame();

	public:
		// Shared between threads

		Graphics::IWindow* getWindow() { return *m_window; }
		void requestExit();

		// Work thread exclusive

		IFrameRateController* getFrameRateController() { return &m_frame_rate_controller; };
		Graphics::IDevice* getDevice() { return *m_device; }
		Graphics::ISwapChain* getSwapChain() { return *m_swapchain; }
		Graphics::IRenderer* getRenderer() { return *m_renderer; }
		Audio::IAudioDevice* getAudioDevice() { return m_audiosys.get(); }
		FrameStatistics getFrameStatistics();
		FrameRenderStatistics getFrameRenderStatistics();

		// Main thread exclusive

		bool run();

	public:
		ApplicationModel_SDL(IApplicationEventListener* p_listener);
		~ApplicationModel_SDL();
	};
}
