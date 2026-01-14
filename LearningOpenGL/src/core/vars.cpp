#include "core/vars.h"

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


void init_vars(HotloadedVariables* vars, const char* path) {
	// Setting some default values if we want to...
	//

	reload_vars(vars, path);

	return;
}

void reload_vars(HotloadedVariables* vars, const char* path) {
	FILE* file = fopen(path, "r");

	if (!file) {
		fprintf(stderr, "ERROR: opening vars file at path: %s\n", path);
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
