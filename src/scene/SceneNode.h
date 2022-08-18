#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "../utilities/NonCopyable.h"

namespace fge
{
	class SceneNode : public sf::Drawable, public sf::Transformable, private NonCopyable
	{
	public:
		using ptr = typename std::unique_ptr<SceneNode>;
		using pair = typename std::pair<SceneNode*, SceneNode*>;

	public:

	};
}

