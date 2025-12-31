#pragma once

#include "core/types.h" // For types, strings and vectors.

// Forward declarations.
struct RenderingContext;

enum class ConsoleState {
	CLOSED = 0,
	OPEN_SMALL,
	OPEN_BIG,

	COUNT
};

struct ConsoleLog {
	std::string message;
	u32 line_number;
};

struct ConsoleInput {
	char* data[1024]; // Fixed amount of chars the user can type.
};

struct Console {
	ConsoleInput input;
	std::vector<ConsoleLog> logs;
	
	ConsoleState state = ConsoleState::CLOSED;

	float openness; // 1.0f = top (console is closed).
};

void init_console(Console* console);
void draw_console(RenderingContext* ctx, Console* console);

