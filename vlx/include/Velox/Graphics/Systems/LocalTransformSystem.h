#pragma once

#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/TransformMatrix.h>

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API LocalTransformSystem final : public SystemAction
	{
	private:
		using SyncLocalSystem = System<Transform, TransformMatrix>;

	public:
		LocalTransformSystem(EntityAdmin& entity_admin, LayerType id);

	public:
		void Update() override;

	private:
		SyncLocalSystem	m_sync;
	};
}