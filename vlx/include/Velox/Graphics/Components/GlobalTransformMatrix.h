#pragma once

#include <Velox/System/Mat4f.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	///	Global transform of an object, containing the combined transforms of all subsequent parents and self.
	///
	struct VELOX_API GlobalTransformMatrix
	{ 
		Mat4f matrix; // combined transform of all parents and this (global space)		
		//struct
		//{
		//	float pad0, pad1;
		//	bool update_inverse {true}; // we borrow some space from the matrix that is usually unused
		//};
	};
}