#pragma once

#include <functional>
#include <chrono>
#include <iostream>
#include <thread>

namespace fge
{
	// TODO: MEASURE CPU, GPU, AND MEMORY USAGE WHILE ACTIVE

	namespace bm
	{
#if _DEBUG
		static thread_local std::chrono::steady_clock::time_point time_begin;

		static thread_local std::thread thread;
		static thread_local bool active{false};

		static void Loop(bool* active)
		{
			while (*active)
			{
				std::cout << "test";
			}
		}

		static void Begin()
		{
			if (active)
				std::runtime_error("a benchmark is already running");

			active = true;
			thread = std::thread(std::bind(Loop, &active));

			time_begin = std::chrono::high_resolution_clock::now();
		}

		static void End()
		{
			if (!active)
				std::runtime_error("no benchmark is running");

			auto time_end = std::chrono::high_resolution_clock::now();

			active = false;
			thread.join();

			auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_begin);
			std::chrono::duration<double, std::milli> ms_double = time_end - time_begin;

			std::cout << ms_int << "\n";
			std::cout << ms_double << "\n";
			std::cout << "\n";
		}

		template<class F, typename... Args>
		static void Run(F&& func, Args&&... args)
		{
			Begin();

			func(std::forward<Args>(args)...);

			End();
		}
#endif
	}
}