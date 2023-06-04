#pragma once

#include <type_traits>

#include <Velox/ECS.hpp>

#include "Object.h"

namespace vlx
{
	template<class T, class U> requires std::is_standard_layout_v<U>
	class DataBehaviour
	{
	public:
		DataBehaviour(EntityAdmin& entity_admin);
		~DataBehaviour();

	protected:
		const EntityAdmin* GetEntityAdmin() const noexcept;
		EntityAdmin* GetEntityAdmin() noexcept;

	private:
		SystemEvent<Object, U>	m_start;
		System<Object, U>		m_pre_update;
		System<Object, U>		m_update;
		System<Object, U>		m_fixed_update;
		System<Object, U>		m_post_update;
		int						m_on_add_id		{-1};
		int						m_on_rmv_id		{-1};
	};

	template<class T, class U> requires std::is_standard_layout_v<U>
	inline DataBehaviour<T, U>::DataBehaviour(EntityAdmin& entity_admin)
		: m_start(entity_admin), m_pre_update(entity_admin), m_update(entity_admin), m_fixed_update(entity_admin), m_post_update(entity_admin)
	{
		entity_admin.RegisterComponent<U>();

		if constexpr (requires(T t, U& u) { t.Start(EntityID(), u); })
		{
			m_start.ForceAdd(LYR_OBJECTS_START);
			m_start.Each(
				[this](EntityID entity_id, Object& obj, U& data)
				{
					static_cast<T*>(this)->Start(entity_id, data);
				});

			m_start.OnEnd += [this]() 
			{
				if (m_on_add_id != -1)
					return;

				// start listening for add events
				m_on_add_id = GetEntityAdmin()->RegisterOnAddListener<U>(
					[this](EntityID entity_id, U& data)
					{
						static_cast<T*>(this)->Start(entity_id, data);
					});
			};
		}

		if constexpr (requires(T t, U& u) { t.PreUpdate(EntityID(), u); })
		{
			m_pre_update.ForceAdd(LYR_OBJECTS_PRE);
			m_pre_update.Each(
				[this](EntityID entity_id, Object& obj, U& data)
				{
					static_cast<T*>(this)->PreUpdate(entity_id, data);
				});
		}

		if constexpr (requires(T t, U& u) { t.Update(EntityID(), u); })
		{
			m_update.ForceAdd(LYR_OBJECTS_UPDATE);
			m_update.Each(
				[this](EntityID entity_id, Object& obj, U& data)
				{
					static_cast<T*>(this)->Update(entity_id, data);
				});
		}

		if constexpr (requires(T t, U& u) { t.FixedUpdate(EntityID(), u); })
		{
			m_fixed_update.ForceAdd(LYR_OBJECTS_FIXED);
			m_fixed_update.Each(
				[this](EntityID entity_id, Object& obj, U& data)
				{
					static_cast<T*>(this)->FixedUpdate(entity_id, data);
				});
		}
		
		if constexpr (requires(T t, U& u) { t.PostUpdate(EntityID(), u); })
		{
			m_post_update.ForceAdd(LYR_OBJECTS_POST);
			m_post_update.Each(
				[this](EntityID entity_id, Object& obj, U& data)
				{
					static_cast<T*>(this)->PostUpdate(entity_id, data);
				});
		}

		if constexpr (requires(T t, U& u) { t.Destroy(EntityID(), u); })
		{
			m_on_rmv_id = entity_admin.RegisterOnRemoveListener<U>(
				[this](EntityID entity_id, U& data)
				{
					static_cast<T*>(this)->Destroy(entity_id, data);
				});
		}
	}

	template<class T, class U> requires std::is_standard_layout_v<U>
	inline DataBehaviour<T, U>::~DataBehaviour()
	{
		if (m_on_add_id != -1) GetEntityAdmin()->DeregisterOnAddListener<U>(m_on_add_id);
		if (m_on_rmv_id != -1) GetEntityAdmin()->DeregisterOnRemoveListener<U>(m_on_rmv_id);
	}

	template<class T, class U> requires std::is_standard_layout_v<U>
	inline const EntityAdmin* DataBehaviour<T, U>::GetEntityAdmin() const noexcept
	{
		return m_update.GetEntityAdmin();
	}
	template<class T, class U> requires std::is_standard_layout_v<U>
	inline EntityAdmin* DataBehaviour<T, U>::GetEntityAdmin() noexcept
	{
		return m_update.GetEntityAdmin();
	}
}