#pragma once

#include <ModBase.hpp>
#include <BaseStateClass.hpp>
#include <glm/glm.hpp>

/**
* Simple movement and camera controls for a first-person, mouse-controlled player character
*/
class TAM::PlayerController {
public:
	float x_rotation = 0.0f; // Total rotation around x axis
	float y_rotation = 0.0f; // Total rotation around y axis
	float sens = 0.001f; // Camera speed factor
	float max_x = (twpi / 4.0f) * 0.98f; // Max rotation around x axis
	float max_y = -1.0f; // Max rotation around y axis (negative value means no cap)
	int forward_key = GLFW_KEY_W;
	int backwards_key = GLFW_KEY_S;
	int left_key = GLFW_KEY_A;
	int right_key = GLFW_KEY_D;

	/*
	* Get camera direction from changes in mouse position across x and y axis.
	* @param delta_x Change is mouse position across width of screen
	* @param delta_y Change is mouse position across height of screen
	*/
	inline glm::vec3 cameraDirection(const float delta_x = (float)TAGBaseState::delta_x, const float delta_y = (float)TAGBaseState::delta_y) {
		if (max_x >= 0.0f) {
			x_rotation = glm::clamp(delta_y * sens + x_rotation, -max_x, max_x);
		}
		else {
			x_rotation += (x_rotation > twpi ? -twpi : (x_rotation < -twpi ? twpi : 0)) + delta_y * sens;
		}

		if (max_y >= 0.0f) {
			y_rotation = glm::clamp(delta_x * sens + y_rotation, -max_y, max_y);
		}
		else {
			y_rotation += (y_rotation > twpi ? -twpi : (y_rotation < -twpi ? twpi : 0)) + delta_x * sens;
		}

		return glm::normalize(glm::vec3(glm::cos(y_rotation) * glm::cos(x_rotation), glm::sin(x_rotation), glm::sin(y_rotation) * glm::cos(x_rotation)));
	}

	/**
	* Get player movement direction based on key presses.
	* @param camera_direction Direction camera is facing
	* @param camera_up Normal to the plane of movement
	*/
	inline glm::vec3 playerDirection(const glm::vec3& camera_direction = TAGBaseState::camera_direction, const glm::vec3& camera_up = TAGBaseState::camera_up) const {
		if (glm::dot(camera_direction, camera_direction) == 0.0f || glm::dot(camera_up, camera_up) == 0.0f) return {};

		glm::vec3 camera_velocity{};
		const glm::vec3 right = glm::normalize(glm::cross(camera_direction, camera_up));
		const glm::vec3 forward = glm::normalize(glm::cross(camera_up, right));

		if (TAGBaseState::getKeyState(forward_key)) camera_velocity += forward;

		if (TAGBaseState::getKeyState(backwards_key)) camera_velocity -= forward;

		if (TAGBaseState::getKeyState(left_key)) camera_velocity -= right;

		if (TAGBaseState::getKeyState(right_key)) camera_velocity += right;

		const float length_sq = glm::dot(camera_velocity, camera_velocity);

		if (length_sq > 0.0001f && glm::abs(length_sq - 1.0f) > 0.0001f) camera_velocity /= glm::sqrt(length_sq);

		return camera_velocity;
	}
private:
	static inline constexpr float twpi = 6.28318530718f;
};
