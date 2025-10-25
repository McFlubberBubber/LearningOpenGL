#include "render_system.h"

#include <map>
#include <iomanip>
#include <sstream>

#include "renderer/render_context.h"
#include "renderer/texture_handler.h"

#include "world/obj_init.h"

// Internal function prototypes.
static bool init_camera(CameraData* camera_data, ViewportState* viewport);
static bool init_textures(Assets* assets);
static bool init_buffers(RenderingContext* ctx);
static bool init_shaders(Assets* assets);
static bool init_models(Assets* assets);
static bool init_fonts(Assets* assets);

static void cleanup_buffers(BufferData* buffers);
static void cleanup_shaders(Assets* assets);
static void cleanup_models(Assets* assets);
static void cleanup_textures(Assets* assets);
static void cleanup_fonts(Assets* assets);


// Managing the rendering system
bool init_rendering_system (RenderingContext* context) {
	std::cout << "Initializing rendering context..." << std::endl;

	if (!init_camera(&context->camera_data, &context->viewport)) {
		std::cout << "ERROR: Failed to init_camera()" << std::endl;
		return false;
	}

	if (!init_textures(&context->assets)) {
		std::cout << "ERROR: Failed to init_textures()" << std::endl;
		return false;
	}
	
	if (!init_buffers(context)) {
		std::cout << "ERROR: Failed to init_buffers()" << std::endl;
		return false;
	}

	if (!init_shaders(&context->assets)) {
		std::cout << "ERROR: Failed to init_shaders()" << std::endl;
		return false;
	}

	if (!init_models(&context->assets)) {
		std::cout << "ERROR: Failed to init_models()" << std::endl;
		return false;
	}

	if (!init_fonts(&context->assets)) {
		std::cout << "ERROR: Failed to init_fonts()" << std::endl;
		return false;
	}

	if (!init_message_queue(context)) {
		std::cout << "ERROR: Failed to init_message_queue()" << std::endl;
		return false;
	}

	init_world_objects(&context->world);
	init_lighting(&context->lighting);

	std::cout << "----- Finished initializing rendering context! -----\n" << std::endl;
	return true;
}

bool cleanup_rendering_system (RenderingContext* ctx) {
	cleanup_buffers(&ctx->buffers);
	cleanup_shaders(&ctx->assets);
	cleanup_models(&ctx->assets);
	cleanup_textures(&ctx->assets);
	cleanup_fonts(&ctx->assets);
	
	std::cout << "===== Finished cleaning up rendering system! =====\n" << std::endl;
	return true;
}

// Helper functions
void apply_matrices(const Shader* shader) {
	set_uniform_buffer(shader, "matrices", 0);
}


// NOTE: This lighting function assumes most of the fragments shader is set up to handle
// exactly 4 point_lights that are in the main scene.
void process_lighting(const Shader* shader, const RenderingContext* ctx) {
	// Setting the camera's view position uniform
	set_vec3(shader, "view_position", ctx->camera_data.camera.position);
	
	// Directional lighting
	set_vec3(shader, "dir_light.direction", ctx->lighting.directional_light_dir);
	set_vec3(shader, "dir_light.ambient", ctx->lighting.directional_ambient);
	set_vec3(shader, "dir_light.diffuse", ctx->lighting.directional_diffuse);
	set_vec3(shader, "dir_light.specular", ctx->lighting.directional_specular);

	// Point lighting
	
	for (u32 i = 0; i < (ctx->lighting.get_point_light_count()); i++) {
		std::string base = "point_lights[" + std::to_string(i) + "]";

		set_vec3(shader, (base + ".position").c_str(), ctx->lighting.point_light_positions[i]);
		set_vec3(shader, (base + ".ambient").c_str(), ctx->lighting.point_light_colors[i] * 0.02f);
		set_vec3(shader, (base + ".diffuse").c_str(), ctx->lighting.point_light_colors[i] * 0.3f);
		set_vec3(shader, (base + ".specular").c_str(), ctx->lighting.point_light_colors[i] * 0.3f);

		set_float(shader, (base + ".constant").c_str(), 1.0f);
		set_float(shader, (base + ".linear").c_str(), 0.09f);
		set_float(shader, (base + ".quadratic").c_str(), 0.032f);
	}
	

	// Spot lighting
	set_vec3(shader, "spot_light.position", ctx->camera_data.camera.position);
	set_vec3(shader, "spot_light.direction", ctx->camera_data.camera.front);
	set_vec3(shader, "spot_light.ambient", 0.0f, 0.0f, 0.0f);
	// set_vec3(shader, "spot_light.diffuse", 1.0f, 1.0f, 1.0f);
	// set_vec3(shader, "spot_light.specular", 1.0f, 1.0f, 1.0f);
	set_vec3(shader, "spot_light.diffuse", 0.0f, 0.0f, 0.0f);  // Off
	set_vec3(shader, "spot_light.specular", 0.0f, 0.0f, 0.0f); // Off

	set_float(shader, "spot_light.constant", 1.0f);
	set_float(shader, "spot_light.linear", 0.09f);
	set_float(shader, "spot_light.quadratic", 0.032f);

	set_float(shader, "spot_light.cut_off", glm::cos(glm::radians(10.0f)));
	set_float(shader, "spot_light.outer_cut_off", glm::cos(glm::radians(15.0f)));

	// Setting the sky color uniform
	set_vec3(shader, "sky_color", ctx->lighting.current_sky_color);
	set_float(shader, "fog_distance", 2.0f);
}

// Assumes that we are only binding one of each texture
void bind_textures(const Shader* shader, u32 diffuse, u32 specular, u32 emission) {
	use_shader(shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse);
	set_int(shader, "material.diffuse1", 0);

	// Reuse the diffuse texture if the user has no specular texture being used
	glActiveTexture(GL_TEXTURE1);
	if (specular != 0)
		glBindTexture(GL_TEXTURE_2D, specular);
	else
		glBindTexture(GL_TEXTURE_2D, diffuse);
	set_int(shader, "material.specular1", 1);	


	// Handling emission (if needed).
	if (emission != 0) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emission);
		set_int(shader, "material.emission1", 2);
	}
}


void set_texture_uniforms(const Shader* shader, bool do_emission) {
	use_shader(shader);
	set_int(shader, "material.diffuse1", 0);
	set_int(shader, "material.specular1", 1);

	if (do_emission)
		set_int(shader, "material.emission1", 2);
}


void update_camera_projection(RenderingContext* ctx) {
	CameraData* cd = &ctx->camera_data;
	ViewportState* viewport = &ctx->viewport;

	cd->projection_matrix = glm::perspective(
		glm::radians(cd->camera.zoom),
		viewport->aspect_ratio,
		cd->near_plane,
		cd->far_plane);

	glm::mat4 view_matrix = get_view_matrix(&cd->camera);

	// Updating the matrices block
	glBindBuffer(GL_UNIFORM_BUFFER, ctx->buffers.UBO_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(ctx->camera_data.projection_matrix));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view_matrix));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void resize_framebuffer(RenderingContext* context, u32 width, u32 height) {
	update_message_queue(context);

	glBindTexture(GL_TEXTURE_2D, context->assets.textures[TEXTURE_COLOR_BUFFER]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, context->buffers.RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);	
}

void apply_render_mode_to_screen_shader(const RenderingContext* context) {
	const Shader* shader = &context->assets.shaders[SHADER_SCREEN];

	use_shader(shader);
	set_int(shader, "render_mode", static_cast<s32>(context->post_processing.mode));
}

void draw_screen_texture(RenderingContext* ctx) {
	apply_render_mode_to_screen_shader(ctx);
	glActiveTexture(GL_TEXTURE0);
	
	if (ctx->app.multisampling)
		glBindTexture(GL_TEXTURE_2D, ctx->assets.textures[TEXTURE_SCREEN]);
	else
		glBindTexture(GL_TEXTURE_2D, ctx->assets.textures[TEXTURE_COLOR_BUFFER]);
	
	glBindVertexArray(ctx->buffers.quad_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}



// ----- Internal functions -----
static bool init_camera(CameraData* camera_data, ViewportState* viewport) {
	camera_data->camera = create_camera(glm::vec3(0.0f, 0.0f, 10.0f));
	camera_data->aspect_ratio = viewport->aspect_ratio;

	camera_data->near_plane = 0.1f;
	camera_data->far_plane  = 1000.0f; // @HARDCODE: We can adjust this value to a render_distance option?

	// Calculating initializing projection matrix
	camera_data->projection_matrix = glm::perspective(
		glm::radians(camera_data->camera.zoom),
		camera_data->aspect_ratio,
		camera_data->near_plane,
		camera_data->far_plane);

	std::cout << "----- Finished initializing camera! -----\n" << std::endl;
	return true;
}

// 3D Cubes
static void setup_cube_buffers(BufferData* buffers, GeometryData* geometry) {
	glGenVertexArrays(1, &buffers->cube_VAO);
	glGenBuffers(1, &buffers->cube_VBO);
	glBindVertexArray(buffers->cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->cube_vertices), geometry->cube_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

// 2D walls
static void setup_wall_buffers(BufferData* buffers, GeometryData* geometry) {
	glGenVertexArrays(1, &buffers->wall_VAO);
	glGenBuffers(1, &buffers->wall_VBO);
	glBindVertexArray(buffers->wall_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->wall_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->wall_vertices), geometry->wall_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

// 2D lines
static void setup_line_buffers(BufferData* buffers, GeometryData* geometry) {
	glGenVertexArrays(1, &buffers->line_VAO);
	glGenBuffers(1, &buffers->line_VBO);
	glBindVertexArray(buffers->line_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->line_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->points), geometry->points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

static void setup_skybox_buffers(BufferData* buffers, GeometryData* geometry) {
	glGenVertexArrays(1, &buffers->skybox_VAO);
	glGenBuffers(1, &buffers->skybox_VBO);
	glBindVertexArray(buffers->skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->skybox_vertices), geometry->skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}


static void setup_special_cube_buffers(BufferData* buffers, GeometryData* geometry) {
	glGenVertexArrays(1, &buffers->special_cube_VAO);
	glGenBuffers(1, &buffers->special_cube_VBO);
	glBindVertexArray(buffers->special_cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->special_cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->cube_vertices_2), geometry->cube_vertices_2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

// Includes screen quad, framebuffer + renderbuffer
static bool setup_screen_buffers(BufferData* buffers, GeometryData* geometry, ViewportState* viewport, Assets* assets, ApplicationState* app) {
	const u32 width = viewport->width;
	const u32 height = viewport->height;

	u32 texture_color_buffer = assets->textures[TEXTURE_COLOR_BUFFER];
	u32 texture_screen       = assets->textures[TEXTURE_SCREEN];

	auto samples = app->sample_count;

	// Generating things...
	glGenVertexArrays(1, &buffers->quad_VAO);
	glGenBuffers(1, &buffers->quad_VBO);
	glGenFramebuffers(1, &buffers->FBO);
	glGenFramebuffers(1, &buffers->intermediate_FBO);	
	glGenRenderbuffers(1, &buffers->RBO);
	
	if (!app->multisampling) {
		// Frame buffer VAO + VBO
		glBindFramebuffer(GL_FRAMEBUFFER, buffers->FBO);
		glBindVertexArray(buffers->quad_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, buffers->quad_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->quad_vertices), geometry->quad_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


		// Texture color attachment
		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);


		// Setting render buffers
		glBindRenderbuffer(GL_RENDERBUFFER, buffers->RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  // Creating a depth + stencil render buffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffers->RBO);
		// The rule with knowing when to use an RBO is when you never need to sample data from a buffer,
		// then you should use a render buffer for that specific buffer. BUT if you do need to sample data
		// (like color and texture values), then you should use a texture attachment instead.

		// Checking if the frame buffer status is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		// @TODO: If we want to add an in-app toggle to multisampling, then we need to create new buffers here that would initialize them
		// since this approach checks if the bool is toggled at the start of the application, then moves on.
	} else { 
		// Framebuffer (multisampled)
		glBindFramebuffer(GL_FRAMEBUFFER, buffers->FBO);
		glBindVertexArray(buffers->quad_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, buffers->quad_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->quad_vertices), geometry->quad_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		// Color attachment (multisampled)
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_color_buffer);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_color_buffer, 0);

		// Renderbuffer (multisampled)
		glBindRenderbuffer(GL_RENDERBUFFER, buffers->RBO);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffers->RBO);

		// Checking if the frame buffer status is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// Configuring second post-processing framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, buffers->intermediate_FBO);

		// Using the screen texture
		glBindTexture(GL_TEXTURE_2D, assets->textures[TEXTURE_SCREEN]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, assets->textures[TEXTURE_SCREEN], 0);	

		// Logging again
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	return true;
}


// Currently sets up ONE uniform binding which are the matrices.
static void setup_uniform_buffers(BufferData* buffers, GeometryData* geometry) {
	glGenBuffers(1, &buffers->UBO_matrices);
	glBindBuffer(GL_UNIFORM_BUFFER, buffers->UBO_matrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, buffers->UBO_matrices, 0, 2 * sizeof(glm::mat4));
}

// Might get removed later.
static void setup_instance_buffers(BufferData* buffers, GeometryData* geometry) {
	glGenVertexArrays(1, &buffers->mini_quad_VAO);
	glGenBuffers(1, &buffers->mini_quad_VBO);
	glGenBuffers(1, &buffers->instance_VBO);

	// Instance rendering example
	glBindVertexArray(buffers->mini_quad_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->mini_quad_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->mini_quad_vertices), geometry->mini_quad_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

	// Instanced data stuff here
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &buffers->translations[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glVertexAttribDivisor(2, 1); // specifies index of the vertex attribute + number of instance passes
}


static void setup_textbox_buffers(BufferData* buffers, GeometryData* geometry) {
	glGenVertexArrays(1, &buffers->textbox_VAO);
	glGenBuffers(1, &buffers->textbox_VBO);
	glGenBuffers(1, &buffers->textbox_EBO);

	// VBO for dynamic vertex data.
	glBindVertexArray(buffers->textbox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->textbox_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, NULL, GL_DYNAMIC_DRAW); // 4 Vertices * 2 components.

	// EBO for the static indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->textbox_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(geometry->indices), geometry->indices, GL_STATIC_DRAW);

	// Position attributes.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

static bool init_buffers(RenderingContext* ctx) {
	auto buffers  = &ctx->buffers;
	auto geometry = &ctx->geometry;
	auto viewport = &ctx->viewport;
	auto assets   = &ctx->assets;
	auto app      = &ctx->app;

	setup_cube_buffers(buffers, geometry);
	setup_wall_buffers(buffers, geometry);
	setup_line_buffers(buffers, geometry);

	setup_skybox_buffers(buffers, geometry);
	setup_special_cube_buffers(buffers, geometry);

	setup_screen_buffers(buffers, geometry, viewport, assets, app);
	setup_uniform_buffers(buffers, geometry);

	setup_instance_buffers(buffers, geometry); // May get removed?

	setup_textbox_buffers(buffers, geometry);

	// Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
	std::cout << "----- Finished initializing buffers! -----\n" << std::endl;
	return true;
}


// @Hardcode: similar to init_textures()
static bool init_shaders(Assets* assets) {
	std::string base = "res/shaders/";

	struct ShaderPaths {
		const char* vertex;
		const char* fragment;
		const char* geometry;
	};
	
	// Must match the order seen in the ShaderType struct.
	// @TODO: We gotta organize ts.
	const ShaderPaths shader_paths[] = {
		// Cube things.
		{"cubes/container.vert", "cubes/container.frag", nullptr},
		{"cubes/container.vert", "cubes/emission.frag", nullptr},
		{"cubes/container.vert", "cubes/light_cube.frag", nullptr},

		// Useless stuff for now.
		{"wall/wall.vert", "wall/wall.frag", nullptr},
		{"wall/wall.vert", "wall/wall.frag", nullptr},
		{"cubes/container.vert", "cubes/window.frag", nullptr},
		{"cubes/container.vert", "cubes/grass.frag", nullptr},

		// Models in the main scene.
		{"backpack/backpack.vert", "backpack/backpack.frag", nullptr},
		{"blahaj/blahaj.vert", "blahaj/blahaj.frag", nullptr},
		{"geometry/explode_model.vert", "geometry/explode_model.frag", "geometry/explode_model.geom"},

		// Core things.
		{"screenbuffer.vert", "screenbuffer.frag", nullptr},
		{"text/font.vert", "text/font.frag", nullptr},
		{"geometry/geometry.vert", "geometry/geometry.frag", "geometry/geometry.geom"},

		// Skybox-related things.
		{"skybox/skybox.vert", "skybox/skybox.frag", nullptr},
		{"cubes/special_cube.vert", "cubes/reflection.frag", nullptr},
		{"cubes/special_cube.vert", "cubes/refraction.frag", nullptr},

		// Misc.
		{"geometry/normals.vert", "geometry/normals.frag", "geometry/normals.geom"},
		{"instancing/instance_render.vert", "instancing/instance_render.frag", nullptr},

		// For the queue system.
		{"text/textbox.vert", "text/textbox.frag", nullptr},

		// For the space scene.
		{"space/asteroid.vert", "space/asteroid.frag", nullptr},
		{"space/planet.vert", "space/planet.frag", nullptr},
		{"space/sun.vert", "space/sun.frag", nullptr}
	};

	// Creating each shader
	for (u32 i = 0; i < SHADER_COUNT; i++) {
		std::string vertex_path = base + shader_paths[i].vertex;
		std::string fragment_path = base + shader_paths[i].fragment;
		
		// If there is a geometry path given, then create the shader with it.
		std::string geometry_path = shader_paths[i].geometry ? (base + shader_paths[i].geometry) : "";

		assets->shaders[i] = create_shader(vertex_path.c_str(), fragment_path.c_str(), shader_paths[i].geometry ? geometry_path.c_str() : nullptr);


		// Error logging
		if (!assets->shaders[i].is_valid) {
			std::cout << "ERROR: Failed to create shader: " << i << std::endl;
			return false;
		}
	}

	std::cout << "----- Finished initializing shaders! -----\n" << std::endl;
	return true;
}

// @Hardcode: check note in render_data.h
static bool init_models(Assets* assets) {
	std::string base = "res/models/";

	struct ModelPath {
		const char* path;
		bool flip_UVs;
	};

	// The true / false statement is there to specify whether the UVs should be flipped or not.
	const ModelPath model_paths[] = {
		{"backpack/backpack.obj", true},
		{"blahaj/blahaj.obj", false},
		{"planet/planet.obj", true},
		{"rock/rock.obj", true}
	};


	// Initializing each model
	for (u32 i = 0; i < MODEL_COUNT; i++) {
		assets->models[i] = create_model();

		if (model_paths[i].path != nullptr) {
			std::string full_path = base + model_paths[i].path;
			if (!load_model(&assets->models[i], full_path.c_str(), model_paths[i].flip_UVs, assets)) {
				std::cout << "ERROR: Failed to load model " << i << " at path: " << full_path << std::endl;
				return false;
			}
		}
	}

	std::cout << "----- Finished initializing models! -----\n" << std::endl;
	return true;
}

// @Hardcode: check note in render_data.h to see note regarding this function.
static bool init_textures(Assets* assets) {
	std::string base = "res/textures/";

	// Array of texture paths (must match order of TextureType):
	const char* texture_2D_paths[] = {
		"container2.png",
		"container2_specular.png",
		"matrix.jpg",
		
		"dark_wooden_planks.jpg",
		"wallpaper.jpg",
		"transparent_window.png",
		"grass.png",
		"grassland.jpg",
		
		nullptr,  // Main skybox
		nullptr,  // Space skybox
		
		nullptr,  // Color buffer
		nullptr   // Screen texture
	};

	for (u32 i = 0; i < TEXTURE_COUNT; i++) {
		if (texture_2D_paths[i] != nullptr) {
			if(!add_2D_texture(assets, (TextureType)i, (base + texture_2D_paths[i]).c_str()))
				return false;
		}
	}

	// Loading cubemap textures
	std::vector<std::string> main_skybox = {
		base + "main_skybox/right.jpg",
		base + "main_skybox/left.jpg",
		base + "main_skybox/top.jpg",
		base + "main_skybox/bottom.jpg",
		base + "main_skybox/front.jpg",
		base + "main_skybox/back.jpg",
	};
	if (!add_cubemap_texture(assets, TEXTURE_MAIN_SKYBOX, main_skybox))
		return false;

	std::vector<std::string> space_skybox = {
		base + "space_skybox/right.png",
		base + "space_skybox/left.png",
		base + "space_skybox/top.png",
		base + "space_skybox/bottom.png",
		base + "space_skybox/front.png",
		base + "space_skybox/back.png",
	};
	if (!add_cubemap_texture(assets, TEXTURE_SPACE_SKYBOX, space_skybox))
		return false;

	// Loading texture color buffers - texture binding happens in init_buffers();
	glGenTextures(1, &assets->textures[TEXTURE_COLOR_BUFFER]);
	glGenTextures(1, &assets->textures[TEXTURE_SCREEN]);

	std::cout << "----- Finished initializing textures! -----\n" << std::endl;
	return true;
}


// @TODO: This could probably be improved, but we just want to get this working
// for the time being. We can do the same thing with using loops to match the
// FontType structure.
static bool init_fonts(Assets* assets) {
//	std::string base = "res/fonts/";
	for (u32 i = 0; i < FONT_COUNT; i++) {
		assets->fonts[i] = create_font();
	}


	// Loading fonts that are in the scene's HUD 
	if (!load_font(&assets->fonts[FONT_REGULAR], "res/fonts/Merriweather_24pt-Regular.ttf", 42)) {
		return false;
	}

	if (!load_font(&assets->fonts[FONT_BOLD], "res/fonts/Merriweather_24pt-Bold.ttf", 42)) {
		return false;
	}

	if (!load_font(&assets->fonts[FONT_ITALIC], "res/fonts/Merriweather_24pt-Italic.ttf", 42)) {
		return false;
	}


	// Loading fonts that are seen in the menu
	if (!load_font(&assets->fonts[FONT_TITLE], "res/fonts/Merriweather_24pt-Italic.ttf", 96)) {
		return false;
	}
	
	if (!load_font(&assets->fonts[FONT_BODY], "res/fonts/Merriweather_24pt-Bold.ttf", 60)) {
		return false;
	}
	
	if (!load_font(&assets->fonts[FONT_SMALL], "res/fonts/Merriweather_24pt-Bold.ttf", 24)) {
		return false;
	}

	std::cout << "----- Finished initializing fonts! -----\n" << std::endl;
	return true;
}

// @TODO: We have the functionality coded here since I don't know where else to dump
// a 'buffer manager' in. We could maybe move it to the render_data.h directly?
static void cleanup_buffers(BufferData* buffers) {
	if (buffers == nullptr) return;

	// Helper lambdas.
	auto delete_VAO = [](u32 &VAO){
		if (VAO != 0) {
			glDeleteVertexArrays(1, &VAO);
			VAO = 0;
		}
	};

	auto delete_VBO = [](u32 &VBO){
		if (VBO != 0) {
			glDeleteBuffers(1, &VBO);
			VBO = 0;
		}
	};

	auto delete_EBO = [](u32 &EBO) {
		if (EBO != 0) {
			glDeleteBuffers(1, &EBO);
			EBO = 0;
		}
	};


	auto delete_FBO = [](u32 &FBO){
		if (FBO != 0) {
			glDeleteFramebuffers(1, &FBO);
			FBO = 0;
		}
	};

	auto delete_RBO = [](u32 &RBO){
		if (RBO != 0) {
			glDeleteRenderbuffers(1, &RBO);
			RBO = 0;
		}
	};


	// Doing cleanup here.
	delete_VAO(buffers->cube_VAO);
	delete_VBO(buffers->cube_VBO);
	delete_VAO(buffers->wall_VAO);
	delete_VBO(buffers->wall_VBO);
	delete_VAO(buffers->line_VAO);
	delete_VBO(buffers->line_VBO);

	delete_VAO(buffers->skybox_VAO);
	delete_VBO(buffers->skybox_VBO);
	delete_VAO(buffers->special_cube_VAO);
	delete_VBO(buffers->special_cube_VBO);

	delete_VAO(buffers->quad_VAO);
	delete_VBO(buffers->quad_VBO);
	delete_VBO(buffers->FBO); 		   // Frame buffer
	delete_VBO(buffers->RBO); 		   // Render buffer
	delete_VBO(buffers->UBO_matrices); // Uniform buffer

	// Instance rendering example
	delete_VAO(buffers->mini_quad_VAO);
	delete_VBO(buffers->mini_quad_VBO);
	delete_VBO(buffers->instance_VBO);

	delete_VAO(buffers->textbox_VAO);
	delete_VBO(buffers->textbox_VBO);
	delete_EBO(buffers->textbox_EBO);
}

static void cleanup_shaders(Assets* assets) {
	for (int i = 0; i < SHADER_COUNT; i++) {
		destroy_shader(&assets->shaders[i]);
	}
}

static void cleanup_models(Assets* assets) {
	for (int i = 0; i < MODEL_COUNT; i++) {
		destroy_model(&assets->models[i]);
	}
}

static void cleanup_textures(Assets* assets) {
	for (int i = 0; i < TEXTURE_COUNT; i++) {
		destroy_texture(&assets->textures[i]);
	}
}

static void cleanup_fonts(Assets* assets) {
	for (int i = 0; i < FONT_COUNT; i++) {
		destroy_font(&assets->fonts[i]);
	}
}

