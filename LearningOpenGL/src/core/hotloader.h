#pragma once

#include "core/types.h"
#include "core/windows_utils.h"

struct Hotloader;

struct FileWatcher {
	char path[512];
	FileTimestamp last_timestamp;
};

struct Hotloader {
	FileWatcher watcher;
	void (*callback)(Hotloader*);

	bool is_initialized;
};

void init_hotloader(Hotloader* hotloader, const char* filepath);

void update_hotloader(Hotloader* hotloader);
void cleanup_hotloader(Hotloader* hotloader);

bool check_for_file_updates(FileWatcher* watcher);

