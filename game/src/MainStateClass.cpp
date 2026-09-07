#include "../include/MainStateClass.hpp"

MainState::MainState() {
	//Set camera position
	camera_position = glm::vec3(0.0f, 2.0f, 0.0f);
	stable_position = camera_position;

	// Create paintings
	glm::vec3 stand = glm::vec3(0, -images.getMesh(image_names[0]).mesh_bb.min.y * 2.0f, 0);
	images.setInstance(
		{
			.position = stand + floor_elevation,
			.scale = 2.0f
		},
		-1,
		image_names[0]
	);
	stand = glm::vec3(0, -images.getMesh(image_names[1]).mesh_bb.min.y * 3.0f, 0);
	images.setInstance(
		{
			.position = glm::vec3(4.0f, 0.0f, 0.0f) + stand + floor_elevation,
			.angle = -glm::half_pi<float>(),
			.scale = 3.0f
		},
		-1,
		image_names[1]
	);

	// Create playground
	playground.setInstance(
		{
			.position = floor_elevation,
			.scale = 3.0f
		}
	);

	// Place lamp
	lamp_pos = glm::vec3(0, -lamp.getMesh("lampion").mesh_bb.min.y * 0.01f, -3.0f) + floor_elevation;

	// Create lights
	std::vector<TAGLight::Point> lights;
	lights.emplace_back(lamp_pos + glm::vec3(0.0f, 0.5f, 0.0f), lamp_light, light_atten);
	light_manager.setAllLights(lights);
	light_manager.bindToShader();

	// Set scene data
	light_manager.setScene({ .ambience = 0.1f });
	light_manager.bindSceneToShader();

	setPerspectiveMatrix();
}

std::string MainState::mainLoop() {
	// Check if window has been minimized
	if (iconified) return "CURRENT";

	// Check if player ended game
	if (end_game) return "END";

	// Check inputs
	glm::vec3 camera_velocity = processInput();

	// Check capsule body for collisions with terrain and get floor plane
	const glm::vec3 foot = stable_position - camera_up * camera_height;
	std::vector<Collision::Info> all_collisions = playground.capsuleCollision<Collision::ALL>(foot, camera_up * total_height, capsule_rad);
	const int floor_index = TAGWorldModel::rayCollision<Collision::CLOSEST>(foot, -camera_up, all_collisions, -1.0f);

	// If no floor plane is found then the player is in the air
	grounded = (floor_index >= 0);

	// If grounded, move across floor surface
	if (grounded) {
		camera_velocity = TAGUtil::perpendicularCompNorm(all_collisions[floor_index].plane.frag_plane.normal, camera_velocity);
		if (!just_jumped) { // Reset y change when landing on surface
			y_comp = 0.0f;
		}
	}
	else { // Otherwise, cause the player to fall
		y_comp = glm::max(-30.0f, y_comp + grav_accel * (float)delta_time);
		just_jumped = false;
	}

	// Move across any walls encountered as well
	for (size_t i = 0; i < all_collisions.size(); i++) {
		const TAGMesh::Plane& collision_plane = all_collisions[i].plane.frag_plane;
		if (i != floor_index && glm::dot(collision_plane.normal, camera_velocity) < 0.0f && collision_plane.normal.y >= 0.0f) {
			camera_velocity = TAGUtil::perpendicularCompNorm(collision_plane.normal, camera_velocity);
		}
	}

	// Rotate the man
	for (const TAGModel::Object& obj : images.getAllInstances("flat_man")) {
		images.setInstance(TAGPaintingModel::faceDirec(camera_position, obj, true));
	}

	// Apply camera position changes
	glm::vec3 bounce = glm::vec3(0);
	if (camera_velocity == glm::vec3(0) || !grounded) {
		const float y_diff = glm::abs(camera_position.y - stable_position.y);
		if (y_diff < 0.01f) {
			camera_position = stable_position;
		}
		else {
			camera_position = glm::mix(camera_position, stable_position, 1.0f - (y_diff / 0.91f));
		}
	}
	else {
		// Bounce while running
		bounce = glm::vec3(0.0f, glm::sin(clock), 0.0f);
		if (clock >= glm::two_pi<float>()) {
			clock -= glm::two_pi<float>();
		}
		else {
			clock += (float)delta_time * 10.0f;
		}
	}

	camera_position += (camera_velocity + bounce + glm::vec3(0.0f, y_comp, 0.0f)) * (float)delta_time;
	stable_position += (camera_velocity + glm::vec3(0.0f, y_comp, 0.0f)) * (float)delta_time;

	// Create "look at" matrix to translate objects to camera view space
	setCameraMatrix();

	TAGShaderManager::Shader shader = shaders.useShader(shader_names[1]);
	lamp.drawOne(shader, { .position = lamp_pos, .scale = 0.01f });

	shader = shaders.useShader(shader_names[0]);
	playground.drawAll(shader);
	for (const std::string& mesh_name : images.getMeshNames()) {
		TAGShaderManager::default_options.cull_backface = (mesh_name != image_names[0]);
		images.drawAll(shader, mesh_name);
	}
	TAGShaderManager::default_options.cull_backface = true;

	shader = shaders.useShader(shader_names[3]);
	skybox.draw(shader);

	return "CURRENT";
}

void MainState::enter() {
	//setWindowFullscreen(TAGEnum::TRUE);
	setMouseLock(TAGEnum::TRUE);
}

void MainState::exit() {
	setMouseLock(TAGEnum::FALSE);
}

// Window callback functions
void MainState::framebufferSizeCallback() {
	setPerspectiveMatrix();
}

void MainState::mouseCallback() {
	x_rotation = glm::max(-1.57f, glm::min(1.57f, ((float)delta_y * sens) + x_rotation));
	y_rotation += (float)(delta_x)*sens;
	camera_direction.x = glm::cos(y_rotation) * glm::cos(x_rotation);
	camera_direction.y = glm::sin(x_rotation);
	camera_direction.z = glm::sin(y_rotation) * glm::cos(x_rotation);
	camera_direction = glm::normalize(camera_direction);
}

// Game state functions
glm::vec3 MainState::processInput() {
	glm::vec3 camera_velocity = glm::vec3(0);
	const glm::vec3 right = glm::normalize(glm::cross(camera_direction, camera_up));
	const glm::vec3 forward = glm::normalize(glm::cross(camera_up, right));
	if (getKeyState(GLFW_KEY_W)) {
		camera_velocity += forward;
	}
	if (getKeyState(GLFW_KEY_S)) {
		camera_velocity -= forward;
	}
	if (getKeyState(GLFW_KEY_A)) {
		camera_velocity -= right;
	}
	if (getKeyState(GLFW_KEY_D)) {
		camera_velocity += right;
	}
	if (getKeyState(GLFW_KEY_ESCAPE)) {
		end_game = true;
	}
	if (getKeyState(GLFW_KEY_P) == 1) {
		setMouseLock(TAGEnum::TOGGLE);
		setWindowFullscreen(TAGEnum::TOGGLE);
	}
	if (grounded && getKeyState(GLFW_KEY_SPACE) == 1) {
		just_jumped = true;
		y_comp += jump_accel;
	}
	if (camera_velocity != glm::vec3(0)) {
		camera_velocity = glm::normalize(camera_velocity) * camera_speed;
	}
	return camera_velocity;
}

void MainState::setPerspectiveMatrix() {
	shaders.setAll<glm::mat4>(camera_shader_names, { TAGShaderManager::default_options.perspective_mat }, { createPerspectiveMatrix() });
}

void MainState::setCameraMatrix() {
	const glm::mat4 view = createCameraMatrix();
	shaders.setAll<glm::mat4>(camera_shader_names, { TAGShaderManager::default_options.camera_mat }, { glm::mat4(glm::mat3(view)), view });
}
