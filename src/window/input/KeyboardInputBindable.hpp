#pragma once

#include <unordered_map>
#include <FGE/Concepts.hpp>

#include "KeyboardInput.hpp"

namespace vlx
{
	template<Enum K>
	class KeyboardInputBindable : public KeyboardInput
	{
	public:
		using KeyboardInput::Held; // make them visible for overloading
		using KeyboardInput::Pressed;
		using KeyboardInput::Released;

		bool Held(const K& key) const
		{
			return Held(m_key_bindings.at(key));
		}
		bool Pressed(const K& key) const
		{
			return Pressed(m_key_bindings.at(key));
		}
		bool Released(const K& key) const
		{
			return Released(m_key_bindings.at(key));
		}

		void SetBinding(const K& name, const sf::Keyboard::Key& key)
		{
			m_key_bindings[name] = key;
		}
		void RemoveBinding(const K& name)
		{
			m_key_bindings.erase(name);
		}

	private:
		std::unordered_map<K, sf::Keyboard::Key> m_key_bindings; // bindings for keys
	};
}