#include <Velox/Graphics/Systems/CullingSystem.h>

using namespace vlx;

CullingSystem::CullingSystem(EntityAdmin& entity_admin, LayerType id, const Camera& camera)
	: SystemAction(entity_admin, id), m_camera(&camera), m_cull_sprites(entity_admin, id), m_cull_meshes(entity_admin, id)
{
	m_cull_sprites.All(&CullingSystem::CullSprites, this);
	m_cull_meshes.All(&CullingSystem::CullMeshes, this);
}

void CullingSystem::PostUpdate()
{
	Execute();
}

void CullingSystem::CullSprites(std::size_t size, Renderable* renderables, GlobalTransformMatrix* gtms, Sprite* sprites) const
{
	const Vector2f camera_size = m_camera->GetSize() / m_camera->GetScale();
	const Vector2f camera_pos = m_camera->GetPosition() - camera_size / 2.0f;

	const RectFloat camera_rect = 
	{ 
		camera_pos.x - LENIENCY,
		camera_pos.y - LENIENCY,
		camera_size.x + LENIENCY,
		camera_size.y + LENIENCY
	};
	const RectFloat gui_camera_rect =
	{
		-LENIENCY,
		-LENIENCY,
		m_camera->GetSize().x + LENIENCY,
		m_camera->GetSize().y + LENIENCY
	};

	for (std::size_t i = 0; i < size; ++i)
	{
		Renderable&	renderable		= renderables[i];
		GlobalTransformMatrix& gtm	= gtms[i];
		Sprite&	sprite				= sprites[i];

		const RectFloat rect = gtm.matrix.TransformRect(RectFloat({ 0, 0 }, sprite.GetSize()));
		renderable.IsCulled = !(renderable.IsGUI ? gui_camera_rect.Overlaps(rect) : camera_rect.Overlaps(rect));
	}
}

void CullingSystem::CullMeshes(std::size_t size, Renderable* renderables, GlobalTransformMatrix* gtms, Mesh* meshes) const
{
	const Vector2f camera_size = m_camera->GetSize() / m_camera->GetScale();
	const Vector2f camera_pos = m_camera->GetPosition() - camera_size / 2.0f;

	const RectFloat camera_rect =
	{
		camera_pos.x - LENIENCY,
		camera_pos.y - LENIENCY,
		camera_size.x + LENIENCY,
		camera_size.y + LENIENCY
	};
	const RectFloat gui_camera_rect =
	{
		-LENIENCY,
		-LENIENCY,
		m_camera->GetSize().x + LENIENCY,
		m_camera->GetSize().y + LENIENCY
	};

	for (std::size_t i = 0; i < size; ++i)
	{
		Renderable& renderable		= renderables[i];
		GlobalTransformMatrix& gtm	= gtms[i];
		Mesh& mesh					= meshes[i];

		const RectFloat rect = gtm.matrix.TransformRect(py::ComputeAABB(mesh.GetVertices())); // TODO: maybe replace in future
		renderable.IsCulled = !(renderable.IsGUI ? gui_camera_rect.Overlaps(rect) : camera_rect.Overlaps(rect));
	}
}
