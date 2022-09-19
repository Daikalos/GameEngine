#pragma once

#include <Velox/Components/Transform.h>

namespace vlx
{
	class SpriteBatch;

	struct VELOX_API IBatchable
	{
		virtual ~IBatchable() = default;
		virtual void Batch(SpriteBatch& sprite_batch, const Transform& transform, float depth = 0.0f) const = 0;
	};
}