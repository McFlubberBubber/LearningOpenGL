#include "console.h"

#include "renderer/render_context.h"
#include "core/time.h"

#include "math.h" // For fabs()

namespace ConsoleSpecs {
	static const glm::vec3 BG_COLOR { 0.0f, 0.2f, 0.4f };
	static const glm::vec3 INPUT_FIELD_COLOR { 0.0f, 0.4f, 0.6f };
	static constexpr float ALPHA = 0.9f;

	static const glm::vec3 LOG_FONT_COLOR {1.0f, 1.0f, 1.0f};
	static const glm::vec3 INPUT_FONT_COLOR {0.0f, 1.0f, 0.0f};

	// These variables define what are the appropriate y-levels for the console on the screen.
	static constexpr float SMALL_OPENNESS  = 0.8f;
	static constexpr float BIG_OPENNESS    = 0.2f;
	static constexpr float CLOSED_OPENNESS = 1.75f; // We are accounting for the input field.
	static constexpr float OPENNESS_DT     = 0.3f;

	static constexpr float INPUT_FIELD_HEIGHT = 34.0f;
};


static std::string console_state_to_string(ConsoleState state) {
	switch (state) {
		case ConsoleState::CLOSED:			return "CLOSED";
		case ConsoleState::OPEN_SMALL:		return "OPEN_SMALL";
		case ConsoleState::OPEN_BIG:		return "OPEN_BIG";
		default:							return "ERROR!";
	}
}

// This draw function could be moved to a more general draw source file instead of dumping it here.
static void draw_quad(const RenderingContext* ctx, float x0, float y0, float x1, float y1, const glm::vec3& color, float alpha) {
	auto shader = &ctx->assets.shaders[SHADER_TEXTBOX];

	use_shader(shader);
	const glm::mat4& ortho = ctx->viewport.ortho_projection;

	set_mat4(shader, "projection", ortho);
	set_vec3(shader, "color", color); // Should the shader here be set to a vec4?
	set_float(shader, "alpha", alpha);

	float vertices[] = {
		// Positions
		x0,	y0,	// Bottom left
		x1,	y0,	// Bottom right
		x1,	y1,	// Top right
		x0,	y1,	// Top left
	};

	// Binding + drawing the elements
	glBindVertexArray(ctx->buffers.textbox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, ctx->buffers.textbox_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

static void update_openness(Console* console) {
	using namespace ConsoleSpecs;
	float target_openness = 0.0f;

	switch (console->state) {
	case ConsoleState::CLOSED:
		target_openness = CLOSED_OPENNESS;
		break;

	case ConsoleState::OPEN_SMALL:
		target_openness = SMALL_OPENNESS;
		break;

	case ConsoleState::OPEN_BIG:
		target_openness = BIG_OPENNESS;
		break;
	}

	float d = target_openness - console->openness;
	if (fabs(d) > 0.001f) {
		console->openness += d * OPENNESS_DT * (Time::get_delta_time() * 60.0f);
	} else {
		console->openness = target_openness;
	}
}

void init_console(Console* console) {
	console->state = ConsoleState::CLOSED;
	console->openness = ConsoleSpecs::CLOSED_OPENNESS;
}

void draw_console(RenderingContext* ctx, Console* console) {
	using namespace ConsoleSpecs;
	update_openness(console); // Animates the console movement.
	// std::string state_string = console_state_to_string(console->state);
	// std::cout << "Drawing console in " << state_string << "\n";
	
	// Temporary variables, might move these to a namespace here.
	float x1 = static_cast<float>(ctx->viewport.width);
	float y1 = static_cast<float>(ctx->viewport.height);
	float x0 = 0;
	float y0 = y1 * console->openness;

	// Don't do anything if the console state is closed.
	if (console->state == ConsoleState::CLOSED && y0 >= y1) {
		return;	
	}

	// Drawing both the report log and the input field area.
	draw_quad(ctx, x0, y0, x1, y1, BG_COLOR, ALPHA);
	draw_quad(ctx, x0, y0 - INPUT_FIELD_HEIGHT, x1, y0, INPUT_FIELD_COLOR, ALPHA);
}


