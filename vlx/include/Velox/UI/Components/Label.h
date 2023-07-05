#pragma once

#include <SFML/Graphics/Text.hpp>

#include <Velox/Config.hpp>

#include "GUIComponent.h"

namespace vlx::ui
{
	///	For now, just standard SFML implementation of Text, will probably be reimplemented with a similiar
	///	structure to sf::Text with the exception of some inherited classes such as sf::Transformable
	/// 
	struct VELOX_API Label : public GUIComponent
	{
		using GUIComponent::GUIComponent; // inherit constructor

		constexpr bool IsSelectable() const noexcept override;
	};

	constexpr bool Label::IsSelectable() const noexcept
	{
		return false;
	}
}

