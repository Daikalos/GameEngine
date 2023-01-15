#pragma once

#include <SFML/Graphics.hpp>

#include "GUIComponent.h"

namespace vlx::gui
{
	/// <summary>
	///		For now, just standard SFML implementation of Text, will probably be reimplemented with a similiar
	///		structure to sf::Text with the exception of some inherited classes such as sf::Transformable
	/// </summary>
	struct VELOX_API Label : public GUIComponent, public sf::Text
	{
		using GUIComponent::GUIComponent; // inherit constructor

		[[nodiscard]] constexpr bool IsSelectable() const noexcept override;
	};
}

