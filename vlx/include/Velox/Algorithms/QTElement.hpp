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
		template<typename... Args> requires std::constructible_from<T, Args...>
		bool Insert(LQuadTree<T>& quad_tree, const RectFloat& rect, Args&&... args);

		template<typename... Args> requires std::constructible_from<T, Args...>
		bool Update(Args&&... args);

		bool Erase();

		const T& Get() const;
		T& Get();

	protected:
		virtual void Copied(const EntityAdmin& entity_admin, const EntityID entity_id) override;
		virtual void Modified(const EntityAdmin& entity_admin, const EntityID entity_id, IComponent& new_data) override;
		virtual void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) override;

	protected:
		LQuadTree<T>*	m_quad_tree {nullptr};
		int				m_index		{0};
	};

	template<std::equality_comparable T>
	inline bool QTElement<T>::IsInserted() const noexcept
	{
		return m_quad_tree != nullptr;
	}

	template<std::equality_comparable T>
	template<typename... Args> requires std::constructible_from<T, Args...>
	inline bool QTElement<T>::Insert(LQuadTree<T>& quad_tree, const RectFloat& rect, Args&&... args)
	{
		if (!IsInserted())
		{
			m_index = quad_tree.Insert(rect, std::forward<Args>(args)...);

			if (m_index == -1)
				return false;

			m_quad_tree = &quad_tree;

			return true;
		}
		return false;
	}

	template<std::equality_comparable T>
	template<typename... Args> requires std::constructible_from<T, Args...>
	inline bool QTElement<T>::Update(Args&&... args)
	{
		if (IsInserted())
		{
			bool result = m_quad_tree->Update(m_index, std::forward<Args>(args)...); 
			assert(result); // make sure it succeeded

			return true;
		}

		return false;
	}

	template<std::equality_comparable T>
	inline bool QTElement<T>::Erase()
	{
		if (IsInserted())
		{
			bool result = m_quad_tree->Erase(m_index);
			assert(result); // make sure it succeeded

			m_quad_tree = nullptr;
			m_index = 0;

			return true;
		}
		return false;
	}

	template<std::equality_comparable T>
	inline const T& QTElement<T>::Get() const
	{
		assert(IsInserted());
		return m_quad_tree->Get(m_index);
	}
	template<std::equality_comparable T>
	inline T& QTElement<T>::Get()
	{
		assert(IsInserted());
		return m_quad_tree->Get(m_index);
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
		if (new_element.m_index != m_index) // erase current if new
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