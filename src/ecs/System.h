#pragma once

#include <functional>

#include "Identifiers.h"
#include "Archetype.h"
#include "ECS.h"

#include "../utilities/NonCopyable.h"

namespace fge
{
	class SystemBase : private NonCopyable
	{
	public:
		virtual ~SystemBase() {}
		virtual ArchetypeID get_key() const = 0;
		virtual void update(const float& dt, Archetype* archetype) = 0;
	};

	template<class... Args>
	class System : public SystemBase
	{
	public:
		friend class ECS;
		using Func = std::function<void(const float, const std::vector<EntityID>&, Args*...)>;

	public:
		System(ECS& ecs, const std::uint8_t& layer);
		// no virtual destructor because the system is not supposed to contain variables, only behaviors

		virtual ArchetypeID get_key() const override;

		void set_update(Func func);

	protected:
		ECS* _ecs;
		Func _func;
		bool _func_set;
	};

	template<class ...Args>
	System<Args...>::System(ECS& ecs, const std::uint8_t& layer)
		: _ecs(&ecs), _func(), _func_set(false)
	{
		//_ecs.register_system(layer, this);
	}

	template<class ...Args>
	void System<Args...>::set_update(Func func)
	{
		_func = func;
		_func_set = true;
	}
}

