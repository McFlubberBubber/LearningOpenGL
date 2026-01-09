#include "core/hotloader.h"

#include <cstring>

static void init_filewatcher(FileWatcher* watcher, const char* filepath) {
	strncpy_s(watcher->path, sizeof(watcher->path), filepath, _TRUNCATE);
	watcher->last_timestamp = get_file_timestamp(watcher->path);

	std::cout << "Watching file at path: " << watcher->path << "\n";
}

static void hotloader_callback(Hotloader* hotloader) {
	std::cout << "File changed at path: " << hotloader->watcher.path << "\n";
}

bool check_for_file_updates(FileWatcher* watcher) {
	FileTimestamp current = get_file_timestamp(watcher->path);

	if (current == 0) { return false; }

	if (current != watcher->last_timestamp) {
		watcher->last_timestamp = current;
		return true;
	}

	return false;
}

void init_hotloader(Hotloader* hotloader, const char* filepath) {
	init_filewatcher(&hotloader->watcher, filepath);
	hotloader->callback = hotloader_callback;
	hotloader->is_initialized = true;
}

void update_hotloader(Hotloader* hotloader) {
	if (!hotloader->is_initialized) { return; }

	if (check_for_file_updates(&hotloader->watcher)) {
		if (hotloader->callback) {
			hotloader->callback(hotloader);
		}
	}
}

void cleanup_hotloader(Hotloader* hotloader) {
	hotloader->callback = NULL;
	hotloader->is_initialized = false;
}
