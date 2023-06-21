#pragma once

#include <Velox/System/Vector2.hpp>
#include <Velox/Utility/ArithmeticUtils.h>

#include <Velox/VeloxTypes.hpp>
#include <Velox/Config.hpp>

namespace vlx
{

	class VELOX_API Animation
	{
	public:
		Animation();
		Animation(const Vector2u& size, float speed);
		Animation(uint32 width, uint32 height, float speed);
		Animation(const Vector2u& size);
		Animation(uint32 width, uint32 height);

	public:
		bool Update(float dt);

	public:
		const Vector2u& GetSize() const noexcept;
		uint32 GetIndex() const noexcept;
		float GetSpeed() const noexcept;

		void SetSize(const Vector2u& size);
		void SetSize(uint32 width, uint32 height);
		void SetIndex(uint32 idx);
		void SetSpeed(float speed);

	private:
		Vector2u	m_size;
		float		m_index	{0};
		float		m_speed	{0.5f};
	};
}
