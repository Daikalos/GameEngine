#pragma once

#include <unordered_map>

#include <Velox/Config.hpp>

#include "InputHandler.h"
#include "Binds.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Handles all of the keyboard input, only supports one
	// keyboard at a time
	////////////////////////////////////////////////////////////
	class KeyboardInput final : public InputHandler
	{
	private:
		template<Enum Bind>
		using KeyboardBinds = Binds<Bind, sf::Keyboard::Key>;

	public:
		VELOX_API void Update(const Time& time, const bool focus) override;
		VELOX_API void HandleEvent(const sf::Event& event) override;

	public:
		VELOX_API [[nodiscard]] bool Held(const sf::Keyboard::Key key) const;
		VELOX_API [[nodiscard]] bool Pressed(const sf::Keyboard::Key key) const;
		VELOX_API [[nodiscard]] bool Released(const sf::Keyboard::Key key) const;

		template<Enum Bind>
		[[nodiscard]] bool Held(const Bind name) const;
		template<Enum Bind>
		[[nodiscard]] bool Pressed(const Bind name) const;
		template<Enum Bind>
		[[nodiscard]] bool Released(const Bind name) const;

		template<Enum Bind>
		[[nodiscard]] KeyboardBinds<Bind>& Get();
		template<Enum Bind>
		[[nodiscard]] const KeyboardBinds<Bind>& Get() const;

		////////////////////////////////////////////////////////////
		// Add the bind for later input, must be done before any
		// operations are performed using the bind
		////////////////////////////////////////////////////////////
		template<Enum Bind>
		void Add();

	private:
		bool	m_current_state		[sf::Keyboard::KeyCount] = {false};
		bool	m_previous_state	[sf::Keyboard::KeyCount] = {false};
		float	m_held_time			[sf::Keyboard::KeyCount] = {0.0f};

		std::unordered_map<std::type_index, BindsBase::Ptr> m_binds;
	};

	template<Enum Bind>
	[[nodiscard]] inline bool KeyboardInput::Held(const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Held(binds.At(name));
	}
	template<Enum Bind>
	[[nodiscard]] inline bool KeyboardInput::Pressed(const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Pressed(binds.At(name));
	}
	template<Enum Bind>
	[[nodiscard]] inline bool KeyboardInput::Released(const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Released(binds.At(name));
	}

	template<Enum Bind>
	[[nodiscard]] inline KeyboardInput::KeyboardBinds<Bind>& KeyboardInput::Get()
	{
		return *static_cast<KeyboardBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
	}
	template<Enum Bind>
	[[nodiscard]] inline const KeyboardInput::KeyboardBinds<Bind>& KeyboardInput::Get() const
	{
		return const_cast<KeyboardInput*>(this)->Get<Bind>();
	}

	template<Enum Bind>
	inline void KeyboardInput::Add()
	{
		m_binds[typeid(Bind)] = std::make_unique<KeyboardBinds<Bind>>();
	}
}