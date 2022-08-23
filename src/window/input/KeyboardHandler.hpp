#pragma once

#include <unordered_map>

#include "InputHandler.hpp"

namespace fge
{
	template<class K, typename std::enable_if_t<std::is_enum_v<K>, bool> = true>
	class KeyboardHandler : public InputHandler
	{
	public:
		void Update(const Time& time, bool focus) override
		{
			for (uint32_t i = 0; i < sf::Keyboard::KeyCount; ++i)
			{
				m_previous_key_state[i] = m_current_key_state[i];
				m_current_key_state[i] = focus && m_enabled && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));

				m_key_held_timer[i] = m_current_key_state[i] ?
					m_key_held_timer[i] + (m_key_held_timer[i] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
			}
		}
		void HandleEvent(const sf::Event& event) override
		{

		}

	public:
		bool GetHeld(const sf::Keyboard::Key& key) const
		{
			return m_current_key_state[key] && m_key_held_timer[key] >= m_held_threshold;
		}
		bool GetPressed(const sf::Keyboard::Key& key) const
		{
			return m_current_key_state[key] && !m_previous_key_state[key];
		}
		bool GetReleased(const sf::Keyboard::Key& key) const
		{
			return !GetPressed(key);
		}

		bool GetHeld(const K& key) const
		{
			if (!m_key_bindings.contains(key))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(key)) + "] does not exist");

			return GetHeld(m_key_bindings.at(key));
		}
		bool GetPressed(const K& key) const
		{
			if (!m_key_bindings.contains(key))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(key)) + "] does not exist");

			return GetPressed(m_key_bindings.at(key));
		}
		bool GetReleased(const K& key) const
		{
			if (!m_key_bindings.contains(key))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(key)) + "] does not exist");

			return GetReleased(m_key_bindings.at(key));
		}

		void SetBinding(const K& name, const sf::Keyboard::Key& key)
		{
			m_key_bindings[name] = key;
		}
		void RemoveBinding(const K& name)
		{
			if (!m_key_bindings.contains(name))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(name)) + "] does not exist");

			m_key_bindings.erase(name);
		}

	private:
		bool	m_current_key_state		[sf::Keyboard::KeyCount]	= {false};
		bool	m_previous_key_state	[sf::Keyboard::KeyCount]	= {false};
		float	m_key_held_timer		[sf::Keyboard::KeyCount]	= {0.0f};

		std::unordered_map<K, sf::Keyboard::Key> m_key_bindings; // bindings for keys
	};
}