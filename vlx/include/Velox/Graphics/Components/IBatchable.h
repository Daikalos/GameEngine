#pragma once

namespace sf
{
	class Transform;
};

namespace vlx
{
	class SpriteBatch;

	struct VELOX_API IBatchable
	{
		virtual ~IBatchable() = default;
		virtual void Batch(SpriteBatch& sprite_batch, const sf::Transform& transform, float depth = 0.0f) const = 0;
	};
}