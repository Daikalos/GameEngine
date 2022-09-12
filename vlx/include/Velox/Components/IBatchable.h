#pragma once

#include <Velox/Graphics/SpriteBatch.h>

namespace vlx
{
	struct IBatchable
	{
		virtual ~IBatchable() = default;
		virtual void Batch(SpriteBatch& sprite_batch, float depth = 0.0f) const = 0;
	};
}