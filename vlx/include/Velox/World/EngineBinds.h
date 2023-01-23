#pragma once

#include <stdint.h>

namespace vlx
{
	namespace ebn
	{
		enum class Button : uint32_t
		{
			GUIButton,

			Count
		};

		enum class Key : uint32_t
		{
			Up,
			Left,
			Right,
			Down,

			Count
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

			Count
		};
		enum class XboxAxis : uint32_t
		{
			LX,
			LY,

			Count
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

			Count
		};
		enum class PlaystationAxis : uint32_t
		{

		};
	}
}