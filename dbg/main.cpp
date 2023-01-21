#include "game/Application.h"

#include <string>

int main()
{
	std::string application_name = "Velox";

		vlx::Application application(application_name);
		application.Run();

	return EXIT_SUCCESS;
}