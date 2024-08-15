#include "Core/ApplicationModel_SDL.hpp"
#include "Core/ApplicationModel.hpp"
// #include "Core/i18n.hpp"
// #include "Platform/WindowsVersion.hpp"
// #include "Platform/DetectCPU.hpp"
#include "TracyOpenGL.hpp"
#include "SDL.h"
#include "spdlog/spdlog.h"
#include <chrono>
#include <limits>
#include <thread>

using Duration = std::chrono::duration<double>;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock, Duration>;

namespace Core
{
	double FrameRateController::indexFPS(size_t idx)
	{
		return fps_[(fps_index_ + std::size(fps_) - 1 - idx) % std::size(fps_)];
	}

	double FrameRateController::udateData(TimePoint curr)
	{
		// Update Values
		double const fps = 1.0 / Duration(curr - last_).count();
		double const s = 1.0 / fps;
		total_frame_ += 1;
		total_time_ += s;
		fps_[fps_index_] = fps;
		fps_index_ = (fps_index_ + 1) % std::size(fps_);
		last_ = curr;

		// Update Stats
		fps_min_ = std::numeric_limits<double>::max();
		fps_max_ = -std::numeric_limits<double>::max();
		size_t const total_history = total_frame_ < std::size(fps_) ? (size_t)total_frame_ : std::size(fps_);
		if (total_history > 0)
		{
			double total_fps = 0.0;
			double total_time = 0.0;
			size_t history_count = 0;
			for (size_t i = 0; i < total_history; i += 1)
			{
				double const fps_history = indexFPS(i);
				total_fps += fps_history;
				fps_min_ = std::min(fps_min_, fps_history);
				fps_max_ = std::max(fps_max_, fps_history);
				total_time += 1.0 / fps_history;
				history_count += 1;
				if (total_time >= 0.25)
				{
					break;
				}
			}
			fps_avg_ = total_fps / (double)history_count;
		}
		else
		{
			fps_avg_ = 0.0;
			fps_min_ = 0.0;
			fps_max_ = 0.0;
		}

		return s;
	}
	bool FrameRateController::arrive()
	{
		TimePoint curr_ = Clock::now();

		if ((curr_ - last_) < Duration(wait_))
		{
			return false;
		}
		else
		{
			udateData(curr_);
			return true;
		}
	}
	double FrameRateController::update()
	{
		TimePoint curr_ = Clock::now();

		// unsure if we still need overflow handling code
		// if (curr_.QuadPart < last_)
		// {
		// 	total_frame_ += 1;
		// 	total_time_ += 1.0 / indexFPS(0);
		// 	last_ = curr_.QuadPart;
		// 	return indexFPS(0);
		// }

		Duration const sleep_dur = Duration(wait_ - 0.0001) - (Duration)(curr_ - last_);
		if (sleep_dur > Duration::zero())
		{
			std::this_thread::sleep_for(sleep_dur);
		}

		// Make sure the duration has fully elapsed by spinning the CPU
		do
		{
			curr_ = Clock::now();
		} while ((curr_ - last_) < Duration(wait_));

		return udateData(curr_);
	}

	uint32_t FrameRateController::getTargetFPS()
	{
		return (uint32_t)target_fps_;
	}
	void FrameRateController::setTargetFPS(uint32_t target_FPS)
	{
		target_fps_ = (double)(target_FPS > 0 ? target_FPS : 1);
		wait_ = (1.0 / target_fps_);
	}
	double FrameRateController::getFPS()
	{
		return indexFPS(0);
	}
	uint64_t FrameRateController::getTotalFrame()
	{
		return total_frame_;
	}
	double FrameRateController::getTotalTime()
	{
		return total_time_;
	}
	double FrameRateController::getAvgFPS()
	{
		return fps_avg_;
	}
	double FrameRateController::getMinFPS()
	{
		return fps_min_;
	}
	double FrameRateController::getMaxFPS()
	{
		return fps_max_;
	}

	FrameRateController::FrameRateController(uint32_t target_FPS)
	{
		setTargetFPS(target_FPS);
		last_ = Clock::now();
	}
	FrameRateController::~FrameRateController() {}
}

namespace Core
{
	// static std::string bytes_count_to_string(DWORDLONG size)
	// {
	// 	int count = 0;
	// 	char buffer[64] = {};
	// 	if (size < 1024llu) // B
	// 	{
	// 		count = std::snprintf(buffer, 64, "%u B", (unsigned int)size);
	// 	}
	// 	else if (size < (1024llu * 1024llu)) // KB
	// 	{
	// 		count = std::snprintf(buffer, 64, "%.2f KiB", (double)size / 1024.0);
	// 	}
	// 	else if (size < (1024llu * 1024llu * 1024llu)) // MB
	// 	{
	// 		count = std::snprintf(buffer, 64, "%.2f MiB", (double)size / 1048576.0);
	// 	}
	// 	else // GB
	// 	{
	// 		count = std::snprintf(buffer, 64, "%.2f GiB", (double)size / 1073741824.0);
	// 	}
	// 	return std::string(buffer, count);
	// }
	// static void get_system_memory_status()
	// {
	// 	MEMORYSTATUSEX info = { sizeof(MEMORYSTATUSEX) };
	// 	if (GlobalMemoryStatusEx(&info))
	// 	{
	// 		spdlog::info("[core] 系统内存使用情况：\n"
	// 			"    使用百分比：{}%\n"
	// 			"    总物理内存：{}\n"
	// 			"    剩余物理内存：{}\n"
	// 			"    当前进程可提交内存限制：{}\n"
	// 			"    当前进程剩余的可提交内存：{}\n"
	// 			"    当前进程用户模式内存空间限制*1：{}\n"
	// 			"    当前进程剩余的用户模式内存空间：{}\n"
	// 			"        *1 此项反映此程序实际上能用的最大内存，在 32 位应用程序上此项一般为 2 GB，修改 Windows 操作系统注册表后可能为 1 到 3 GB"
	// 			, info.dwMemoryLoad
	// 			, bytes_count_to_string(info.ullTotalPhys)
	// 			, bytes_count_to_string(info.ullAvailPhys)
	// 			, bytes_count_to_string(info.ullTotalPageFile)
	// 			, bytes_count_to_string(info.ullAvailPageFile)
	// 			, bytes_count_to_string(info.ullTotalVirtual)
	// 			, bytes_count_to_string(info.ullAvailVirtual)
	// 		);
	// 	}
	// 	else
	// 	{
	// 		spdlog::error("[fancy2d] 无法获取系统内存使用情况");
	// 	}
	// }

	struct ScopeTimer
	{
		// LARGE_INTEGER freq{};
		// LARGE_INTEGER last{};
		TimePoint start;
		double& t;
		ScopeTimer(double& v_ref) : t(v_ref)
		{
			// QueryPerformanceFrequency(&freq);
			// QueryPerformanceCounter(&last);
			start = Clock::now();
		}
		~ScopeTimer()
		{
			// LARGE_INTEGER curr{};
			// QueryPerformanceCounter(&curr);
			// t = (double)(curr.QuadPart - last.QuadPart) / (double)freq.QuadPart;
			Duration dur = Clock::now() - start;
			t = dur.count();
		}
	};

	bool ApplicationModel_SDL::runSingleThread()
	{
		// 设置线程优先级为高，并尽量让它运行在同一个 CPU 核心上，降低切换开销
		// SetThreadAffinityMask(GetCurrentThread(), 1);
		// SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

		// 初次收集诊断信息
		TracyGpuCollect;
		FrameMark;
		{
			ZoneScopedN("OnInitWait");
			// m_swapchain->waitFrameLatency();
			m_frame_rate_controller.update();
		}

		// 游戏循环
		// MSG msg{};
		while (!m_exit_flag)
		{
			runFrame();
		}

		return true;
	}

	void ApplicationModel_SDL::runFrame()
	{
		size_t const i = (m_framestate_index + 1) % 2;
		FrameStatistics& d = m_framestate[i];
		ScopeTimer gt(d.total_time);
		// size_t const next_frame_query_index = (m_frame_query_index + 1) % m_frame_query_list.size();
		// FrameQuery& frame_query = m_frame_query_list[next_frame_query_index];
		
		bool update_result = false;

		// Update
		{
			ZoneScopedN("OnUpdate");
			ScopeTimer t(d.update_time);
			m_window->handleEvents();
			update_result = m_listener->onUpdate();
		}

		bool render_result = false;

		// Render
		if (update_result)
		{
			ZoneScopedN("OnRender");
			TracyGpuZone("OnRender");
			ScopeTimer t(d.render_time);
			// frame_query.begin();
			m_swapchain->applyRenderAttachment();
			m_swapchain->clearRenderAttachment();
			render_result = m_listener->onRender();
			// frame_query.end();
		}

		// Present
		if (render_result)
		{
			ZoneScopedN("OnPresent");
			TracyGpuZone("OnPresent");
			ScopeTimer t(d.present_time);
			m_swapchain->present();
			TracyGpuCollect;
		}

		// Wait for next frame
		{
			ZoneScopedN("OnWait");
			ScopeTimer t(d.wait_time);
			// m_swapchain->waitFrameLatency();
			m_frame_rate_controller.update();
		}

		m_framestate_index = i;
		// m_frame_query_index = next_frame_query_index;
		FrameMark;
	}

	FrameStatistics ApplicationModel_SDL::getFrameStatistics()
	{
		return m_framestate[m_framestate_index];
	}
	FrameRenderStatistics ApplicationModel_SDL::getFrameRenderStatistics()
	{
		// FrameQuery& frame_query = m_frame_query_list[m_frame_query_index];
		FrameRenderStatistics statistics{};
		// statistics.render_time = frame_query.getTime();
		statistics.render_time = m_framestate[m_framestate_index].render_time;
		return statistics;
	}

	void ApplicationModel_SDL::requestExit()
	{
		// SetEvent(win32_event_exit.Get());
		m_exit_flag = true;
	}
	bool ApplicationModel_SDL::run()
	{
		return runSingleThread();
	}

	ApplicationModel_SDL::ApplicationModel_SDL(IApplicationEventListener* p_listener)
		: m_listener(p_listener)
	{
		assert(m_listener);
		// spdlog::info("[core] System {}", Platform::WindowsVersion::GetName());
		// spdlog::info("[core] Kernel {}", Platform::WindowsVersion::GetKernelVersionString());
		// spdlog::info("[core] CPU {} {}", InstructionSet::Vendor(), InstructionSet::Brand());
		// if (m_steady_frame_rate_controller.available()) {
		// 	spdlog::info("[core] High Resolution Waitable Timer available, enable SteadyFrameRateController");
		// 	m_p_frame_rate_controller = &m_steady_frame_rate_controller;
		// }
		// else {
		// 	m_p_frame_rate_controller = &m_frame_rate_controller;
		// }
		// get_system_memory_status();
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
		if (!Graphics::Window_SDL::create(~m_window))
			throw std::runtime_error("Graphics::Window_SDL::create");
		m_window->implSetApplicationModel(this);
		if (!Graphics::Device_OpenGL::create(~m_device))
			throw std::runtime_error("Graphics::Device_OpenGL::create");
		if (!Graphics::SwapChain_OpenGL::create(*m_window, *m_device, ~m_swapchain))
			throw std::runtime_error("Graphics::SwapChain_OpenGL::create");
		if (!Graphics::Renderer_OpenGL::create(*m_device, ~m_renderer))
			throw std::runtime_error("Graphics::Renderer_OpenGL::create");
		if (!Audio::Device_SDL::create(~m_audiosys))
			throw std::runtime_error("Audio::Device_SDL::create");
		// m_frame_query_list.reserve(2);
		// for (int i = 0; i < 2; i += 1) {
		// 	m_frame_query_list.emplace_back(m_device.get());
		// }
	}
	ApplicationModel_SDL::~ApplicationModel_SDL()
	{
		std::ignore = 0;
	}

	bool IApplicationModel::create(IApplicationEventListener* p_app, IApplicationModel** pp_model)
	{
		try
		{
			*pp_model = new ApplicationModel_SDL(p_app);
			return true;
		}
		catch (...)
		{
			*pp_model = nullptr;
			return false;
		}
	}
}
