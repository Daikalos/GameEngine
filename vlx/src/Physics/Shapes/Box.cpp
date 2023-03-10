#include <Velox/Physics/Shapes/Box.h>

using namespace vlx;

constexpr Shape::Type Box::GetType() const noexcept
{
	return Shape::Box;
}

Box::Box(const RectFloat& box) 
{ 
	SetBox(box); 
}
Box::Box(const Vector2f& min, const Vector2f& max)
{ 
	SetBox({ min, max }); 
}
Box::Box(float min_x, float min_y, float max_x, float max_y) 
{ 
	SetBox({ min_x, min_y, max_x, max_y }); 
}

auto Box::GetVertices() const noexcept -> const VecArr&
{
	return m_vertices;
}
RectFloat Box::GetBox() const noexcept
{
	return RectFloat(m_vertices[0], m_vertices[3] - m_vertices[0]);
}

float Box::GetWidth() const noexcept
{
	return std::abs(m_vertices[1].x - m_vertices[0].x);
}

float Box::GetHeight() const noexcept
{
	return std::abs(m_vertices[2].y - m_vertices[0].y);
}

void Box::SetBox(const RectFloat& box)
{
	m_vertices[0] = Vector2f(box.left,		box.top);
	m_vertices[1] = Vector2f(box.Right(),	box.top);
	m_vertices[2] = Vector2f(box.left,		box.Bottom());
	m_vertices[3] = Vector2f(box.Right(),	box.Bottom());
}

void Box::SetSize(const Vector2f& size)
{
	m_vertices[1].x = m_vertices[0].x + size.x;
	m_vertices[3].x = m_vertices[0].x + size.x;
	m_vertices[2].y = m_vertices[0].y + size.y;
	m_vertices[3].y = m_vertices[0].y + size.y;
}

void Box::SetLeft(float left)
{
	m_vertices[0].x = left;
	m_vertices[2].x = left;
}

void Box::SetTop(float top)
{
	m_vertices[0].y = top;
	m_vertices[1].y = top;
}

void Box::SetRight(float right)
{
	m_vertices[1].x = right;
	m_vertices[3].x = right;
}

void Box::SetBottom(float bottom)
{
	m_vertices[2].y = bottom;
	m_vertices[3].y = bottom;
}

void Box::Initialize(PhysicsBody& body) const
{
	body.SetMass(GetWidth() * GetHeight() * body.GetDensity());
	body.SetInertia((1.0f / 12.0f) * body.GetMass() * (au::Sq(GetWidth()) + au::Sq(GetHeight())));
}

RectFloat Box::GetAABB() const
{
	return GetBox();
}
