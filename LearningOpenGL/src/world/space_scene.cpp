#include "space_scene.h"

#include "renderer/render_context.h"
#include "renderer/render_system.h"
#include "ui/debug_overlay.h"
#include "core/time.h"

namespace SpaceScene {
	constexpr u32 AMOUNT = 10000;
	glm::mat4 *model_matrices;

	const glm::vec3 SKY_COLOR = {0.1f, 0.1f, 0.1f};
}

// Internal function prototypes.
static void draw_space(RenderingContext* ctx);

void generate_rock_matrices(RenderingContext* ctx) {
	using namespace SpaceScene;
	model_matrices = new glm::mat4[AMOUNT]; // ABSOLUTELY MASSIVE

	srand(static_cast<u32>(Time::get_time()));
	float radius = 50.0f;
	float offset = 5.0f;

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
		model = glm::rotate(model, rotation_angle, glm::vec3(0.4f, 0.6f, 0.8f));


		// Finally, add the matrix data to the array of models.
		model_matrices[i] = model;
	}

	// After generating the matrices, bind them to the rock_buffer.
	auto buffers = &ctx->buffers;
	glGenBuffers(1, &buffers->rock_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->rock_buffer);
	glBufferData(GL_ARRAY_BUFFER, (AMOUNT * sizeof(glm::mat4)), &model_matrices[0],
				 GL_STATIC_DRAW);

	auto rock = &ctx->assets.models[MODEL_ROCK];
	for (u32 i = 0; i < rock->meshes.size(); i++) {
		u32 VAO = rock->meshes[i].VAO;
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, buffers->rock_buffer);
		
		// Vertex attributes
		std::size_t vec4_size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4_size), (void*)0);

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

void render_space_scene(RenderingContext* ctx, float dt) {
	using namespace SpaceScene;
	glBindFramebuffer(GL_FRAMEBUFFER, ctx->buffers.FBO);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_FRAMEBUFFER_SRGB);

	ctx->lighting.current_sky_color = SKY_COLOR;
	glClearColor(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	update_camera_projection(ctx);
	draw_space(ctx);

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
	
	draw_screen_texture(ctx);
	if (ctx->app.config.debug_mode)
		render_debug_overlay(ctx, dt);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void cleanup_space_scene() {
	using namespace SpaceScene;
	delete[] model_matrices;
	model_matrices = nullptr;
}



// Validation check for the rock buffers.
void validate_rock_instancing(RenderingContext* ctx) {
	using namespace SpaceScene;
	auto buffers = &ctx->buffers;
	auto rock = &ctx->assets.models[MODEL_ROCK];
	
	printf("\n========== ROCK INSTANCING VALIDATION ==========\n");
	
	// 1. Validate buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers->rock_buffer);
	GLint buffer_size = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
	printf("Rock buffer ID: %u\n", buffers->rock_buffer);
	printf("Buffer size: %d bytes (expected: %zu bytes)\n", 
	       buffer_size, AMOUNT * sizeof(glm::mat4));
	
	if (buffer_size == 0) {
		printf("ERROR: Buffer has no data!\n");
		return;
	}
	if (buffer_size != AMOUNT * sizeof(glm::mat4)) {
		printf("WARNING: Buffer size mismatch!\n");
	}
	
	// 2. Sample first matrix to verify data
	glm::mat4 test_matrix;
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), &test_matrix);
	printf("\nFirst matrix in buffer:\n");
	printf("  [%.2f %.2f %.2f %.2f]\n", test_matrix[0][0], test_matrix[1][0], test_matrix[2][0], test_matrix[3][0]);
	printf("  [%.2f %.2f %.2f %.2f]\n", test_matrix[0][1], test_matrix[1][1], test_matrix[2][1], test_matrix[3][1]);
	printf("  [%.2f %.2f %.2f %.2f]\n", test_matrix[0][2], test_matrix[1][2], test_matrix[2][2], test_matrix[3][2]);
	printf("  [%.2f %.2f %.2f %.2f]\n", test_matrix[0][3], test_matrix[1][3], test_matrix[2][3], test_matrix[3][3]);
	
	// Check if it's an identity matrix (would mean no transform)
	bool is_identity = (test_matrix[0][0] == 1.0f && test_matrix[1][1] == 1.0f && 
	                    test_matrix[2][2] == 1.0f && test_matrix[3][3] == 1.0f &&
	                    test_matrix[3][0] == 0.0f && test_matrix[3][1] == 0.0f && test_matrix[3][2] == 0.0f);
	if (is_identity) {
		printf("WARNING: First matrix is identity - rocks may be at origin!\n");
	}
	
	// 3. Validate rock model
	printf("\nRock model info:\n");
	printf("  Mesh count: %zu\n", rock->meshes.size());
	
	if (rock->meshes.empty()) {
		printf("ERROR: Rock has no meshes!\n");
		return;
	}
	
	// 4. Validate each mesh VAO
	for (u32 i = 0; i < rock->meshes.size(); i++) {
		printf("\n--- Mesh %u ---\n", i);
		printf("  VAO: %u\n", rock->meshes[i].VAO);
		printf("  Index count: %zu\n", rock->meshes[i].indices.size());
		printf("  Vertex count: %zu\n", rock->meshes[i].vertices.size());
		
		if (rock->meshes[i].VAO == 0) {
			printf("ERROR: Invalid VAO!\n");
			continue;
		}
		
		glBindVertexArray(rock->meshes[i].VAO);
		
		// Check standard attributes (0, 1, 2)
		printf("  Standard attributes:\n");
		for (GLuint loc = 0; loc <= 2; loc++) {
			GLint enabled = 0;
			glGetVertexAttribiv(loc, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
			printf("    Location %u: %s\n", loc, enabled ? "ENABLED" : "DISABLED");
		}
		
		// Check instancing attributes (3, 4, 5, 6)
		printf("  Instancing attributes:\n");
		bool all_enabled = true;
		for (GLuint loc = 3; loc <= 6; loc++) {
			GLint enabled = 0;
			GLint divisor = 0;
			GLint size = 0;
			GLint stride = 0;
			GLvoid* pointer = nullptr;
			
			glGetVertexAttribiv(loc, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
			glGetVertexAttribiv(loc, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &divisor);
			glGetVertexAttribiv(loc, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
			glGetVertexAttribiv(loc, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
			glGetVertexAttribPointerv(loc, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);
			
			printf("    Location %u: %s", loc, enabled ? "ENABLED" : "DISABLED");
			
			if (enabled) {
				printf(", size=%d, stride=%d, divisor=%d, offset=%p", 
				       size, stride, divisor, pointer);
				
				if (divisor != 1) {
					printf(" [ERROR: divisor should be 1!]");
					all_enabled = false;
				}
				if (size != 4) {
					printf(" [ERROR: size should be 4!]");
					all_enabled = false;
				}
				if (stride != 64) {
					printf(" [ERROR: stride should be 64!]");
					all_enabled = false;
				}
			} else {
				printf(" [ERROR: should be enabled!]");
				all_enabled = false;
			}
			printf("\n");
		}
		
		if (!all_enabled) {
			printf("  RESULT: Instancing NOT properly configured!\n");
		} else {
			printf("  RESULT: Instancing appears correctly configured!\n");
		}
	}
	
	glBindVertexArray(0);
	
	// 5. Check for OpenGL errors
	GLenum err;
	bool had_errors = false;
	while ((err = glGetError()) != GL_NO_ERROR) {
		printf("\nOpenGL Error: 0x%x\n", err);
		had_errors = true;
	}
	
	if (!had_errors) {
		printf("\nNo OpenGL errors detected.\n");
	}
	
	printf("================================================\n\n");
}



// ----- Internal functions -----
static void draw_skybox(RenderingContext* ctx) {
	const Shader* shader  = &ctx->assets.shaders[SHADER_SKYBOX];
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
	glBindTexture(GL_TEXTURE_CUBE_MAP, ctx->assets.textures[TEXTURE_SPACE_SKYBOX]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Resetting depth function
	glDepthMask(GL_TRUE);
}

static void process_sunlight(const RenderingContext* ctx, const Shader* shader) {
	// Setting the camera's view position uniform
	set_vec3(shader, "view_position", ctx->camera_data.camera.position);

	// Setting the sunlight uniforms - adjust these...
	std::string base = "sunlight";
	set_vec3(shader, (base + ".position").c_str(), ctx->lighting.sunlight_position);
	set_vec3(shader, (base + ".ambient").c_str(), ctx->lighting.sunlight_color * 0.15f);
	set_vec3(shader, (base + ".diffuse").c_str(), ctx->lighting.sunlight_color * 2.0f);
	set_vec3(shader, (base + ".specular").c_str(), ctx->lighting.sunlight_color * 1.5f);

	set_float(shader, (base + ".constant").c_str(), 1.0f);
	set_float(shader, (base + ".linear").c_str(), 0.007f);
	set_float(shader, (base + ".quadratic").c_str(), 0.0002f);
}


static void draw_planets(RenderingContext* ctx) {
	// Drawing the sun.
	auto sun_shader = &ctx->assets.shaders[SHADER_SUN];
	use_shader(sun_shader);
	apply_matrices(sun_shader);

	glBindVertexArray(ctx->buffers.cube_VAO);
	set_vec3(sun_shader, "light_color", ctx->lighting.sunlight_color);
	glm::mat4 sun = glm::mat4(1.0f);
	sun = glm::translate(sun, ctx->lighting.sunlight_position);
	sun = glm::scale(sun, glm::vec3(5.0f));
	set_mat4(sun_shader, "model_matrix", sun);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// Drawing the planet thingymabob.
	auto planet_shader = &ctx->assets.shaders[SHADER_PLANET];
	auto planet = &ctx->assets.models[MODEL_PLANET];
	use_shader(planet_shader);
	apply_matrices(planet_shader);
	process_sunlight(ctx, planet_shader);

	set_float(planet_shader, "material.shininess", 8.0f);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f));
	model = glm::rotate(model, (Time::get_time() * glm::radians(1.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
	set_mat4(planet_shader, "model_matrix", model);
	draw_model(planet, planet_shader);
}

static void draw_instanced_rocks(RenderingContext* ctx) {
	using namespace SpaceScene;
	auto shader = &ctx->assets.shaders[SHADER_ASTEROID];
	auto rock   = &ctx->assets.models[MODEL_ROCK];
	use_shader(shader);
	apply_matrices(shader);
	process_sunlight(ctx, shader);
	set_float(shader, "material.shininess", 4.0f);

	// Manually binding the textures for now (since we aren't using draw_model() anymore).
	glActiveTexture(GL_TEXTURE0);	
	set_int(shader, "material.diffuse1", 0);
	glBindTexture(GL_TEXTURE_2D, rock->texture_ids[0]);

/*
	// Updating rotation matrices
	// @NOTE: This causes some frame drops so we stick with just drawing the models
	// in a static rotation.
	for (u32 i = 0; i < AMOUNT; i++) {
		float angle = 0.001f + (i / 100000.0f);
		model_matrices[i] = glm::rotate(model_matrices[i], 
			Time::get_time() * glm::radians(angle),
			glm::vec3((1.0f - angle), (angle + 0.2f), (angle + 1.0f)));
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, ctx->buffers.rock_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, AMOUNT * sizeof(glm::mat4), &model_matrices[0]);
*/

	// Then drawing the rocks
	for (u32 i = 0; i < rock->meshes.size(); i++) {
		glBindVertexArray(rock->meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)rock->meshes[i].indices.size(), GL_UNSIGNED_INT,	0, AMOUNT);
	}

	glBindVertexArray(0);

}

static void draw_space(RenderingContext* ctx) {
	using namespace SpaceScene;
	// Draw skybox first.
	draw_skybox(ctx);
	
	// Then drawing the planets.
	draw_planets(ctx);

	// Finally, drawing the instanced rocks.
	draw_instanced_rocks(ctx);
}
