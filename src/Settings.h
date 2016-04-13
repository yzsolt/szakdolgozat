
#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

struct Settings {

	glm::uvec2 window_size			= { 1280, 800 };
	bool fullscreen					= false;
	unsigned int multisample_count	= 0;

	Settings(const std::vector<std::string>& arguments);

};

#endif
