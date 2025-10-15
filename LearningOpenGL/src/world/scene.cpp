#include "scene.h"

#include "renderer/render_context.h"
#include "renderer/render_system.h"
#include "ui/debug_overlay.h"
#include "core/time.h"

constexpr float DEFAULT_SHININESS = 32.0f;

static 
void draw_skybox(RenderingContext* ctx) {
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

static 
void draw_wooden_containers(const RenderingContext* ctx) {
	const Shader* shader = &ctx->assets.shaders[SHADER_CONTAINER];
	// use_shader(shader);
	const u32 diffuse    = ctx->assets.textures[TEXTURE_DIFFUSE];
	const u32 specular   = ctx->assets.textures[TEXTURE_SPECULAR];
	
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


static 
void draw_light_sources(const RenderingContext* ctx) {
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
static 
void show_model_normals(const RenderingContext* ctx, const Model* model, const glm::mat4 &model_matrix) {
	const Shader* shader = &ctx->assets.shaders[SHADER_NORMALS];

	use_shader(shader);
	apply_matrices(shader);
	set_mat4(shader, "model_matrix", model_matrix);
	draw_model(model, shader);
}

static 
void draw_world_models(const RenderingContext* ctx) {
	const Shader* bp_shader = &ctx->assets.shaders[SHADER_BACKPACK];
	const Model* bp_model 	= &ctx->assets.models[MODEL_BACKPACK];

	const Shader* blahaj_shader = &ctx->assets.shaders[SHADER_BLAHAJ];
	const Model* blahaj_model 	= &ctx->assets.models[MODEL_BLAHAJ];
	
	const Shader* explode_shader = &ctx->assets.shaders[SHADER_EXPLODE_MODEL];
	glm::mat4 model;

	// Drawing backpack model.
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


	// Drawing blahaj models.	
	for (int i = 0; i < ctx->world.blahaj_positions.size(); i++) {
		use_shader(blahaj_shader);
		apply_matrices(blahaj_shader);
		set_float(blahaj_shader, "material.shininess", DEFAULT_SHININESS);
		process_lighting(blahaj_shader, ctx);
		
		float angle = 20.0f * i;
		model = glm::mat4(1.0f);
		model = glm::translate(model, ctx->world.blahaj_positions[i]);
		model = glm::scale(model, glm::vec3(1.5f));
		model = glm::rotate(model, Time::get_time() * glm::radians(angle), glm::vec3(1.0f, 2.5f, 0.5f));
		set_mat4(blahaj_shader, "model_matrix", model);
		draw_model(blahaj_model, blahaj_shader);
//		show_model_normals(ctx, blahaj_model, model);
	}

	
	// Drawing the weird exploding backpack thingy with the geometry shader.
	use_shader(explode_shader);
	apply_matrices(explode_shader);
	process_lighting(explode_shader, ctx);
	set_float(explode_shader, "material.shininess", DEFAULT_SHININESS);
	set_float(explode_shader, "time", Time::get_time());

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -15.0f));
	model = glm::scale(model, glm::vec3(0.25f));
	model = glm::rotate(model, Time::get_time() * glm::radians(20.0f), glm::vec3(1.0f));
	set_mat4(explode_shader, "model_matrix", model);
	draw_model(bp_model, explode_shader);
}


static 
void draw_world(RenderingContext* ctx) {
	// Draw skybox-related things first.
	draw_skybox(ctx);

	// Draw actual world objects (the spinning, floating ones + models + lights)
	draw_wooden_containers(ctx);
	draw_light_sources(ctx);
	draw_world_models(ctx);

	// Draw room + grass (includes grassland and grass) + windows last
}


static 
void render_instanced_quads(const RenderingContext* ctx) {
	const Shader* shader = &ctx->assets.shaders[SHADER_INSTANCE_EXAMPLE];

	use_shader(shader);
	// apply_matrices(shader);
	for (unsigned int i = 0; i < 100; i++) {
		set_vec2(shader, ("offsets[" + std::to_string(i) + "]").c_str(), ctx->buffers.translations[i]);
	}

	glBindVertexArray(ctx->buffers.mini_quad_VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
}


void render_scene(RenderingContext* ctx, float dt) {
	// ----- Binding framebuffer + enabling depth testing
	glBindFramebuffer(GL_FRAMEBUFFER, ctx->buffers.FBO);
	glEnable(GL_DEPTH_TEST);


	// ----- Clearing screen and calculating sky color -----
	ctx->lighting.apply_sky_color(Time::get_time());
	glClearColor(ctx->lighting.current_sky_color.r,
				 ctx->lighting.current_sky_color.g,
			     ctx->lighting.current_sky_color.b,
				 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// ----- Updating uniform buffer with camera matrices -----
	update_camera_projection(ctx);

	
	// ----- Draw world objects -----
	draw_world(ctx);

	// ----- Instance rendering example -----
//	render_instanced_quads(ctx);

	// ----- Unbinding the framebuffer + disabling depth testing -----
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);


	// ----- Applying post-processing using the screen shader -----
	apply_render_mode_to_screen_shader(ctx);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ctx->assets.textures[TEXTURE_COLOR_BUFFER]);
	glBindVertexArray(ctx->buffers.quad_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	// ----- Resetting -----
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ----- Drawing UI elements -----
	if (ctx->debug_mode)
		render_debug_overlay(ctx, dt);
}
