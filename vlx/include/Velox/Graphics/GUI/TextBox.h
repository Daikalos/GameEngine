#pragma once

#include "GUIComponent.h"

namespace vlx::gui
{
	class TextBox : public GUIComponent
	{
	public:
		//TestBox(const Vector2Type& size);
		//TestBox(const SizeType& width, const SizeType& height);

		constexpr bool IsSelectable() const noexcept override;

	private:


	};
}