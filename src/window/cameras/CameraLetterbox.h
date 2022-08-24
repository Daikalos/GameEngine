#pragma once

#include "../CameraBehaviour.hpp"
#include "../cm.h"
#include "../Camera.h"

namespace fge
{
	class CameraLetterbox final : public CameraBehaviour
	{
	public:
		CameraLetterbox(cm::ID id, Camera& camera, Context context) :
			CameraBehaviour(id, camera, context) { }

	private:
		bool HandleEvent(const sf::Event& event) override
		{
			switch (event.type)
			{
			case sf::Event::Resized:
				SetLetterboxView(event.size.width, event.size.height);
				break;
			}

			return true;
		}

		bool Update(const Time& time) override
		{
			return true;
		}

		void SetLetterboxView(int width, int height)
		{
			float window_ratio = width / (float)height;
			float view_ratio = GetCamera().getSize().x / (float)GetCamera().getSize().y;

			float size_x = 1.0f;
			float size_y = 1.0f;
			float pos_x = 0.0f;
			float pos_y = 0.0f;

			if (window_ratio >= view_ratio)
			{
				size_x = view_ratio / window_ratio;
				pos_x = (1.0f - size_x) / 2.0f;
			}
			else
			{
				size_y = window_ratio / view_ratio;
				pos_y = (1.0f - size_y) / 2.0f;
			}

			GetCamera().setViewport(sf::FloatRect(sf::Vector2f(pos_x, pos_y), sf::Vector2f(size_x, size_y)));
		}
	};
}