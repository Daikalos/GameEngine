#pragma once

#include <numeric>
#include <unordered_set>

#include <SFML/Graphics.hpp>

#include <Velox/Graphics/Components/IBatchable.h>
#include <Velox/System/Mat4f.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	enum class BatchMode : uint8
	{
		Deferred,
		BackToFront,
		FrontToBack,
		Texture,
	};

	/// SpriteBatch used to combine similar textures or shaders to reduce number of drawcalls. 
	///
	class VELOX_API SpriteBatch final : public sf::Drawable
	{
	private:
		using SizeType = uint32;

		static constexpr SizeType TRIANGLE_COUNT = 3;

		struct VELOX_API Triangle
		{
			Triangle(sf::Vertex&& v0, sf::Vertex&& v1, sf::Vertex&& v2, const sf::Texture* t, const sf::Shader* s, float d);

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

		void Batch(
			const IBatchable& batchable, 
			const Mat4f& transform,
			float depth = 0.0f);

		void Batch(
			const Mat4f& transform,
			const sf::Vertex* vertices, 
			std::size_t count, 
			sf::PrimitiveType type, 
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
		mutable std::vector<SizeType>		m_indices;
		mutable sf::VertexArray				m_vertices;

		BatchMode							m_batch_mode		{BatchMode::Deferred};
		mutable bool						m_update_required	{true};
	};
}

