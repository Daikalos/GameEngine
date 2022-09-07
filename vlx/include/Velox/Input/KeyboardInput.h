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

		using BtnFuncs = std::pair<sf::Keyboard::Key, ButtonFuncs>;

	public:
		VELOX_API void Update(const Time& time, const bool focus) override;
		VELOX_API void HandleEvent(const sf::Event& event) override;
		VELOX_API void ExecuteFuncs() override;

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
		[[nodiscard]] KeyboardBinds<Bind>& GetBindMap();
		template<Enum Bind>
		[[nodiscard]] const KeyboardBinds<Bind>& GetBindMap() const;

		////////////////////////////////////////////////////////////
		// Add the bind for later input, must be done before any
		// operations are performed using the bind
		////////////////////////////////////////////////////////////
		template<Enum Bind>
		void AddBindMap();

		template<typename Func>
		void AddFunc(sf::Keyboard::Key key, ButtonEvent event, Func&& func);

		template<Enum Bind, typename Func>
		void AddFunc(Bind key, ButtonEvent event, Func&& func);

	private:
		bool	m_current_state		[sf::Keyboard::KeyCount] = {false};
		bool	m_previous_state	[sf::Keyboard::KeyCount] = {false};
		float	m_held_time			[sf::Keyboard::KeyCount] = {0.0f};

		std::unordered_map<std::type_index, BindsBase::Ptr> m_binds;

		std::vector<BtnFuncs> m_btn_funcs;
	};

	template<Enum Bind>
	[[nodiscard]] inline bool KeyboardInput::Held(const Bind name) const
	{
		const auto& binds = GetBindMap<Bind>();
		return binds.GetEnabled() && Held(binds.At(name));
	}
	template<Enum Bind>
	[[nodiscard]] inline bool KeyboardInput::Pressed(const Bind name) const
	{
		const auto& binds = GetBindMap<Bind>();
		return binds.GetEnabled() && Pressed(binds.At(name));
	}
	template<Enum Bind>
	[[nodiscard]] inline bool KeyboardInput::Released(const Bind name) const
	{
		const auto& binds = GetBindMap<Bind>();
		return binds.GetEnabled() && Released(binds.At(name));
	}

	template<Enum Bind>
	[[nodiscard]] inline KeyboardInput::KeyboardBinds<Bind>& KeyboardInput::GetBindMap()
	{
		return *static_cast<KeyboardBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
	}
	template<Enum Bind>
	[[nodiscard]] inline const KeyboardInput::KeyboardBinds<Bind>& KeyboardInput::GetBindMap() const
	{
		return const_cast<KeyboardInput*>(this)->Get<Bind>();
	}

	template<Enum Bind>
	inline void KeyboardInput::AddBindMap()
	{
		m_binds[typeid(Bind)] = std::make_unique<KeyboardBinds<Bind>>();
	}

	template<typename Func>
	inline void KeyboardInput::AddFunc(sf::Keyboard::Key key, ButtonEvent event, Func&& func)
	{
		m_btn_funcs.push_back(std::make_pair(key, ButtonFuncs()));
		m_btn_funcs.back().second.at(event).emplace_back(std::forward<Func>(func));
	}

	template<Enum Bind, typename Func>
	inline void KeyboardInput::AddFunc(Bind key, ButtonEvent event, Func&& func)
	{
		const auto& binds = GetBindMap<Bind>();
		AddFunc(binds.At(key), event, std::forward<Func>(func));
	}
}