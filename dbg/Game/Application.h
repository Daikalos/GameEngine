#pragma once

#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <string_view>

#include <Velox/World/World.h>
#include <Velox/ObjectTypes.hpp>

#include "Binds.h"
#include "Scenes/StateTest.h"

#include "Cameras/CameraDrag.h"
#include "Cameras/CameraZoom.h"

static const std::string DATA_FOLDER = "../data/";
static const std::string AUDIO_FOLDER = DATA_FOLDER + "audio/";
static const std::string TEXTURE_FOLDER = DATA_FOLDER + "textures/";

namespace vlx
{
	class Application final
	{
	public:
		Application(const std::string_view name);

		void Run();

	private:
		void LoadTextures();
		void RegisterStates();
		void RegisterControls();

	private:
		World m_world;
	};
}

