#pragma once

#include <concepts>

namespace vlx
{
	class SpriteBatch;
	class Mat4f;

	template<class T>
	concept IsBatchable = requires(const T& batchable, SpriteBatch& sprite_batch, const Mat4f& transform, float depth)
	{
		{ batchable.BatchImpl(sprite_batch, transform, depth) } -> std::same_as<void>;
	};

	template<class T>
	struct Batchable
	{
		void Batch(SpriteBatch& sprite_batch, const Mat4f& transform, float depth) const
		{
			static_assert(IsBatchable<T>, "T has not implemented BatchImpl");
			static_cast<const T*>(this)->BatchImpl(sprite_batch, transform, depth);
		}
	};
}