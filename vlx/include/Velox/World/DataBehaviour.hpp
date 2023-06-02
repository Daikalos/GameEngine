#pragma once

#include <type_traits>

#include <Velox/ECS.hpp>

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
		System<U> m_objects;
		int m_on_add_id {-1};
		int m_on_rmv_id {-1};
	};

	template<class T, class U> requires std::is_standard_layout_v<U>
	inline DataBehaviour<T, U>::DataBehaviour(EntityAdmin& entity_admin)
		: m_objects(entity_admin, LYR_OBJECTS)
	{
		entity_admin.RegisterComponent<U>();
		
		m_objects.Each(
			[this](EntityID entity_id, U& data)
			{
				static_cast<T*>(this)->Update(entity_id, data);
			});

		m_on_add_id = entity_admin.RegisterOnAddListener<U>(
			[this](EntityID entity_id, U& data)
			{
				static_cast<T*>(this)->Start(entity_id, data);
			});

		m_on_rmv_id = entity_admin.RegisterOnRemoveListener<U>(
			[this](EntityID entity_id, U& data)
			{
				static_cast<T*>(this)->Destroy(entity_id, data);
			});
	}

	template<class T, class U> requires std::is_standard_layout_v<U>
	inline DataBehaviour<T, U>::~DataBehaviour()
	{
		GetEntityAdmin()->DeregisterOnAddListener<U>(m_on_add_id);
		GetEntityAdmin()->DeregisterOnRemoveListener<U>(m_on_rmv_id);
	}

	template<class T, class U> requires std::is_standard_layout_v<U>
	inline const EntityAdmin* DataBehaviour<T, U>::GetEntityAdmin() const noexcept
	{
		return m_objects.GetEntityAdmin();
	}
	template<class T, class U> requires std::is_standard_layout_v<U>
	inline EntityAdmin* DataBehaviour<T, U>::GetEntityAdmin() noexcept
	{
		return m_objects.GetEntityAdmin();
	}
}