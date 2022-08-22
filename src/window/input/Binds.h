#pragma once

#include <stdint.h>

namespace fge
{
	////////////////////////////////////////////////////////////
	// Contains a set of unique binds used to set the binds
	// in the InputHandler, e.g., Attack is LMB
	////////////////////////////////////////////////////////////
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
}