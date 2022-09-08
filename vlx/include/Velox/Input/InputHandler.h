#pragma once

#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <typeindex>
#include <memory>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "Binds.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Main class for Keyboard, Mouse, and Joystick that
	// have been expanded for extended usage, e.g., binds
	////////////////////////////////////////////////////////////
	class VELOX_API InputHandler : private NonCopyable
	{
	public:
		using Ptr			= std::unique_ptr<InputHandler>;
		using ControlMap	= std::unordered_map<std::type_index, Ptr>;

	public:
		InputHandler() = default;
		virtual ~InputHandler() = default;

	public:
		////////////////////////////////////////////////////////////
		// Set if the input is currently enabled. Disables results
		// in all values returning false or 0.
		////////////////////////////////////////////////////////////
		void SetEnabled(const bool flag) noexcept;

		////////////////////////////////////////////////////////////
		// Set the threshold before which the button/key is 
		// considered held
		////////////////////////////////////////////////////////////
		void SetHeldThreshold(const float value) noexcept;

	public:
		////////////////////////////////////////////////////////////
		// Always put before handle event
		////////////////////////////////////////////////////////////
		virtual void Update(const Time& time, const bool focus = true) = 0;
		virtual void HandleEvent(const sf::Event& event) = 0;

	protected:
		bool	m_enabled		{true};	// is always enabled initially
		float	m_held_threshold{0.1f}; // threshold before button/key is considered held
	};
}

