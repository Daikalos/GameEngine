#pragma once

#include <functional>

#include <Velox/Utilities.hpp>

#include "Identifiers.hpp"
#include "Archetype.h"

namespace vlx
{
	class ECS;

	class SystemBase : private NonCopyable
	{
	public:
		virtual ~SystemBase() {}

		virtual ArchetypeID GetKey() const = 0;

	protected:
		virtual void PreUpdate(Time& time, Archetype* archetype) = 0;
		virtual void Update(Time& time, Archetype* archetype) = 0;
		virtual void FixedUpdate(Time& time, Archetype* archetype) = 0;
		virtual void PostUpdate(Time& time, Archetype* archetype) = 0;
		virtual void Draw(float interp, Archetype* archetype) = 0;
	};

	template<class... Args>
	class System : public SystemBase
	{
	public:
		friend class ECS;

	public:
		System(ECS& ecs, const std::uint8_t& layer);

		ArchetypeID GetKey() const override;

	protected:
		void PreUpdate(Time& time, Archetype* archetype) override;
		void Update(Time& time, Archetype* archetype) override;
		void FixedUpdate(Time& time, Archetype* archetype) override;
		void PostUpdate(Time& time, Archetype* archetype) override;
		void Draw(float interp, Archetype* archetype) override; // TODO: here

	protected:
		ECS* m_ecs;
	};

	template<class ...Args>
	inline System<Args...>::System(ECS& ecs, const std::uint8_t& layer) : m_ecs(&ecs)
	{
		m_ecs->RegisterSystem(layer, this);
	}

	template<class... Cs>
	inline ArchetypeID SortKeys(ArchetypeID types)
	{
		std::sort(types.begin(), types.end());
		return types;
	}

	template<class... Cs>
	inline ArchetypeID System<Cs...>::GetKey() const
	{
		return SortKeys({{ Component<Cs>::GetTypeId()... }});
	}	
}

