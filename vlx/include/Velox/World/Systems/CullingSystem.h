#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/World/SystemObject.h>
#include <Velox/Window/Camera.h>

#include <Velox/Components/QTElement.hpp>
#include <Velox/Components/Object.h>
#include <Velox/Components/Transform.h>
#include <Velox/Components/Sprite.h>

namespace vlx
{
	class VELOX_API CullingSystem : public SystemObject
	{
	public:
		using QTSystem	= System<Object, Transform, Sprite>;

	private:
		static constexpr int LENIENCY = 128;

	public:
		CullingSystem(EntityAdmin& entity_admin, const LayerType id, const Camera& camera);

	public:
		void Update() override;

	private:
		QTSystem			m_system;
		const Camera*		m_camera {nullptr};
	};
}