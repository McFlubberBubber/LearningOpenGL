#include "space_scene.h"

#include "renderer/render_context.h"
#include "renderer/render_system.h"
#include "ui/debug_overlay.h"
#include "core/time.h"

// @TODO: Temporary stuff.
namespace SpaceScene {
	constexpr u32 AMOUNT = 1000;
	glm::mat4 *model_matrices;
}

// @TODO: Currently, the world models, specifically the TEXTURES of the models are kinda messed up,
// so we gotta dig into the mesh/model.cpp files to see why the textures are not being handled properly.
// Hopefully I get a fix into this next time? Because the planets looks absolutely HORRIBLE.
static 
void draw_space(RenderingContext* ctx) {
	using namespace SpaceScene;
	auto shader = &ctx->assets.shaders[SHADER_SPACE];
	auto planet = &ctx->assets.models[MODEL_PLANET];
	auto rock   = &ctx->assets.models[MODEL_ROCK];

	use_shader(shader);
	apply_matrices(shader);

	// Drawing the planet first.
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f));
	set_mat4(shader, "model_matrix", model);
	draw_model(planet, shader);

	// Then draw all the asteroids.
	for (u32 i = 0; i < AMOUNT; i++) {
		set_mat4(shader, "model_matrix", model_matrices[i]);
		draw_model(rock, shader);
	}
}


void generate_rock_matrices() {
	using namespace SpaceScene;
	model_matrices = new glm::mat4[AMOUNT]; // ABSOLUTELY MASSIVE

	srand(static_cast<u32>(Time::get_time()));
	float radius = 50.0f;
	float offset = 2.5f;

	for (u32 i = 0; i < AMOUNT; i++) {
		glm::mat4 model = glm::mat4(1.0f);

		// Step 1: Translation = displace along circle with radius in range [offset, -offset].
		float angle = (float)i / (float)AMOUNT * 360.0f;
		float displacement = (rand() & (s32)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z.
		
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;

		model = glm::translate(model, glm::vec3(x, y, z));


		// Step 2: Scale = between 0.05f and 0.25f.
		float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
		model = glm::scale(model, glm::vec3(scale));


		// Step 3: Rotation = add random rotation around a semi-randomly picked rotation axis vector.
		float rotation_angle = static_cast<float>((rand() % 360));
		model = glm::rotate(model, angle, glm::vec3(0.4f, 0.6f, 0.8f));


		// Finally, add the matrix data to the array of models.
		model_matrices[i] = model;
	}
}

void render_space_scene(RenderingContext* ctx, float dt) {
	// ----- Binding framebuffer + enabling depth testing
	glBindFramebuffer(GL_FRAMEBUFFER, ctx->buffers.FBO);
	glEnable(GL_DEPTH_TEST);


	// ----- Clearing screen and calculating sky color -----
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// ----- Updating uniform buffer with camera matrices -----
	update_camera_projection(ctx);


	// ----- Draw world objects -----
	glDisable(GL_BLEND);
	draw_space(ctx);
	glEnable(GL_BLEND);


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

void cleanup_space_scene() {
	using namespace SpaceScene;
	delete[] model_matrices;
	model_matrices = nullptr;
}