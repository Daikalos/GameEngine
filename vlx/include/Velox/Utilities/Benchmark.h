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

namespace vlx::bm
{
	// TODO: MEASURE CPU, GPU, AND MEMORY USAGE WHILE ACTIVE

	static constexpr long double Kilobyte(const long double bytes) { return bytes / 1024.0L; }
	static constexpr long double Megabyte(const long double bytes) { return Kilobyte(bytes) / 1024.0L; }
	static constexpr long double Gigabyte(const long double bytes) { return Megabyte(bytes) / 1024.0L; }

	class MeasureSystem
	{
	public:
		using Ptr = std::unique_ptr<MeasureSystem>;

	public:
		MeasureSystem(const std::string_view message) 
			: m_message(message)
		{ 
			m_active = true;
			m_thread = std::thread(&MeasureSystem::Loop, this);
		}

		~MeasureSystem()
		{
			m_active = false;
			m_thread.join();
		}

		void Loop()
		{
			PROCESS_MEMORY_COUNTERS_EX pmc{};
			GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

			const long double initial_ram = Megabyte((long double)pmc.WorkingSetSize);
			const long double initial_cpu = GetCPU();

			long double total_ram_size	= 0.0;
			long double total_cpu_size	= 0.0;

			long double ram_average		= 0.0;
			long double ram_usage		= 0.0;
			long double ram_curr		= 0.0;
			long double ram_diff		= 0.0;

			long double cpu_average		= 0.0;
			long double cpu_usage		= 0.0;

			std::size_t total_ram_samples = 0;
			std::size_t total_cpu_samples = 0;

			SYSTEM_INFO sys_info;
			GetSystemInfo(&sys_info);
			m_num_processors = sys_info.dwNumberOfProcessors;

			sf::Clock time_begin;

			while (m_active)
			{
				GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

				const long double ram = Megabyte((long double)pmc.WorkingSetSize);
				const long double cpu = GetCPU();

				total_ram_size += ram; // we measure in megabytes
				total_cpu_size += cpu;

				if (ram > LDBL_EPSILON)
					++total_ram_samples;

				if (cpu > LDBL_EPSILON)
					++total_cpu_samples;
			}

			sf::Time elapsed = time_begin.getElapsedTime();

			auto microseconds = elapsed.asMicroseconds();
			auto milliseconds = elapsed.asMilliseconds();

			ram_average = total_ram_samples != 0 ? total_ram_size / total_ram_samples : 0.0;
			ram_usage = ram_average - initial_ram;

			cpu_average = total_cpu_samples != 0 ? total_cpu_size / total_cpu_samples : 0.0;
			cpu_usage = cpu_average - initial_cpu;

			GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
			ram_curr = Megabyte((long double)pmc.WorkingSetSize);
			ram_diff = ram_curr - initial_ram;

			// -- print relevant information --

			const std::function<std::string(long double, int)> print = [](const long double val, const int places) -> std::string
				{ return su::RemoveTrailingZeroes(std::to_string(au::SetPrecision(val, places))); };

			std::puts(m_message.data());
			std::puts("");

			std::puts("--- BENCHMARK BEGIN ---");
			std::puts("");
			std::puts(std::string("INITIAL RAM:	" + print(initial_ram, 2) + "MB").c_str());
			std::puts(std::string("INITIAL CPU:	" + print(initial_cpu, 2) + "%").c_str());
			std::puts("");

			std::puts("--- BENCHMARK END ---");
			std::puts("");
			std::puts(std::string("RAM TOTAL:	"	+ print(ram_average, 2)	+ "MB").c_str());
			std::puts(std::string("RAM USED:	"	+ print(ram_usage, 2)	+ "MB").c_str());
			std::puts(std::string("RAM CURR:	"	+ print(ram_curr, 2)	+ "MB").c_str());
			std::puts(std::string("RAM DIFF:	"	+ print(ram_diff, 2)	+ "MB").c_str());
			std::puts(std::string("CPU TOTAL:	" + print(cpu_average, 2)	+ "%").c_str());
			std::puts(std::string("CPU USED:	"	+ print(cpu_usage, 2)	+ "%").c_str());
			std::puts("");

			std::puts("TOTAL RUN TIME");
			std::cout << microseconds << "ns\n";
			std::cout << milliseconds << "ms\n";
			std::cout << "\n";

			std::puts("---------------------");
		}

		long double GetCPU()
		{
			FILETIME file_time, file_sys, file_user;
			ULARGE_INTEGER now, sys, user;
			long double percent;

			GetSystemTimeAsFileTime(&file_time);
			memcpy(&now, &file_time, sizeof(FILETIME));

			GetProcessTimes(GetCurrentProcess(), &file_time, &file_time, &file_sys, &file_user);
			memcpy(&sys, &file_sys, sizeof(FILETIME));
			memcpy(&user, &file_user, sizeof(FILETIME));

			percent = ((long double)sys.QuadPart - (long double)m_last_sys_cpu.QuadPart) + ((long double)user.QuadPart - (long double)m_last_user_cpu.QuadPart);
			percent /= ((long double)now.QuadPart - (long double)m_last_cpu.QuadPart);
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

		std::thread								m_thread;
		bool									m_active{false};

		ULARGE_INTEGER							m_last_cpu;
		ULARGE_INTEGER							m_last_sys_cpu;
		ULARGE_INTEGER							m_last_user_cpu;
		int										m_num_processors;
	};

	static std::queue<MeasureSystem::Ptr> queue;
	static std::mutex mutex;

	static void Begin(const std::string_view message = "BENCHMARK PROFILE")
	{
		std::lock_guard<std::mutex> lock(mutex);

		queue.push(std::make_unique<MeasureSystem>(message));
	}

	static void End()
	{
		std::lock_guard<std::mutex> lock(mutex);

		if (queue.empty())
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