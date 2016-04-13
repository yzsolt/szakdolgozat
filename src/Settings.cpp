
#include "Settings.h"

Settings::Settings(const std::vector<std::string>& arguments) {

	for (const std::string& argument : arguments) {

		size_t key_end = argument.find('=');

		std::string value;

		if (key_end == std::string::npos) {
			key_end = argument.length();
		} else {
			value = argument.substr(key_end + 1);
		}

		std::string key = argument.substr(0, key_end);

		if (key == "fullscreen") {
			fullscreen = true;
		}

		if (key == "width") {
			window_size.x = std::stoul(value);
		}

		if (key == "height") {
			window_size.y = std::stoul(value);
		}

		if (key == "msaa") {
			multisample_count = std::stoul(value);
		}

	}

}
