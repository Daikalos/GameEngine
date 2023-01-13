#pragma once

#include <SFML/Graphics.hpp>

#include "GUIComponent.h"

namespace vlx::gui
{
	/// <summary>
	///		For now, just standard SFML implementation of Text
	/// </summary>
	struct VELOX_API Text : public GUIComponent, public sf::Text
	{
		using GUIComponent::GUIComponent; // inherit constructor

		[[nodiscard]] constexpr bool IsSelectable() const noexcept override;
	};
}

