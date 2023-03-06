#pragma once

#include <Velox/Algorithms/LQuadTree.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Graphics/Rectangle.hpp>

#include <Velox/Config.hpp>

namespace vlx
{
	/// <summary>
	///		Designed to add convenience for interacting with quadtree, such as keeping track 
	///		of index to element and also erasing it when entity is suddenly destroyed.
	/// </summary>
	template<std::equality_comparable T = int>
	class QTElement : public IComponent
	{
	public:
		QTElement() = default;

	public:
		bool IsInserted() const noexcept;

	public:
		bool Insert(LQuadTree<T>& quad_tree, const T& item, const RectFloat& rect);
		bool Erase();

	private:
		void Copied(const EntityAdmin& entity_admin, const EntityID entity_id) override;
		void Modified(const EntityAdmin& entity_admin, const EntityID entity_id, IComponent& new_data) override;
		void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) override;

	private:
		LQuadTree<T>*	m_quad_tree {nullptr};
		int				m_index		{-1};
	};

	template<std::equality_comparable T>
	inline bool QTElement<T>::IsInserted() const noexcept
	{
		return m_index != -1;
	}

	template<std::equality_comparable T>
	inline bool QTElement<T>::Insert(LQuadTree<T>& quad_tree, const T& item, const RectFloat& rect)
	{
		if (!IsInserted() && m_quad_tree == nullptr)
		{
			m_index = m_quad_tree->Insert({ item, rect });
			m_quad_tree = &quad_tree;

			return true;
		}
		return false;
	}

	template<std::equality_comparable T>
	inline bool QTElement<T>::Erase()
	{
		if (IsInserted() && m_quad_tree != nullptr)
		{
			bool result = m_quad_tree->Erase(m_index);
			assert(result); // make sure that it succeeded

			m_index = -1;
			m_quad_tree = nullptr;

			return true;
		}
		return false;
	}

	template<std::equality_comparable T>
	inline void QTElement<T>::Copied(const EntityAdmin& entity_admin, const EntityID entity_id)
	{
		m_index = -1; // copied elements will need to reinserted
		m_quad_tree = nullptr;
	}

	template<std::equality_comparable T>
	inline void QTElement<T>::Modified(const EntityAdmin& entity_admin, const EntityID entity_id, IComponent& new_data)
	{
		if (static_cast<QTElement&>(new_data).m_index != m_index) // erase current if new
		{
			Erase();
		}
	}

	template<std::equality_comparable T>
	inline void QTElement<T>::Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id)
	{
		Erase(); // erase when component is destroyed
	}
}