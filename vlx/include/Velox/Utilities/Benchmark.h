#pragma once

#include <memory>
#include <functional>
#include <chrono>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <string_view>
#include <iomanip>

#include <Windows.h>
#include <Psapi.h>
#include <Pdh.h>

#include "ArithmeticUtils.h"
#include "StringUtils.h"

namespace vlx
{
	// TODO: MEASURE CPU, GPU, AND MEMORY USAGE WHILE ACTIVE

	namespace bm
	{
		static constexpr long double kilobyte(const std::size_t bytes) { return bytes / 1024; }
		static constexpr long double megabyte(const std::size_t bytes) { return kilobyte(bytes) / 1024; }
		static constexpr long double gigabyte(const std::size_t bytes) { return megabyte(bytes) / 1024; }

		class MeasureSystem
		{
		public:
			using Ptr = std::unique_ptr<MeasureSystem>;

		public:
			MeasureSystem(const std::string_view message) :  
				m_message(message), m_time_begin(std::chrono::high_resolution_clock::now()), 
				m_thread(&MeasureSystem::Loop, this), m_active(true) 
			{ 
				SYSTEM_INFO sys_info;
				FILETIME file_time, file_sys, file_user;

				GetSystemInfo(&sys_info);
				m_num_processors = sys_info.dwNumberOfProcessors;

				GetSystemTimeAsFileTime(&file_time);
				memcpy(&m_last_cpu, &file_time, sizeof(FILETIME));

				m_self = GetCurrentProcess();

				GetProcessTimes(m_self, &file_time, &file_time, &file_sys, &file_user);
				memcpy(&m_last_sys_cpu, &file_sys, sizeof(FILETIME));
				memcpy(&m_last_user_cpu, &file_user, sizeof(FILETIME));
			}

			~MeasureSystem()
			{
				auto time_end = std::chrono::high_resolution_clock::now();

				m_active = false;
				m_thread.join();

				auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - m_time_begin);
				std::chrono::duration<double, std::milli> ms_double = time_end - m_time_begin;

				std::cout << "TOTAL RUN TIME" << "\n";
				std::cout << ms_int << "\n";
				std::cout << ms_double << "\n";
				std::cout << "\n";
			}

			void Loop()
			{
				PROCESS_MEMORY_COUNTERS_EX pmc;
				GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

				long double initial_ram = megabyte(pmc.WorkingSetSize);
				long double initial_cpu = GetCPU();

				long double total_ram_size = 0.0;
				long double total_cpu_size = 0.0;

				long double ram_average = 0.0;
				long double ram_usage = 0.0;

				long double cpu_average = 0.0;
				long double cpu_usage = 0.0;

				std::size_t total_ram_samples = 0;
				std::size_t total_cpu_samples = 0;

				while (m_active)
				{
					GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

					long double ram = pmc.WorkingSetSize;
					long double cpu = GetCPU();

					total_ram_size += megabyte(ram); // we measure in megabytes
					total_cpu_size += cpu;

					if (ram > LDBL_EPSILON)
						++total_ram_samples;

					if (cpu > LDBL_EPSILON)
						++total_cpu_samples;
				}

				ram_average = total_ram_samples != 0 ? total_ram_size / total_ram_samples : 0.0f;
				ram_usage = ram_average - initial_ram;

				cpu_average = total_cpu_samples != 0 ? total_cpu_size / total_cpu_samples : 0.0f;
				cpu_usage = cpu_average - initial_cpu;

				// -- print relevant information --

				std::puts(m_message.data());
				std::puts("");

				std::puts("BENCHMARK BEGIN");
				std::puts("");
				std::puts(std::string("INITIAL RAM: " + su::RemoveTrailingZeroes(std::to_string(au::SetPrecision(initial_ram, 3))) + "MB").c_str());
				std::puts(std::string("INITIAL CPU: " + su::RemoveTrailingZeroes(std::to_string(au::SetPrecision(initial_cpu, 3))) + "%").c_str());
				std::puts("");

				std::puts("BENCHMARK END");
				std::puts("");
				std::puts(std::string("RAM TOTAL: " + su::RemoveTrailingZeroes(std::to_string(au::SetPrecision(ram_average, 3)))	+ "MB").c_str());
				std::puts(std::string("RAM USED: "	+ su::RemoveTrailingZeroes(std::to_string(au::SetPrecision(ram_usage, 3)))		+ "MB").c_str());
				std::puts(std::string("CPU TOTAL: " + su::RemoveTrailingZeroes(std::to_string(au::SetPrecision(cpu_average, 3)))	+ "%").c_str());
				std::puts(std::string("CPU USED: "	+ su::RemoveTrailingZeroes(std::to_string(au::SetPrecision(cpu_usage, 3)))		+ "%").c_str());
				std::puts("");
			}

			long double GetCPU()
			{
				FILETIME file_time, file_sys, file_user;
				ULARGE_INTEGER now, sys, user;
				long double percent;

				GetSystemTimeAsFileTime(&file_time);
				memcpy(&now, &file_time, sizeof(FILETIME));

				GetProcessTimes(m_self, &file_time, &file_time, &file_sys, &file_user);
				memcpy(&sys, &file_sys, sizeof(FILETIME));
				memcpy(&user, &file_user, sizeof(FILETIME));

				percent = (sys.QuadPart - m_last_sys_cpu.QuadPart) + (user.QuadPart - m_last_user_cpu.QuadPart);

				percent /= (now.QuadPart - m_last_cpu.QuadPart);
				percent /= m_num_processors;

				m_last_cpu = now;
				m_last_user_cpu = user;
				m_last_sys_cpu = sys;

				if (isnan(percent) || isinf(percent))
					return 0.0;

				return percent;
			}

		private:
			const std::string_view					m_message;
			std::chrono::steady_clock::time_point	m_time_begin;

			std::thread								m_thread;
			bool									m_active;

			ULARGE_INTEGER							m_last_cpu;
			ULARGE_INTEGER							m_last_sys_cpu;
			ULARGE_INTEGER							m_last_user_cpu;
			int										m_num_processors;
			HANDLE									m_self;
		};

		static std::queue<MeasureSystem::Ptr> queue;
		static std::mutex mutex;

		static void Begin(const std::string_view message = "BENCHMARK PROFILE")
		{
			std::lock_guard<std::mutex> lock(mutex);

			queue.push(MeasureSystem::Ptr(new MeasureSystem(message)));
		}

		static void End()
		{
			std::lock_guard<std::mutex> lock(mutex);

			if (!queue.size())
				throw std::runtime_error("cant end something when there is no beginning");

			queue.pop();
		}

		template<class F, typename... Args>
		static void Run(F&& func, Args&&... args)
		{
			Begin();

			func(std::forward<Args>(args)...);

			End();
		}
	}
}