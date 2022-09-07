#pragma once

#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <typeindex>
#include <memory>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	enum ButtonEvent : std::uint16_t
	{
		BE_Begin	= 0,

		BE_Pressed	= 0,
		BE_Released = 1,
		BE_Held		= 2,

		BE_End		= 3
	};

	inline ButtonEvent& operator++(ButtonEvent& button_event)
	{
		button_event = static_cast<ButtonEvent>(static_cast<std::uint16_t>(button_event) + 1);
		return button_event;
	}

	////////////////////////////////////////////////////////////
	// Main class for Keyboard, Mouse, and Joystick that
	// have been expanded for extended usage, e.g., binds
	////////////////////////////////////////////////////////////
	class VELOX_API InputHandler : private NonCopyable
	{
	public:
		using Ptr			= std::unique_ptr<InputHandler>;
		using ControlMap	= std::unordered_map<std::type_index, Ptr>;

		using ButtonFuncs = std::array<std::vector<std::function<void()>>, BE_End>;

	public:
		InputHandler() = default;
		virtual ~InputHandler() = default;

		////////////////////////////////////////////////////////////
		// Always put before handle event
		////////////////////////////////////////////////////////////
		virtual void Update(const Time& time, const bool focus = true) = 0;
		virtual void HandleEvent(const sf::Event& event) = 0;

		virtual void ExecuteFuncs() = 0;

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

	protected:
		bool	m_enabled		{true};	// is always enabled initially
		float	m_held_threshold{0.1f}; // threshold before button/key is considered held
	};
}

