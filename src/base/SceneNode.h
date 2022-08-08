#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "../utilities/NonCopyable.h"

namespace fge
{
	class SceneNode : public sf::Drawable, public sf::Transformable, private NonCopyable
	{
	public:
		typedef std::unique_ptr<SceneNode> ptr;
		typedef std::pair<SceneNode*, SceneNode*> pair;

	public:

	};
}

