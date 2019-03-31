#pragma once
#include <glm/glm.hpp>

namespace example {
	struct Camera {
		glm::mat4 perspective;
		glm::mat4 view;
	};
}