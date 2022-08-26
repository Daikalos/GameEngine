#pragma once

#include <Velox/Utilities.hpp>

#include <unordered_map>

#include "MouseInput.hpp"

namespace vlx
{
	template<Enum B>
	class MouseInputBindable final : public MouseInput
	{
	public:
		using MouseInput::Held;
		using MouseInput::Pressed;
		using MouseInput::Released;

		bool Held(const B& button) const
		{
			return Held(m_button_bindings.at(button));
		}
		bool Pressed(const B& button) const
		{
			return Pressed(m_button_bindings.at(button));
		}
		bool Released(const B& button) const
		{
			return Released(m_button_bindings.at(button));
		}

		void SetBinding(const B& name, const sf::Mouse::Button& button)
		{
			m_button_bindings[name] = button;
		}
		void RemoveBinding(const B& name)
		{
			m_button_bindings.erase(name);
		}

	private:
		std::unordered_map<B, sf::Mouse::Button> m_button_bindings;	// bindings for buttons
	};
}