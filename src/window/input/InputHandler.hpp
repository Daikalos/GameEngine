#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <typeindex>
#include <memory>

#include "../../utilities/NonCopyable.h"
#include "../../utilities/Time.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Main class for Keyboard, Mouse, and Joystick that
	// have been expanded for extended usage, e.g., binds
	////////////////////////////////////////////////////////////
	class InputHandler : NonCopyable
	{
	public:
		using Ptr		= std::unique_ptr<InputHandler>;
		using Controls	= std::unordered_map<std::type_index, Ptr>;

	public:
		InputHandler() {}
		virtual ~InputHandler() {};

		////////////////////////////////////////////////////////////
		// Always put before handle event
		////////////////////////////////////////////////////////////
		virtual void Update(const Time& time, bool focus = true) = 0;
		virtual void HandleEvent(const sf::Event& event) = 0;

	public:
		////////////////////////////////////////////////////////////
		// Set if the input is currently enabled. Disables results
		// in all values returning false or 0.
		////////////////////////////////////////////////////////////
		void SetEnabled(bool flag) noexcept
		{
			m_enabled = flag;
		}

		////////////////////////////////////////////////////////////
		// Set the threshold before which the button/key is 
		// considered held
		////////////////////////////////////////////////////////////
		void SetHeldThreshold(float value)
		{
			m_held_threshold = value;
		}

	protected:
		bool	m_enabled {true};		 // is always enabled initially
		float	m_held_threshold {0.1f}; // threshold before button/key is considered held
	};
}

