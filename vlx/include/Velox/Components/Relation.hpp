#pragma once

#include <unordered_set>
#include <type_traits>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// <summary>
	/// Represents the relationship between entities that allows for scene graphs and possibly other things... 
	/// Is NOT a standard component, rather a property that other components can inherit to allow this feature
	/// </summary>
	template<class T>
	class Relation : public IComponent
	{
	public:
		virtual ~Relation() = 0;
		virtual void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) override;

	public:
		void AttachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation<T>& child);
		const EntityID DetachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation<T>& child);

		[[nodiscard]] bool HasParent() const noexcept;

	private:
		void PropagateAttach(const EntityAdmin& entity_admin, const EntityID entity_id);
		void PropagateDetach(const EntityAdmin& entity_admin, const EntityID entity_id);

	protected:
		EntityID						m_parent	{NULL_ENTITY};
		std::vector<EntityID>			m_children;

	private:
		std::unordered_set<EntityID>	m_closed; // list of all descendants (to prevent parenting them when attaching)
	};

	template<class T>
	inline Relation<T>::~Relation() = default;

	template<class T>
	inline void Relation<T>::Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id)
	{
		if (!HasParent())
		{
			static_cast<Relation&>(entity_admin.GetComponent<T>(m_parent))
				.DetachChild(entity_admin, m_parent, entity_id, *this);
		}

		for (const EntityID& child : m_children)
		{
			static_cast<Relation&>(entity_admin.GetComponent<T>(child)).m_parent = NULL_ENTITY;
		}
	}

	template<class T>
	inline void Relation<T>::AttachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation<T>& child)
	{
		assert(!m_closed.contains(child_id));

		if (child.m_parent == entity_id) // child is already correctly parented
			return;

		if (m_parent == child_id) // special case
		{
			static_cast<Relation&>(entity_admin.GetComponent<T>(m_parent))
				.DetachChild(entity_admin, m_parent, entity_id, *this);
		}

		if (child.m_parent != NULL_ENTITY) // if child already has an attached parent we need to detach it
		{
			static_cast<Relation&>(entity_admin.GetComponent<T>(child.m_parent))
				.DetachChild(entity_admin, child.m_parent, child_id, child);
		}

		child.m_parent = entity_id;
		m_children.push_back(child_id);

		PropagateAttach(entity_admin, child_id);
		child.PropagateInvalidation(entity_admin);
	}

	template<class T>
	inline const EntityID Relation<T>::DetachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation<T>& child)
	{
		auto found = std::find(m_children.begin(), m_children.end(), child_id);

		if (found == m_children.end())
			return NULL_ENTITY;

		PropagateDetach(entity_admin, child_id);
		child.PropagateInvalidation(entity_admin);

		child.m_parent = NULL_ENTITY;
		m_children.erase(found);
	}

	template<class T>
	inline bool Relation<T>::HasParent() const noexcept
	{
		return m_parent != NULL_ENTITY;
	}

	template<class T>
	inline void Relation<T>::PropagateAttach(const EntityAdmin& entity_admin, const EntityID entity_id)
	{
		m_closed.insert(entity_id);

		if (HasParent())
		{
			static_cast<Relation&>(entity_admin.GetComponent<T>(m_parent))
				.PropagateAttach(entity_admin, entity_id);
		}
	}

	template<class T>
	inline void Relation<T>::PropagateDetach(const EntityAdmin& entity_admin, const EntityID entity_id)
	{
		auto it = m_closed.find(entity_id);
		assert(it != m_closed.end());
		m_closed.erase(it);

		if (HasParent())
		{
			static_cast<Relation&>(entity_admin.GetComponent<T>(m_parent))
				.PropagateDetach(entity_admin, entity_id);
		}
	}
}