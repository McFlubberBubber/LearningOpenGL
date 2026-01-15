#pragma once

#include "core/types.h"  // For types, strings and vectors.
#include <unordered_map> // For hashmap lookups for argument parsing. 

// Forward declarations.
struct RenderingContext;
struct Console;

enum CommandType {
	CMD_CLEAR = 0,
	CMD_RESET,

	CMD_SCENE_CHANGE,
	
	CMD_COUNT
};

struct Command {
	void (*procedure_ptr)(Console*, const std::vector<std::string>&);
};

enum class LogType {
	COMMAND = 0,
	OUTPUT,
	ERROR,
	WARNING,
	INFO,

	COUNT
};

enum class ConsoleState {
	CLOSED = 0,
	OPEN_SMALL,
	OPEN_BIG,

	COUNT
};

struct ConsoleLog {
	std::string message;
	LogType type = LogType::INFO;
};

struct ConsoleInput {
	char data[1024] = { 0 }; // Fixed amount of chars the user can type.
	int length = 0;
	
	int cursor_pos;
	float cursor_blink_time;
};

struct Console {
	ConsoleState state = ConsoleState::CLOSED;

	ConsoleInput input;
	std::vector<ConsoleLog> logs;
	
	std::vector<std::string> command_history;
	int history_index = -1; // -1 = not browsing history.

	Command commands[CMD_COUNT] = { 0 };
	std::unordered_map<std::string, Command> arguments;

	RenderingContext* render_ctx_ptr = NULL; // This is here to make console commands access the "app".

	float openness; // 1.0f = top (console is closed).
	bool is_initialized = false;
};

void init_console(Console* console, RenderingContext* ctx);
void draw_console(Console* console, RenderingContext* ctx);

void execute_command(Console* console);
void autocomplete_command(Console* console);

void insert_character(Console* console, char character);
void delete_character(Console* console);
void delete_word(Console* console);

void move_cursor_by_char(Console* console, bool is_forward);
void move_cursor_by_word(Console* console, bool is_forward);

void navigate_command_history(Console* console, bool is_forward);
