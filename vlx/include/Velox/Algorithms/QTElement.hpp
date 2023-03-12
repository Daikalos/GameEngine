#pragma once

#include <Velox/Algorithms/LQuadTree.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/System/Rectangle.hpp>

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
		using value_type = T;
		using Element = typename LQuadTree<T>::Element;

	public:
		QTElement() = default;

	protected:
		bool IsInserted() const noexcept;

	protected:
		bool Insert(LQuadTree<T>& quad_tree, T&& item, const RectFloat& rect);
		bool Erase();

	protected:
		virtual void Copied(const EntityAdmin& entity_admin, const EntityID entity_id) override;
		virtual void Modified(const EntityAdmin& entity_admin, const EntityID entity_id, IComponent& new_data) override;
		virtual void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) override;

	protected:
		LQuadTree<T>*	m_quad_tree {nullptr};
		value_type		m_item		{};
		RectFloat		m_rect		{};
	};

	template<std::equality_comparable T>
	inline bool QTElement<T>::IsInserted() const noexcept
	{
		return m_quad_tree != nullptr;
	}

	template<std::equality_comparable T>
	inline bool QTElement<T>::Insert(LQuadTree<T>& quad_tree, T&& item, const RectFloat& rect)
	{
		if (!IsInserted())
		{
			m_item = std::move(item);
			m_rect = rect;

			m_quad_tree = &quad_tree;
			m_quad_tree->Insert({ m_item, m_rect });

			return true;
		}
		return false;
	}

	template<std::equality_comparable T>
	inline bool QTElement<T>::Erase()
	{
		if (IsInserted())
		{
			bool result = m_quad_tree->Erase({ m_item, m_rect });
			assert(result); // make sure that it succeeded

			m_quad_tree = nullptr;

			return true;
		}
		return false;
	}

	template<std::equality_comparable T>
	inline void QTElement<T>::Copied(const EntityAdmin& entity_admin, const EntityID entity_id)
	{
		m_quad_tree = nullptr; // copied elements will need to reinserted
	}

	template<std::equality_comparable T>
	inline void QTElement<T>::Modified(const EntityAdmin& entity_admin, const EntityID entity_id, IComponent& new_data)
	{
		QTElement new_element = static_cast<QTElement&>(new_data);
		if (new_element.m_item != m_item || new_element.m_rect != m_rect) // erase current if new
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