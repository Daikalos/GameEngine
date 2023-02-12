#pragma once

#include <Velox/Algorithms/LQuadTree.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Utilities/Rectangle.hpp>

#include <Velox/Config.hpp>

namespace vlx
{
	template<std::equality_comparable T = int>
	class QTElement : public IComponent
	{
	public:
		QTElement() = default;
		QTElement(LQuadTree<T>* quad_tree);

		bool Insert(const T& item, const RectFloat& rect);
		bool Erase();

	private:
		void Copied(const EntityAdmin& entity_admin, const EntityID entity_id) override;
		void Modified(const EntityAdmin& entity_admin, const EntityID entity_id, IComponent& new_data) override;
		void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) override;

	private:
		LQuadTree<T>*	m_quad_tree {nullptr};
		int				m_index		{-1};

		friend class CullingSystem;
	};

	template<std::equality_comparable T>
	QTElement<T>::QTElement(LQuadTree<T>* quad_tree)
		: m_quad_tree(quad_tree), m_index(-1)
	{
	}

	template<std::equality_comparable T>
	bool QTElement<T>::Insert(const T& item, const RectFloat& rect)
	{
		if (m_index == -1)
		{
			m_index = m_quad_tree->Insert({ item, rect });
			return true;
		}
		return false;
	}

	template<std::equality_comparable T>
	bool QTElement<T>::Erase()
	{
		if (m_index != -1)
		{
			m_quad_tree->Erase(m_index);
			m_index = -1;
			return true;
		}
		return false;
	}

	template<std::equality_comparable T>
	void QTElement<T>::Copied(const EntityAdmin& entity_admin, const EntityID entity_id)
	{
		m_index = -1; // copied elements will need to reinserted
	}

	template<std::equality_comparable T>
	void QTElement<T>::Modified(const EntityAdmin& entity_admin, const EntityID entity_id, IComponent& new_data)
	{
		if (static_cast<QTElement&>(new_data).m_index != m_index) // erase current if new
		{
			Erase();
		}
	}

	template<std::equality_comparable T>
	void QTElement<T>::Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id)
	{
		Erase(); // erase when component is destroyed
	}
}