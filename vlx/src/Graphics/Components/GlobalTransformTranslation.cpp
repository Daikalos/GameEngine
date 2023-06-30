#include <Velox/Graphics/Components/GlobalTransformTranslation.h>

using namespace vlx;

const Vector2f& GlobalTransformTranslation::GetPosition() const
{
	return m_position;
}
