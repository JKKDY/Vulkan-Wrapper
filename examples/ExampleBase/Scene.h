#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vkx {
	enum CameraType {
		lookAt,
		firstPerson
	};

	class Camera {
	public:
		Camera(CameraType type = CameraType::lookAt):
			view(view_m),
			perspective(perspective_m)
		{}
		
		const glm::mat4 & perspective;
		const glm::mat4 & view;
		
		void setPerspective(float fov, float aspect, float znear = 0.001f, float zfar = 256.0f) {
			this->fov = fov;
			this->znear = znear;
			this->zfar = zfar;
			this->aspect = aspect;

			perspective_m = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		}

		void lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
			view_m = glm::lookAt(eye, center, up);
		}

		void translate(glm::vec3 delta, bool reset = false) {
			if (reset)
				position = delta;
			else
				position += delta;
			updateViewMatrix();
		}

		void rotate(glm::vec3 delta, bool reset = false) {
			if (reset)
				rotation = delta;
			else
				rotation += delta;
			updateViewMatrix();
		}

	private:

		void updateViewMatrix()
		{
			glm::mat4 rotM = glm::mat4(1.0f);
			glm::mat4 transM;

			rotM = glm::rotate(rotM, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			transM = glm::translate(glm::mat4(1.0f), position);

			if (type == CameraType::firstPerson)
			{
				view_m = rotM * transM;
			}
			else
			{
				view_m = transM * rotM;
			}
		};

		CameraType type;

		float fov;
		float aspect;
		float znear, zfar;

		glm::mat4 perspective_m;
		glm::mat4 view_m;
		
		glm::vec3 position;
		glm::vec3 rotation;
	};
}