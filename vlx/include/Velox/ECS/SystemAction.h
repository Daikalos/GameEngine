#pragma once

#include <memory>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// Interface for defining system's behaviour
	/// 
	class VELOX_API SystemAction
	{
	public:
		using Ptr = std::unique_ptr<SystemAction>;

	public:
		SystemAction(EntityAdmin& entity_admin, const LayerType id);
		virtual ~SystemAction() = default;

	public:
		/// Determines if this system is able of being removed from the world
		///
		NODISC virtual bool IsRequired() const noexcept = 0;

		/// Returns the layer this system is located in
		///
		NODISC LayerType GetLayerID() const noexcept;

	public:
		virtual void PreUpdate()	= 0;
		virtual void Update()		= 0;
		virtual void FixedUpdate()	= 0;
		virtual void PostUpdate()	= 0;

	protected:
		void Execute() const;

	protected:
		EntityAdmin* m_entity_admin {nullptr};

	private:
		LayerType m_id {LYR_NONE};
	};
}