#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>

#include <SFML/Window/Event.hpp>

#include <Velox/Utility/NonCopyable.h>
#include <Velox/System/Time.h>
#include <Velox/Config.hpp>

#include "Binds.hpp"

namespace vlx
{
	///	Base class for Keyboard, Mouse, and Joystick 
	/// 
	class VELOX_API InputHandler : private NonCopyable
	{
	public:
		InputHandler() = default;
		virtual ~InputHandler() = default;

	public:
		/// Enable or disable the InputHandler, will cause all input to return false or 0.0f.
		/// 
		void SetEnabled(bool flag) noexcept;

		///	Determine the threshold before the button/key is considered held
		/// 
		void SetHeldThreshold(float value) noexcept;

	public:
		///	Always put Update before HandleEvent for input work correctly
		/// 
		virtual void Update(const Time& time, bool focus = true) = 0;
		virtual void HandleEvent(const sf::Event& event) = 0;

	protected:
		bool	m_enabled			{true};	// is always enabled initially
		float	m_held_threshold	{0.1f}; // threshold before button/key is considered held
	};
}

