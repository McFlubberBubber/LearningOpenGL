#include "core/vars.h"

#include "ui/console.h" // For console logging.

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static bool is_whitespace(char c) {
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static const char* skip_whitespace(const char* s) {
	while (*s && is_whitespace(*s)) {
		s++;
	}
	
	return s;
}

static void trim_end(char* s) { // For placing null terminator.
	int length = (int)strlen(s);

	while (length > 0 && is_whitespace(s[length - 1])) {
		s[length - 1] = '\0';
		length--;
	}
}

static bool parse_float(const char* s, float* result) {
	char* end;
	float value = strtof(s, &end);

	// Check if conversion worked (move end pointer).
	if (end != s && *end == '\0') {
		*result = value;
		return true;
	}

	return false;
}

static bool parse_int(const char* s, int* result) {
	static constexpr int base10 = 10;
	char* end;
	s64 value = strtol(s, &end, base10);

	// Check if conversion worked (move end pointer).
	if (end != s && *end == '\0') {
		*result = (int)value;
		return true;
	}

	return false;
	
}

static bool parse_bool(const char* s, bool* result) {
	if (strcmp(s, "true") == 0 || strcmp(s, "1") == 0) {
		*result = true;
		return true;
	} else if (strcmp(s, "false") == 0 || strcmp(s, "0") == 0) {
		*result = false;
		return true;
	}

	return false;
}

void init_vars(HotloadedVariables* vars, const char* path, Console* console) {
	vars->file_path = path;
	vars->console_ptr = console;
	assert(vars->console_ptr != NULL);

	reload_vars(vars);
	return;
}

void reload_vars(HotloadedVariables* vars) {
	FILE* file = fopen(vars->file_path, "r");

	if (!file) {
		char log[128];
		snprintf(log, sizeof(log), "ERROR: Could not open vars file at path: %s", vars->file_path);
		push_log(vars->console_ptr, std::string(log), LogType::ERROR);
		fprintf(stderr, "ERROR: opening vars file at path: %s\n", vars->file_path);
		return;
	}

	char current_section[64] = { 0 };
	char line[512];
	int line_number = 0;

	while (fgets(line, sizeof(line), file)) {
		line_number++;

		// Remove newline.
		int length = (int)strlen(line);
		if (length > 0 && line[length - 1] == '\n') {
			line[length - 1] = '\0';
		}

		const char* trimmed = skip_whitespace(line);

		// Skipping empty lines and comments.
		if (trimmed[0] == '\0' || trimmed[0] == '#') { continue; }

		// Checking for section headers.
		if (trimmed[0] == ':' && trimmed[1] == '/') {
			const char* section_name = skip_whitespace(trimmed + 2);
			strncpy(current_section, section_name, sizeof(current_section) - 1);
			// printf("Entering [%s] section\n", current_section);
			continue;
		}

		// Parse variable assignment.
		char* equals = strchr(line, '=');
		if (!equals) {
			fprintf(stderr, "Invalid syntax at line %d: %s\n", line_number, trimmed);
			continue;
		}

		*equals = '\0';
		char* var_name  = (char*)skip_whitespace(line);
		char* var_value = (char*)skip_whitespace(equals + 1);
		trim_end(var_name);
		trim_end(var_value);
		
		// Binding variables...
		bool found = false;
		
		#define BIND(section, name, target, type) \
			if (strcmp(current_section, #section) == 0 && strcmp(var_name, #name) == 0) { \
				if (parse_##type(var_value, &target)) { \
					/* printf("%s = %s\n", #name, var_value); */ \
					found = true; \
				} else { \
					fprintf(stderr, "ERROR: Failed to parse a " #type " value: '%s' for %s at line %d\n", var_value, #name, line_number); \
				} \
			}

		VAR_BINDINGS // Expands to the vars.h bindings macro we had.

		#undef BIND
			
		if (!found && current_section[0] != '\0') {
			fprintf(stderr, "Unknown variable '%s' in section [%s] at line %d\n", var_name, current_section, line_number);
		}
		
	} // End of while loop.
	
	fclose(file);
	// printf("Vars reloaded from path: %s\n", path);
}

bool write_to_vars(HotloadedVariables* vars, const std::vector<std::string>& tokens) {
	FILE* file = fopen(vars->file_path, "r");
	if (!file) {
		fprintf(stderr, "ERROR: opening vars file at path: %s\n", vars->file_path);
		return false;
	}

	static constexpr int MAX_LINES = 2048;
	static constexpr int MAX_LINE_LENGTH = 512;

	typedef char Line[MAX_LINE_LENGTH];
	Line* lines = (Line*)malloc(MAX_LINES * sizeof(Line));
	memset(lines, 0, MAX_LINES * sizeof(Line));
	if (!lines) {
		fprintf(stderr, "ERROR: Failed to allocate memory for lines!\n");
		fclose(file);
		return false;
	}
	int line_count = 0;
	
	const char* target_section = tokens[1].c_str();
	const char* target_var     = tokens[2].c_str();
	const char* new_value      = tokens[3].c_str();

	// First, read file into memory.
	while (fgets(lines[line_count], MAX_LINE_LENGTH, file) && line_count < MAX_LINES) {
		int len = (int)strlen(lines[line_count]);
		if (len > 0 && lines[line_count][len - 1] == '\n') {
			lines[line_count][len - 1] = '\0';
		}
		line_count++;
	}
	fclose(file);

	char current_section[64] = { 0 };
	bool found = false;

	// Then, modify the contents of the file in memory.
	for (size_t i = 0; i < line_count; ++i) {

		if (line_count >= MAX_LINES) {
			fprintf(stderr, "ERROR: Config file too large\n");
			break;
		}

		const char* trimmed = skip_whitespace(lines[i]);
		if (trimmed[0] == '\0' || trimmed[0] == '#') { continue; }

		if (trimmed[0] == ':' && trimmed[1] == '/') {
			const char* section = skip_whitespace(trimmed + 2);
			strncpy(current_section, section, sizeof(current_section) - 1); // Space for null-term.
			current_section[sizeof(current_section) - 1] = '\0';
			continue;
		}

		if (strcmp(current_section, target_section) != 0) { continue; }

		char current_line[MAX_LINE_LENGTH] = { 0 };
		strncpy(current_line, trimmed, sizeof(current_line) - 1);
		char* equals_pos = strchr(current_line, '=');
		if (!equals_pos) {
			fprintf(stderr, "ERROR: Syntax error at line %d, missing equals operator!\n", (int)(i + 1));
			free(lines);
			return false;
		}

		char* var_name = (char*)skip_whitespace(current_line);
		char* c = var_name;
		while (!is_whitespace(*c)) {
			c++;
		}
		*c = '\0';

		// If we find our variable...
		if (strcmp(var_name, target_var) == 0) {
			char new_line[MAX_LINE_LENGTH];
			char* value_start = (char*)skip_whitespace(equals_pos + 1);
			char* comment	  = strchr(value_start, '#');

			if (comment) {
				snprintf(new_line, sizeof(new_line), "%s = %s %s", var_name, new_value, comment);
			} else {
				snprintf(new_line, sizeof(new_line), "%s = %s", var_name, new_value);
			}

			strncpy(lines[i], new_line, MAX_LINE_LENGTH - 1);
			lines[i][MAX_LINE_LENGTH - 1] = '\0';
			found = true;
			break;
		}
	}

	if (!found) {
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "ERROR: Variable %s not found in section %s", target_var, target_section);
		push_log(vars->console_ptr, std::string(buffer), LogType::ERROR);
		free(lines);
		return false;
	}

	// Finally, write the new contents into the actual file in write mode.
	file = fopen(vars->file_path, "w");
	if (!file) {
		fprintf(stderr, "ERROR: opening vars file at path: %s\n", vars->file_path);
		free(lines);
		return false;
	}

	for (int i = 0; i < line_count; ++i) {
		fprintf(file, "%s\n", lines[i]);
	}
	
	fclose(file);
	free(lines);
	// reload_vars(vars);
	return true;
}

std::vector<std::string> get_all_lines(HotloadedVariables* vars) {
	FILE* file = fopen(vars->file_path, "r");
	std::vector<std::string> results = {};
	
	if (!file) {
		fprintf(stderr, "ERROR: opening vars file at path: %s\n", vars->file_path);
		return results;
	}

	char line[512];
	while (fgets(line, sizeof(line), file)) {
		int length = (int)strlen(line);
		if (length > 0 && line[length - 1] == '\n') {
			line[length - 1] = '\0';
		}

		const char* trimmed = skip_whitespace(line);
		if (trimmed[0] == '#') { continue; }

		results.push_back(trimmed);
	}
	
	fclose(file);
	return results;
}
