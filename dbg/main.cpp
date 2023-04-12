#include "game/Application.h"

#include <string>

int main()
{
	std::string name = "Velox";

	vlx::Application application(name);
	application.Run();

	return EXIT_SUCCESS;
}