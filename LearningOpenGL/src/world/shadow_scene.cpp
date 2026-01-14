#include "world/shadow_scene.h"

#include "renderer/render_context.h"
#include "renderer/render_system.h"

#include "ui/debug_overlay.h"

#include "core/time.h"

// Globals for now.
namespace ShadowScene {
	static constexpr float NEAR_PLANE = 0.1f;
	static constexpr float FAR_PLANE  = 100.0f;

	static constexpr float LIGHT_FRUSTUM = 10.0f;
	static const glm::mat4 LIGHT_PROJECTION = glm::ortho(-LIGHT_FRUSTUM, LIGHT_FRUSTUM, -LIGHT_FRUSTUM, LIGHT_FRUSTUM, NEAR_PLANE, FAR_PLANE);
	static const glm::mat4 LIGHT_VIEW = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
													glm::vec3( 0.0f, 0.0f,  0.0f),
													glm::vec3( 0.0f, 1.0f,  0.0f));
	static const glm::mat4 LIGHT_SPACE_MATRIX = LIGHT_PROJECTION * LIGHT_VIEW;
	static const glm::vec3 LIGHT_POS = glm::vec3(-2.0f, 5.0f, -3.0f);

	static constexpr bool SHOW_DEBUG_DEPTH_MAP = true;
}

static void draw_world_with_shader(RenderingContext* ctx, Shader* shader) {
	use_shader(shader);

	// Floor
	glBindVertexArray(ctx->buffers.cube_VAO);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(25.0f, 0.01f, 25.0f));
	set_mat4(shader, "model_matrix", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	// Cubes
	glm::vec3 cube_positions[3] = {
		glm::vec3( 0.0f, 1.5f, 0.0f),
		glm::vec3( 2.0f, 3.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 2.0f)
	};
	
	for (int i = 0; i < 3; ++i) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, cube_positions[i]);
		model = glm::scale(model, glm::vec3(0.5f));
		float angle = 20.0f + (i * 3);
		model = glm::rotate(model, Time::get_time() * glm::radians(angle), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		set_mat4(shader, "model_matrix", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Model
	auto backpack = &ctx->assets.models[MODEL_BACKPACK];
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(0.25f));
	model = glm::rotate(model, Time::get_time() * glm::radians(20.0f), glm::vec3(1.0f));
	set_mat4(shader, "model_matrix", model);
	draw_model(backpack, shader);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void init_shadow_scene(RenderingContext* ctx) {
	using namespace ShadowScene;
	auto shadow_shader = &ctx->assets.shaders[SHADER_SHADOW_MAPPING];
	use_shader(shadow_shader);
	set_int(shadow_shader, "diffuse", 0);
	set_int(shadow_shader, "shadow_map", 1);

	auto screen_shader = &ctx->assets.shaders[SHADER_SCREEN];
	use_shader(screen_shader);
	set_int(screen_shader, "depth_map", 1);
}

void render_shadow_scene(RenderingContext* ctx, float dt) {
	using namespace ShadowScene;
	glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
	update_camera_projection(ctx);
	
	// 1. First, render depth of scene to the texture (from light's perspective).	
	glBindFramebuffer(GL_FRAMEBUFFER, ctx->buffers.depth_map_FBO);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_DEPTH_BUFFER_BIT);

	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);

	auto depth_shader = &ctx->assets.shaders[SHADER_DEPTH];
	use_shader(depth_shader);
	set_mat4(depth_shader, "light_space_matrix", LIGHT_SPACE_MATRIX);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ctx->assets.textures[TEXTURE_FLOOR]);
	draw_world_with_shader(ctx, depth_shader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glCullFace(GL_BACK);
	
	// 2. Render scene normally using shadow map.
	glViewport(0, 0, ctx->viewport.width, ctx->viewport.height);
	glBindFramebuffer(GL_FRAMEBUFFER, ctx->buffers.FBO);
	// glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shadow_shader = &ctx->assets.shaders[SHADER_SHADOW_MAPPING];
	use_shader(shadow_shader);
	apply_matrices(shadow_shader);
	set_vec3(shadow_shader, "view_pos", ctx->camera_data.camera.position);
	set_vec3(shadow_shader, "light_pos", LIGHT_POS);
	set_mat4(shadow_shader, "light_space_matrix", LIGHT_SPACE_MATRIX);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ctx->assets.textures[TEXTURE_FLOOR]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ctx->assets.textures[TEXTURE_DEPTH_MAP]);
	draw_world_with_shader(ctx, shadow_shader);

	// 3. Resolve MSAA if enabled.
	if (ctx->app.config.multisampling) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, ctx->buffers.FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ctx->buffers.intermediate_FBO);
		glBlitFramebuffer(0, 0,
						  ctx->viewport.width, ctx->viewport.height, 0, 0,
						  ctx->viewport.width, ctx->viewport.height,
						  GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
	}
	
	// 4. Draw screen texture.
	draw_screen_texture(ctx, true);
	if (ctx->app.config.debug_mode)
		render_debug_overlay(ctx, dt);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

