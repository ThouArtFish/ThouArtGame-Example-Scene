#pragma once

#include <ModBase.hpp>
#include <WorldModelClass.hpp>
#include <glm/glm.hpp>

/**
* Calculate a new vector from various factors such as collided surfaces, gravity and friction
*/
class TAM::MovementSolver {
public:
	static inline unsigned int MAX_ITERATIONS = 5; // Number of iterations on collisions to ensure velocity does not clip

	glm::vec3 grav_dir = { 0.0f, -1.0f, 0.0f }; // Direction of gravity, keep normalized for accurate results
	float grav_accel = 15.0f; // Acceleration of object under gravity
	float bounce_factor = 0.0f; // Factor of velocity perpendicular to surface returned after clipping into surface
	float friction = 0.5f; // Factor of weight used for friction
	bool front_side_collisions = true; // Treat all collisions as if they had happened on the front face of the plane

	/**
	* Calculate the new velocity of an object to avoid clipping with each of the surfaces in collision vector.
	* Applies gravitational acceleration and friction if enabled.
	*/
	inline glm::vec3 calculateVelocity(const float dt, const std::vector<Collision::Info>& collisions, const glm::vec3& velocity, const glm::vec3& accel = {}) const {
		// Calculate desired velocity 
		glm::vec3 final_vel = velocity + (grav_dir * grav_accel + accel) * dt;

		// Filter for collision normals
		std::vector<glm::vec3> normals;
		normals.reserve(collisions.size());
		for (const auto& coll : collisions) {
			if (front_side_collisions && glm::dot(coll.collision_plane.normal, coll.plane.frag_plane.normal) < 0.0f) {
				normals.push_back(glm::normalize(coll.collision_plane.normal - glm::dot(coll.collision_plane.normal, coll.plane.frag_plane.normal) * coll.plane.frag_plane.normal));
			}
			else {
				normals.push_back(coll.collision_plane.normal);
			}
		}

		// Iterate clip removal phase multiple times
		for (size_t i = 0; i < MAX_ITERATIONS; i++) {
			bool clipping = false;

			for (const glm::vec3& normal : normals) {
				const float vel_dot = glm::dot(normal, final_vel);

				if (vel_dot < 0.0f || (i == 0 && vel_dot < 0.0001f)) {
					clipping = true;
				}
				else {
					continue;
				}

				// Perpendicular comp and parallel comp of velocity to surface normal
				glm::vec3 perp_comp = normal * vel_dot;
				glm::vec3 para_comp = final_vel - perp_comp;

				// Apply friction 
				const float para_length = glm::length(para_comp);
				if (para_length > 0.0001f && i == 0) {
					para_comp -= (para_comp / para_length) * glm::min(para_length, glm::max(0.0f, -glm::dot(grav_dir * grav_accel * dt, normal) * friction));
				}

				final_vel = para_comp - (i == 0 ? perp_comp * bounce_factor : glm::vec3(0.0f));
			}

			if (!clipping) break;
		}

		return final_vel;
	}
};
