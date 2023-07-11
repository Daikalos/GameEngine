#pragma once

#include <numeric>
#include <vector>
#include <array>
#include <span>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <Velox/System/Mat4f.hpp>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "Batchable.h"

namespace vlx
{
	enum class BatchMode : uint8
	{
		Deferred,
		BackToFront,
		FrontToBack,
		Texture,
	};

	/// SpriteBatch used to combine similar textures to reduce number of draw-calls. 
	///
	class VELOX_API SpriteBatch final : public sf::Drawable
	{
	private:
		using VertexSpan = std::span<const sf::Vertex>;
		using IndicesSpan = std::span<const uint64>;

		static constexpr auto TRIANGLE_COUNT = 3;

		struct VELOX_API Triangle
		{
			Triangle(
				sf::Vertex&& v0, 
				sf::Vertex&& v1, 
				sf::Vertex&& v2, 
				const sf::Texture* t, 
				const sf::Shader* s, 
				float d);

			std::array<sf::Vertex, TRIANGLE_COUNT> vertices;

			const sf::Texture*	texture	{nullptr};
			const sf::Shader*	shader	{nullptr};
			float				depth	{0.0f};
		};

		struct BatchInfo
		{
			const sf::Texture*	texture	{nullptr};
			const sf::Shader*	shader	{nullptr};
			std::size_t			count	{0};
		};

	public:
		void SetBatchMode(BatchMode batch_mode);
		void Reserve(std::size_t size);
		void Shrink();

		void AddTriangle(
			const Mat4f& transform, 
			const sf::Vertex& v0, 
			const sf::Vertex& v1, 
			const sf::Vertex& v2, 
			const sf::Texture* texture, 
			const sf::Shader* shader, 
			float depth = 0.0f);

		template<IsBatchable T>
		void Batch(
			const Batchable<T>& batchable,
			const Mat4f& transform,
			float depth = 0.0f);

		void Batch(
			const Mat4f& transform,
			VertexSpan vertices,
			sf::PrimitiveType type, 
			const sf::Texture* texture, 
			const sf::Shader* shader, 
			float depth = 0.0f);

		void Batch(
			const Mat4f& transform,
			VertexSpan vertices,
			IndicesSpan indices,
			const sf::Texture* texture,
			const sf::Shader* shader,
			float depth = 0.0f);

		void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

		void Clear();

	private:
		void SortTriangles() const;
		void CreateBatches() const;

	private:
		std::vector<Triangle>			m_triangles;

		mutable std::vector<BatchInfo>	m_batches;
		mutable std::vector<uint64>		m_indices;
		mutable sf::VertexArray			m_vertices;

		BatchMode						m_batch_mode		{BatchMode::Deferred};
		mutable bool					m_update_required	{true};
	};

	template<IsBatchable T>
	inline void SpriteBatch::Batch(const Batchable<T>& batchable, const Mat4f& transform, float depth)
	{
		batchable.Batch(*this, transform, depth);
	}
}

