#pragma once

#include <numeric>
#include <unordered_set>

#include <SFML/Graphics.hpp>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>
#include <Velox/Components/IBatchable.h>

namespace vlx
{
	enum class BatchMode : std::uint8_t
	{
		Deferred,
		BackToFront,
		FrontToBack,
		Texture,
	};

	class VELOX_API SpriteBatch final : public sf::Drawable
	{
	private:
		using SizeType = std::uint32_t;

		struct VELOX_API Triangle
		{
			Triangle(sf::Vertex&& v0, sf::Vertex&& v1, sf::Vertex&& v2, const sf::Texture* t, const sf::Shader* s, const float d);

			sf::Vertex			vertices[TRIANGLE_COUNT];

			const sf::Texture*	texture	{nullptr};
			const sf::Shader*	shader	{nullptr};
			float				depth	{0.0f};
		};

		struct BatchInfo
		{
			const sf::Texture*	texture	{nullptr};
			const sf::Shader*	shader	{nullptr};
			SizeType			count	{0};
		};

	public:
		void SetBatchMode(const BatchMode batch_mode);
		void Reserve(const std::size_t size);
		void Shrink();

		void AddTriangle(
			const sf::Transform& transform, 
			const sf::Vertex& v0, 
			const sf::Vertex& v1, 
			const sf::Vertex& v2, 
			const sf::Texture* texture, 
			const sf::Shader* shader, 
			const float depth = 0.0f);

		void Batch(
			const IBatchable& batchable, 
			const sf::Transform& transform, 
			const float depth = 0.0f);

		void Batch(
			const sf::Transform& transform, 
			const sf::Vertex* vertices, 
			const std::size_t count, 
			const sf::PrimitiveType type, 
			const sf::Texture* texture, 
			const sf::Shader* shader, 
			const float depth = 0.0f);

		void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

		void Clear();

	private:
		bool CompareBackToFront(const Triangle& lhs, const Triangle& rhs) const;
		bool CompareFrontToBack(const Triangle& lhs, const Triangle& rhs) const;
		bool CompareTexture(const Triangle& lhs, const Triangle& rhs) const;

		void SortTriangles() const;
		void CreateBatches() const;

	private:
		std::vector<Triangle>				m_triangles;

		mutable std::vector<BatchInfo>		m_batches;
		mutable std::vector<SizeType>		m_indices;
		mutable sf::VertexArray				m_vertices;

		BatchMode							m_batch_mode		{BatchMode::Deferred};
		mutable bool						m_update_required	{true};
	};
}

