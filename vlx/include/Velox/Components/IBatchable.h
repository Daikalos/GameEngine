#pragma once

namespace vlx
{
	class SpriteBatch;
	class Transform;

	struct VELOX_API IBatchable
	{
		virtual ~IBatchable() = default;
		virtual void Batch(SpriteBatch& sprite_batch, const Transform& transform, float depth = 0.0f) const = 0;
	};
}