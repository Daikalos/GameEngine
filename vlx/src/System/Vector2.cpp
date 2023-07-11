#include <Velox/System/Vector2.hpp>

using namespace vlx;

template<Arithmetic T>
Vector2<T> Vector2<T>::RotatePoint(const Vector2<T>& point, const Vector2<T>& center, sf::Angle angle) requires FloatingPoint<T>
{
	const Vector2<T> dir = Direction(center, point);

	float rad = angle.asRadians();

	float s = std::sin(rad);
	float c = std::cos(rad);

	return Vector2<T>(
		(dir.x * c - dir.y * s) + center.x,
		(dir.x * s + dir.y * c) + center.y);
}

template<Arithmetic T>
sf::Angle Vector2<T>::Angle() const requires FloatingPoint<T>
{
	return v.angle();
}

template<Arithmetic T>
sf::Angle Vector2<T>::AngleTo(const Vector2& rhs) const requires FloatingPoint<T>
{
	return v.angleTo(rhs);
}

template<Arithmetic T>
Vector2<T> Vector2<T>::RotatedBy(sf::Angle phi) const requires FloatingPoint<T>
{
	return v.rotatedBy(phi);
}

template<Arithmetic T>
Vector2<T> Vector2<T>::Abs() const requires FloatingPoint<T>
{
	return Vector2<T>(std::abs(x), std::abs(y));
}
template<Arithmetic T>
Vector2<T> Vector2<T>::Floor() const requires FloatingPoint<T>
{
	return Vector2<T>(std::floor(x), std::floor(y));
}

// explicit template instantiations

template class vlx::Vector2<float>;
template class vlx::Vector2<double>;
template class vlx::Vector2<long double>;