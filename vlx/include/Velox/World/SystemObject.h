#pragma once

#include <Velox/ECS.hpp>

#include <Velox/Config.hpp>
#include <memory>

namespace vlx
{
	class VELOX_API SystemObject
	{
	public:
		using Ptr = std::shared_ptr<SystemObject>;

	public:
		SystemObject(EntityAdmin& entity_admin, const LayerType id);
		virtual ~SystemObject() = default;

	public:
		NODISC constexpr LayerType GetID() const noexcept;

	public:
		virtual void PreUpdate() {}
		virtual void Update() = 0;
		virtual void FixedUpdate() {}
		virtual void PostUpdate() {}

	protected:
		EntityAdmin* m_entity_admin {nullptr};

	private:
		LayerType m_id {LYR_NONE};
	};
}