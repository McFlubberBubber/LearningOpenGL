#include "console.h"

#include "renderer/render_context.h"
#include "core/time.h"

#include <math.h>  // For fabs()
#include <cstring> // For std::memset()

namespace ConsoleSpecs {
	static const glm::vec3 BG_COLOR 		 { 0.05f, 0.35f, 0.35f };
	static const glm::vec3 INPUT_FIELD_COLOR { 0.07f, 0.50f, 0.50f };
	static constexpr float BG_ALPHA = 0.9f;
	
	static const glm::vec3 LOG_FONT_COLOR 	{ 1.0f, 1.0f, 1.0f };
	static const glm::vec3 INPUT_FONT_COLOR { 0.0f, 1.0f, 0.0f };
	static const glm::vec3 CURSOR_COLOR 	{ 0.5f, 0.9f, 0.5f };
	static constexpr float TEXT_ALPHA = 1.0f;

	// These variables define what are the appropriate y-levels for the console on the screen.
	static constexpr float SMALL_OPENNESS  = 0.8f;
	static constexpr float BIG_OPENNESS    = 0.2f;
	static constexpr float CLOSED_OPENNESS = 1.2f; // We are accounting for the input field.
	static constexpr float OPENNESS_DT     = 0.3f;

	static constexpr float INPUT_FIELD_HEIGHT = 40.0f;
	static constexpr float INPUT_X_PADDING    = 4.0f;
	static constexpr float INPUT_Y_PADDING    = 10.0f;
	static constexpr float INPUT_SCALE        = 0.65f;

	static constexpr float CARET_WIDTH = 3.0f;
	static constexpr float CURSOR_PADDING = 4.0f;
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

static void draw_cursor(Console* console, RenderingContext* ctx, float x, float y0, float y1, float text_width) {
	using namespace ConsoleSpecs;
#if 0
	float cursor_x  = x  + INPUT_X_PADDING;
	float cursor_y0 = y0 + CURSOR_PADDING;
	float cursor_y1 = y1 - CURSOR_PADDING;
	cursor_x += text_width;
	float cursor_x1 = cursor_x + CURSOR_LENGTH;

	// Cursor blink
	console->input.cursor_blink_time += Time::get_delta_time();
	if (console->input.cursor_blink_time <= 1.0f) {
		draw_quad(ctx, cursor_x, cursor_y0, cursor_x1, cursor_y1, CURSOR_COLOR, TEXT_ALPHA);
	} else if (console->input.cursor_blink_time > 2.0f) {
		console->input.cursor_blink_time = 0.0f;
	}
	
#else
	float caret_x0 = x + INPUT_X_PADDING + text_width;
	float caret_y0 = y0 + CURSOR_PADDING;
	float caret_y1 = y1 - CURSOR_PADDING;
	float caret_x1 = caret_x0 + CARET_WIDTH;

	// Cursor blink
	console->input.cursor_blink_time += Time::get_delta_time();
	if (console->input.cursor_blink_time <= 1.0f) {
		draw_quad(ctx, caret_x0, caret_y0, caret_x1, caret_y1, CURSOR_COLOR, TEXT_ALPHA);
	} else if (console->input.cursor_blink_time > 2.0f) {
		console->input.cursor_blink_time = 0.0f;
	}

#endif
}

static void draw_input_area(Console* console, RenderingContext* ctx, float x, float y) {
	using namespace ConsoleSpecs;
	auto font = &ctx->assets.fonts[FONT_REGULAR];
	
	float input_x = x + INPUT_X_PADDING;
	float input_y = y - INPUT_FIELD_HEIGHT + INPUT_Y_PADDING;
	
	std::string text = console->input.data;
	draw_text(font, ctx, text, input_x, input_y, INPUT_SCALE, INPUT_FONT_COLOR, TEXT_ALPHA, TextAlign::LEFT, false);
	float text_width = get_string_width_in_pixels(font, text, INPUT_SCALE); // Not being used now.

	std::string text_to_cursor = text.substr(0, console->input.cursor_pos);
	float text_width_to_cursor = get_string_width_in_pixels(font, text_to_cursor, INPUT_SCALE);

	float input_y0 = y - INPUT_FIELD_HEIGHT;
	float input_y1 = y;
	draw_cursor(console, ctx, x, input_y0, input_y1, text_width_to_cursor);
}

void init_console(Console* console) {
	console->state = ConsoleState::CLOSED;
	console->openness = ConsoleSpecs::CLOSED_OPENNESS;

	console->input.length = 0;
	
	console->input.cursor_pos 		 = 0;
	console->input.cursor_blink_time = 0.0f;
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
	draw_quad(ctx, x0, y0, x1, y1, BG_COLOR, BG_ALPHA);

	float input_y0 = y0 - INPUT_FIELD_HEIGHT; // Start a bit lower down.
	float input_y1 = y0; // Start where the prev quad ended.
	draw_quad(ctx, x0, input_y0, x1, input_y1, INPUT_FIELD_COLOR, BG_ALPHA);
	
	// Drawing text-stuff.
	// Temp vars.
	std::string log_example = "This is a log example!";
	float log_x_padding		= 4.0f;
	float log_y_padding		= 8.0f;
	float log_x 			= x0 + log_x_padding;
	float log_y				= y0 + log_y_padding; // This y-coord needs to vary.
	float log_scale			= 0.6f;
	draw_text(&ctx->assets.fonts[FONT_REGULAR], ctx, log_example, log_x, log_y, log_scale, LOG_FONT_COLOR, TEXT_ALPHA, TextAlign::LEFT, false);

	// @TODO: draw_console_logs();
	draw_input_area(console, ctx, x0, y0);
}


void insert_character(Console* console, char character) {
	if (console->input.cursor_pos >= 1023) {
		std::cout << "CONSOLE_ERROR: Reached maximum number of character buffer!\n";
		return;
	}
	
	int pos = console->input.cursor_pos;

	// Make room for the new character
	std::memmove(&console->input.data[pos + 1], &console->input.data[pos], console->input.length - pos + 1); // Moving the null terminator

	console->input.data[pos] = character;
	console->input.cursor_pos++;
	console->input.length++;
	console->input.cursor_blink_time = 0.0f;
}

void delete_character(Console* console) {
	if (console->input.cursor_pos == 0) { return; }
	
	int pos = console->input.cursor_pos;
	
	// Shift everything after the cursor to the left.
	std::memmove(&console->input.data[pos - 1], &console->input.data[pos], console->input.length - pos + 1); // We do the +1 to move the null terminator.
	
	console->input.cursor_pos--;	
	console->input.length--;
	console->input.cursor_blink_time = 0.0f;
}

void execute_command(Console* console) {
	std::string command = console->input.data;
	std::cout << "Command inputted: " << command << "\n";

	// Clearing the input field.
	std::memset(console->input.data, 0, sizeof(console->input.data));
	console->input.cursor_pos = 0;
	console->input.length = 0;
	console->input.cursor_blink_time = 0.0f;
}


void move_cursor(Console* console, bool is_forward) {
	std::cout << "Cursor position: " << console->input.cursor_pos << "\n";
	std::cout << "Text length:     " << console->input.length << "\n";

	int old_pos = console->input.cursor_pos;
	if (is_forward) {
		if (console->input.cursor_pos < console->input.length) {
			console->input.cursor_pos++;
		}
	} else {
		if (console->input.cursor_pos > 0) {
			console->input.cursor_pos--;
		}
	}

	if (console->input.cursor_pos != old_pos) {
		console->input.cursor_blink_time = 0.0f;
	}
}
