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
	/// <summary>
	///		Main class for Keyboard, Mouse, and Joystick that	
	///		have been expanded for extended usage, e.g., binds
	/// </summary>
	class VELOX_API InputHandler : private NonCopyable
	{
	public:
		using Ptr			= std::unique_ptr<InputHandler>;
		using ControlsMap	= std::unordered_map<std::type_index, Ptr>;

	public:
		InputHandler() = default;
		virtual ~InputHandler() = default;

	public:
		/// <summary>
		///		Enables or disables the input controller, e.g., causes all input 
		///		from the keyboard to return false.
		/// </summary>
		void SetEnabled(const bool flag) noexcept;

		/// <summary>
		///		Set the threshold before the button/key is considered held
		/// </summary>
		void SetHeldThreshold(const float value) noexcept;

	public:
		/// <summary>
		///		Always put Update before HandleEvent
		/// </summary>
		virtual void Update(const Time& time, const bool focus = true) = 0;
		virtual void HandleEvent(const sf::Event& event) = 0;

	protected:
		bool	m_enabled			{true};	// is always enabled initially
		float	m_held_threshold	{0.1f}; // threshold before button/key is considered held
	};
}

