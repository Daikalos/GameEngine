#pragma once

#include <functional>

#include "Identifiers.h"
#include "Archetype.h"

#include "../utilities/NonCopyable.h"
#include "../utilities/Time.hpp"

namespace fge
{
	class ECS;

	class SystemBase : private NonCopyable
	{
	public:
		virtual ~SystemBase() {}
		virtual ArchetypeID GetKey() const = 0;
		virtual void Update(Time& time, Archetype* archetype) = 0;
	};

	template<class... Args>
	class System : public SystemBase
	{
	public:
		friend class ECS;
		using Func = std::function<void(const Time& time, const std::vector<EntityID>&, Args*...)>;

	public:
		System(ECS& ecs, const std::uint8_t& layer);
		// no virtual destructor because the system is not supposed to contain variables, only behaviors

		virtual ArchetypeID GetKey() const override;

		void SetUpdate(Func func);

	protected:
		ECS* m_ecs;
		Func m_func;
		bool m_func_set;
	};

	template<class ...Args>
	System<Args...>::System(ECS& ecs, const std::uint8_t& layer)
		: m_ecs(&ecs), m_func(), m_func_set(false)
	{
		//_ecs.register_system(layer, this);
	}

	template<class... Ts>
	ArchetypeID SortKeys(ArchetypeID types)
	{
		std::sort(types.begin(), types.end());
		return types;
	}

	template<class... Ts>
	inline ArchetypeID System<Ts...>::GetKey() const
	{
		return SortKeys({{ Component<Ts>::GetTypeId()... }});
	}

	template<class ...Args>
	void System<Args...>::SetUpdate(Func func)
	{
		m_func = func;
		m_func_set = true;
	}
}

