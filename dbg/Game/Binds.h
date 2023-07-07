#pragma once

#include <stdint.h>

#include <Velox/Input.hpp>

namespace bn
{
	enum class Button : uint32_t
	{
		Drag,

		ButtonBindingCount
	};

	enum class Key : uint32_t
	{
		Up,
		Left,
		Right,
		Down,

		KeyBindingCount
	};

	enum class XboxButton : uint32_t
	{
		A,
		B,
		X,
		Y,
		LB,
		RB,
		Back,
		Start,
		L,
		R,

		XboxButtonCount
	};
	enum class XboxAxis : uint32_t
	{
		LX,
		LY,

		XboxAxisCount
	};

	enum class PlaystationButton : uint32_t // not checked
	{
		A,
		B,
		X,
		Y,
		LB,
		RB,
		Back,
		Start,
		L,
		R,

		Playstation
	};
	enum class PlaystationAxis : uint32_t
	{

	};
}

using GameKeyboard = vlx::KeyboardBindable<bn::Key>;
using GameMouse = vlx::MouseBindable<bn::Button>;