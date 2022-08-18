#pragma once


#include "Camera.h"
#include "InputHandler.h"

namespace fge
{
	class CameraDrag final : public Camera
	{
	public:
		CameraDrag() : Camera(), m_dragPos(0, 0) { }
		~CameraDrag() = default;

		virtual void Update(const InputHandler& input_handler, const sf::RenderWindow& window) override;

	private:
		sf::Vector2f m_dragPos;
	};
}