#pragma once

#include <compare>

#include <Velox/Utility/NonCopyable.h>

#include "Identifiers.hpp"
#include "Archetype.hpp"

namespace vlx
{
	class EntityAdmin;

	class VELOX_API SystemBase : private NonCopyable
	{
	public:
		virtual ~SystemBase() = default;

	public:
		friend auto operator<=>(const SystemBase& lhs, const SystemBase& rhs)
		{
			return lhs.GetPriority() <=> rhs.GetPriority();
		}

	public:
		NODISC virtual ArchetypeID GetIDKey() const = 0;
		NODISC virtual const ComponentIDs& GetArchKey() const = 0;

		NODISC float GetPriority() const noexcept;
		NODISC bool IsRunningParallel() const noexcept;
		NODISC bool IsEnabled() const noexcept;

		virtual void SetPriority(float val);
		virtual void SetRunParallel(bool flag);
		virtual void SetEnabled(bool flag);

	public:
		virtual void Start() const;
		virtual void End() const;

	protected:
		virtual void Run(const Archetype* const archetype) const = 0;

	private:
		float	m_priority		{0.0f};		// priority is for controlling the underlying order of calls inside a layer
		bool	m_run_parallel	{false};	// determines if whether to parallelize archetypes when being run
		bool	m_enabled		{true};		// enables or disables system being run

		friend class EntityAdmin;
	};
}