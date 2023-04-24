#pragma once

namespace vlx
{
	/// Denotes common properties for rendering
	///
	struct Renderable
	{
		bool IsActive	{true};
		bool IsStatic	{false};
		bool IsGUI		{false};
		bool IsVisible	{true};
	};
}