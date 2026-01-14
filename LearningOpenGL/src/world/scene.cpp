#include "scene.h"

#include "renderer/render_context.h"
#include "renderer/render_system.h"
#include "ui/debug_overlay.h"
#include "core/time.h"

namespace MainScene {
	static constexpr float DEFAULT_SHININESS = 32.0f;

	// Blahaj instance rendering.
	glm::mat4 *blahaj_matrices;
	
	static constexpr bool SHOW_DEBUG_DEPTH_MAP = false;
}

// Internal function prototypes.
static void update_sky(RenderingContext* ctx);
static void draw_world(RenderingContext* ctx);
static void render_instanced_quads(const RenderingContext* ctx);


void generate_blahaj_matrices(RenderingContext* ctx) {
	using namespace MainScene;
	u32 amount = (u32)ctx->world.blahaj_positions.size();
	blahaj_matrices = new glm::mat4[amount];

	for (u32 i = 0; i < amount; i++) {
		glm::mat4 model;
		
		// Translation.
		model = glm::translate(model, ctx->world.blahaj_positions[i]);

		// Random scaling
		float scale = static_cast<float>(rand()) / RAND_MAX * 1.0f + 0.5f;
		model = glm::scale(model, glm::vec3(scale));
		
		blahaj_matrices[i] = model;
	}

	// Bind the matricers to the buffer.
	auto buffers = &ctx->buffers;
	glGenBuffers(1, &buffers->blahaj_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->blahaj_buffer);
	glBufferData(GL_ARRAY_BUFFER, (amount * sizeof(glm::mat4)), &blahaj_matrices[0],
				 GL_STATIC_DRAW);

	auto blahaj = &ctx->assets.models[MODEL_BLAHAJ];
	for (u32 i = 0; i < blahaj->meshes.size(); i++) {
		u32 VAO = blahaj->meshes[i].VAO;
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, buffers->blahaj_buffer);

		// Vertex attributes
		std::size_t vec4_size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4_size),
							  (void*)0);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4_size),
							  (void*)(1 * vec4_size));

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4_size),
							  (void*) (2 * vec4_size));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4_size),
							  (void*) (3 * vec4_size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glBindVertexArray(0);
	}
}

void render_scene(RenderingContext* ctx, float dt) {
	glBindFramebuffer(GL_FRAMEBUFFER, ctx->buffers.FBO);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_FRAMEBUFFER_SRGB);
	
	update_sky(ctx);
	update_camera_projection(ctx);
	draw_world(ctx);
//	render_instanced_quads(ctx);

	// Multisampling branch
	if (!ctx->app.config.multisampling) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
	} else {
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

	// glEnable(GL_FRAMEBUFFER_SRGB);

	draw_screen_texture(ctx, MainScene::SHOW_DEBUG_DEPTH_MAP);
	if (ctx->app.config.debug_mode)
		render_debug_overlay(ctx, dt);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void cleanup_main_scene() {
	using namespace MainScene;
	delete[] blahaj_matrices;
	blahaj_matrices = nullptr;
}


// ----- Internal functions -----
static void draw_skybox(RenderingContext* ctx) {
	const Shader* shader = &ctx->assets.shaders[SHADER_SKYBOX];
	glm::mat4 view_matrix = glm::mat4(glm::mat3(get_view_matrix(&ctx->camera_data.camera)));

	// Depth test passes when values are equal to the depth buffer's content
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	// Adjusting the shader uniforms
	use_shader(shader);
	set_mat4(shader, "projection", ctx->camera_data.projection_matrix);
	set_mat4(shader, "view", view_matrix);

	// Drawing the skybox - we aren't using several active textures other
	// than the one cubemap, so we don't need to set the texture uniform.
	glBindVertexArray(ctx->buffers.skybox_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ctx->assets.textures[TEXTURE_MAIN_SKYBOX]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Resetting depth function
	glDepthMask(GL_TRUE);
}

static void draw_wooden_containers(const RenderingContext* ctx) {
	using namespace MainScene;
	auto shader = &ctx->assets.shaders[SHADER_CONTAINER];
	u32 diffuse    = ctx->assets.textures[TEXTURE_DIFFUSE];
	u32 specular   = ctx->assets.textures[TEXTURE_SPECULAR];
	
	bind_textures(shader, diffuse, specular, 0);
	set_float(shader, "material.shininess", DEFAULT_SHININESS);
	apply_matrices(shader);
	process_lighting(shader, ctx);

	// Drawing the cube 10 times
	glBindVertexArray(ctx->buffers.cube_VAO);
	for (u32 i = 0; i < ctx->world.cube_positions.size(); i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, ctx->world.cube_positions[i]);
		model = glm::translate(model, glm::vec3(0.0f, 0.51f, 0.0f));
		float angle = 20.0f + (i * 3);
		model = glm::rotate(model, Time::get_time() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		set_mat4(shader, "model_matrix", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}


static void draw_light_sources(const RenderingContext* ctx) {
	const Shader* shader = &ctx->assets.shaders[SHADER_LIGHT_CUBE];
	use_shader(shader);

	apply_matrices(shader);
	process_lighting(shader, ctx);
	glBindVertexArray(ctx->buffers.cube_VAO);

	// Drawing point lights.
	for (u32 i = 0; i < ctx->lighting.get_point_light_count(); i++) {
		set_vec3(shader, "light_color", ctx->lighting.point_light_colors[i]);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, ctx->lighting.point_light_positions[i]);
		model = glm::scale(model, glm::vec3(0.5f));
		set_mat4(shader, "model_matrix", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Drawing the one directional light (currently).
	glm::mat4 model = glm::mat4(1.0f);
	set_vec3(shader, "light_color", glm::vec3(1.0f));
	model = glm::translate(model, ctx->lighting.directional_light_dir);
	set_mat4(shader, "model_matrix", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

// Utility function for displaying the normals of a model using a geometry shader.
static void show_model_normals(const RenderingContext* ctx, const Model* model, const glm::mat4 &model_matrix) {
	const Shader* shader = &ctx->assets.shaders[SHADER_NORMALS];

	use_shader(shader);
	apply_matrices(shader);
	set_mat4(shader, "model_matrix", model_matrix);
	draw_model(model, shader);
}

static void draw_backpacks(const RenderingContext* ctx) {
	using namespace MainScene;
	auto bp_shader  = &ctx->assets.shaders[SHADER_BACKPACK];
	auto exp_shader = &ctx->assets.shaders[SHADER_EXPLODE_MODEL];
	auto bp_model   = &ctx->assets.models[MODEL_BACKPACK];
	glm::mat4 model;
	
	// Draw regular backpack first.
	use_shader(bp_shader);
	apply_matrices(bp_shader);
	set_float(bp_shader, "material.shininess", DEFAULT_SHININESS);
	process_lighting(bp_shader, ctx);
	
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -6.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::rotate(model, Time::get_time() * glm::radians(45.0f), glm::vec3(1.0f));
	set_mat4(bp_shader, "model_matrix", model);
	draw_model(bp_model, bp_shader);

	// Then, draw the exploding backpack thing.
	use_shader(exp_shader);
	apply_matrices(exp_shader);
	process_lighting(exp_shader, ctx);
	set_float(exp_shader, "material.shininess", DEFAULT_SHININESS);
	set_float(exp_shader, "time", Time::get_time());

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -15.0f));
	model = glm::scale(model, glm::vec3(0.25f));
	model = glm::rotate(model, Time::get_time() * glm::radians(20.0f), glm::vec3(1.0f));
	set_mat4(exp_shader, "model_matrix", model);
	draw_model(bp_model, exp_shader);
	
}

static void update_blahaj_matrices(RenderingContext* ctx) {
	using namespace MainScene;
	u32 amount = (u32)ctx->world.blahaj_positions.size();

	for (u32 i = 0; i < amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);

		// Translations.
		model = glm::translate(model, ctx->world.blahaj_positions[i]);

		// Rotations.
		float angle = 20.0f * (i + 0.1f);
		model = glm::rotate(model, Time::get_time() * glm::radians(angle),
							glm::vec3(1.0f, 2.5f, 0.5f));

		// Scaling.
//		float scale = static_cast<float>(rand()) / RAND_MAX * 1.0f + 0.5f;
//		model = glm::scale(model, glm::vec3(scale));

		blahaj_matrices[i] = model;
	}
	
	// Update the buffer.
	glBindBuffer(GL_ARRAY_BUFFER, ctx->buffers.blahaj_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (amount * sizeof(glm::mat4)), &blahaj_matrices[0]);
}

static void draw_blahajs(RenderingContext* ctx) {
	using namespace MainScene;
	auto shader  = &ctx->assets.shaders[SHADER_BLAHAJ];
	auto blahaj  = &ctx->assets.models[MODEL_BLAHAJ];

	// @TODO: There is a branch in here that specifies whether we draw the
	// the blahajs in an instance rendering way or the regular way which is
	// through the uniform mat4 matrix in a for loop. The trade-off is that
	// the instance rendering example doesn't rotate the models overtime
	// (yet). One of the recommendations I got was to upload all the data to
	// the GPU and allow the vertex shader to handle rotation calculations instead
	// of updating the rotation matrices on the CPU side. This prevents us from
	// calling an update_matrices() function every frame. But then again, this
	// depends on how many objects we are rendering. Since there are only
	// 5 models that are being instanced, doing this operation CPU-side isn't
	// as bad.

	// I am also realizing that after doing the space scene and this main scene
	// that an init_scene() may be in order since there are now some preparations
	// that need to be done before we begin drawing (setting up the instance
	// buffers mainly).

#if 0
	glm::mat4 model;

	for (int i = 0; i < ctx->world.blahaj_positions.size(); i++) {
		use_shader(shader);
		apply_matrices(shader);
		set_float(shader, "material.shininess", DEFAULT_SHININESS);
		process_lighting(shader, ctx);
		
		float angle = 20.0f * i;
		model = glm::mat4(1.0f);
		model = glm::translate(model, ctx->world.blahaj_positions[i]);
		model = glm::scale(model, glm::vec3(1.5f));
		model = glm::rotate(model, Time::get_time() * glm::radians(angle), glm::vec3(1.0f, 2.5f, 0.5f));
		set_mat4(shader, "model_matrix", model);
		draw_model(blahaj, shader);
		
		// show_model_normals(ctx, blahaj_model, model);
	}
	
#else
	update_blahaj_matrices(ctx); // For rotations.

	use_shader(shader);
	apply_matrices(shader);
	process_lighting(shader, ctx);
	set_float(shader, "material.shininess", DEFAULT_SHININESS);

	// Manually binding the textures for now.
	glActiveTexture(GL_TEXTURE0);
	set_int(shader, "material.diffuse1", 0);
	glBindTexture(GL_TEXTURE_2D, blahaj->texture_ids[0]);

	// Drawing the blahajs.
	for (u32 i = 0; i < blahaj->meshes.size(); i++) {
		glBindVertexArray((GLsizei)blahaj->meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)blahaj->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, (GLsizei)ctx->world.blahaj_positions.size());
	}
	
#endif
}

static void draw_world_models(RenderingContext* ctx) {
	draw_backpacks(ctx);
	draw_blahajs(ctx);
}

static void draw_floor(RenderingContext* ctx) {
	using namespace MainScene;
	auto shader  = &ctx->assets.shaders[SHADER_WALL];
	auto diffuse = ctx->assets.textures[TEXTURE_FLOOR];

	use_shader(shader);
	bind_textures(shader, diffuse, 0, 0);
	apply_matrices(shader);
	process_lighting(shader, ctx);
	set_float(shader, "texture_tiling", 64.0f);
	set_float(shader, "material.shininess", DEFAULT_SHININESS);
	glBindVertexArray(ctx->buffers.cube_VAO);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(50.0f, 0.01f, 50.0f));
	set_mat4(shader, "model_matrix", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


static void draw_world(RenderingContext* ctx) {
	// Draw skybox-related things first.
	draw_skybox(ctx);

	// Draw actual world objects (the spinning, floating ones + models + lights)
	draw_wooden_containers(ctx);
	// draw_floor(ctx);
	draw_world_models(ctx);
	draw_light_sources(ctx);

	// Draw room + grass (includes grassland and grass) + windows last
}


static void render_instanced_quads(const RenderingContext* ctx) {
	const Shader* shader = &ctx->assets.shaders[SHADER_INSTANCE_EXAMPLE];

	use_shader(shader);
	// apply_matrices(shader);
	for (unsigned int i = 0; i < 100; i++) {
		set_vec2(shader, ("offsets[" + std::to_string(i) + "]").c_str(), ctx->buffers.translations[i]);
	}

	glBindVertexArray(ctx->buffers.mini_quad_VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
}

static void update_sky(RenderingContext* ctx) {
	// Interpolating between dark and grey sky color
	// ctx->lighting.apply_sky_color(Time::get_time());
	ctx->lighting.current_sky_color = glm::vec3(0.5f, 0.8f, 0.9f); // Bright blue

	glClearColor(ctx->lighting.current_sky_color.r,
				 ctx->lighting.current_sky_color.g,
			     ctx->lighting.current_sky_color.b,
				 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*
// @TODO: Move to render_system.cpp?
static void draw_screen_texture(RenderingContext* ctx) {
	apply_render_mode_to_screen_shader(ctx);
	glActiveTexture(GL_TEXTURE0);
	
	if (ctx->app.multisampling)
		glBindTexture(GL_TEXTURE_2D, ctx->assets.textures[TEXTURE_SCREEN]);
	else
		glBindTexture(GL_TEXTURE_2D, ctx->assets.textures[TEXTURE_COLOR_BUFFER]);
	
	glBindVertexArray(ctx->buffers.quad_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
*/
