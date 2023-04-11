#pragma once

#include <Velox/Config.hpp>

namespace vlx
{
	class SpriteBatch;
	class Mat4f;

	struct VELOX_API IBatchable
	{
		virtual ~IBatchable() = default;
		virtual void Batch(SpriteBatch& sprite_batch, const Mat4f& transform, float depth = 0.0f) const = 0;
	};
}