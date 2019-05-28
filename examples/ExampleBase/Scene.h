#pragma once
#include <glm/glm.hpp>

namespace vkx {
	struct Camera {
		glm::mat4 perspective;
		glm::mat4 view;
	};
}