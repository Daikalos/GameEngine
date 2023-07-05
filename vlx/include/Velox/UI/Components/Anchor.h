#pragma once

namespace vlx::ui
{
	enum class AnchorPoint
	{
		TopLeft,
		TopRight,
		MidLeft,
		Middle,
		MidRight,
		BotLeft,
		BotRight
	};

	struct Anchor
	{
		AnchorPoint anchor { AnchorPoint::TopLeft }; // top left is default
	};
}