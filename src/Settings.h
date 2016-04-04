
#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>

#include <glm/glm.hpp>

struct Settings {

	glm::uvec2 window_size			= { 1280, 800 };
	bool fullscreen					= false;
	bool vsync						= true;
	unsigned int multisample_count	= 0;

	Settings(const std::vector<std::string>& arguments);

};

#endif