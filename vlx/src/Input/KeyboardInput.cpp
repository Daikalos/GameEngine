#include <Velox/Input/KeyboardInput.h>

using namespace vlx;

void KeyboardInput::Update(const Time& time, const bool focus)
{
	for (uint32_t i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		bool& prev_state = m_previous_state[i];
		bool& curr_state = m_current_state[i];
		float& held_time = m_held_time[i];

		prev_state = curr_state;
		curr_state = focus && m_enabled && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));

		held_time = curr_state ?
			held_time + (held_time < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
	}
}

void KeyboardInput::HandleEvent(const sf::Event& event)
{

}

void vlx::KeyboardInput::ExecuteFuncs()
{
	for (auto rit = m_btn_funcs.rbegin(); rit != m_btn_funcs.rend(); ++rit)
	{
		const sf::Keyboard::Key& key = rit->first;
		const InputHandler::ButtonFuncs& button_funcs = rit->second;

		for (ButtonEvent event = BE_Begin; event != BE_End; ++event)
		{
			bool triggered = false;

			switch (event)
			{
			case BE_Pressed:	triggered = Pressed(key);	break;
			case BE_Released:	triggered = Released(key);	break;
			case BE_Held:		triggered = Held(key);		break;
			}

			if (!triggered)
				continue;

			for (const auto& func : button_funcs[event])
			{
				func();
			}
		}
	}
}

[[nodiscard]] bool KeyboardInput::Held(const sf::Keyboard::Key key) const
{
	return m_current_state[key] && m_held_time[key] >= m_held_threshold;
}
[[nodiscard]] bool KeyboardInput::Pressed(const sf::Keyboard::Key key) const
{
	return m_current_state[key] && !m_previous_state[key];
}
[[nodiscard]] bool KeyboardInput::Released(const sf::Keyboard::Key key) const
{
	return !m_current_state[key] && m_previous_state[key];
}