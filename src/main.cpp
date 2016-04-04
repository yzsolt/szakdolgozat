
#include <vector>

#include "Renderer.h"
#include "log.h"

int main(int argc, char** argv) {

	std::vector<std::string> arguments(argv, argv + argc);

	Settings settings(arguments);

	try {
		Renderer renderer(settings);
		renderer.run();
	} catch (const std::runtime_error& error) {
		LOG("A runtime error occured: " + std::string(error.what()));
		std::getchar();
	} catch (...) {
		LOG("An unknown exception was thrown.");
		std::getchar();
	}

    return 0;

}
