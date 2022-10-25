#pragma once

#include <Velox/Utilities.hpp>

namespace vlx
{
	/// <summary>
	/// The ComponentHandler is to ensure that the component pointers remains valid even after the internal data
	/// has been modified. This is to prevent having to write GetCompoonent everywhere all the time.
	/// </summary>
	template<IsComponent C>
	class ComponentHandler
	{

	private:
		C* m_component;
	};
}