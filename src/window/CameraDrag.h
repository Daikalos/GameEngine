#pragma once

#include "CameraBehaviour.hpp"
#include "Camera.h"
#include "InputHandler.h"

namespace fge
{
	class CameraDrag final : public CameraBehaviour
	{
	public:
		CameraDrag(Camera* const camera, const InputHandler& input_handler, const sf::RenderWindow& window);

		virtual void Update(const InputHandler& input_handler, const sf::RenderWindow& window) override;

	private:
		sf::Vector2f m_dragPos;
	};
}