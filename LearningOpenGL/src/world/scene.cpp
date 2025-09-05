#include "scene.h"

#include "renderer/render_context.h"
#include "renderer/render_system.h"
#include "ui/debug_overlay.h"
#include "core/time.h"


// @TODO: Specular lighting seems kinda messed up?
void draw_wooden_containers(const RenderingContext* ctx) {
	const Shader* shader = &ctx->assets.shaders[SHADER_CONTAINER];
	// use_shader(shader);
	const u32 diffuse    = ctx->assets.textures[TEXTURE_DIFFUSE];
	const u32 specular   = ctx->assets.textures[TEXTURE_SPECULAR];
	
	bind_textures(shader, diffuse, specular, 0);
	set_float(shader, "u_material.shininess", 32.0f);
	apply_matrices(shader);
	process_lighting(shader, ctx);

	// Drawing the cube 10 times
	glBindVertexArray(ctx->buffers.cube_VAO);
	for (u32 i = 0; i < ctx->world.cube_positions.size(); i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, ctx->world.cube_positions[i]);
		model = glm::translate(model, glm::vec3(0.0f, 0.51f, 0.0f));
		//enabling rotations
		float angle = 20.0f + (i * 3);
		model = glm::rotate(model, Time::get_time() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		set_mat4(shader, "u_modelMatrix", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}


// @Incomplete: Do this.
void draw_world(const RenderingContext* ctx) {
	//
	// Draw skybox-related things first.
	//
	


	//
	// Draw actual world objects (the spinning, floating ones + models + lights)
	//
	draw_wooden_containers(ctx);


	// 
	// Draw room + grass (includes grassland and grass) + windows last
	//
}


void render_scene(RenderingContext* ctx, float dt) {
	// We call update_camera_projection() and view_matrix should be getting
	// updated on every process_mouse_movement call.
		
	// ----- Binding framebuffer + enabling depth testing
	glBindFramebuffer(GL_FRAMEBUFFER, ctx->buffers.FBO);
	glEnable(GL_DEPTH_TEST);


	// @TODO: ----- Clearing screen and calculating sky color -----
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// ----- Updating uniform buffer with camera matrices -----
	glBindBuffer(GL_UNIFORM_BUFFER, ctx->buffers.UBO_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(ctx->camera_data.projection_matrix));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(ctx->camera_data.camera.view_matrix));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	// @TODO: ----- Draw world objects -----
	draw_world(ctx);

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
	render_debug_overlay(ctx, dt);
}
