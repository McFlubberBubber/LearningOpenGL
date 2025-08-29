#include "scene.h"

#include "renderer/render_context.h"
#include "renderer/render_system.h"
#include "ui/debug_overlay.h"

void render_scene(RenderingContext* context, float dt) {
	// We call update_camera_projection() and view_matrix should be getting
	// updated on every process_mouse_movement call
		
	// ----- Binding framebuffer + enabling depth testing
	glBindFramebuffer(GL_FRAMEBUFFER, context->buffers.FBO);
	glEnable(GL_DEPTH_TEST);


	// @TODO: ----- Clearing screen and calculating sky color -----
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// ----- Updating uniform buffer with camera matrices -----
	glBindBuffer(GL_UNIFORM_BUFFER, context->buffers.UBO_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(context->camera_data.projection_matrix));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(context->camera_data.camera.view_matrix));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	// @TODO: ----- Draw world objects -----


	// ----- Unbinding the framebuffer + disabling depth testing -----
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);


	// ----- Applying post-processing using the screen shader -----
	apply_render_mode_to_screen_shader(context);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, context->assets.textures[TEXTURE_COLOR_BUFFER]);
	glBindVertexArray(context->buffers.quad_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	
	// ----- Resetting -----
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);


	// ----- Drawing UI elements -----
	render_debug_overlay(context, dt);
}
