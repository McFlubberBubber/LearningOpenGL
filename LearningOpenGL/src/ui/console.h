#pragma once

#include "core/types.h" // For types, strings and vectors.

// Forward declarations.
struct RenderingContext;

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
	ConsoleInput input;
	std::vector<ConsoleLog> logs;
	std::vector<std::string> command_history;
	int history_index = -1; // -1 = not browsing history.
	
	ConsoleState state = ConsoleState::CLOSED;

	float openness; // 1.0f = top (console is closed).
};

void init_console(Console* console);
void draw_console(RenderingContext* ctx, Console* console);

void execute_command(Console* console);
void autocomplete_command(Console* console);

void insert_character(Console* console, char character);
void delete_character(Console* console);
void delete_word(Console* console);

void move_cursor_by_char(Console* console, bool is_forward);
void move_cursor_by_word(Console* console, bool is_forward);

void navigate_command_history(Console* console, bool is_forward);
