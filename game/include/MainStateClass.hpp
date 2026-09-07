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

class MainState : public TAGBaseState {
	public:
		MainState();
		std::string mainLoop();
		void enter();
		void exit();
		void framebufferSizeCallback();
		void mouseCallback();
		void iconifyCallback() {};
	private:
		const float camera_speed = 14.0f;
		const float camera_height = 1.9f;
		const float total_height = 2.0f;
		const float capsule_rad = 0.8f;
		const float grav_accel = -10.0f;
		const float jump_accel = 20.0f;
		const float sens = 0.001f;
		const glm::vec2 light_atten = { 0.0f, 0.02f };
		const glm::vec3 floor_elevation = glm::vec3(0.0f, -20.0f, 0.0f);
		const glm::vec4 player_light = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
		const glm::vec4 lamp_light = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
		const static inline std::vector<std::string> shader_names = { "instanced", "uninstanced", "hud", "skybox" };
		const static inline std::vector<std::string> camera_shader_names = { shader_names[3], shader_names[0], shader_names[1] };
		const static inline std::vector<std::string> image_names = { "flat_man", "pineapple" };
		float clock = 0.0f;
		float x_rotation = 0.0f;
		float y_rotation = 0.0f;
		bool just_jumped = false;
		bool end_game = false;
		bool grounded = false;
		float y_comp = 0.0f;

		glm::vec3 lamp_pos;
		glm::vec3 stable_position;

		TAGLightManager<TAGLight::Point> light_manager = TAGLightManager<TAGLight::Point>(TAGResourceManager::BufferAccess::STREAM, 2);

		TAGShaderManager shaders = TAGShaderManager({
			{ .name = shader_names[0], .shader_type = TAGShaderManager::ShaderType::INSTANCED_MODEL_DRAW},
			{ .name = shader_names[1], .shader_type = TAGShaderManager::ShaderType::UNINSTANCED_MODEL_DRAW},
			{ .name = shader_names[2], .shader_type = TAGShaderManager::ShaderType::HUD_DRAW},
			{ .name = shader_names[3], .shader_type = TAGShaderManager::ShaderType::SKYBOX_DRAW}
		});

		TAGPaintingModel images = TAGPaintingModel(
			{ "images/flat_man.png", "images/pineapple.png" },
			{ TAGTexParam::CLAMP_TO_EDGE_TEX, TAGTexParam::LINEAR_INTERP_PIX, TAGTexParam::LINEAR_INTERP_PIX, false, true },
			TAGResourceManager::BufferAccess::DYNAMIC,
			TAGMesh::Material()
		);

		TAGSkybox skybox = TAGSkybox("skybox/", { TAGTexParam::CLAMP_TO_EDGE_TEX, TAGTexParam::LINEAR_INTERP_PIX, TAGTexParam::LINEAR_INTERP_PIX, false, false });

		static const inline TAGTexLoader::Params model_params = { TAGTexParam::REPEAT_TEX, TAGTexParam::LINEAR_INTERP_PIX, TAGTexParam::LINEAR_INTERP_PIX, false, true };

		TAGModel lamp = TAGModel(model_params, TAGResourceManager::BufferAccess::STATIC, "lamp.txt");

		TAGWorldModel playground = TAGWorldModel(model_params, TAGResourceManager::BufferAccess::STATIC, "playground.txt");

		glm::vec3 processInput();
		void setPerspectiveMatrix();
		void setCameraMatrix();
};
