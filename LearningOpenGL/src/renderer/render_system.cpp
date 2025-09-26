#include "render_system.h"

#include <map>
#include <iomanip>
#include <sstream>

#include "renderer/render_context.h"
#include "renderer/texture_handler.h"


/*


So I am leaving *most* of these draw calls here, commented out for the time being
until they get properly implemented into our scene.cpp (which currently has all
the updated draw calls organized over there). The reason for these staying here
for a bit longer is because I can use these as a reference if I ever need to draw
anything from here again. -Nathan, 16 sep 2025


//        
// ========== DRAWING OBJECTS ==========
//
void drawEmissionContainer() {
	bind_textures(emissionShader, diffuseMap, specularMap, emissionMap);
	applyMatrixes(emissionShader);
	emissionShader.setFloat("u_material.shininess", 32.0f);
	processLighting(emissionShader);

	//drawing emission cube
	glBindVertexArray(cubeVAO);
	glm::mat4 emissionCubeModel = glm::mat4(1.0f);
	emissionCubeModel = glm::translate(emissionCubeModel, glm::vec3(5.0f, -2.0f, 7.0f));
	//enabling rotations
	float angle = 20.0f;
	emissionCubeModel = glm::rotate(emissionCubeModel, Time::get_time() *glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	emissionShader.setMat4("u_modelMatrix", emissionCubeModel);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

// @TODO The house vectors are successfully drawn at the right world pos, but the
// texturing of the model is currently messed up (possibly due to naming conventions
// or due to file formatting?
void drawHouse() {
	houseShader.useProgram();
	applyMatrixes(houseShader);
	houseShader.setFloat("u_material.shininess", 32.0f);
	processLighting(houseShader);

	glm::mat4 houseModel = glm::mat4(1.0f);
	houseModel = glm::translate(houseModel, glm::vec3(12.5f, -4.5f, 0.0f));
	houseModel = glm::scale(houseModel, glm::vec3(0.3f));
	houseModel = glm::rotate(houseModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	houseShader.setMat4("u_modelMatrix", houseModel);
	house.Draw(houseShader);
}

void drawGrassLand() {
	bind_textures(floorShader, grassLandTexture, 0, 0);
	floorShader.setFloat("u_material.shininess", 1.0f);
	floorShader.setFloat("u_textureTiling", 64.0f);
	applyMatrixes(floorShader);
	processLighting(floorShader);

	glBindVertexArray(cubeVAO);
	glm::mat4 floorModel = glm::mat4(1.0f);
	floorModel = glm::translate(floorModel, glm::vec3(-10.0f, -5.0f, 5.0f));
	floorModel = glm::scale(floorModel, glm::vec3(100.0f, 0.001f, 100.0f));
	floorShader.setMat4("u_modelMatrix", floorModel);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawFoliage() {
	bind_textures(grassShader, grassTexture, 0, 0);
	grassShader.setFloat("u_material.shininess", 32.0f);
	applyMatrixes(grassShader);
	processLighting(grassShader);

	glBindVertexArray(wallVAO);
	for (unsigned int i = 0; i < foliagePositions.size(); i++) {
		glm::mat4 grassModel = glm::mat4(1.0f);
		grassModel = glm::translate(grassModel, foliagePositions[i]);
		grassModel = glm::rotate(grassModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		grassShader.setMat4("u_modelMatrix", grassModel);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void drawGrass() {
	drawGrassLand();
	drawFoliage();
}

void drawWindows(const Camera& camera) {
	bind_textures(windowShader, windowTexture, 0, 0);
	windowShader.setFloat("u_material.shininess", 64.0f);
	applyMatrixes(windowShader);
	processLighting(windowShader);

	// Mapping the window positions based on which is the farthest away from
	// the camera. This allows us to draw the furthest ones away first to ensure
	// the windows will render properly behind eachother.
	std::map<float, glm::vec3> sortedWindows;
	for (unsigned int i = 0; i < windowPositions.size(); i++)
	{
		float distance = glm::length(camera.position - windowPositions[i]);
		sortedWindows[distance] = windowPositions[i];
	}

	//Drawing the windows at their positions based on the map
	for (std::map<float, glm::vec3>::reverse_iterator it = sortedWindows.rbegin(); it != sortedWindows.rend(); ++it)
	{
		glm::mat4 windowModel = glm::mat4(1.0f);
		windowModel = glm::translate(windowModel, it->second);
		windowModel = glm::rotate(windowModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		windowShader.setMat4("u_modelMatrix", windowModel);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}


void drawFloor() {
	bind_textures(floorShader, floorTexture, 0, 0);
	floorShader.setFloat("u_material.shininess", 32.0f);
	floorShader.setFloat("u_textureTiling", 10.0f);
	applyMatrixes(floorShader);
	processLighting(floorShader);

	glBindVertexArray(cubeVAO);
	glm::mat4 floorModel = glm::mat4(1.0f);
	floorModel = glm::translate(floorModel, glm::vec3(-10.0f, -5.0f, 5.0f));
	floorModel = glm::scale(floorModel, glm::vec3(10.0f, 0.1f, 10.0f));
	floorShader.setMat4("u_modelMatrix", floorModel);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawWalls() {
	bind_textures(wallShader, wallTexture, 0, 0);
	wallShader.setFloat("u_material.shininess", 16.0f);
	wallShader.setFloat("u_textureTiling", 10.0f);
	applyMatrixes(wallShader);
	processLighting(wallShader);

	for (int i = 0; i < wallPositions.size(); i++) {
		glBindVertexArray(cubeVAO);
		glm::mat4 wallModel = glm::mat4(1.0f);
		wallModel = glm::translate(wallModel, wallPositions[i]);
		wallModel = glm::rotate(wallModel, glm::radians(wallRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f));
		wallModel = glm::scale(wallModel, glm::vec3(0.1f, 2.0f, 10.0f)); // thickness, height, length
		wallShader.setMat4("u_modelMatrix", wallModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void drawRoom() {
	drawFloor();
	drawWalls();
}


void draw_lines() {
	line_shader.useProgram();
	glBindVertexArray(line_VAO);
	glDrawArrays(GL_POINTS, 0, 4);
}

void cleanupScene() {
	delete_skybox_buffers();
	delete_vertex_data(cubeVAO, cubeVBO);	
	delete_vertex_data(wallVAO, wallVBO);
	delete_vertex_data(quadVAO, quadVBO);	

	glDeleteBuffers(1, 		 &EBO);
	glDeleteBuffers(1, 		 &UBO_matrices);	
	glDeleteFramebuffers(1,  &FBO);
	glDeleteRenderbuffers(1, &RBO);
}
*/


bool init_camera(CameraData* camera_data, ViewportState* viewport) {
	camera_data->camera = create_camera(glm::vec3(0.0f, 0.0f, 10.0f));
	camera_data->aspect_ratio = viewport->aspect_ratio;

	camera_data->near_plane = 0.1f;
	camera_data->far_plane  = 100.0f;

	// Calculating initializing projection matrix
	camera_data->projection_matrix = glm::perspective(
		glm::radians(camera_data->camera.zoom),
		camera_data->aspect_ratio,
		camera_data->near_plane,
		camera_data->far_plane);

	std::cout << "----- Finished initializing camera! -----" << std::endl;
	return true;
}


bool init_buffers(BufferData* buffers, GeometryData* geometry, ViewportState* viewport, u32 texture_color_buffer) {
	const u32 width  = viewport->width;
	const u32 height = viewport->height;

	// Generating vertex arrays + buffers
	glGenVertexArrays(1, &buffers->cube_VAO);
	glGenBuffers(1,		 &buffers->cube_VBO);
	glGenVertexArrays(1, &buffers->wall_VAO);
	glGenBuffers(1,	     &buffers->wall_VBO);
	glGenVertexArrays(1, &buffers->line_VAO);
	glGenBuffers(1, 	 &buffers->line_VBO);
	glGenVertexArrays(1, &buffers->skybox_VAO);
	glGenBuffers(1,		 &buffers->skybox_VBO);
	glGenVertexArrays(1, &buffers->special_cube_VAO);
	glGenBuffers(1,		 &buffers->special_cube_VBO);

//	glGenBuffers(1,		  &buffers->EBO);					// Not needed right now.
	glGenVertexArrays(1,  &buffers->quad_VAO);
	glGenBuffers(1,		  &buffers->quad_VBO);
	glGenFramebuffers(1,  &buffers->FBO);
	glGenRenderbuffers(1, &buffers->RBO);

	glGenBuffers(1, &buffers->UBO_matrices);


	// 3D Cubes
	glBindVertexArray(buffers->cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->cube_vertices), geometry->cube_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// 2D Rendering walls / rectangles
	glBindVertexArray(buffers->wall_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->wall_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->wall_vertices), geometry->wall_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// 2D line rendering
	glBindVertexArray(buffers->line_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->line_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->points), geometry->points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// Skybox stuff
	glBindVertexArray(buffers->skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->skybox_vertices), geometry->skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	// Special cube (reflection + refractive cubes)
	glBindVertexArray(buffers->special_cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->special_cube_VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->cube_vertices_2), geometry->cube_vertices_2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// Currently not being used, but it's a good reminder as to how to
	// render a rectangle with the indices being utilised.
	/*
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndices), wallIndices, GL_STATIC_DRAW);
	*/


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
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);		// Creating a depth + stencil render buffer
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


	// Binding a uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, buffers->UBO_matrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, buffers->UBO_matrices, 0, 2 * sizeof(glm::mat4));

	
	std::cout << "----- Finished initializing buffers! -----" << std::endl;
	return true;
}


// @Hardcode: similar to init_textures()
bool init_shaders(Assets* assets) {
	std::string base = "res/shaders/";

	struct ShaderPaths {
		const char* vertex;
		const char* fragment;
		const char* geometry;
	};
	
	// Must match the order seen in the ShaderType struct.
	const ShaderPaths shader_paths[] = {
		{"container.vert", "container.frag", nullptr},
		{"container.vert", "emission.frag", nullptr},
		{"container.vert", "light_cube.frag", nullptr},

		{"wall.vert", "wall.frag", nullptr},
		{"wall.vert", "wall.frag", nullptr},
		{"container.vert", "window.frag", nullptr},
		{"container.vert", "grass.frag", nullptr},

		{"backpack.vert", "backpack.frag", nullptr},
		{"blahaj.vert", "blahaj.frag", nullptr},
		{"container.vert", "container.frag", nullptr},
		{"explode_model.vert", "explode_model.frag", "explode_model.geom"},

		{"screenbuffer.vert", "screenbuffer.frag", nullptr},
		{"font.vert", "font.frag", nullptr},
		{"geometry.vert", "geometry.frag", "geometry.geom"},

		{"skybox.vert", "skybox.frag", nullptr},
		{"special_cube.vert", "reflection.frag", nullptr},
		{"special_cube.vert", "refraction.frag", nullptr},
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

	std::cout << "----- Finished initializing shaders! -----" << std::endl;
	return true;
}

// @Hardcode: check note in render_data.h
bool init_models(Assets* assets) {
	std::string base = "res/models/";

	struct ModelPath {
		const char* path;
		bool flip_UVs;
	};

	const ModelPath model_paths[] = {
		{"backpack/backpack.obj", true},
		{"blahaj/blahaj.obj", false},
		{"house/house.fbx", true},
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

	std::cout << "----- Finished initializing models! -----" << std::endl;
	return true;
}

// @Hardcode: check note in render_data.h to see note regarding this function.
bool init_textures(Assets* assets) {
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
		
		nullptr,
		nullptr
	};

	for (u32 i = 0; i < TEXTURE_COUNT; i++) {
		if (texture_2D_paths[i] != nullptr) {
			if(!add_2D_texture(assets, (TextureType)i, (base + texture_2D_paths[i]).c_str()))
				return false;
		}
	}


	// Loading cubemap textures
	std::vector<std::string> skybox_faces = {
		base + "skybox/right.jpg",
		base + "skybox/left.jpg",
		base + "skybox/top.jpg",
		base + "skybox/bottom.jpg",
		base + "skybox/front.jpg",
		base + "skybox/back.jpg",
	};
	if (!add_cubemap_texture(assets, TEXTURE_SKYBOX, skybox_faces))
		return false;


	// Loading texture color buffers - texture binding happens in init_buffers();
	glGenTextures(1, &assets->textures[TEXTURE_COLOR_BUFFER]);

	std::cout << "----- Finished initializing textures! -----" << std::endl;
	return true;
}


// @TODO: This could probably be improved, but we just want to get this working
// for the time being. We can do the same thing with using loops to match the
// FontType structure.
bool init_fonts(Assets* assets) {
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

	std::cout << "----- Finished initializing fonts! -----" << std::endl;
	return true;
}


// Managing the rendering system
bool init_rendering_system (RenderingContext* context) {
	std::cout << "Initializing rendering context..." << std::endl;

	// Error logging
	if (!init_camera(&context->camera_data, &context->viewport)) {
		std::cout << "ERROR: Failed to init_camera()" << std::endl;
		return false;
	}


	if (!init_textures(&context->assets)) {
		std::cout << "ERROR: Failed to init_textures()" << std::endl;
		return false;
	}
	
	if (!init_buffers(&context->buffers, &context->geometry, &context->viewport, context->assets.textures[TEXTURE_COLOR_BUFFER])) {
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


	std::cout << "----- Finished initializing rendering context! -----" << std::endl;
	return true;
}


// @Incomplete: do - Cleanup functions
void cleanup_buffers(BufferData* buffers) {}
void cleanup_shaders(Assets* assets) {}
void cleanup_models(Assets* assets) {}
void cleanup_textures(Assets* assets) {}
void cleanup_fonts(Assets* assets) {}

bool cleanup_rendering_system (RenderingContext* context) {
	return true;
}



// Helper functions
void apply_matrices(const Shader* shader) {
	set_uniform_buffer(shader, "matrices", 0);
}

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
		set_vec3(shader, (base + ".ambient").c_str(), ctx->lighting.point_light_colors[i] * 0.1f);
		set_vec3(shader, (base + ".diffuse").c_str(), ctx->lighting.point_light_colors[i]);
		set_vec3(shader, (base + ".specular").c_str(), ctx->lighting.point_light_colors[i]);

		set_float(shader, (base + ".constant").c_str(), 1.0f);
		set_float(shader, (base + ".linear").c_str(), 0.09f);
		set_float(shader, (base + ".quadratic").c_str(), 0.032f);
	}

	// Spot lighting
	set_vec3(shader, "spot_light.position", ctx->camera_data.camera.position);
	set_vec3(shader, "spot_light.direction", ctx->camera_data.camera.front);
	set_vec3(shader, "spot_light.ambient", 0.0f, 0.0f, 0.0f);
	set_vec3(shader, "spot_light.diffuse", 1.0f, 1.0f, 1.0f);
	set_vec3(shader, "spot_light.specular", 1.0f, 1.0f, 1.0f);

	set_float(shader, "spot_light.constant", 1.0f);
	set_float(shader, "spot_light.linear", 0.09f);
	set_float(shader, "spot_light.quadratic", 0.032f);

	set_float(shader, "spot_light.cut_off", glm::cos(glm::radians(10.0f)));
	set_float(shader, "spot_light.outer_cut_off", glm::cos(glm::radians(15.0f)));

	// Setting the sky color uniform
	set_vec3(shader, "sky_color", ctx->lighting.current_sky_color);
	set_float(shader, "fog_distance", 3.0f);
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

void resize_framebuffer(const RenderingContext* context, u32 width, u32 height) {
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
