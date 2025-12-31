#pragma once

// Forward declarations.
struct RenderingContext;

enum class ConsoleState {
	CLOSED = 0,
	OPEN_SMALL,
	OPEN_BIG,

	COUNT
};

struct Console {
	ConsoleState state = ConsoleState::CLOSED;
	
	float openness;
};

void init_console(Console* console);

void open_console();
void draw_console(RenderingContext* ctx, Console* console);
void close_console();
