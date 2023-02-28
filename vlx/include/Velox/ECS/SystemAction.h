#pragma once

#include <memory>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API SystemAction
	{
	public:
		using Ptr = std::unique_ptr<SystemAction>;

	public:
		SystemAction(EntityAdmin& entity_admin, const LayerType id);
		virtual ~SystemAction() = default;

	public:
		NODISC constexpr LayerType GetID() const noexcept;
		NODISC virtual constexpr bool IsRequired() const noexcept = 0;

	public:
		virtual void PreUpdate()	= 0;
		virtual void Update()		= 0;
		virtual void FixedUpdate()	= 0;
		virtual void PostUpdate()	= 0;

	protected:
		EntityAdmin* m_entity_admin {nullptr};

	private:
		LayerType m_id {LYR_NONE};
	};
}