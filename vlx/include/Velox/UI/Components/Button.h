#pragma once

#include <Velox/System/Event.hpp>
#include <Velox/Config.hpp>

namespace vlx::ui
{
	///	The button contains events for certain behaviours, e.g., press, release, and hover. They are usually 
	/// called when the GUI component is given the respective inputs, but can be manually called if the user desires to.
	/// 
	class VELOX_API Button
	{
	public:
		void Click();
		void Press();
		void Release();
		void Enter();
		void Exit();

	private:
		bool m_call_pressed		{false};
		bool m_call_clicked		{false};
		bool m_call_released	{false};
		bool m_call_entered		{false};
		bool m_call_exited		{false};

		bool m_pressed	{false};
		bool m_entered	{false};

		friend class ButtonSystem;
	};

	struct VELOX_API ButtonClick
	{
		vlx::Event<> OnClick;
	};

	struct VELOX_API ButtonPress
	{
		vlx::Event<> OnPress;
	};

	struct VELOX_API ButtonRelease
	{
		vlx::Event<> OnRelease;
	};

	struct VELOX_API ButtonEnter
	{
		vlx::Event<> OnEnter;
	};

	struct VELOX_API ButtonExit
	{
		vlx::Event<> OnExit;
	};
}