
#include <vector>
#include <array>

#include "TextureCubeMap.h"

std::array<std::unique_ptr<float[]>, 6> TextureCubeMap::_load_hdr_vertical_cross(const std::string & file_path) {

	std::unique_ptr<float[]> data = Texture::load_hdr_image(file_path, m_component_count, m_size);

	if (!(m_size.x % 3 == 0 && m_size.y % 4 == 0 && m_size.x / 3 == m_size.y / 4)) {
		throw std::runtime_error("Wrong size for vertical cross skybox: width must be 3/4 of height.");
	}

	unsigned int section_width = m_size.x / 3;
	//assert(m_size.y == section_width * 4);

	std::vector<glm::uvec2> sections;

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 3; x++) {

			if (y != 1 && (x == 0 || x == 2)) {
				continue;
			}

			sections.push_back(glm::uvec2(x * section_width, y * section_width));

		}
	}

	std::array<int, 6> mapping = { 3, 1, 0, 4, 2, 5 };

	size_t row_size_in_floats = m_size.x * m_component_count;
	size_t section_row_in_floats = section_width * m_component_count;

	std::array<std::unique_ptr<float[]>, 6> sides;

	for (int i = 0; i < 6; i++) {

		const auto& section = sections[mapping[i]];

		std::unique_ptr<float[]> section_buffer(new float[section_width * (section_width * m_component_count)]);
		float* buffer = section_buffer.get();

		for (int row = 0; row < section_width; row++) {

			float* source = data.get() + (section.y + row) * row_size_in_floats + section.x * m_component_count;

			if (i == 5) {
				// With a vertical cross input, we have to flip the section both vertically and horizontally
				float* destination = buffer + (section_width - row - 1) * (row_size_in_floats / 3);

				std::copy(source, source + section_row_in_floats, destination);

				//std::swap_ranges(destination, destination + section_row_in_floats, destination);
				for (size_t x = 0; x < section_width / 2; x++) {
					for (int c = 0; c < m_component_count; c++) {
						float* from = destination + x * m_component_count + c;
						float* to = destination + section_row_in_floats - x * m_component_count - (m_component_count - c - 1) - 1;
						std::swap(*from, *to);
					}
				}

			} else {
				float* destination = buffer + row * (row_size_in_floats / 3);
				std::copy(source, source + section_row_in_floats, destination);
			}

		}

		sides[i] = std::move(section_buffer);

	}

	m_size = { section_width, section_width };

	return sides;

}

TextureCubeMap::TextureCubeMap(const glm::uvec2& size) : Texture(GL_TEXTURE_CUBE_MAP, size) {

	bind();

	set_filters(Filter::LINEAR, Filter::LINEAR);
	set_wrap_mode(WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE);// , WrapMode::CLAMP_TO_EDGE);

}

void TextureCubeMap::upload_ldr(const std::string& folder) {

	bind();

	for (int i = 0; i < 6; i++) {

		GLubyte* data = Texture::load_ldr_image(folder + "/" + std::to_string(i) + ".jpg", m_component_count, m_size);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, m_size.x, m_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		free((void*)data);

	}

	unbind();

}

void TextureCubeMap::upload_hdr(const std::string& path, InputType input_type) {

	bind();

	std::array<std::unique_ptr<float[]>, 6> sides;

	switch (input_type) {
	case InputType::VERTICAL_CROSS:
		sides = _load_hdr_vertical_cross(path);
		break;
	}

	assert(m_size.x == m_size.y);

	for (int i = 0; i < sides.size(); i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, m_size.x, m_size.y, 0, GL_RGB, GL_FLOAT, sides[i].get());
	}

	unbind();

}

void TextureCubeMap::upload_hdr_sides(const std::string &folder, const std::string &name) {

	bind();

	for (GLuint i = 0; i < 6; i++) {

		int component_count;
		glm::uvec2 size;

		std::unique_ptr<float[]> data = Texture::load_hdr_image(folder + name + "." + std::to_string(i) + ".hdr", component_count, size);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, component_count == 4 ? GL_RGBA32F : GL_RGB32F, size.x, size.y, 0, component_count == 4 ? GL_RGBA : GL_RGB, GL_FLOAT, data.get());

		if (i == 0) {
			m_size = size;
		}

	}

	unbind();

}

void TextureCubeMap::upload_hdr_sides_and_mips(const std::string &folder, const std::string &name, GLuint mip_count) {

	bind();

	set_filters(Texture::Filter::LINEAR, Texture::Filter::TRILINEAR);
	glTexParameteri(m_type, GL_TEXTURE_MAX_LEVEL, mip_count - 1);

	for (GLuint i = 0; i < 6; i++) {
		for (GLuint j = 0; j < mip_count; j++) {

			int component_count;
			glm::uvec2 size;

			std::unique_ptr<float[]> data = Texture::load_hdr_image(folder + name + "." + std::to_string(i) + "." + std::to_string(j) + ".hdr", component_count, size);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, component_count == 4 ? GL_RGBA32F : GL_RGB32F, size.x, size.y, 0, component_count == 4 ? GL_RGBA : GL_RGB, GL_FLOAT, data.get());

			if (i == 0 && j == 0) {
				m_size = size;
			}

		}
	}

	unbind();

}
