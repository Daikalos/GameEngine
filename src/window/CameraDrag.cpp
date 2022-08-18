#include "CameraDrag.h"

using namespace fge;

void CameraDrag::Update(const InputHandler& input_handler, const sf::RenderWindow& window)
{
	if (input_handler.GetButtonPressed(Binds::Button::Drag))
		m_dragPos = GetMouseWorldPosition(window);
	if (input_handler.GetButtonHeld(Binds::Button::Drag))
		m_position += (m_dragPos - GetMouseWorldPosition(window));

	if (input_handler.GetScrollUp())
		m_scale += sf::Vector2f(0.1f, 0.1f);
	if (input_handler.GetScrollDown())
		m_scale -= sf::Vector2f(0.1f, 0.1f);

	SetPosition(m_position);
	SetScale(m_scale);
}
