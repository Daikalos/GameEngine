#pragma once

#include <unordered_map>

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>

#include <Velox/Config.hpp>

#include "InputHandler.h"
#include "Binds.hpp"

namespace vlx
{
	/// Handles all of the mouse input, only supports one mouse at a time. 
	///
	class MouseInput final : public InputHandler
	{
	private:
		template<Enum Bind>
		using MouseBinds = Binds<Bind, sf::Mouse::Button>;

	public:
		VELOX_API NODISC bool ScrollUp() const noexcept;
		VELOX_API NODISC bool ScrollDown() const noexcept;

		VELOX_API NODISC bool Held(const sf::Mouse::Button button) const;
		VELOX_API NODISC bool Pressed(const sf::Mouse::Button button) const;
		VELOX_API NODISC bool Released(const sf::Mouse::Button button) const;

	public:
		template<Enum Bind>
		NODISC bool Held(const Bind name) const;
		template<Enum Bind>
		NODISC bool Pressed(const Bind name) const;
		template<Enum Bind>
		NODISC bool Released(const Bind name) const;

		template<Enum Bind>
		NODISC MouseBinds<Bind>& GetMap();
		template<Enum Bind>
		NODISC const MouseBinds<Bind>& GetMap() const;

		///	Add the bind for later input, must be done before any operations are performed using the bind
		/// 
		template<Enum Bind>
		void AddMap();

	public:
		VELOX_API void Update(const Time& time, const bool focus) override;
		VELOX_API void HandleEvent(const sf::Event& event) override;

	private:
		float	m_scroll_delta		{0.0f};
		float	m_scroll_threshold	{0.01f}; // threshold before scroll is considered up/down

		bool	m_current_state		[sf::Mouse::ButtonCount] = {false};
		bool	m_previous_state	[sf::Mouse::ButtonCount] = {false};
		float	m_held_time			[sf::Mouse::ButtonCount] = {0.0f};

		std::unordered_map<std::type_index, IBinds::Ptr> m_binds;
	};

	template<Enum Bind>
	inline bool MouseInput::Held(const Bind name) const
	{
		const auto& binds = GetMap<Bind>();
		return binds.GetEnabled() && Held(binds.At(name));
	}
	template<Enum Bind>
	inline bool MouseInput::Pressed(const Bind name) const
	{
		const auto& binds = GetMap<Bind>();
		return binds.GetEnabled() && Pressed(binds.At(name));
	}
	template<Enum Bind>
	inline bool MouseInput::Released(const Bind name) const
	{
		const auto& binds = GetMap<Bind>();
		return binds.GetEnabled() && Released(binds.At(name));
	}

	template<Enum Bind>
	inline MouseInput::MouseBinds<Bind>& MouseInput::GetMap()
	{
		return *static_cast<MouseBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
	}
	template<Enum Bind>
	inline const MouseInput::MouseBinds<Bind>& MouseInput::GetMap() const
	{
		return const_cast<MouseInput*>(this)->GetMap<Bind>();
	}

	template<Enum Bind>
	inline void MouseInput::AddMap()
	{
		m_binds[typeid(Bind)] = std::make_unique<MouseBinds<Bind>>();
	}
}