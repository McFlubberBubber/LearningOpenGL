#include "console.h"

#include "renderer/render_context.h"

#include "core/time.h"
#include "core/vars.h" // For writing to the vars file from the console.

#include <math.h>   // For fabs()
#include <cstring>  // For std::memset()

namespace ConsoleSpecs {
	// Just in case, to prevent bloat.
	static constexpr u32 MAX_LOGS    = 1000;
	static constexpr u32 MAX_HISTORY = 100;
	
	static const glm::vec3 BG_COLOR { 0.05f, 0.35f, 0.35f };
	static constexpr float BG_ALPHA = 0.9f;
	
	static const glm::vec3 LOG_FONT_COLOR { 1.0f, 1.0f, 1.0f }; // @Cleanup: not needed?
	static constexpr float LOG_SCALE 	   = 0.6f;
	static constexpr float LOG_X_PADDING   = 4.0f;
	static constexpr float LOG_Y_PADDING   = 8.0f;
	static constexpr float LOG_LINE_HEIGHT = 36.0f;

	static constexpr float CLOSED_OPENNESS = 1.2f; // We are accounting for the input field.

	static const glm::vec3 INPUT_FIELD_COLOR { 0.07f, 0.50f, 0.50f }; // @Cleanup: not needed too?
	static const glm::vec3 INPUT_FONT_COLOR  { 0.0f, 1.0f, 0.0f };
	static constexpr float INPUT_FIELD_HEIGHT = 40.0f;
	static constexpr float INPUT_X_PADDING    = 4.0f;
	static constexpr float INPUT_Y_PADDING    = 10.0f;
	static constexpr float INPUT_SCALE        = 0.65f;
	
	static const glm::vec3 CURSOR_COLOR	{ 0.5f, 0.9f, 0.5f };
	static constexpr float CARET_WIDTH 	  = 3.0f;
	static constexpr float CURSOR_PADDING = 4.0f;

	static constexpr float TEXT_ALPHA = 1.0f;

	// These colors under are specifically for the different log messages.
	static const glm::vec3 WHITE  { 1.0f, 1.0f, 1.0f }; // COMMAND
	static const glm::vec3 BRONZE { 0.8f, 0.7f, 0.5f }; // OUTPUT
	static const glm::vec3 RED    { 1.0f, 0.0f, 0.0f }; // ERROR
	static const glm::vec3 ORANGE { 1.0f, 0.5f, 0.1f }; // WARNING
	static const glm::vec3 GRAY   { 0.5f, 0.5f, 0.5f }; // INFO : can consider a green?
};

// Quick helper function for identifying if the char we are on is a space or indent.
static inline bool is_space(char c) {
	return c == ' ' || c == '\t';
}

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

	// These variables are hotloaded, therefore we prefix them with HL, might get changed in the future.
	auto vars = &console->render_ctx_ptr->vars;
	float HL_small_openness = vars->dev.console_small_openness;
	float HL_big_openness = vars->dev.console_big_openness;
	float HL_openness_dt = vars->dev.console_openness_dt;

	switch (console->state) {
	case ConsoleState::CLOSED:
		target_openness = CLOSED_OPENNESS;
		break;

	case ConsoleState::OPEN_SMALL:
//		target_openness = SMALL_OPENNESS;
		target_openness = HL_small_openness;
		break;

	case ConsoleState::OPEN_BIG:
//		target_openness = BIG_OPENNESS;
		target_openness = HL_big_openness;
		break;
	}

	float d = target_openness - console->openness;
	if (fabs(d) > 0.001f) {
//		console->openness += d * OPENNESS_DT * (Time::get_delta_time() * 60.0f);
		console->openness += d * HL_openness_dt * (Time::get_delta_time() * 60.0f);
	} else {
		console->openness = target_openness;
	}
}

static void draw_cursor(Console* console, RenderingContext* ctx, float x, float y0, float y1, float text_width) {
	using namespace ConsoleSpecs;
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

static void draw_logs(Console* console, RenderingContext* ctx, float y) {
	using namespace ConsoleSpecs;
    auto font = &ctx->assets.fonts[FONT_REGULAR];
    if (console->logs.empty()) return;
	
	float text_x = LOG_X_PADDING;
	float text_y = y + LOG_Y_PADDING;
	float console_top = static_cast<float>(ctx->viewport.height) * 0.99f;
	int log_count = (int)console->logs.size(); 
	glm::vec3 color;

	for (int i = log_count - 1; i >= 0; --i) {
		auto& log = console->logs[i];

		if (text_y > console_top) { break; };
		
		switch (log.type) {
		case LogType::COMMAND: { color = WHITE;  break; }
		case LogType::OUTPUT:  { color = BRONZE; break; }
		case LogType::ERROR:   { color = RED;	 break; }
		case LogType::WARNING: { color = ORANGE; break; }
		case LogType::INFO:	   { color = BRONZE;  break; }
		default:  			   { color = WHITE;  break; } 
		}
		
		draw_text(font, ctx, log.message, text_x, text_y, INPUT_SCALE, color, TEXT_ALPHA, TextAlign::LEFT, false);
		
		text_y += LOG_LINE_HEIGHT;
	}
}


// These functions below are all in relation to what happens after the user presses ENTER.
static void push_log(Console* console, const std::string& command, LogType type = LogType::INFO) {
	ConsoleLog log;
	log.message = command;
	log.type    = type;

	console->logs.push_back(log);

	// Ensure that we don't go over the max limit of logs within the vector.
	if (console->logs.size() > ConsoleSpecs::MAX_LOGS) {
		console->logs.erase(console->logs.begin()); // Remove the oldest log first.
	}
}

static void add_to_history(Console* console, const std::string& command) {
	// Prevent adding duplicates to the history.
	if (!console->command_history.empty() && console->command_history.back() == command) {
		return;
	}

	console->command_history.push_back(command);
	console->history_index = -1;

	// Check we don't go over the command_history limit.
	if (console->command_history.size() > ConsoleSpecs::MAX_HISTORY) {
		console->command_history.erase(console->command_history.begin());
	}
	
}

static void log_unknown_command(Console* console) {
	std::string command = console->input.data;
	push_log(console, command + ": Unkown command.", LogType::ERROR);
}

static void parse_and_tokenize(Console* console, const std::string& command) {
	std::vector<std::string> tokens;
	std::string remaining = command;

	// Eat any spaces the user may have entered before inputting their command.
	size_t spaces = 0;
	while (spaces < remaining.size() && is_space(remaining[spaces])) {
		spaces++;
	}
	remaining = remaining.substr(spaces);
	
	// We already check if the command has any length, so we don't need to check anything else.
	bool in_quotes = false;
	std::string current_token;
	for (size_t i = 0; i < remaining.length(); i++) {
		char c = remaining[i];

		// Toggling the in_quotes flag if we find the pair of quotes in the command.
		if (c == '"') {
			in_quotes = !in_quotes;
			
		} else if (c == ' ' && !in_quotes) { // If we find a whitespace..

			// If we have a token that has data, send it.
			if (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
			
		} else { // Else, keep adding to the current_token.
			current_token += c;
		}
	}

	if (!current_token.empty()) { // If there was a token at the end, add that.
		tokens.push_back(current_token);
	}

	auto it = console->arguments.find(tokens[0]);
	if (it != console->arguments.end()) {
		if (it->second.procedure_ptr == NULL) {
			std::cout << "procedure_ptr not found for: " << it->first << "\n";
			return;
			
		} else {
			std::cout << "procedure_ptr found for: " << it->first << "\n";
			it->second.procedure_ptr(console, tokens);
		}
		
	} else { // Else, we have not found a command that matches.
		std::cout << "Command not found for matching token: " << tokens[0] << "\n";
		log_unknown_command(console);
	}
}

static void clear_input(Console* console) {
	std::memset(console->input.data, 0, sizeof(console->input.data));
	console->input.cursor_pos = 0;
	console->input.length = 0;
	console->input.cursor_blink_time = 0.0f;
//	console->history_index = -1;
}


static void clear_logs(Console* console, const std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		push_log(console, "ERROR::expected usage: clear", LogType::ERROR);
		return;
	}

	console->logs.clear();
	return;
}

static void clear_command_history(Console* console, const std::vector<std::string>& tokens) {
	if (console->command_history.empty()) { return; }
	
	if (tokens.size() != 2) {
		push_log(console, "ERROR::expected usage: reset <keyword>", LogType::ERROR);
		return;
	}

	if (tokens[1].compare("command_history") == 0) {
		console->command_history.clear();
		push_log(console, "Cleared command history from console.", LogType::OUTPUT);
		return;
	} else {
		push_log(console, "ERROR::keyword does not exist!", LogType::ERROR);
		push_log(console, "Did you mean to type ""command_history""?", LogType::ERROR);
		return;
	}
}

static void change_current_scene(Console* console, const std::vector<std::string>& tokens) {
	if (console->command_history.empty()) { return; }

	auto ctx = console->render_ctx_ptr;

	if (tokens.size() != 2) {
		push_log(console, "ERROR::expected usage: scene <keyword>", LogType::ERROR);
		return;
	}

	if (tokens[1].compare("normal") == 0) {
		ctx->app.scene = SceneState::MAIN;
		push_log(console, "Changing current scene to NORMAL.", LogType::OUTPUT);
		return;
	} else if (tokens[1].compare("space") == 0) {
		ctx->app.scene = SceneState::SPACE;
		push_log(console, "Changing current scene to SPACE.", LogType::OUTPUT);
		return;
	} else {
		push_log(console, "ERROR::keyword does not exist!", LogType::ERROR);
		push_log(console, "Type ""help scene"" to see a list of keywords.", LogType::ERROR);
		return;
	}
}

static void print_vars_file(Console* console, const std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		push_log(console, "ERROR::expected usage: print_hotloader", LogType::ERROR);
		return;
	}
	
	auto vars = &console->render_ctx_ptr->vars;
	std::vector<std::string> lines = get_all_lines(vars);

	for (auto& line : lines) {
		std::cout << line << "\n";
		push_log(console, line, LogType::OUTPUT);
	}
}

static void update_vars_file(Console* console, const std::vector<std::string>& tokens) {
	std::string sections[] = {"Display", "Audio", "Scene", "Dev"};

	if (tokens.size() == 2 && tokens[1].compare("help") == 0) {
		// Display how to use hotloader command.
	}


	if (tokens.size() != 4) {
		push_log(console, "ERROR::expected usage: hotloader <section> <variable> <value>", LogType::ERROR);
		push_log(console, "Type: ""hotloader help <section>"" to see a list of valid variables in each category.", LogType::ERROR);
		push_log(console, "List of sections: Display | Audio | Scene | Dev", LogType::ERROR);	
		return;
	}
	
	auto vars = &console->render_ctx_ptr->vars;
	write_to_vars(vars, tokens);
}

static void init_arguments(Console* console) {
	console->commands[CMD_CLEAR].procedure_ptr = clear_logs;
	console->commands[CMD_RESET].procedure_ptr = clear_command_history;
	console->commands[CMD_SCENE_CHANGE].procedure_ptr = change_current_scene;
	console->commands[CMD_PRINT_HOTLOADER].procedure_ptr = print_vars_file;
	console->commands[CMD_HOTLOADER].procedure_ptr = update_vars_file;

	auto& args = console->arguments;
	args.insert({ "clear", console->commands[CMD_CLEAR] });
	args.insert({ "reset", console->commands[CMD_RESET] });
	args.insert({ "scene", console->commands[CMD_SCENE_CHANGE] });
	args.insert({ "print_hotloader", console->commands[CMD_PRINT_HOTLOADER] });
	args.insert({ "hotloader", console->commands[CMD_HOTLOADER] });
}

// ----- Public functions -----
void init_console(Console* console, RenderingContext* ctx) {
	init_arguments(console);
	
	console->state = ConsoleState::CLOSED;
	console->openness = ConsoleSpecs::CLOSED_OPENNESS;

	console->input.length = 0;
	
	console->input.cursor_pos 		 = 0;
	console->input.cursor_blink_time = 0.0f;

	console->render_ctx_ptr = ctx;
	if (console->render_ctx_ptr == NULL) {
		std::cout << "Console pointer to renderer is NULL!\n";
		console->is_initialized = false;
		return;
	}

	console->is_initialized = true;
	std::string welcome_msg = "Welcome to the console! Type 'help' for a list of commands.";
	push_log(console, welcome_msg, LogType::INFO);
}

void draw_console(Console* console, RenderingContext* ctx) {
	using namespace ConsoleSpecs;
	if (!console->is_initialized) {
		push_message(&ctx->message_queue, "Console not initialized!");
		return;
	}

	update_openness(console); // Animates the console movement.
	
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
	float input_y0 = y0 - INPUT_FIELD_HEIGHT; // Start a bit lower down.
	float input_y1 = y0; // Start where the prev quad ended.
	draw_quad(ctx, x0, y0, x1, y1, BG_COLOR, BG_ALPHA);
	draw_quad(ctx, x0, input_y0, x1, input_y1, INPUT_FIELD_COLOR, BG_ALPHA);

	draw_logs(console, ctx, input_y1); // We pass the input_y1 as a ref to the input field.
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
	if (console->input.length == 0) { return; }
	std::string command = console->input.data;

	push_log(console, "> " + command, LogType::COMMAND);
	add_to_history(console, command);
	parse_and_tokenize(console, command);
	clear_input(console);
}

void move_cursor_by_char(Console* console, bool is_forward) {
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

void move_cursor_by_word(Console* console, bool is_forward) {
	int old_pos = console->input.cursor_pos;
	int& current_pos = console->input.cursor_pos;
	
	char* text = console->input.data;
	int length = console->input.length;

	if (is_forward) { // Parse in front of the cursor position.
		if (current_pos >= length) {
			return;
		}

		// Skip the current word first, then the whitespaces.
		while (current_pos < length && !is_space(text[current_pos])) {
			current_pos++;
		}
		while (current_pos < length && is_space(text[current_pos])) {
			current_pos++;
		}
		
	} else { // Parse behind the cursor position.
		if (current_pos <= 0) {
			return;
		}

		// Skip the whitespaces first, then the previous word.
		while (current_pos > 0 && is_space(text[current_pos - 1])) {
			current_pos--;
		}
		while (current_pos > 0 && !is_space(text[current_pos - 1])) {
			current_pos--;
		}
	}

	if (current_pos != old_pos) { console->input.cursor_blink_time = 0.0f; }
}

void delete_word(Console* console) {
	int& current_pos = console->input.cursor_pos;
	if (current_pos <= 0) {
		return;
	}
	
	int old_pos = console->input.cursor_pos;
	char* text = console->input.data;
	int& current_length = console->input.length;
	
	// First, eat all the whitespaces, then the word
	while (current_pos > 0 && is_space(text[current_pos - 1])) {
		current_pos--;
	}
	while (current_pos > 0 && !is_space(text[current_pos - 1])) {
		current_pos--;
	}

	int num_of_chars_to_delete = old_pos - current_pos;
	if (num_of_chars_to_delete > 0) {
		// Move everything from the old_pos to the current_pos.
		// Src: where the text originally starts (old_pos).
		// Dest: where the new text should go to (current_pos).
		// Size: remaining text with its null terminator.
		std::memmove(&text[current_pos], &text[old_pos], current_length - old_pos + 1);
		current_length -= num_of_chars_to_delete;
		console->input.cursor_blink_time = 0.0f;
	}
}

void navigate_command_history(Console* console, bool is_forward) {
	if (console->command_history.empty()) { return; }
	clear_input(console);
	std::string current_command = {};

	int command_history_count = static_cast<int>(console->command_history.size());
	if (console->history_index == -1) {
		console->history_index = command_history_count;
	}
	command_history_count -= 1; // This is for zero-indexing

	if (!is_forward) {
		console->history_index--;
		if (console->history_index <= 0) {
			console->history_index = 0;
		}
	} else {
		console->history_index++;
		if (console->history_index >= command_history_count) {
			console->history_index = command_history_count;
		}
	}

	current_command = console->command_history[console->history_index];
	for (char c : current_command) {
		insert_character(console, c); // @Speed: We could try putting the whole command in at the same
									  // time instead of inserting it one by one?
	}
}
