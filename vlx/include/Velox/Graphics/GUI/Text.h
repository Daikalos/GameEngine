#pragma once

#include <SFML/Graphics.hpp>

namespace vlx::gui
{
	class Text
	{
	public:
		void SetFont();

	private:
		sf::Text m_text;
	};
}

