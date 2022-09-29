#pragma once

#include <SFML/Graphics.hpp>

#include <numeric>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>
#include <Velox/Components/IBatchable.h>
#include <Velox/Components/Sprite.h>
#include <Velox/Components/Transform.h>

namespace vlx
{
	constexpr std::size_t TRIANGLE_COUNT = 3;

	enum class SortMode : std::uint8_t
	{
		Deferred,
		BackToFront,
		FrontToBack,
		Texture,
	};

	class VELOX_API SpriteBatch final : public sf::Drawable
	{
	private:
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
			std::size_t			count	{0};
		};

	public:
		void Reserve(const std::size_t size);

		void SetSortMode(const SortMode sort_mode);

		void AddTriangle(
			const Transform& transform, 
			const sf::Vertex& v0, 
			const sf::Vertex& v1, 
			const sf::Vertex& v2, 
			const sf::Texture* texture, 
			const sf::Shader* shader, 
			float depth = 0.0f);

		void Batch(
			const IBatchable& batchable, 
			const Transform& transform, 
			float depth = 0.0f);

		void Batch(
			const Transform& transform, 
			const sf::Vertex* vertices, 
			const std::size_t count, 
			const sf::PrimitiveType type, 
			const sf::Texture* texture, 
			const sf::Shader* shader, 
			float depth = 0.0f);

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
		mutable std::vector<std::size_t>	m_proxy;
		mutable sf::VertexArray				m_vertices;

		SortMode		m_sort_mode			{SortMode::Deferred};
		mutable bool	m_update_required	{true};
	};
}

