#include "console.h"

#include "renderer/render_context.h"

static std::string console_state_to_string(ConsoleState state) {
	switch (state) {
		case ConsoleState::CLOSED:			return "CLOSED";
		case ConsoleState::OPEN_SMALL:		return "OPEN_SMALL";
		case ConsoleState::OPEN_BIG:		return "OPEN_BIG";
		default:							return "ERROR!";
	}
}

// This draw function could be moved to a more general draw source file instead of dumping it here.
static void draw_quad(const RenderingContext* ctx, float x, float y, float width, float height, const glm::vec3& color, float alpha) {
	auto shader = &ctx->assets.shaders[SHADER_TEXTBOX];

	use_shader(shader);
	const glm::mat4& ortho = ctx->viewport.ortho_projection;

	set_mat4(shader, "projection", ortho);
	set_vec3(shader, "color", color);
	set_float(shader, "alpha", alpha);

	float vertices[] = {
		// Positions
		x,			y,			// Bottom left
		x + width,	y,			// Bottom right
		x + width,	y + height,	// Top right
		x,			y + height, // Top left
	};

	// Binding + drawing the elements
	glBindVertexArray(ctx->buffers.textbox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, ctx->buffers.textbox_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void init_console(Console* console) {
	console->state = ConsoleState::CLOSED; 
};	

void draw_console(RenderingContext* ctx, Console* console) {
	// std::string state_string = console_state_to_string(console->state);
	// std::cout << "Drawing console in " << state_string << "\n";
	
	// Temporary variables, might move these to a namespace here.
	float console_width  = (float)ctx->viewport.width;
	float console_height = (float)ctx->viewport.height;
	float console_x = 0;
	float console_y = console_height * 0.7f;
	const glm::vec3 console_color { 0.0f, 0.2f, 0.4f };
	const float alpha = 0.9f;

	if (console->state == ConsoleState::OPEN_SMALL) {
		draw_quad(ctx, console_x, console_y, console_width, console_height, console_color, alpha);
	}
};

void close_console() {

};


