#pragma once

#include <Velox/ECS/IComponent.h>

namespace vlx::gui
{
	enum class AnchorPoint
	{
		TopLeft,
		TopRight,
		BotLeft,
		BotRight,
		Middle
	};

	struct Anchor : public IComponent
	{
		AnchorPoint anchor { AnchorPoint::TopLeft }; // top left is default
	};
}