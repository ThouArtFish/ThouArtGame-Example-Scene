#include "../include/MainStateClass.hpp"

MainState::MainState() {
	//Set camera position
	stable_position = camera_up * (capsule_rad + camera_height) + glm::vec3(0.0f, 15.0f, 0.0f);
	camera_position = stable_position;

	// Create paintings
	glm::vec3 stand = glm::vec3(0, -images.getMesh("flat_man").mesh_bb.min.y * 2.0f, 0);
	images.setInstance(
		{
			.position = stand,
			.scale = 2.0f
		},
		-1,
		"flat_man"
	);
	stand = glm::vec3(0, -images.getMesh("pineapple").mesh_bb.min.y * 3.0f, 0);
	images.setInstance(
		{
			.position = glm::vec3(4.0f, 0.0f, 0.0f) + stand,
			.angle = -glm::half_pi<float>(),
			.scale = 3.0f
		},
		-1,
		"pineapple"
	);

	// Create playground
	playground.setInstance(
		{
			.scale = 4.0f
		}
	);

	// Place lamp
	lamp_pos = glm::vec3(0, -lamp.getMesh("lampion").mesh_bb.min.y * 0.01f, -3.0f);

	// Create lights
	point_lights.setLight({ lamp_pos + glm::vec3(0.0f, 0.5f, 0.0f), lamp_light, lamp_light_atten });
	point_lights.bindToShader();

	flash_lights.setLight({ camera_position, camera_direction, player_light, player_light_atten, glm::radians(30.0f)});
	flash_lights.bindToShader();

	// Set scene data
	point_lights.setScene({ .ambience = 0.1f });
	point_lights.bindSceneToShader();
	
	// Set perspective matrix
	setPerspectiveMatrix();
}

std::string MainState::mainLoop() {
	// Get delta time
	const float dt = (float)delta_time;

	// Check if window has been minimized
	if (iconified) return "CURRENT";

	// Check if player ended game
	if (end_game) return "END";

	// Get all collisions with player
	const glm::vec3 spine = camera_up * camera_height;
	const glm::vec3 foot = stable_position - spine;
	std::vector<Collision::Info> all_collisions = playground.capsuleCollision<Collision::ALL>(foot, spine, capsule_rad);
	grounded = !all_collisions.empty();

	// Check inputs
	const glm::vec3 input_vel = processInput();
	const bool any_move = (input_vel != glm::vec3(0.0f));
	
	// Calculate velocity based on collisions and gravity
	const glm::vec3 last_vel = (jump_timer > 0.0f || !grounded ? TAGUtil::parallelCompNorm(solver.grav_dir, camera_velocity) : glm::vec3());
	glm::vec3 jump{};
	if (jump_timer > 0.0f) {
		jump = -solver.grav_dir * jump_accel * (jump_timer < dt ? jump_timer / dt : 1.0f);
		jump_timer -= dt;
	}
	camera_velocity = solver.calculateVelocity(dt, all_collisions, input_vel + last_vel, jump);

	// Rotate the man
	for (const TAGModel::Object& obj : images.getAllInstances("flat_man")) {
		images.setInstance(TAGPaintingModel::faceDirec(stable_position, obj, true), 0, "flat_man");
	}

	// Bounce while running
	glm::vec3 bounce = glm::vec3(0);
	if (!any_move || !grounded) {
		const float y_diff = glm::abs(camera_position.y - stable_position.y);
		if (y_diff < 0.0001f) {
			camera_position = stable_position;
		}
		else {
			camera_position = glm::mix(camera_position, stable_position, 1.0f - (y_diff / 0.91f));
		}
	}
	else {
		bounce = glm::vec3(0.0f, glm::sin(clock) * 10.0f, 0.0f);
		if (clock >= glm::two_pi<float>()) {
			clock -= glm::two_pi<float>();
		}
		else {
			clock += dt * 10.0f;
		}
	}

	// Apply velocity
	stable_position += camera_velocity * dt;
	camera_position = stable_position + bounce * dt;

	// Change player flashlight to follow player
	flash_lights.setLightMember<FlashLightMemberName::POSITION>(stable_position, 0);
	flash_lights.setLightMember<FlashLightMemberName::DIRECTION>(camera_direction, 0);

	// Create "look at" matrix to translate objects to camera view space based on current camera direction
	setCameraMatrix();

	// Draw all objects using the correct shaders
	TAGShaderManager::Shader shader = shaders.useShader("uninstanced");
	lamp.drawOne(shader, { .position = lamp_pos, .scale = 0.01f });

	shader = shaders.useShader("instanced");
	playground.drawAll(shader);
	for (const std::string& mesh_name : images.getMeshNames()) {
		TAGShaderManager::default_options.cull_backface = (mesh_name == "flat_man");
		images.drawAll(shader, mesh_name);
	}
	TAGShaderManager::default_options.cull_backface = true;

	shader = shaders.useShader("skybox");
	skybox.draw(shader);

	// Continue with current state
	return "CURRENT";
}

// Window callback functions
void MainState::framebufferSizeCallback() {
	setPerspectiveMatrix();
}

void MainState::mouseCallback() {
	camera_direction = controller.cameraDirection();
}

// Game state functions
glm::vec3 MainState::processInput() {
	if (getKeyState(GLFW_KEY_ESCAPE)) end_game = true;	

	if (getKeyState(GLFW_KEY_SPACE) && grounded) jump_timer = jump_duration;

	if (getKeyState(GLFW_KEY_P) == 1) { // Toggle fullscreen and mouse lock
		setMouseLock(TAGEnum::TOGGLE);
		setWindowFullscreen(TAGEnum::TOGGLE);
	}

	return controller.playerDirection() * camera_speed;
}

// Set the perspective projection matrix for required shaders
void MainState::setPerspectiveMatrix() {
	shaders.setAll<glm::mat4>(camera_shader_names, { TAGShaderManager::default_options.perspective_mat }, { createPerspectiveMatrix() });
}

// Set the camera space matrix for required shaders
void MainState::setCameraMatrix() {
	const glm::mat4 view = createCameraMatrix();
	shaders.setAll<glm::mat4>(camera_shader_names, { TAGShaderManager::default_options.camera_mat }, { glm::mat4(glm::mat3(view)), view });
}
