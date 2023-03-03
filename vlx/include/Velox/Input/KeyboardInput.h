#pragma once

#include <unordered_map>

#include <Velox/Config.hpp>

#include "InputHandler.h"
#include "Binds.hpp"

namespace vlx
{
	/// Handles all of the keyboard input, only supports one keyboard 
	///
	class KeyboardInput final : public InputHandler
	{
	private:
		template<Enum Bind>
		using KeyboardBinds = Binds<Bind, sf::Keyboard::Key>;

	public:
		VELOX_API NODISC bool Held(const sf::Keyboard::Key key) const;
		VELOX_API NODISC bool Pressed(const sf::Keyboard::Key key) const;
		VELOX_API NODISC bool Released(const sf::Keyboard::Key key) const;

		template<Enum Bind>
		NODISC bool Held(const Bind name) const;
		template<Enum Bind>
		NODISC bool Pressed(const Bind name) const;
		template<Enum Bind>
		NODISC bool Released(const Bind name) const;

		template<Enum Bind>
		NODISC KeyboardBinds<Bind>& GetMap();
		template<Enum Bind>
		NODISC const KeyboardBinds<Bind>& GetMap() const;

		///	Add the bind for later input, must be done before any operations are performed using the bind
		/// 
		template<Enum Bind>
		void AddMap();

	public:
		VELOX_API void Update(const Time& time, const bool focus) override;
		VELOX_API void HandleEvent(const sf::Event& event) override;

	private:
		bool	m_current_state		[sf::Keyboard::KeyCount] = {false};
		bool	m_previous_state	[sf::Keyboard::KeyCount] = {false};
		float	m_held_time			[sf::Keyboard::KeyCount] = {0.0f};

		std::unordered_map<std::type_index, IBinds::Ptr> m_binds;
	};

	template<Enum Bind>
	inline bool KeyboardInput::Held(const Bind name) const
	{
		const auto& binds = GetMap<Bind>();
		return binds.GetEnabled() && Held(binds.At(name));
	}
	template<Enum Bind>
	inline bool KeyboardInput::Pressed(const Bind name) const
	{
		const auto& binds = GetMap<Bind>();
		return binds.GetEnabled() && Pressed(binds.At(name));
	}
	template<Enum Bind>
	inline bool KeyboardInput::Released(const Bind name) const
	{
		const auto& binds = GetMap<Bind>();
		return binds.GetEnabled() && Released(binds.At(name));
	}

	template<Enum Bind>
	inline KeyboardInput::KeyboardBinds<Bind>& KeyboardInput::GetMap()
	{
		return *static_cast<KeyboardBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
	}
	template<Enum Bind>
	inline const KeyboardInput::KeyboardBinds<Bind>& KeyboardInput::GetMap() const
	{
		return const_cast<KeyboardInput*>(this)->GetMap<Bind>();
	}

	template<Enum Bind>
	inline void KeyboardInput::AddMap()
	{	
		m_binds[typeid(Bind)] = std::make_unique<KeyboardBinds<Bind>>();
	}
}