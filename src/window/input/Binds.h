#pragma once

namespace fge
{
	namespace Binds
	{
#if defined(KEYBOARDMOUSE_ENABLED) && KEYBOARDMOUSE_ENABLED
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
#endif

#if defined(JOYSTICK_ENABLED) && JOYSTICK_ENABLED
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
#endif
	}
}