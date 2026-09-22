#pragma once

#include <algorithm>
#include <map>
#include <iostream>
#include <random>
#include <glm/glm.hpp>
#include <glfw_imp.hpp>
#include <BaseStateClass.hpp>
#include <ShaderManagerClass.hpp>
#include <ModelClass.hpp>
#include <SkyboxClass.hpp>
#include <LightManagerClass.hpp>
#include <WorldModelClass.hpp>
#include <HUDManagerClass.hpp>
#include <PaintingModelClass.hpp>
#include <UtilClass.hpp>
#include "TAMMovementSolver.hpp"
#include "TAMPlayerController.hpp"

class MainState : public TAGBaseState {
public:
	MainState();
	std::string mainLoop();
	void enter() {};
	void exit() {};
	void framebufferSizeCallback();
	void mouseCallback();
	void iconifyCallback() {};
private:
	const float camera_speed = 14.0f;
	const float camera_height = 1.9f;
	const float total_height = 2.0f;
	const float capsule_rad = 0.8f;
	const float jump_accel = 200.0f;
	const float jump_duration = 0.1f;
	const glm::vec2 player_light_atten = { 0.0f, 0.02f };
	const glm::vec2 lamp_light_atten = { 0.0f, 0.02f };
	const glm::vec3 player_light = { 1.0f, 1.0f, 1.0f };
	const glm::vec3 lamp_light = { 1.0f, 1.0f, 1.0f };
	const static inline std::vector<std::string> camera_shader_names = { "skybox", "uninstanced", "instanced" };
	const static inline TAGTexLoader::Params model_params = { TAGTexParam::REPEAT_TEX, TAGTexParam::LINEAR_INTERP_PIX, TAGTexParam::LINEAR_INTERP_PIX, false, true };
	float clock = 0.0f;
	float jump_timer = 0.0f;
	bool end_game = false;
	bool grounded = false;
	glm::vec3 camera_velocity{};
	glm::vec3 lamp_pos{};
	glm::vec3 stable_position{};

	TAGLightManager<TAGLight::Point> point_lights = TAGLightManager<TAGLight::Point>(TAGResourceManager::BufferAccess::STATIC, 1);
	TAGLightManager<TAGLight::Flash> flash_lights = TAGLightManager<TAGLight::Flash>(TAGResourceManager::BufferAccess::STREAM, 1);

	TAGShaderManager shaders = TAGShaderManager({
		{ .name = "instanced", .shader_type = TAGShaderManager::ShaderType::INSTANCED_MODEL_DRAW },
		{ .name = "uninstanced", .shader_type = TAGShaderManager::ShaderType::UNINSTANCED_MODEL_DRAW },
		{ .name = "skybox", .shader_type = TAGShaderManager::ShaderType::SKYBOX_DRAW }
	});

	TAGPaintingModel images = TAGPaintingModel(
		{ "images/flat_man.png", "images/pineapple.png" },
		{ TAGTexParam::CLAMP_TO_EDGE_TEX, TAGTexParam::LINEAR_INTERP_PIX, TAGTexParam::LINEAR_INTERP_PIX, false, true },
		TAGResourceManager::BufferAccess::DYNAMIC,
		TAGMesh::Material()
	);

	TAGSkybox skybox = TAGSkybox("skybox/", { TAGTexParam::CLAMP_TO_EDGE_TEX, TAGTexParam::LINEAR_INTERP_PIX, TAGTexParam::LINEAR_INTERP_PIX, false, false });


	TAGModel lamp = TAGModel(model_params, TAGResourceManager::BufferAccess::STATIC, "lamp.txt");

	TAGWorldModel playground = TAGWorldModel(model_params, TAGResourceManager::BufferAccess::STATIC, "playground.txt");

	TAM::PlayerController controller;
	TAM::MovementSolver solver;

	glm::vec3 processInput();
	void setPerspectiveMatrix();
	void setCameraMatrix();
};
