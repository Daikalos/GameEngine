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
		AnchorPoint Anchor { AnchorPoint::TopLeft }; // top left is default
	};
}