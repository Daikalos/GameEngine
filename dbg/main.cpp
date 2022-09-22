#include "game/Application.h"

#include <string>

int main()
{
	std::string application_name = "Velox";

	try
	{
		vlx::Application application(application_name);
		application.Run();
	}
	catch (std::exception e)
	{
		std::puts(e.what());
	}

	return EXIT_SUCCESS;
}