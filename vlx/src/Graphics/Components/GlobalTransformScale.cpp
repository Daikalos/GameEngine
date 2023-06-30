#include <Velox/Graphics/Components/GlobalTransformScale.h>

using namespace vlx;

const Vector2f& GlobalTransformScale::GetScale() const 
{
	return m_scale;
}